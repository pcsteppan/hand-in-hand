#include "ofApp.h"
#include "networkCredentials.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // FRAMERATE
    ofSetFrameRate(30);
    
    // DEBUG GUI
    gui.setup();
    gui.add(threshold.setup("Background subtraction threshold", 0.02f, 0.0f, 1.0f));
    
    // ZMQ NETWORK
    publisher.bind(publisherIP);
    subscriber.connect(subscriberIP);
    
    // UDP NETWORK
    
    /*
    udpPublisher.Create();
    udpPublisher.Connect("192.168.1.195",9998);
    udpPublisher.SetNonBlocking(true);
    
    udpSubscriber.Create();
    udpSubscriber.Bind(9999);
    udpSubscriber.SetNonBlocking(true);
     */
    
    // WEB CAMERA
    grabberLocalRawHand.setup(WIDTH,HEIGHT,GL_RGB);
    
    imgRemoteProcessedHand.allocate(WIDTH, HEIGHT, OF_IMAGE_COLOR);
    pixRemoteProcessedHand.allocate(WIDTH, HEIGHT, OF_IMAGE_GRAYSCALE);
    
    
    fboBackground.allocate(WIDTH, HEIGHT, GL_RGB);
    fboLocalProcessedHand.allocate(WIDTH, HEIGHT, GL_RGB);
    
    bufferTemp.allocate(1 * WIDTH * HEIGHT * sizeof(unsigned char));
    
    shaderPlane.set(WIDTH,HEIGHT,10,10);
    shaderPlane.mapTexCoords(0,0,WIDTH,HEIGHT);
    
    loadShaders();
}

//--------------------------------------------------------------
void ofApp::update(){
    grabberLocalRawHand.update();
    loadShaders();
    
    
    
    // Receiving a packet
    while (subscriber.hasWaitingMessage())
	{
		bufferTemp.clear();
		subscriber.getNextMessage(bufferTemp);
        
		pixRemoteProcessedHand.setFromPixels((unsigned char*) bufferTemp.getData(), 640, 480, OF_IMAGE_GRAYSCALE);
        imgRemoteProcessedHand.setFromPixels(pixRemoteProcessedHand);
        imgRemoteProcessedHand.update();
    }
    
    // Sending a packet
    ofPixels pix;
    if(!bHandInFrame && danceFrames.size() > 0){
        pix = danceFrames[ofGetFrameNum()%danceFrames.size()];
    } else {
        fboLocalProcessedHand.readToPixels(pix);
    }
    
    
    unsigned char * luminanceBufferPtr = pix.getChannel('r').getData();
    if (!publisher.send(luminanceBufferPtr, 1 * 640 * 480 * sizeof(unsigned char)))
	{
		cout << "send failed" << endl;
    } else {
        // poll hand presence once every second
        if(ofGetFrameNum() % 30 == 0){
            // logic for hand detection and looping
            if(!bHandInFrame && danceFrames.size() > 0) {
                fboLocalProcessedHand.readToPixels(pix);
            }
            int sum = 0;
            for (int i = 0; i < 640*480; i++) {
                sum += pix[i];
            }
            cout << "sum: " << sum << endl;
            if(sum > 1000000 && !bHandInFrame){
                //pushback on vector
                danceFrames.clear();
                bHandInFrame = true;
            } else if(sum < 1000000) {
                //clear vector
                
                bHandInFrame = false;
            }
        }
        
        if(bHandInFrame){
            danceFrames.push_back(ofImage(pix));
            cout << danceFrames.size() << endl;
        }
    }
    
    // UDP CODE, does not work due to limitations on packet size
    // Another possible solution may be to break the image into max_packet_size chunks
    // Another even coolor solution is to simply send the contour data (some vertices)
        // Note: that's a tradeoff between processing and network speed
    
    /*
    // SENDING UDP PACKET
    ofPixels pix;
    fboLocalProcessedHand.readToPixels(pix);
    const char * pixPtr = (char*)pix.getChannel('r').getData();
    udpPublisher.Send(pixPtr, 640*480);
    
    // -----------------
    
    // RECEIVING UDP PACKET
    char udpMessage[640*480];
    udpSubscriber.Receive(udpMessage,640*480);
    
    pixRemoteProcessedHand.setFromPixels((unsigned char*) udpMessage, 640, 480, OF_IMAGE_GRAYSCALE);
    imgRemoteProcessedHand.setFromPixels(pixRemoteProcessedHand);
    imgRemoteProcessedHand.update();
    
    // -----------------
     */
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    //ofClear(20,20,20,255);
    
    // Capture very first frame to substract from all future frames
    
    if (bFirstFrame){
        //fboBackground.clear();
        fboBackground.begin();
        grabberLocalRawHand.draw(0,0,640,480);
        fboBackground.end();
        bFirstFrame = false;
    }
    
    // -----------------
    
    
    
    
    // Remove background from video grabber
    
    
    fboLocalProcessedHand.begin();
    ofClear(0);
    shaderProcessor.begin();
    
    shaderProcessor.setUniform1f("threshold", threshold);
    shaderProcessor.setUniformTexture("pBackground", fboBackground.getTexture(), 1);
    shaderProcessor.setUniformTexture("pLocalRaw", grabberLocalRawHand.getTexture(),  2);
    ofPushMatrix();
    ofTranslate(WIDTH/2, HEIGHT/2);
    shaderPlane.draw();
    ofPopMatrix();
    
    shaderProcessor.end();
    fboLocalProcessedHand.end();
    
    // -----------------
    
    
    
    // Draw historied hands or new hands
    
    if (bHandInFrame || danceFrames.size() == 0){
        fboLocalProcessedHand.draw(0,0,320,240);
    }else {
        danceFrames[ofGetFrameNum()%danceFrames.size()].draw(0,0,320,240);
        cout << danceFrames.size() << ": " << ofGetFrameNum()%danceFrames.size() << endl;
    }
    
    imgRemoteProcessedHand.draw(320,0,320,240);
    
    // -----------------
    
    
    
    // Draw future art
    
    shaderInterplay.begin();
    
    
    if (bHandInFrame || danceFrames.size() == 0)
        shaderInterplay.setUniformTexture("pLocalProcessed", fboLocalProcessedHand.getTexture(), 1);
    else
        shaderInterplay.setUniformTexture("pLocalProcessed", danceFrames[ofGetFrameNum()%danceFrames.size()].getTexture(), 1);

    shaderInterplay.setUniformTexture("pRemote", imgRemoteProcessedHand.getTexture(), 2);
    ofPushMatrix();
    ofTranslate(WIDTH/2, HEIGHT);
    shaderPlane.draw();
    ofPopMatrix();
    
    shaderInterplay.end();
    
    // -----------------
    
    // Debug GUI
    if(bDebug)
        gui.draw();
}

void ofApp::loadShaders(){
    shaderProcessor.load("shaderProcessor");
    shaderInterplay.load("shaderInterplay");
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'r')
        bFirstFrame = true;
    else if(key == 'd')
        bDebug = !bDebug;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

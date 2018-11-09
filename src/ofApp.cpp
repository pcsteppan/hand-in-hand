#include "ofApp.h"
#include "networkCredentials.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // FRAMERATE
    ofSetFrameRate(FRAMERATE);
    
    // DEBUG GUI
    gui.setup();
    gui.add(backgroundSubtractionThreshold.setup("Background subtraction threshold", 0.02f, 0.0f, 1.0f));
    gui.add(handPresenceThreshold.setup("Hand presence threshold", 0.013f, 0.0f, 0.1f));
    
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
    
    imgRemoteProcessedHand.allocate(WIDTH, HEIGHT, OF_IMAGE_GRAYSCALE);
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
    
    // local vars
    unsigned char * luminanceBufferPtr;
    
    ofPixels pTemp;
    pTemp.allocate(640, 480, OF_IMAGE_GRAYSCALE);
    
    
    // fboLocalProcessedHand is RGB, but R, G, and B are the same value, 0-255
    fboLocalProcessedHand.readToPixels(pTemp);
    unsigned char * luminanceLiveBufferPtr = pTemp.getChannel('r').getData();
    
    // Sending a packet
    if(!bHandInFrame && danceFrames.size() > 0){
        luminanceBufferPtr = danceFrames[ofGetFrameNum() % danceFrames.size()].getPixels().getData();
    } else {
        luminanceBufferPtr = luminanceLiveBufferPtr;
    }
    
    if (!publisher.send(luminanceBufferPtr, 1 * 640 * 480 * sizeof(unsigned char)))
	{
		cout << "send failed" << endl;
    } else {
        // poll hand presence once every second
        if(ofGetFrameNum() % 30 == 0){
            if(isHandInFrame(luminanceLiveBufferPtr)){
                if(!bHandInFrame){
                    /*
                    for(int i = 0; i < danceFrames.size(); i++) {
                        delete[] danceFrames[i];
                    }
                     */
                    danceFrames.clear();
                }
                bHandInFrame = true;
            } else {
                bHandInFrame = false;
            }
        }
        
        if(bHandInFrame){
            //danceFrames.push_back(new unsigned char[640*480]);
            //memcpy(danceFrames[danceFrames.size()-1], luminanceLiveBufferPtr, 640*480);
            ofPixels p;
            p.allocate(640,480,1);
            p.setFromPixels(pTemp.getChannel('r').getData(), 640, 480, OF_IMAGE_GRAYSCALE);
            ofImage i;
            i.setFromPixels(p);
            i.update();
            danceFrames.push_back(ofImage(i));
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
    
    if ((bFirstFrame || bRefreshBackground) && grabberLocalRawHand.isFrameNew()){
        //fboBackground.clear();
        fboBackground.begin();
        grabberLocalRawHand.draw(0,0,640,480);
        fboBackground.end();
        bFirstFrame = false;
        bRefreshBackground = false;
    } //else if (!bFirstFrame) {
    
    // -----------------
    
    
    
    
    // Remove background from video grabber
    
    
    fboLocalProcessedHand.begin();
    ofClear(0);
    shaderProcessor.begin();
    
    shaderProcessor.setUniform1f("threshold", backgroundSubtractionThreshold);
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
    
    //ofTexture t;
    //t.allocate(640, 480, GL_LUMINANCE);
    //t.allocate(640,480,GL_R8);
    
    if (bHandInFrame || danceFrames.size() == 0){
        fboLocalProcessedHand.draw(0,0,320,240);
    }else {
        //t.loadData(danceFrames[ofGetFrameNum()%danceFrames.size()].getPixels().getData(), 640, 480, GL_LUMINANCE);
        //t.draw(0,0,320,240);
        danceFrames[ofGetFrameNum()%danceFrames.size()].update();
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
    
    if(bHandInFrame){
        ofPushMatrix();
        ofSetColor(0,255,255);
        ofDrawRectangle(0,0,10,10);
        ofPopMatrix();
    }
    //}
}

void ofApp::loadShaders(){
    shaderProcessor.load("shaderProcessor");
    shaderInterplay.load("shaderInterplay");
}

bool ofApp::isHandInFrame(unsigned char * p){
    int sum = 0;
    for (int i = 0; i < 640*480; i++) {
        sum += p[i];
    }
    
    
    
    const int HAND_THRESHOLD = (640*480*255*handPresenceThreshold);
    
    cout << "sum: " << sum << endl;
    cout << "threshold: " << HAND_THRESHOLD << endl;
    
    return sum > HAND_THRESHOLD;
    
    /*
    if(sum > HAND_THRESHOLD && !bHandInFrame){
        //pushback on vector
        //danceFrames.clear();
        //bHandInFrame = true;
        return true
    } else if(sum < HAND_THRESHOLD) {
        //clear vector
        //bRefreshBackground = true;
        bHandInFrame = false;
    }
    */
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'r')
        bRefreshBackground = true;
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

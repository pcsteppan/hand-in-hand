#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    // DEBUG GUI
    gui.setup();
    gui.add(threshold.setup("Background subtraction threshold", 0.5f, 0.0f, 1.0f));
    
    // ZMQ NETWORK
    publisher.bind("tcp://*:9998");
    subscriber.connect("tcp://192.168.1.186:9999");
    
    grabberLocalRawHand.setup(WIDTH,HEIGHT,GL_RGB);
    
    imgRemoteProcessedHand.allocate(WIDTH, HEIGHT, OF_IMAGE_COLOR);
    pixRemoteProcessedHand.allocate(WIDTH, HEIGHT, OF_IMAGE_GRAYSCALE);
    
    
    fboBackground.allocate(WIDTH, HEIGHT, GL_RGB);
    fboLocalProcessedHand.allocate(WIDTH, HEIGHT, GL_RGB);
    
    bufferTemp.allocate(3 * WIDTH * HEIGHT * sizeof(unsigned char));
    
    shaderPlane.set(WIDTH,HEIGHT,10,10);
    shaderPlane.mapTexCoords(0,0,WIDTH,HEIGHT);
    
    loadShaders();
}

//--------------------------------------------------------------
void ofApp::update(){
    grabberLocalRawHand.update();
    loadShaders();
    
    while (subscriber.hasWaitingMessage())
	{
		bufferTemp.clear();
		subscriber.getNextMessage(bufferTemp);
        
		pixRemoteProcessedHand.setFromPixels((unsigned char*) bufferTemp.getData(), 640, 480, OF_IMAGE_GRAYSCALE);
        imgRemoteProcessedHand.setFromPixels(pixRemoteProcessedHand);
        imgRemoteProcessedHand.update();
    }
    
    ofPixels pix;
    fboLocalProcessedHand.readToPixels(pix);
    if (!publisher.send(pix.getChannel('r').getData(), 1 * 640 * 480 * sizeof(unsigned char)))
	{
		cout << "send failed" << endl;
    }
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
    
    
    
    // Draw debug hands
    
    fboLocalProcessedHand.draw(0,0,320,240);
    imgRemoteProcessedHand.draw(320,0,320,240);
    
    // -----------------
    
    
    
    // Draw future art
    
    shaderInterplay.begin();
    
    shaderInterplay.setUniformTexture("pLocalProcessed", fboLocalProcessedHand.getTexture(), 1);
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

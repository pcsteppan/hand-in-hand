#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    publisher.bind("tcp://*:9998");
    subscriber.connect("tcp://192.168.1.186:9999");
    
    grabber.setup(640,480);
    
    myImage.allocate(640, 480, OF_IMAGE_COLOR);
    myPixels.allocate(640, 480, OF_IMAGE_COLOR);
    bg.allocate(640,480, GL_RGB);

    buffer.allocate(3 * 640 * 480 * sizeof(unsigned char));
    
    shaderPlane.set(640,480,10,10);
    shaderPlane.mapTexCoords(0,0,640,480);
    
    shader.load("shader");
}

//--------------------------------------------------------------
void ofApp::update(){
    grabber.update();
    shader.load("shader");
    
    while (subscriber.hasWaitingMessage())
	{
		buffer.clear();
		subscriber.getNextMessage(buffer);
        
		myPixels.setFromPixels((unsigned char*) buffer.getData(), 640, 480, OF_IMAGE_COLOR);
        myImage.setFromPixels(myPixels);
        myImage.update();
    }
    
    if (!publisher.send(grabber.getPixels().getData(), 3 * 640 * 480 * sizeof(unsigned char)))
	{
		cout << "send failed" << endl;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    if (bFirstFrame){
        bg.begin();
        grabber.draw(0,0,640,480);
        bg.end();
    }
    
    grabber.draw(0,0,320,240);
    myImage.draw(320,0,320,240);
    
    shader.begin();
    shader.setUniformTexture("bg", bg.getTexture(), 1);
    shader.setUniformTexture("localHand", grabber.getTexture(),  2);
    shader.setUniformTexture("remoteHand", myImage.getTexture(), 3);
    
    ofTranslate(320, 480);
    shaderPlane.draw();
    
    shader.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
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

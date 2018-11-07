#pragma once

#include "ofMain.h"
#include "ofxZmq.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
        void loadShaders();
    
    // DEBUG GUI
    ofxFloatSlider threshold;
    ofxPanel gui;
    
    // ZMQ NETWORK
    ofxZmqSubscriber subscriber;
    ofxZmqPublisher publisher;
    
    
    // VIDEO PROCESSING
    ofImage imgRemoteProcessedHand;
    ofPixels pixRemoteProcessedHand;
    
    ofBuffer bufferTemp;

    ofVideoGrabber grabberLocalRawHand;
    ofFbo fboLocalProcessedHand;
    
    ofFbo fboBackground;
    
    // SHADERS (AND SHADER ACCESSORIES)
    ofShader shaderProcessor;
    ofShader shaderInterplay;
    
    ofPlanePrimitive shaderPlane;
    
    
    // FLAGS
    bool bFirstFrame = true;
    bool bDebug = true;
    
    // CONSTANTS
    const int WIDTH = 640;
    const int HEIGHT = 480;
};

#pragma once

#include "ofMain.h"
#include "ofxZmq.h"
#include "ofxGui.h"
#include "ofxNetwork.h"

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
        bool isHandInFrame(unsigned char * p);
    
    // DEBUG GUI
    ofxFloatSlider backgroundSubtractionThreshold;
    ofxFloatSlider handPresenceThreshold;
    ofxPanel gui;
    
    // ZMQ NETWORK
    ofxZmqSubscriber subscriber;
    ofxZmqPublisher publisher;
    
    // UDP NETWORK
    //ofxUDPManager udpSubscriber;
    //ofxUDPManager udpPublisher;
    
    
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
    
    // FRAMES
    //vector<unsigned char *> danceFrames;
    vector<ofImage> danceFrames;
    
    // FLAGS
    bool bFirstFrame = true;
    bool bRefreshBackground = false;
    bool bDebug = true;
    bool bHandInFrame = false;
    
    // CONSTANTS
    const int WIDTH = 640;
    const int HEIGHT = 480;
    const int MAX_PERFORMANCE_LENGTH = 10;
    const int FRAMERATE = 30;
};

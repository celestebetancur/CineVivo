#pragma once

#include "ofMain.h"
#include "ofxGUI.hpp"
#include "ofxSyphon.h"

#define PREVIEWS 16
#define WIDTH 1280
#define HEIGHT 800

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void setupGui();
        void drawGui(ofEventArgs & args);
        void testGui(ofEventArgs & args);
        void setupGuiB();
        void drawGuiB(ofEventArgs & args);
        void setupOut();
        void drawOut(ofEventArgs & args);
        void setupPreview();
        void drawPreview(ofEventArgs & args);

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

    //Syphon
    ofxSyphonServer server;
    
    //Building
    ofImage back;

    ofTrueTypeFont font;
    
    //Load default
    ofDirectory videos;
    
    //Preview activate
    int currentA = 0;
    int currentB = 1;
    //ofxGUI buttons[PREVIEWS];

    bool test = false;
    
    float posX = 30;
    float posY = 250;
    
    //Sliders
    ofxGUI slider[4];
    ofxGUI sliderB[4];
    shared_ptr<ofAppBaseWindow> mapping;
    
    //Buttons
    ofxGUI but1A, but2A, but3A;
    ofxGUI but1B, but2B, but3B;
    
    //Color Prev
    ofColor colorPrevA;
    ofColor colorPrevB;
    
    string FILE;
    
    //Mapping
    
    ofVideoPlayer video, video1;
    ofPixels pixels, pixels1, r,g,b,a, R, G, B, A;
    ofTexture texture, texture1;
    
    ofPixels Pixels[1280], Pixels1[1280];
    ofTexture textures[1280], textures1[1280];
    
    int W, H, W1, H1, distance, modul, state, div, div1, divOn, div1On;
    int multiply, mstate;
    int toMove, toMove1;
    int move[16], move1[16];
    int up[16], up1[16];
        
    float scaleX, scaleY;
    
    ofxGUI butDiv[16], butMov[16];
    ofxGUI butDiv1[16], butMov1[16];

    //------------------
    float wi;
    float WI;
    float he;
    float HE;

};

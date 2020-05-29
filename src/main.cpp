#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGlutWindow.h"
#include "ofxOsc.h"

//========================================================================
int main( ){
    
    ofSetupOpenGL(640, 480, OF_WINDOW);
    ofRunApp(new ofApp());
}

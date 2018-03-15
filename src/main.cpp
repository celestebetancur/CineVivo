#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGlutWindow.h"
#include "ofxOsc.h"

//========================================================================
int main( ){
    
    ofAppGlutWindow window;
    /*//ofSetupOpenGL(&window,640,480, OF_WINDOW);			// <-------- setup the GL context
    
    // this kicks off the running of my app
    // can be OF_WINDOW or OF_FULLSCREEN
    // pass in width and height too:
    
    
    ofRunApp( new ofApp());*/
    
    ofGLFWWindowSettings settings;

    settings.width = 300;
    settings.height = 200;
    settings.setPosition(ofVec2f(450,0));
    settings.multiMonitorFullScreen = false;
    settings.resizable = true;
    settings.monitor = 0;
    settings.windowMode = OF_WINDOW;
    shared_ptr<ofAppBaseWindow> guiWindow = ofCreateWindow(settings);

   
    settings.width = 200;
    settings.height = 200;
    settings.setPosition(ofVec2f(0,0));
    settings.multiMonitorFullScreen = true;
    settings.resizable = true;
    settings.monitor = 1; //multimonitor
    settings.windowMode = OF_WINDOW;
    //settings.monitor = 0; //single monitor
    //settings.windowMode = OF_WINDOW;
    shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings);
    
    shared_ptr<GuiApp> guiApp(new GuiApp);
    shared_ptr<ofApp> mainApp(new ofApp);

    mainApp->gui = guiApp;

    ofRunApp(mainWindow, mainApp);
    ofRunApp(guiWindow, guiApp);
    
    ofRunMainLoop();
}

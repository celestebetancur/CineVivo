#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){

    ofGLFWWindowSettings settings;

    // MAIN WINDOW
    //settings.width = 100;
    //settings.height = 100;
    settings.setPosition(ofVec2f(WIDTH * 0.323,0));
    settings.resizable = true;
    shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings);

    //PREVIEW A
    settings.width = WIDTH * 0.23;
    settings.height = HEIGHT * 0.28;
    settings.setPosition(ofVec2f(0,0));
    settings.resizable = false;
    settings.shareContextWith = mainWindow;
    shared_ptr<ofAppBaseWindow> guiWindow = ofCreateWindow(settings);
    guiWindow->setVerticalSync(false);
    
    shared_ptr<ofApp> mainApp(new ofApp);
    mainApp->setupGui();
    ofAddListener(guiWindow->events().draw,mainApp.get(),&ofApp::drawGui);
    ofAddListener(guiWindow->events().draw,mainApp.get(),&ofApp::testGui);
    
    //PREVIEW B
    settings.width = WIDTH * 0.23;
    settings.height = HEIGHT * 0.29;
    settings.setPosition(ofVec2f(0,HEIGHT * .33));
    settings.resizable = false;
    settings.shareContextWith = mainWindow;
    shared_ptr<ofAppBaseWindow> bWindow = ofCreateWindow(settings);
    bWindow->setVerticalSync(false);
    
    mainApp->setupGuiB();
    ofAddListener(bWindow->events().draw,mainApp.get(),&ofApp::drawGuiB);
    
    //OUTPUT
    settings.width = WIDTH * 0.23;
    settings.height = HEIGHT * 0.28;
    settings.setPosition(ofVec2f(0,HEIGHT * .66));
    settings.resizable = true;
    settings.shareContextWith = mainWindow;
    shared_ptr<ofAppBaseWindow> outWindow = ofCreateWindow(settings);
    outWindow->setVerticalSync(false);
    
    mainApp->setupOut();
    ofAddListener(outWindow->events().draw,mainApp.get(),&ofApp::drawOut);

    //PREVIEW
    /*settings.width = WIDTH * 0.8;
    settings.height = HEIGHT * 0.25;
    settings.setPosition(ofVec2f(0,0));
    settings.resizable = false;
    settings.shareContextWith = mainWindow;
    shared_ptr<ofAppBaseWindow> prevWindow = ofCreateWindow(settings);
    prevWindow->setVerticalSync(false);
    
    mainApp->setupPreview();
    ofAddListener(prevWindow->events().draw,mainApp.get(),&ofApp::drawPreview);*/
    
    ofRunApp(mainWindow, mainApp);
    ofRunMainLoop();
}

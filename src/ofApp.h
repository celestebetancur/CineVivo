#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxXmlSettings.h"
#include "ofxFilterLibrary.h"
#include "ofxAssimpModelLoader.h"

#if (defined(__APPLE__) && defined(__MACH__))
    #include "ofxSyphon.h"
#endif

#define LIM 10
#define CAMERAS 2 // Change this if you want to use more than 2 videocameras

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    void exit();

    void timmer(float time);

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

    void chroma(ofPixels *src, ofTexture *texture, ofColor colorChroma);
    void chromaMask(ofPixels *src1, ofPixels *src2, ofTexture *texture, ofColor colorChroma);
    void blend(ofPixels *src1, ofPixels *src2, ofTexture *texture);

    void executeScriptEvent(string getText, bool verbose);
	  void tidalOSCincoming(ofxOscMessage tidal);
    void tidalOSCNewSpec(ofxOscMessage tidal);

    //OSC
    ofxXmlSettings XML;
    ofxOscReceiver reciever;
    ofxOscSender sender;
    ofxOscSender osc;
    int portIn;
    int portOut;
    bool verboseOSC;
    bool verbose;

    float wRate;
    float hRate;
    int WIDTH;
    int HEIGHT;

    // CineVivo LiveCoding
    ofPoint one[LIM],two[LIM],three[LIM],four[LIM];
    ofTexture texVideo[LIM];
    ofPixels pix[LIM];

    bool fullScreen = false;
    bool vExt = false;
    ofColor backgroundColor;
    bool backgroundAuto;
    int windowWidth = 640;
    int windowHeight = 480;

    ofVideoGrabber cam[CAMERAS];
    int deviceNUM;
    bool camON[LIM];
    int deviceID[LIM];

    //ofxHapPlayer videoLC[LIM]; //testing
    const string videoFormats[3] = {".mp4",".mov",".mkv"};
    ofVideoPlayer videoLC[LIM];
    ofImage pictures[LIM];
    ofSoundPlayer audio[20];

    string prevVideo[LIM];
    int worldCenterX[LIM];
    int worldCenterY[LIM];
    int rectMode[LIM];
    float rotationSpeedX[LIM];
    float rotationSpeedY[LIM];
    float rotationSpeedZ[LIM];
    int vX[LIM];
    int vY[LIM];
    int vIndex[LIM];
    int vIndexPlaying[LIM];
    ofImage mask[LIM];
    int maskedIndex[LIM];
    float vScaleX[LIM];
    float vScaleY[LIM];
    ofColor vColor[LIM];
    int vRedChann[LIM];
    int vGreenChann[LIM];
    int vBlueChann[LIM];
    int vOpacity[LIM];
    float vVolume[LIM];
    float vRotX [LIM];
    float vRotY [LIM];
    float vRotZ [LIM];
    float vSpeed [LIM];
    int vTotalFrames [LIM];
    int vInitPoint [LIM];
    int vEndPoint [LIM];
    float vW [LIM];
    float vH [LIM];
    bool imageON[LIM];
    ofLoopType vLoopState[LIM];

    //glsl shader
    ofShader shader[LIM];
    bool shaderOn[LIM];
    string shaderName[LIM];
    ofFbo fbo[LIM];

    int ambientLight;

    // utilities
    int numVideosLoaded;

    // filters
    int currentFilter[LIM];
    vector<AbstractFilter *> filters[LIM];
    bool shaderLoaded[LIM];
    bool filterOn[LIM];
    //blur
    int blurAmount[LIM];
    bool blur[LIM];
    vector<AbstractFilter *> filtBlur[LIM];
    //blend and chroma
    bool vChroma[LIM];
    ofColor vColorChroma[LIM];
    bool vBlend[LIM];
    int vToBlend[LIM][2];
    ofImage imageBlend;

    //Draw effects
    bool vFeedback[LIM];

    ofxAssimpModelLoader models3D[LIM];
    bool model3DOn[LIM];
    ofLight light;
    ofLight lightC;

    //Simple Text Editor
    ofTrueTypeFont font;
    int fontSize;
    string textToExe;
    bool t_visible;
    std::vector<string> lineas;
    std::vector<string> texto;
    string digit[10] = {"0","1","2","3","4","5","6","7","8","9"};
    ofxXmlSettings xmlVariables;
    ofColor fontColor;
    bool cursor;
    int numLines = 0;

//-----TIDAL----------------------------------------------------------

    //Timer
    float time = 0;
    int referenceCycle = 0;

	  float _tCps = 0.562;
	  float _tCycle = 0;
	  float _tDelta = 0.889;
    float _tPreviousCycle = -1.0f;
    string _tTemp;
    int _tCpsToCV = 562;
    bool _tScheduler = false;

//--------------------------------------------------------------------

#if (defined(__APPLE__) && defined(__MACH__))
    // syphon
    ofxSyphonServerDirectory syphonDir;
    ofxSyphonClient syphonClient[LIM];
    bool syphonON[LIM];
    int syphonDirId[LIM];

    bool s_superAsModifier = true;

#else
    bool s_superAsModifier = false;
#endif
};

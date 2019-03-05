#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetEscapeQuitsApp(false);
    ofSetWindowShape(640, 480);
    ofSetWindowTitle("CineVivo");
    ofSetVerticalSync(true);

    XML.load ("xml/OSCConf.xml");
    
    //----------------------OSC setup -------------------------
    
    portOut = XML.getValue("PORT:NAME:OUT",5613);
    sender.setup("127.0.0.1",portOut);
    //OSC in
    portIn = XML.getValue("PORT:NAME:IN",5612);
    reciever.setup(portIn);
    osc.setup("127.0.0.1",5612);
    
    //--------------------filters setup -----------------------

    ambientLight = 155;
    ofDisableArbTex();
    ofEnableSmoothing();
    ofEnableAlphaBlending();
    lightC.setAreaLight(200, 200);
    ofSetGlobalAmbientColor(ambientLight);

    //-----------------------------------------------------------

    fontSize = 14;
    font.load("fonts/font.ttf", fontSize, true, true);
    t_visible = true;
    fontColor = ofColor(0,235,0);
    cursor = true;
    // code poetry ;)
    xmlVariables.load ("xml/variables.xml");
    digit[0] = xmlVariables.getValue("VAR:NAME:CERO","0");
    digit[1] = xmlVariables.getValue("VAR:NAME:UNO","1");
    digit[2] = xmlVariables.getValue("VAR:NAME:DOS","2");
    digit[3] = xmlVariables.getValue("VAR:NAME:TRES","3");
    digit[4] = xmlVariables.getValue("VAR:NAME:CUATRO","4");
    digit[5] = xmlVariables.getValue("VAR:NAME:CINCO","5");
    digit[6] = xmlVariables.getValue("VAR:NAME:SEIS","6");
    digit[7] = xmlVariables.getValue("VAR:NAME:SIETE","7");
    digit[8] = xmlVariables.getValue("VAR:NAME:OCHO","8");
    digit[9] = xmlVariables.getValue("VAR:NAME:NUEVE","9");
    
    backgroundAuto = true;
    numVideosLoaded = 0;
    backgroundColor = ofColor(0,0,0);

    for(int i = 0; i < LIM; i++){
        worldCenterX[i] = 0;
        worldCenterY[i] = 0;
        vX[i] = 0;
        vY[i] = 0;
        vIndex[i] = 0;
        vIndexPlaying[i] = 0;
        vColor[i] = ofColor(255);
        vScaleX[i] = 1;
        vScaleY[i] = 1;
        vOpacity[i] = 255;
        vRotX[i] = 0;
        vRotY[i] = 0;
        vRotZ[i] = 0;
        vSpeed[i] = 1;
        camON[i] = false;
        deviceID[i] = -1;
        one[i].set(0,0);
        videoLC[i].setPixelFormat(OF_PIXELS_RGBA);
        videoLC[i].load("/videos/fluido.mov"); //init video layers for windows compatibility
        pix[i].allocate(WIDTH, HEIGHT, OF_PIXELS_RGBA);
        texVideo[i].allocate(pix[i]);
        mask[i].allocate(WIDTH, HEIGHT, OF_IMAGE_COLOR_ALPHA);
        pictures[i].allocate(WIDTH, HEIGHT, OF_IMAGE_COLOR);
        rectMode[i] = 0;
        maskedIndex[i] = 0;
        currentFilter[i] = 0;
        filterOn[i] = false;
        shaderLoaded[i] = false;
        blur[i] = false;
        blurAmount[i] = 0;
        vBlend[i] = false;
        vChroma[i] = false;
        vColorChroma[i] = ofColor(0);
        vLoopState[i] = OF_LOOP_NORMAL;
        imageON[i]  = false;
        
        fbo[i].allocate(WIDTH, HEIGHT, GL_RGBA);
        fbo[i].begin();
            ofClear(0, 0, 0, 0);
        fbo[i].end();
        shaderOn[i] = false;
        
        vFeedback[i] = false;
        
#if (defined(__APPLE__) && defined(__MACH__))
        //-------------------Syphon--------------------------------
        
        syphonDir.setup();
        syphonClient[i].setup();
        syphonDirId[i] = -1;
        syphonON[i] = false;
#endif
    }
    deviceNUM = cam[0].listDevices().size();
    for(int i = 0; i < deviceNUM; i++){
        cam[i].setDeviceID(i);
        cam[i].initGrabber(640,480);
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    
    while (reciever.hasWaitingMessages()){

        ofxOscMessage m;
        reciever.getNextMessage(m);

        for(int i  = 0; i < m.getNumArgs(); i++){
            if (m.getAddress() == "/fileExtension"  &&  m.getNumArgs() == 1){
                vExt = m.getArgAsInt(0);
            }
            if (m.getAddress() == "/backgroundAuto"  &&  m.getNumArgs() == 1){
                backgroundAuto = m.getArgAsInt(0);
            }
            if (m.getAddress() == "/windowShape"  &&  m.getNumArgs() == 2){
                ofSetWindowShape(m.getArgAsInt(0), m.getArgAsInt(1));
            }
            if (m.getAddress() == "/backColor" && m.getNumArgs() == 3){
                backgroundColor = ofColor(m.getArgAsInt(0),m.getArgAsInt(1),m.getArgAsInt(2));
            }
            if (m.getAddress() == "/backColor" && m.getNumArgs() == 1){
                backgroundColor = ofColor(m.getArgAsInt(0));
            }
            if (m.getAddress() == "/fullscreen" &&  m.getNumArgs() == 1){
                if(m.getArgAsInt(0) == 1)
                    fullScreen = true;
                if(m.getArgAsInt(0) == 0)
                    fullScreen = false;
            }
            if (m.getAddress() == "/clean"){
                numVideosLoaded = 0;
                for(int i = 0; i < LIM; i++){
                    deviceID[i] = -1;
                    videoLC[i].stop();
                    videoLC[i].close();
                    vIndex[i] = 0;
                    vIndexPlaying[i] = false;
#if (defined(__APPLE__) && defined(__MACH__))
                    syphonON[i] = false;
#endif
                    camON[i] = false;
                    model3DOn[i] = false;
                    vX[i] = 0;
                    vY[i] = 0;
                    vRotX[i] = 0;
                    vRotY[i] = 0;
                    vRotZ[i] = 0;
                    vW[i] = ofGetWidth();
                    vH[i] = ofGetHeight();
                    vSpeed[i] = 1;
                    vOpacity[i] = 255;
                    vColor[i] = ofColor(255);
                    pix[i].clear();
                    texVideo[i].clear();
                    pix[i].allocate(WIDTH, HEIGHT, OF_PIXELS_RGBA);
                    texVideo[i].allocate(pix[i]);
                    maskedIndex[i] = 0;
                    shaderLoaded[i] = false;
                    filters[i].clear();
                    shaderOn[i] = false;
                    filterOn[i] = false;
                    vBlend[i] = false;
                    vChroma[i] = false;
                    vColorChroma[i] = ofColor(0);
                    fbo[i].begin();
                    ofClear(0, 0, 0, 0);
                    fbo[i].end();
                    vFeedback[i] = false;
                    imageON[i] = false;
                    pictures[i].clear();
                }
            }
            if (m.getAddress() == "/playAll"){
                for(int i = 0; i < LIM; i++){
                    if(videoLC[i].isLoaded()){
                        vIndexPlaying[i] = 1;
                        videoLC[i].play();
                    }
                }
            }
            if (m.getAddress() == "/stopAll"){
                for(int i = 0; i < LIM; i++){
                    if(vIndexPlaying[i] == 1){
                        vIndexPlaying[i] = 0;
                        videoLC[i].stop();
                    }
                }
            }
            if (m.getAddress() == "/pauseAll"){
                for(int i = 0; i < LIM; i++){
                    if(vIndexPlaying[i] == 1){
                        vIndexPlaying[i] = 0;
                        videoLC[i].setPaused(true);
                    }
                }
            }
            if(m.getArgAsInt(0) >= 0 &&  m.getArgAsInt(0) < LIM){
                if (m.getAddress() == "/load"  &&  (m.getNumArgs() == 2 || m.getNumArgs() == 3)){
                   if (m.getArgAsString(1) == XML.getValue("WORDS:NAME:CAMERA","camera")){
                        int n = m.getArgAsInt(0);
                        int device = m.getArgAsInt(2);
                        if(m.getNumArgs() == 3 && device < deviceNUM){
                          camON[n] = true;
                          deviceID[n] = device;
                        } else{
                          camON[n] = true;
                          deviceID[n] = 0;
                        }
                        if(cam[deviceID[n]].isInitialized()){
                            vIndex[n] = 0;
#if (defined(__APPLE__) && defined(__MACH__))
                            syphonON[n] = false;
#endif
                            vIndexPlaying[n] = false;
                            model3DOn[n] = false;
                            vW[n] = ofGetWidth();
                            vH[n] = ofGetHeight();
                            four[n].set(ofPoint(0,vH[n]));
                            three[n].set(ofPoint( vW[n] ,vH[n]));
                            two[n].set(ofPoint(vW[n],0));

                            if(shaderLoaded[n] == false){
                                
                                FilterChain * charcoal = new FilterChain(vW[n], vH[n], "Charcoal");
                                charcoal->addFilter(new GaussianBlurFilter(vW[n], vH[n], 2.f ));
                                charcoal->addFilter(new DoGFilter(vW[n], vH[n], 12, 1.2, 8, 0.99, 4));
                                filters[n].push_back(charcoal);
                                
                                // Basic filter examples
                                
                                filters[n].push_back(new HalftoneFilter(vW[n], vH[n], 0.01));
                                filters[n].push_back(new CrosshatchFilter(vW[n], vH[n]));
                                filters[n].push_back(new KuwaharaFilter(6));
                                filters[n].push_back(new SobelEdgeDetectionFilter(vW[n], vH[n]));
                                filters[n].push_back(new BilateralFilter(vW[n], vH[n]));
                                filters[n].push_back(new SketchFilter(vW[n], vH[n]));
                                filters[n].push_back(new DilationFilter(vW[n],vH[n]));
                                filters[n].push_back(new PerlinPixellationFilter(vW[n], vH[n]));
                                filters[n].push_back(new XYDerivativeFilter(vW[n], vH[n]));
                                filters[n].push_back(new ZoomBlurFilter());
                                filters[n].push_back(new EmbossFilter(vW[n], vH[n], 2.f));
                                filters[n].push_back(new SmoothToonFilter(vW[n], vH[n]));
                                filters[n].push_back(new TiltShiftFilter(cam[deviceID[n]].getTexture()));
                                filters[n].push_back(new VoronoiFilter(cam[deviceID[n]].getTexture()));
                                filters[n].push_back(new CGAColorspaceFilter());
                                filters[n].push_back(new ErosionFilter(vW[n], vH[n]));
                                filters[n].push_back(new LookupFilter(vW[n], vH[n], "img/lookup_amatorka.png"));
                                filters[n].push_back(new LookupFilter(vW[n],vH[n], "img/lookup_miss_etikate.png"));
                                filters[n].push_back(new LookupFilter(vW[n], vH[n], "img/lookup_soft_elegance_1.png"));
                                filters[n].push_back(new VignetteFilter());
                                filters[n].push_back(new PosterizeFilter(8));
                                filters[n].push_back(new LaplacianFilter(vW[n], vH[n], ofVec2f(1, 1)));
                                filters[n].push_back(new PixelateFilter(vW[n], vH[n]));
                                filters[n].push_back(new HarrisCornerDetectionFilter(cam[deviceID[n]].getTexture()));
                                filters[n].push_back(new MotionDetectionFilter(cam[deviceID[n]].getTexture()));
                                filters[n].push_back(new LowPassFilter(vW[n], vH[n], 0.9));
                                
                                // blending examples
                                
                                ofImage wes = ofImage("img/wes.jpg");
                                ChromaKeyBlendFilter * chroma = new ChromaKeyBlendFilter(ofVec3f(0.f, 1.f, 0.f), 0.4);
                                chroma->setSecondTexture(wes.getTexture());
                                filters[n].push_back(chroma);
                                
                                filters[n].push_back(new DisplacementFilter("img/mandel.jpg", vW[n], vH[n], 25.f));
                                filters[n].push_back(new PoissonBlendFilter(wes.getTexture(), vW[n],vH[n], 2.0));
                                filters[n].push_back(new DisplacementFilter("img/glass/3.jpg", vW[n], vH[n], 40.0));
                                filters[n].push_back(new ExclusionBlendFilter(wes.getTexture()));
                                
                                // Convolution filters
                                
                                Abstract3x3ConvolutionFilter * convolutionFilter1 = new Abstract3x3ConvolutionFilter(vW[n], vH[n]);
                                convolutionFilter1->setMatrix(-1, 0, 1, -2, 0, 2, -1, 0, 1);
                                filters[n].push_back(convolutionFilter1);
                                
                                Abstract3x3ConvolutionFilter * convolutionFilter2 = new Abstract3x3ConvolutionFilter(vW[n], vH[n]);
                                convolutionFilter2->setMatrix(4, 4, 4, 4, -32, 4, 4,  4, 4);
                                filters[n].push_back(convolutionFilter2);
                                
                                Abstract3x3ConvolutionFilter * convolutionFilter3 = new Abstract3x3ConvolutionFilter(vW[n], vH[n]);
                                convolutionFilter3->setMatrix(1.2,  1.2, 1.2, 1.2, -9.0, 1.2, 1.2,  1.2, 1.2);
                                filters[n].push_back(convolutionFilter3);
                                
                                //  daisy-chain a bunch of filters
                                
                                FilterChain * foggyTexturedGlassChain = new FilterChain(vW[n], vH[n], "Weird Glass");
                                foggyTexturedGlassChain->addFilter(new PerlinPixellationFilter(vW[n], vH[n], 13.f));
                                foggyTexturedGlassChain->addFilter(new EmbossFilter(vW[n], vH[n], 0.5));
                                foggyTexturedGlassChain->addFilter(new GaussianBlurFilter(vW[n], vH[n], 3.f));
                                filters[n].push_back(foggyTexturedGlassChain);
                                
                                // unimaginative filter chain
                                
                                FilterChain * watercolorChain = new FilterChain(vW[n], vH[n], "Monet");
                                watercolorChain->addFilter(new KuwaharaFilter(9));
                                watercolorChain->addFilter(new LookupFilter(vW[n], vH[n], "img/lookup_miss_etikate.png"));
                                watercolorChain->addFilter(new BilateralFilter(vW[n], vH[n]));
                                watercolorChain->addFilter(new PoissonBlendFilter("img/canvas_texture.jpg",vW[n], vH[n], 2.0));
                                watercolorChain->addFilter(new VignetteFilter());
                                filters[n].push_back(watercolorChain);
                                
                                //  random gradient map for posterity
                                
                                vector<GradientMapColorPoint> colors;
                                for (float percent=0.0; percent<=1.0; percent+= 0.1)
                                    colors.push_back( GradientMapColorPoint(ofRandomuf(),ofRandomuf(),ofRandomuf(),percent) );
                                filters[n].push_back(new GradientMapFilter(colors));
                                
                                shaderLoaded[n] = true;
                            }
                        }
                    } else {
                        string temp;
                        if(vExt == 1){
                            temp = "videos/" + m.getArgAsString(1);
                            ofLogNotice() << temp;
                        }
                        if(vExt == 0) {
                            temp = "videos/" + m.getArgAsString(1) + ".mov";
                            ofLogNotice() << temp;
                        }
                        int n = m.getArgAsInt(0);
                        videoLC[n].load(temp);
                        videoLC[n].play();
                        if(videoLC[n].isLoaded()){
                            camON[n] = false;
#if (defined(__APPLE__) && defined(__MACH__))
                            syphonON[n] = false;
#endif
                            model3DOn[n] = false;
                            vIndex[n] = 1;
                            vIndexPlaying[n] = true;
                            vW[n] = videoLC[n].getWidth();
                            vH[n] = videoLC[n].getHeight();
                            four[n].set(ofPoint(0,vH[n]));
                            three[n].set(ofPoint( vW[n] ,vH[n]));
                            two[n].set(ofPoint(vW[n],0));
                        }
                    }
                }
                if (m.getAddress() == "/random"  &&  m.getNumArgs() == 1){
                    ofDirectory dir;
                    dir.listDir("videos");
                    string temp = dir.getPath(int(ofRandom(0,dir.size())));
                    int n = m.getArgAsInt(0);
                    if(videoLC[n].isLoaded()){
                        videoLC[n].close();
                    }
                    videoLC[n].load(temp);
                    if(videoLC[n].isLoaded()){
                        videoLC[n].play();
                        camON[n] = false;
#if (defined(__APPLE__) && defined(__MACH__))
                        syphonON[n] = false;
#endif
                        model3DOn[n] = false;
                        vIndex[n] = 1;
                        vIndexPlaying[n] = true;
                        vW[n] = videoLC[n].getWidth();
                        vH[n] = videoLC[n].getHeight();
                        four[n].set(ofPoint(0,vH[n]));
                        three[n].set(ofPoint( vW[n] ,vH[n]));
                        two[n].set(ofPoint(vW[n],0));
                    }
                }
                if (m.getAddress() == "/worldCenter"  &&  m.getNumArgs() == 3){
                    worldCenterX[m.getArgAsInt(0)] = m.getArgAsInt(1);
                    worldCenterY[m.getArgAsInt(0)] = m.getArgAsInt(2);
                }
                if (m.getAddress() == "/free"  &&  m.getNumArgs() == 1){
                    int n = m.getArgAsInt(0);
                    camON[n] = false;
                    model3DOn[n] = false;
                    videoLC[n].stop();
                    videoLC[n].close();
                    vIndex[n] = 0;
                    vX[n] = 0;
                    vY[n] = 0;
                    vRotX[n] = 0;
                    vRotY[n] = 0;
                    vRotZ[n] = 0;
                    vW[n] = ofGetWidth();
                    vH[n] = ofGetHeight();
                    vSpeed[n] = 1;
                    vOpacity[n] = 255;
                    vColor[n] = ofColor(255,255,255);
                    maskedIndex[n] = 0;
                    filterOn[n] = false;
                    shaderLoaded[n] = false;
                    filters[n].clear();
                    vBlend[n] = false;
                    vChroma[n] = false;
                    vColorChroma[n] = ofColor(0);
                    fbo[n].allocate(WIDTH, HEIGHT, GL_RGBA);
                    fbo[n].begin();
                    ofClear(0, 0, 0, 0);
                    fbo[n].end();
                    shaderOn[n] = false;
                    vFeedback[n] = false;
                    imageON[n] = false;
                    pictures[n].clear();
                }
                if (m.getAddress() == "/loadAudio"  &&  m.getNumArgs() == 2){
                  int n = m.getArgAsInt(0);
                  if(vExt == false){
                    audio[n].load("audio/"+m.getArgAsString(1)+".mp3");
                    audio[n].play();
                  } else {
                    audio[n].load("audio/"+m.getArgAsString(1));
                    audio[n].play();
                  }
                }
                if (m.getAddress() == "/volumeAudio"  &&  m.getNumArgs() == 2){
                    audio[m.getArgAsInt(0)].setVolume(m.getArgAsFloat(1));
                }
                if (m.getAddress() == "/load3D"  &&  m.getNumArgs() == 2){
                    int n = m.getArgAsInt(0);
                    camON[n] = false;
                    videoLC[n].stop();
                    vIndex[n] = 0;
                    vX[n] = 0;
                    vY[n] = 0;
                    vRotX[n] = 0;
                    vRotY[n] = 0;
                    vRotZ[n] = 0;
                    vW[n] = videoLC[n].getWidth();
                    vH[n] = videoLC[n].getHeight();
                    vSpeed[n] = 1;
                    vOpacity[n] = 255;
                    vColor[n] = ofColor(255,255,255);
                    maskedIndex[n] = 0;
                    shaderLoaded[n] = false;
                    filters[n].clear();
                    string temp = "models/" + m.getArgAsString(1);
                    model3DOn[n] = true;
                    models3D[n].loadModel(temp,false);
                    models3D[n].setPosition(ofGetWidth()*0.5,ofGetHeight()*0.5,0);
                    models3D[n].setLoopStateForAllAnimations(OF_LOOP_NORMAL);
                    models3D[n].playAllAnimations();
                }
                if (m.getAddress() == "/mask"  &&  m.getNumArgs() == 2){
                    maskedIndex[m.getArgAsInt(0)] = 1;
                    if(vExt == true){
                      if(ofFile::doesFileExist("mask/"+m.getArgAsString(1)+".png")){
                        mask[i].load("mask/"+m.getArgAsString(1));
                        vChroma[i] = true;
                      }
                    }
                    if(vExt == false){
                      if(ofFile::doesFileExist("mask/"+m.getArgAsString(1)+".png")){
                        if(mask[i].load("mask/"+m.getArgAsString(1)+".png")){
                        vChroma[i] = true;
                      }
                    }
                  }
                }
                if (m.getAddress() == "/image"  &&  m.getNumArgs() == 2){
                    int n = m.getArgAsInt(0);
                    string temp = "images/"+m.getArgAsString(1);
                    bool exist = ofFile::doesFileExist("images/"+m.getArgAsString(1)+".jpg");
                    if(vExt == true){
                        if(exist){
                            pictures[n].load(temp);
                            ofLog() << "imagen";
                        }
                    }
                    if(vExt == false){
                        if(exist){
                            pictures[n].load(temp+".jpg");
                        }
                    }
                    if(exist){
                        imageON[n] = true;
                        vW[n] = pictures[n].getWidth();
                        vH[n] = pictures[n].getHeight();
                        four[n].set(ofPoint(0,vH[n]));
                        three[n].set(ofPoint( vW[n] ,vH[n]));
                        two[n].set(ofPoint(vW[n],0));
                    }
                }
                if (m.getAddress() == "/rectMode" && m.getNumArgs() == 2){
                    rectMode[m.getArgAsInt(0)] = m.getArgAsInt(1);
                }
                if (m.getAddress() == "/pos" && m.getNumArgs() == 3){
                    vX[m.getArgAsInt(0)] = m.getArgAsInt(1);
                    vY[m.getArgAsInt(0)] = m.getArgAsInt(2);
                }
                if (m.getAddress() == "/posX" && m.getNumArgs() == 2){
                    vX[m.getArgAsInt(0)] = m.getArgAsInt(1);
                }
                if (m.getAddress() == "/posY" && m.getNumArgs() == 2){
                    vY[m.getArgAsInt(0)] = m.getArgAsInt(1);
                }
                if (m.getAddress() == "/rot" && m.getNumArgs() == 4){
                    vRotX[m.getArgAsInt(0)] = m.getArgAsFloat(1);
                    vRotY[m.getArgAsInt(0)] = m.getArgAsFloat(2);
                    vRotZ[m.getArgAsInt(0)] = m.getArgAsFloat(3);
                }
                if (m.getAddress() == "/rotX" && m.getNumArgs() == 2){
                    vRotX[m.getArgAsInt(0)] = m.getArgAsFloat(1);
                }
                if (m.getAddress() == "/rotY" && m.getNumArgs() == 2){
                    vRotY[m.getArgAsInt(0)] = m.getArgAsFloat(1);
                }
                if (m.getAddress() == "/rotZ" && m.getNumArgs() == 2){
                    vRotZ[m.getArgAsInt(0)] = m.getArgAsFloat(1);
                }
                if (m.getAddress() == "/size" && m.getNumArgs() == 3){
                    vScaleX[m.getArgAsInt(0)] = m.getArgAsFloat(1)/vW[m.getArgAsInt(0)];
                    vScaleY[m.getArgAsInt(0)] = m.getArgAsFloat(2)/vH[m.getArgAsInt(0)];
                }
                if (m.getAddress() == "/width" && m.getNumArgs() == 2){
                    vScaleX[m.getArgAsInt(0)] = m.getArgAsFloat(1)/vW[m.getArgAsInt(0)];
                }
                if (m.getAddress() == "/height" && m.getNumArgs() == 2){
                    vScaleY[m.getArgAsInt(0)] = m.getArgAsFloat(1)/vH[m.getArgAsInt(0)];
                }
                if (m.getAddress() == "/scale" && m.getNumArgs() == 3){
                    vScaleX[m.getArgAsInt(0)] = m.getArgAsFloat(1);
                    vScaleY[m.getArgAsInt(0)] = m.getArgAsFloat(2);
                }
                if (m.getAddress() == "/speed" && m.getNumArgs() == 2){
                    vSpeed[m.getArgAsInt(0)] = m.getArgAsFloat(1);
                }
                if (m.getAddress() == "/loopState" && m.getNumArgs() == 2){
                  int s = m.getArgAsFloat(1);
                    if(s == 0){
                      vLoopState[m.getArgAsInt(0)] = OF_LOOP_NONE;
                      videoLC[m.getArgAsInt(0)].setLoopState(OF_LOOP_NONE);
                    }
                    if(s == 1){
                      vLoopState[m.getArgAsInt(0)]  = OF_LOOP_NORMAL;
                      videoLC[m.getArgAsInt(0)].setLoopState(OF_LOOP_NORMAL);
                    }
                }
                if (m.getAddress() == "/play" && m.getNumArgs() == 1){
                    vIndexPlaying[m.getArgAsInt(0)] = 1;
                    videoLC[m.getArgAsInt(0)].play();
                }
                if (m.getAddress() == "/stop" && m.getNumArgs() == 1){
                    vIndexPlaying[m.getArgAsInt(0)] = 0;
                    videoLC[m.getArgAsInt(0)].stop();
                }
                if (m.getAddress() == "/pause" && m.getNumArgs() == 1){
                    vIndexPlaying[m.getArgAsInt(0)] = 0;
                    videoLC[m.getArgAsInt(0)].setPaused(true);
                }
                if (m.getAddress() == "/setFrame" && m.getNumArgs() == 2){
                    videoLC[m.getArgAsInt(0)].setFrame(m.getArgAsInt(1));
                }
                if (m.getAddress() == "/color" && m.getNumArgs() == 4){
                    vColor[m.getArgAsInt(0)] = ofColor(m.getArgAsInt(1),m.getArgAsInt(2),m.getArgAsInt(3));
                }
                if (m.getAddress() == "/opacity" && m.getNumArgs() == 2){
                    vOpacity[m.getArgAsInt(0)] = m.getArgAsInt(1);
                }
                if (m.getAddress() == "/filter"){
                    int n = m.getArgAsInt(0);
#if (defined(__APPLE__) && defined(__MACH__))
                    if(syphonON[m.getArgAsInt(0)] == 1)
                        ofLogNotice() << "CineVivo[notice]: CV Filters can not be applied to syphon inputs";
                    else{
#endif
                        if(m.getArgAsInt(1) == 1)
                            filterOn[m.getArgAsInt(0)] = true;
                        if(m.getArgAsInt(1) == 0)
                            filterOn[m.getArgAsInt(0)] = false;
#if (defined(__APPLE__) && defined(__MACH__))
                    }
#endif
                    if(shaderLoaded[n] == false && filterOn[n] && videoLC[n].isLoaded()){
                        FilterChain * charcoal = new FilterChain(vW[n], vH[n], "Charcoal");
                        charcoal->addFilter(new GaussianBlurFilter(vW[n], vH[n], 2.f ));
                        charcoal->addFilter(new DoGFilter(vW[n], vH[n], 12, 1.2, 8, 0.99, 4));
                        filters[n].push_back(charcoal);
                        
                        // Basic filter examples
                        
                        filters[n].push_back(new HalftoneFilter(vW[n], vH[n], 0.01));
                        filters[n].push_back(new CrosshatchFilter(vW[n], vH[n]));
                        filters[n].push_back(new KuwaharaFilter(6));
                        filters[n].push_back(new SobelEdgeDetectionFilter(vW[n], vH[n]));
                        filters[n].push_back(new BilateralFilter(vW[n], vH[n]));
                        filters[n].push_back(new SketchFilter(vW[n], vH[n]));
                        filters[n].push_back(new DilationFilter(vW[n],vH[n]));
                        filters[n].push_back(new PerlinPixellationFilter(vW[n], vH[n]));
                        filters[n].push_back(new XYDerivativeFilter(vW[n], vH[n]));
                        filters[n].push_back(new ZoomBlurFilter());
                        filters[n].push_back(new EmbossFilter(vW[n], vH[n], 2.f));
                        filters[n].push_back(new SmoothToonFilter(vW[n], vH[n]));
                        filters[n].push_back(new TiltShiftFilter(videoLC[n].getTexture()));
                        filters[n].push_back(new VoronoiFilter(videoLC[n].getTexture()));
                        filters[n].push_back(new CGAColorspaceFilter());
                        filters[n].push_back(new ErosionFilter(vW[n], vH[n]));
                        filters[n].push_back(new LookupFilter(vW[n], vH[n], "img/lookup_amatorka.png"));
                        filters[n].push_back(new LookupFilter(vW[n],vH[n], "img/lookup_miss_etikate.png"));
                        filters[n].push_back(new LookupFilter(vW[n], vH[n], "img/lookup_soft_elegance_1.png"));
                        filters[n].push_back(new VignetteFilter());
                        filters[n].push_back(new PosterizeFilter(8));
                        filters[n].push_back(new LaplacianFilter(vW[n], vH[n], ofVec2f(1, 1)));
                        filters[n].push_back(new PixelateFilter(vW[n], vH[n]));
                        filters[n].push_back(new HarrisCornerDetectionFilter(videoLC[n].getTexture()));
                        filters[n].push_back(new MotionDetectionFilter(videoLC[n].getTexture()));
                        filters[n].push_back(new LowPassFilter(vW[n], vH[n], 0.9));
                        
                        // blending examples
                        
                        ofImage wes = ofImage("img/wes.jpg");
                        ChromaKeyBlendFilter * chroma = new ChromaKeyBlendFilter(ofVec3f(0.f, 1.f, 0.f), 0.4);
                        chroma->setSecondTexture(wes.getTexture());
                        filters[n].push_back(chroma);
                        
                        filters[n].push_back(new DisplacementFilter("img/mandel.jpg", vW[n], vH[n], 25.f));
                        filters[n].push_back(new PoissonBlendFilter(wes.getTexture(), vW[n],vH[n], 2.0));
                        filters[n].push_back(new DisplacementFilter("img/glass/3.jpg", vW[n], vH[n], 40.0));
                        filters[n].push_back(new ExclusionBlendFilter(wes.getTexture()));
                        
                        // Convolution filters
                        
                        Abstract3x3ConvolutionFilter * convolutionFilter1 = new Abstract3x3ConvolutionFilter(vW[n], vH[n]);
                        convolutionFilter1->setMatrix(-1, 0, 1, -2, 0, 2, -1, 0, 1);
                        filters[n].push_back(convolutionFilter1);
                        
                        Abstract3x3ConvolutionFilter * convolutionFilter2 = new Abstract3x3ConvolutionFilter(vW[n], vH[n]);
                        convolutionFilter2->setMatrix(4, 4, 4, 4, -32, 4, 4,  4, 4);
                        filters[n].push_back(convolutionFilter2);
                        
                        Abstract3x3ConvolutionFilter * convolutionFilter3 = new Abstract3x3ConvolutionFilter(vW[n], vH[n]);
                        convolutionFilter3->setMatrix(1.2,  1.2, 1.2, 1.2, -9.0, 1.2, 1.2,  1.2, 1.2);
                        filters[n].push_back(convolutionFilter3);
                        
                        //  daisy-chain a bunch of filters
                        
                        FilterChain * foggyTexturedGlassChain = new FilterChain(vW[n], vH[n], "Weird Glass");
                        foggyTexturedGlassChain->addFilter(new PerlinPixellationFilter(vW[n], vH[n], 13.f));
                        foggyTexturedGlassChain->addFilter(new EmbossFilter(vW[n], vH[n], 0.5));
                        foggyTexturedGlassChain->addFilter(new GaussianBlurFilter(vW[n], vH[n], 3.f));
                        filters[n].push_back(foggyTexturedGlassChain);
                        
                        // unimaginative filter chain
                        
                        FilterChain * watercolorChain = new FilterChain(vW[n], vH[n], "Monet");
                        watercolorChain->addFilter(new KuwaharaFilter(9));
                        watercolorChain->addFilter(new LookupFilter(vW[n], vH[n], "img/lookup_miss_etikate.png"));
                        watercolorChain->addFilter(new BilateralFilter(vW[n], vH[n]));
                        watercolorChain->addFilter(new PoissonBlendFilter("img/canvas_texture.jpg",vW[n], vH[n], 2.0));
                        watercolorChain->addFilter(new VignetteFilter());
                        filters[n].push_back(watercolorChain);
                        
                        //  random gradient map for posterity
                        
                        vector<GradientMapColorPoint> colors;
                        for (float percent=0.0; percent<=1.0; percent+= 0.1)
                            colors.push_back( GradientMapColorPoint(ofRandomuf(),ofRandomuf(),ofRandomuf(),percent) );
                        filters[n].push_back(new GradientMapFilter(colors));
                        
                        shaderLoaded[n] = true;
                    }
                }
                if (m.getAddress() == "/shader"){
#if (defined(__APPLE__) && defined(__MACH__))
                    if(syphonON[m.getArgAsInt(0)] == 1)
                        ofLogNotice() << "CineVivo[notice]: Shaders can not be applied to syphon inputs";
                    else{
#endif
                        string shaderName = "shaders/" + m.getArgAsString(1);
                        shaderOn[m.getArgAsInt(0)] = true;
                        shader[m.getArgAsInt(0)].load(shaderName);
#if (defined(__APPLE__) && defined(__MACH__))
                    }
#endif
                }
                if (m.getAddress() == "/filterMode"){
                    if(m.getArgAsInt(1) < filters[m.getArgAsInt(0)].size()){
                        currentFilter[m.getArgAsInt(0)] = m.getArgAsInt(1);
                    } else {
                        ofLogNotice() << "Cinevivo[error]: filterMode " + ofToString(m.getArgAsInt(1)) + " is not available";
                    }
                }
                if (m.getAddress() == "/feedback" && m.getNumArgs() == 2){
                    vFeedback[m.getArgAsInt(0)] = m.getArgAsInt(1);
                }
                if (m.getAddress() == "/chroma" && m.getNumArgs() == 1){
                    vChroma[m.getArgAsInt(0)] = true;
                    vColorChroma[m.getArgAsInt(0)] = ofColor(0);
                }
                if (m.getAddress() == "/chroma" && m.getNumArgs() == 4){
                    vChroma[m.getArgAsInt(0)] = true;
                    vColorChroma[m.getArgAsInt(0)] = ofColor(m.getArgAsInt(1),m.getArgAsInt(2),m.getArgAsInt(3));
                }
                if (m.getAddress() == "/blend" && m.getNumArgs() == 3){
                    if((videoLC[m.getArgAsInt(1)].isLoaded() && videoLC[m.getArgAsInt(2)].isLoaded()) ||
                        (videoLC[m.getArgAsInt(1)].isLoaded() && cam[deviceID[m.getArgAsInt(2)]].isUsingTexture()) ||
                         (cam[deviceID[m.getArgAsInt(1)]].isUsingTexture() && videoLC[m.getArgAsInt(2)].isLoaded()) ||
                         (cam[deviceID[m.getArgAsInt(1)]].isUsingTexture() && cam[deviceID[m.getArgAsInt(2)]].isUsingTexture())){
                        
                        int n = m.getArgAsInt(0);
                        vBlend[n] = true;
                        if(camON[m.getArgAsInt(1)]){
                            vW[n] = cam[deviceID[m.getArgAsInt(1)]].getWidth();
                            vH[n] = cam[deviceID[m.getArgAsInt(1)]].getHeight();
                        } else {
                            vW[n] = videoLC[m.getArgAsInt(1)].getWidth();
                            vH[n] = videoLC[m.getArgAsInt(1)].getHeight();
                        }
                        four[n].set(ofPoint(0,vH[n]));
                        three[n].set(ofPoint( vW[n] ,vH[n]));
                        two[n].set(ofPoint(vW[n],0));
                        vToBlend[n][0] = m.getArgAsInt(1);
                        vToBlend[n][1] = m.getArgAsInt(2);
                    }
                }
                if (m.getAddress() == "/fit" && m.getNumArgs() == 1){
                    vX[m.getArgAsInt(0)] = 0;
                    vY[m.getArgAsInt(0)] = 0;
                    vScaleX[m.getArgAsInt(0)] = (ofGetWidth()*1.0)/vW[m.getArgAsInt(0)];
                    vScaleY[m.getArgAsInt(0)] = (ofGetHeight()*1.0)/vH[m.getArgAsInt(0)];
                }
                if (m.getAddress() == "/fitHorizontal" && m.getNumArgs() == 1){
                    vScaleX[m.getArgAsInt(0)] = (ofGetWidth()*1.0)/vW[m.getArgAsInt(0)];
                }
                if (m.getAddress() == "/fitVertical" && m.getNumArgs() == 1){
                    vScaleY[m.getArgAsInt(0)] = (ofGetHeight()*1.0)/vH[m.getArgAsInt(0)];
                }
                if (m.getAddress() == "/fitAllHorizontal" && m.getNumArgs() == 1){
                    float wPartial = (ofGetWidth()*1.0)/numVideosLoaded;
                    int numPartial = 0;
                    for(int i = 0; i < LIM; i++){
                        if(vIndexPlaying[i] == 1 || camON[i] == 1) {
                            vScaleX[i] = wPartial/vW[i];
                            vScaleY[i] = (ofGetHeight()*1.0)/vH[i];
                            vX[i] = numPartial * wPartial;
                            vY[i] = 0;
                            numPartial++;
                        }
                    }
                }
                if (m.getAddress() == "/fitAllVertical" && m.getNumArgs() == 1){
                    float hPartial = (ofGetHeight()*1.0)/numVideosLoaded;
                    int numPartial = 0;
                    for(int i = 0; i < LIM; i++){
                        if(vIndexPlaying[i] == 1 || camON[i] == 1) {
                            vScaleX[i] = (ofGetWidth()*1.0)/vW[i];
                            vScaleY[i] = hPartial/vH[i];
                            vY[i] = numPartial * hPartial;
                            vX[i] = 0;
                            numPartial++;
                        }
                    }
                }
                if (m.getAddress() == "/points" && m.getNumArgs() == 7){
                    one[m.getArgAsInt(0)].set(ofPoint(m.getArgAsInt(1),m.getArgAsInt(2)));
                    two[m.getArgAsInt(0)].set(ofPoint(m.getArgAsInt(1),m.getArgAsInt(2)));
                    three[m.getArgAsInt(0)].set(ofPoint(m.getArgAsInt(3),m.getArgAsInt(4)));
                    four[m.getArgAsInt(0)].set(ofPoint(m.getArgAsInt(5),m.getArgAsInt(6)));
                }
                if (m.getAddress() == "/points" && m.getNumArgs() == 9){
                    one[m.getArgAsInt(0)].set(ofPoint(m.getArgAsInt(1),m.getArgAsInt(2)));
                    two[m.getArgAsInt(0)].set(ofPoint(m.getArgAsInt(3),m.getArgAsInt(4)));
                    three[m.getArgAsInt(0)].set(ofPoint(m.getArgAsInt(5),m.getArgAsInt(6)));
                    four[m.getArgAsInt(0)].set(ofPoint(m.getArgAsInt(7),m.getArgAsInt(8)));
                }
                if (m.getAddress() == "/blur" && m.getNumArgs() == 2){
                    blur[m.getArgAsInt(0)] = true;
                    filtBlur[m.getArgAsInt(0)].clear();
                    if(vIndexPlaying[m.getArgAsInt(0)] == true){
                        filtBlur[m.getArgAsInt(0)].push_back(new GaussianBlurFilter(videoLC[m.getArgAsInt(0)].getWidth(), videoLC[m.getArgAsInt(0)].getHeight(), m.getArgAsFloat(1)));
                    }
                    if(camON[i] == true){
                        filtBlur[m.getArgAsInt(0)].push_back(new GaussianBlurFilter(cam[i].getWidth(), cam[i].getHeight(), m.getArgAsFloat(1)));
                    }
                }
#if (defined(__APPLE__) && defined(__MACH__))
                if (m.getAddress() == "/syphon"){
                    int n = m.getArgAsInt(0);
                    syphonDirId[n] = m.getArgAsInt(1);
                    camON[n] = false;
                    model3DOn[n] = false;
                    vIndex[n] = 0;
                    vIndexPlaying[n] = false;
                    syphonON[n] = true;
                    vW[n] = syphonClient[n].getWidth();
                    vH[n] = syphonClient[n].getHeight();
                    filterOn[n] = false;
                }
#endif
                //----------------------- get functions ------------------------

                if (m.getAddress() == "/getWidth"){
                    ofxOscMessage s;
                    s.setAddress(m.getAddress());
                    s.addIntArg(vW[m.getArgAsInt(0)]);
                    sender.sendMessage(s);
                }
                if (m.getAddress() == "/getHeight"){
                    ofxOscMessage s;
                    s.setAddress(m.getAddress());
                    s.addIntArg(vH[m.getArgAsInt(0)]);
                    sender.sendMessage(s);
                }
                if (m.getAddress() == "/getFrames"){
                    ofxOscMessage s;
                    s.setAddress(m.getAddress());
                    s.addIntArg(videoLC[m.getArgAsInt(0)].getTotalNumFrames());
                    sender.sendMessage(s);
                }
            }
        }
    }
// Update Audio-----------------------------------------------------

    ofSoundUpdate();

// Update Graphic Things --------------------------------------------

    int c = 0;
    for(int i = 0; i < LIM; i++){
        if(vIndexPlaying[i]){
            c++;
            //videoLC[i].setLoopState(vLoopState[i]);
            videoLC[i].update();
            if(videoLC[i].isFrameNew() && !vChroma[i]){
                pix[i] = videoLC[i].getPixels();
                texVideo[i].loadData(pix[i]);
            }
            else if(videoLC[i].isFrameNew() && vChroma[i]){
                pix[i] = videoLC[i].getPixels();
                chroma(&pix[i],&texVideo[i],vColorChroma[i]);
            }
            if(videoLC[i].isFrameNew() && vChroma[i] && maskedIndex[i] == 1){
                pix[i] = videoLC[i].getPixels();
                chromaMask(&pix[i], &mask[i].getPixels(), &texVideo[i],vColorChroma[i]);
            }
        }
        if(i < deviceNUM){
          cam[i].update();
        }
        if(camON[i]){
            c++;
            if( maskedIndex[i] != 1){
              pix[i] = cam[deviceID[i]].getPixels();
              texVideo[i].loadData(pix[i]);
            }
            else if(maskedIndex[i] == 1){
                pix[i] = cam[deviceID[i]].getPixels();
                if(mask[i].isUsingTexture()){
                  chromaMask(&pix[i], &mask[i].getPixels(), &texVideo[i],ofColor(0));
                }
            }
        }
        if(model3DOn[i]){
            c++;
            models3D[i].update();
        }
        if(vIndexPlaying[vToBlend[i][0]] && vIndexPlaying[vToBlend[i][1]] && vBlend[i]){
            pix[vToBlend[i][0]] = videoLC[vToBlend[i][0]].getPixels();
            pix[vToBlend[i][1]] = videoLC[vToBlend[i][1]].getPixels();
            blend(&pix[vToBlend[i][0]], &pix[vToBlend[i][1]], &texVideo[i]);
        }
        if(camON[vToBlend[i][0]] && camON[vToBlend[i][1]] && vBlend[i]){
            pix[vToBlend[i][0]] = cam[deviceID[vToBlend[i][0]]].getPixels();
            pix[vToBlend[i][1]] = cam[deviceID[vToBlend[i][1]]].getPixels();
            blend(&pix[vToBlend[i][0]], &pix[vToBlend[i][1]], &texVideo[i]);
        }
        if(vIndexPlaying[vToBlend[i][0]] && camON[vToBlend[i][1]] && vBlend[i]){
            pix[vToBlend[i][0]] = videoLC[vToBlend[i][0]].getPixels();
            pix[vToBlend[i][1]] = cam[deviceID[vToBlend[i][1]]].getPixels();
            blend(&pix[vToBlend[i][0]], &pix[vToBlend[i][1]], &texVideo[i]);
        }
        if(camON[vToBlend[i][0]] && vIndexPlaying[vToBlend[i][1]] && vBlend[i]){
            pix[vToBlend[i][0]] = cam[deviceID[vToBlend[i][0]]].getPixels();
            pix[vToBlend[i][1]] = videoLC[vToBlend[i][1]].getPixels();
            blend(&pix[vToBlend[i][0]], &pix[vToBlend[i][1]], &texVideo[i]);
        }
    }
    numVideosLoaded = c;
    if(time+reference < ofGetElapsedTimeMillis()) {
        time = ofGetElapsedTimeMillis();
        cursor = !cursor;
    }
}
//--------------------------------------------------------------
void ofApp::draw(){

    ofSetFullscreen(fullScreen);
    ofSetBackgroundColor(backgroundColor);
    ofSetBackgroundAuto(backgroundAuto);
    //ofSetRectMode(OF_RECTMODE_CENTER);
    
    for(int i = 0; i < LIM; i++){
        if(vIndex[i] == 1 || camON[i] || imageON[i] ||
#if (defined(__APPLE__) && defined(__MACH__))
           syphonON[i] ||
#endif
           model3DOn[i] || shaderOn[i] || vBlend[i]){
            ofPushMatrix();
            ofTranslate(worldCenterX[i],worldCenterY[i]);
            ofPushMatrix();
    
            ofRotateXDeg(vRotX[i]);
            ofRotateYDeg(vRotY[i]);
            ofRotateZDeg(vRotZ[i]);
            if(rectMode[i] == 0){
                ofTranslate(0,0);
            }
            if(rectMode[i] == 1){
                ofTranslate(-vW[i]/2,-vH[i]/2);
            }
            ofSetColor(vColor[i],vOpacity[i]);
            ofTranslate(vX[i],vY[i]);
            ofScale(vScaleX[i],vScaleY[i]);
            videoLC[i].setSpeed(vSpeed[i]);
            if(blur[i] == true){
                filtBlur[i][0]->begin();
            }
            if(filterOn[i] == true && model3DOn[i] == false){
                ofPushMatrix();
                ofScale(-1, 1);
                ofTranslate(-vW[i], 0);
                filters[i][currentFilter[i]]->begin();
            }
            if(vIndexPlaying[i] == true){
                if(vFeedback[i]){
                    ofPushMatrix();
                    ofSetRectMode(OF_RECTMODE_CENTER);
                    ofTranslate(ofGetWidth()/2,ofGetHeight()/2);
                    for(int f = 0; f < 450; f++){
                        ofScale(0.99,0.99);
                        ofSetColor(255, 255, 255, 12);
                        texVideo[i].draw(0,0);
                    }
                    ofSetRectMode(OF_RECTMODE_CORNER);
                    ofPopMatrix();
                } else {
                    texVideo[i].draw(one[i], two[i], three[i], four[i]);
                }
            }
            if(camON[i]){
                if(vFeedback[i]){
                    ofPushMatrix();
                    ofSetRectMode(OF_RECTMODE_CENTER);
                    ofTranslate(ofGetWidth()/2,ofGetHeight()/2);
                    for(int f = 0; f < 450; f++){
                        ofScale(0.99,0.99);
                        ofSetColor(255, 255, 255, 12);
                        texVideo[i].draw(0,0);
                    }
                    ofSetRectMode(OF_RECTMODE_CORNER);
                    ofPopMatrix();
                } else {
                    texVideo[i].draw(one[i], two[i], three[i], four[i]);
                }
            }
            if(vBlend[i]){
                texVideo[i].draw(one[i], two[i], three[i], four[i]);
            }
#if (defined(__APPLE__) && defined(__MACH__))
            if(syphonON[i] && syphonDir.isValidIndex(syphonDirId[i])){
                syphonClient[i].draw(0,0);
                syphonClient[i].bind();
                syphonClient[i].getTexture().draw(one[i],two[i],three[i],four[i]);
                syphonClient[i].unbind();
            }
#endif
            if(model3DOn[i]){
                ofSetColor(255);
                ofEnableBlendMode(OF_BLENDMODE_ALPHA);
                ofEnableDepthTest();
                light.enable();
                ofEnableSeparateSpecularLight();
                models3D[i].drawFaces();
                ofDisableDepthTest();
                ofDisableBlendMode();
                ofDisableSeparateSpecularLight();
            }
            if(imageON[i] == true){
                pictures[i].getTexture().draw(one[i], two[i], three[i], four[i]);
            }
            if(filterOn[i] == true && model3DOn[i] == false){
                filters[i][currentFilter[i]]->end();
                ofPopMatrix();
            }
            if(blur[i] == true){
                filtBlur[i][0]->end();
            }
            if(shaderOn[i]){
                fbo[i].begin();
                ofClear(0, 0, 0, 0);
                if(shader[i].isLoaded()){
                    shader[i].begin();
                    shader[i].setUniform1f("iGlobalTime", ofGetElapsedTimef());
                    shader[i].setUniform3f("iResolution",WIDTH,HEIGHT, 1);
                    shader[i].setUniformTexture("iChannel0", texVideo[i], 2);
                }
                
                ofSetColor(255, 255, 255);
                ofDrawRectangle(0, 0, WIDTH, HEIGHT);
                if(shader[i].isLoaded()){
                    shader[i].end();
                }
                fbo[i].end();
                fbo[i].draw(0, 0, ofGetWidth(), ofGetHeight());
            }
            ofPopMatrix();
            ofPopMatrix();
        }
    }
    if(t_visible){
        ofSetColor(fontColor);
        if(textToExe.size() > 0){
            font.drawString(textToExe, fontSize , fontSize + 5);
        }
        /*if(cursor == true) {
            ofSetLineWidth(4);
            ofDrawLine((fontSize*0.777)*(textToExe.size())+16,(fontSize*numLines)+(fontSize/2.0),(fontSize*0.777)*(textToExe.size())+16,(numLines*fontSize)+(fontSize*2));
        }*/
        ofSetColor(255);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    bool modifier = ofGetKeyPressed(s_superAsModifier ? OF_KEY_SUPER : OF_KEY_CONTROL);
    if(modifier) {
        switch(key) {
            case 5: case 'e':
                executeScriptEvent(textToExe);
                return;
            case 20: case 't':
                t_visible = !t_visible;
                return;
            case 29: case '+':
                if(fontSize < 40){
                    fontSize += 1;
                    font.load("fonts/font.ttf", fontSize, true, true);
                }
                return;
            case 47: case '-':
                if(fontSize > 2){
                    fontSize -= 1;
                    font.load("fonts/font.ttf", fontSize, true, true);
                }
                return;
        }
    } else {
        switch(key) {
            case 13:
                textToExe += "\n";
                numLines++;
                break;
            case 8:
                if(textToExe.size() != 0){
                    textToExe = textToExe.substr(0, textToExe.length()-1);
                }
                break;
            case OF_KEY_SHIFT: case OF_KEY_LEFT_SHIFT: case OF_KEY_RIGHT_SHIFT:
                break;
            default:
                textToExe.append(1,char(key));
        }
    }
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
    ofLogNotice() << "x: " << x << ", y: " << y;
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
void ofApp::exit() {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}

//--------------------------------------------------------------
void ofApp::chroma(ofPixels *src, ofTexture *texture, ofColor colorChroma){
    ofPixels pix;
    int w = src->getWidth();
    int h = src->getHeight();
    pix.allocate(w, h, OF_PIXELS_RGBA);
    for(int i = 0; i < w; i++){
        for(int j = 0; j < h; j++){
            if(src->getColor(i, j) == colorChroma){
                pix.setColor(i,j,ofColor(0,0,0,0));
            } else {
                pix.setColor(i,j,src->getColor(i, j));
            }
        }
    }
    texture->loadData(pix);
}
//--------------------------------------------------------------
void ofApp::chromaMask(ofPixels *src1, ofPixels *src2, ofTexture *texture, ofColor colorChroma){

    ofPixels pix;
    int w = src1->getWidth();
    int h = src1->getHeight();
    src2->resize(w,h,OF_INTERPOLATE_NEAREST_NEIGHBOR);
    pix.allocate(w, h, OF_PIXELS_RGBA);
    for(int i = 0; i < w; i++){
        for(int j = 0; j < h; j++){
            if(src2->getColor(i, j) == colorChroma){
                pix.setColor(i,j,ofColor(0,0,0,0));
            } else {
                pix.setColor(i,j,src1->getColor(i, j));
            }
        }
    }
    texture->loadData(pix);
}
//--------------------------------------------------------------
void ofApp::blend(ofPixels *src1, ofPixels *src2, ofTexture *texture){
    
    int w = src1->getWidth();
    int h = src1->getHeight();
    src2->resize(w,h,OF_INTERPOLATE_NEAREST_NEIGHBOR);
    ofPixels pix;
    pix.allocate(w, h, OF_PIXELS_RGBA);
    for(int i = 0; i < w; i++){
        for(int j = 0; j < h; j++){
            ofColor colorGrab = src1->getColor(i, j);
            int x1 = i + ( colorGrab.r - 127 );
            x1 = ofClamp( x1, 0, w-1 );
            ofColor color = src2->getColor( x1, j );
            pix.setColor( i, j, color );
        }
    }
    texture->loadData(pix);
}
//-----------------------------------------------------------
void ofApp::executeScriptEvent(string getText) {
    // received on editor CTRL/Super + e
    
    std::vector<string> textClean;
    string txt = getText;
    lineas = ofSplitString(txt, "\n");
    
    for(int i = 0; i < lineas.size();i++){
        ofxOscMessage s;
        if(lineas[i].size() != 0){
            textClean = ofSplitString(lineas[i], " ");
            for(int i = 0; i < textClean.size();i++){
                if(textClean[i] != ""){
                    texto.push_back(textClean[i]);
                }
            }
            if(texto[0] == XML.getValue("WORDS:NAME:FSIZE","fontSize") && texto.size() == 2){
                fontSize = ofToInt(texto[1]);
                font.load("fonts/font.ttf", fontSize, true, true);
                ofLogNotice() << "CineVivo[editor]: Font Size: " + texto[1];
            }
            if(texto[0] == XML.getValue("WORDS:NAME:FCOLOR","fontColor") && texto.size() == 4){
                fontColor = ofColor(ofToInt(texto[1]),ofToInt(texto[2]),ofToInt(texto[3]));
                ofLogNotice() << "CineVivo[editor]: Font Color: " + texto[1] + " " + texto[2] + " " + texto[3];
            }
            if(texto[0] == XML.getValue("WORDS:NAME:FILEEXT","fileExtension") && texto.size() == 2){
                if (ofToInt(texto[1]) == 0 || ofToInt(texto[1]) == 1){
                    string temp = "/fileExtension " + texto[1];
                    ofLogNotice() << "CineVivo[setup]: File extension: " + texto[1];
                    s.setAddress("/fileExtension");
                    s.addIntArg(ofToInt(texto[1]));
                    osc.sendMessage(s);
                } else {
                    ofLogNotice() << "CineVivo[error]: Invalid value: " + texto[1];
                }
            }
            if(texto[0] == "english" && texto.size() == 1){
                
                ofLogNotice() << "CineVivo[setup]: English syntax loaded";
            }
            if(texto[0] == "espanol" && texto.size() == 1){
                XML.load ("xml/languageES.xml");
                ofLogNotice() << "CineVivo[setup]: Sintaxis en Español cargada";
            }
            if(texto[0] == XML.getValue("WORDS:NAME:CUSTOM","custom") && texto.size() == 1){
                XML.load ("xml/languageCustom.xml");
                ofLogNotice() << "CineVivo[setup]: Custom syntax loaded";
            }
            if(texto[0] == XML.getValue("WORDS:NAME:WSHAPE","windowShape") && texto.size() == 3){
                string temp = "/windowShape " + texto[1] + " " + texto[2];
                ofLogNotice() << "CineVivo[setup]: " << temp;
                s.setAddress("/windowShape");
                s.addIntArg(ofToInt(texto[1]));
                s.addIntArg(ofToInt(texto[2]));
                osc.sendMessage(s);
            }
            if(texto[0] == XML.getValue("WORDS:NAME:BCOLOR","backColor") && texto.size() == 4){
                string temp = "/backColor " + texto[1] + " " + texto[2] + " " + texto[3];
                ofLogNotice() << "CineVivo[send]: " << temp;
                s.setAddress("/backColor");
                s.addIntArg(ofToInt(texto[1]));
                s.addIntArg(ofToInt(texto[2]));
                s.addIntArg(ofToInt(texto[3]));
                osc.sendMessage(s);
            }
            if(texto[0] == XML.getValue("WORDS:NAME:BCOLOR","backColor") && texto.size() == 2){
                string temp = "/backColor " + texto[1];
                ofLogNotice() << "CineVivo[send]: " << temp;
                s.setAddress("/backColor");
                s.addIntArg(ofToInt(texto[1]));
                osc.sendMessage(s);
            }
            if(texto[0] == XML.getValue("WORDS:NAME:BAUTO","backgroundAuto") && texto.size() == 2){
                string temp = "/backgroundAuto " + texto[1];
                ofLogNotice() << "CineVivo[send]: " << temp;
                s.setAddress("/backgroundAuto");
                s.addIntArg(ofToInt(texto[1]));
                osc.sendMessage(s);
            }
            if(texto[0] == XML.getValue("WORDS:NAME:FSCREEN","fullscreen") && texto.size() == 2){
                string temp = "/fullscreen " + texto[1];
                ofLogNotice() << "CineVivo[send]: " << temp;
                s.setAddress("/fullscreen");
                s.addIntArg(ofToInt(texto[1]));
                osc.sendMessage(s);
            }
            if(texto[0] == XML.getValue("WORDS:NAME:CLEAN","clean") && texto.size() == 1){
                string temp = "/clean ";
                ofLogNotice() << "CineVivo[send]: " << temp;
                s.setAddress("/clean");
                s.addIntArg(0);
                osc.sendMessage(s);
            }
            if(texto[0] == XML.getValue("WORDS:NAME:PLAYALL","playAll") && texto.size() == 1){
                string temp = "/playAll ";
                ofLogNotice() << "CineVivo[send]: " << temp;
                s.setAddress("/playAll");
                s.addIntArg(0);
                osc.sendMessage(s);
            }
            if(texto[0] == XML.getValue("WORDS:NAME:STOPALL","stopAll") && texto.size() == 1){
                string temp = "/stopAll ";
                ofLogNotice() << "CineVivo[send]: " << temp;
                s.setAddress("/stopAll");
                s.addIntArg(0);
                osc.sendMessage(s);
            }
            if(texto[0] == XML.getValue("WORDS:NAME:PAUSEALL","pauseAll") && texto.size() == 1){
                string temp = "/pauseAll ";
                ofLogNotice() << "CineVivo[send]: " << temp;
                s.setAddress("/pauseAll");
                s.addIntArg(0);
                osc.sendMessage(s);
            }
            if(texto[0] == XML.getValue("WORDS:NAME:FITAHOR","fitAllHorizontal") && texto.size() == 1){
                string temp = "/fitAllHorizontal ";
                ofLogNotice() << "CineVivo[send]: " << temp;
                s.setAddress("/fitAllHorizontal");
                s.addIntArg(0);
                osc.sendMessage(s);
            }
            if(texto[0] == XML.getValue("WORDS:NAME:FITAVER","fitAllVertical") && texto.size() == 1){
                string temp = "/fitAllVertical ";
                ofLogNotice() << "CineVivo[send]: " << temp;
                s.setAddress("/fitAllVertical");
                s.addIntArg(0);
                osc.sendMessage(s);
            }
            
            int numV = -1;
            for(int i = 0; i < 10; i++){
                if(ofIsStringInString(digit[i], texto[0])){
                    numV = i;
                }
            }
            
            if(numV > -1 && numV < 10){
                if(texto[1] == XML.getValue("WORDS:NAME:LOAD","load") && texto.size() == 3){
                    string temp = "/load " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/load");
                    s.addIntArg(numV);
                    s.addStringArg(texto[2]);
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:LOAD","load") && texto.size() == 4){
                    string temp = "/load " + ofToString(numV) + " " + texto[2] + " " + texto[3];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/load");
                    s.addIntArg(numV);
                    s.addStringArg(texto[2]);
                    s.addIntArg(ofToInt(texto[3]));
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:LOAD3D","load3D") && texto.size() == 3){
                  string temp = "/load3D " + ofToString(numV) + " " + texto[2];
                  ofLogNotice() << "CineVivo[send]: " << temp;
                  s.setAddress("/load3D");
                  s.addIntArg(numV);
                  s.addStringArg(texto[2]);
                  osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:LOADAUDIO","loadAudio") && texto.size() == 3){
                  string temp = "/loadAudio " + ofToString(numV) + " " + texto[2];
                  ofLogNotice() << "CineVivo[send]: " << temp;
                  s.setAddress("/loadAudio");
                  s.addIntArg(numV);
                  s.addStringArg(texto[2]);
                  osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:VOLUMEAUDIO","volumeAudio") && texto.size() == 3){
                  string temp = "/volumeAudio " + ofToString(numV) + " " + texto[2];
                  ofLogNotice() << "CineVivo[send]: " << temp;
                  s.setAddress("/volumeAudio");
                  s.addIntArg(numV);
                  s.addFloatArg(ofToFloat(texto[2]));
                  osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:FREE","free") && texto.size() == 2){
                    string temp = "/free " + ofToString(numV);
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/free");
                    s.addIntArg(numV);
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:MASK","mask") && texto.size() == 3){
                    string temp = "/mask " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/mask");
                    s.addIntArg(numV);
                    s.addStringArg(texto[2]);
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:IMAGE","image") && texto.size() == 3){
                    string temp = "/image " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/image");
                    s.addIntArg(numV);
                    s.addStringArg(texto[2]);
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:RECTMODE","rectMode") && texto.size() == 3){
                    string temp = "/rectMode " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/rectMode");
                    s.addIntArg(numV);
                    s.addIntArg(ofToInt(texto[2]));
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:WCENTER","worldCenter") && texto.size() == 4){
                    string temp = "/worldCenter " + texto[1] + " " + texto[2]+ " " + texto[3];
                    ofLogNotice() << "CineVivo[setup]: " << temp;
                    s.setAddress("/worldCenter");
                    s.addIntArg(numV);
                    s.addIntArg(ofToInt(texto[2]));
                    s.addIntArg(ofToInt(texto[3]));
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:POS","pos") && texto.size() == 4){
                    string temp = "/pos " + ofToString(numV) + " " + texto[2] + " " + texto[3];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/pos");
                    s.addIntArg(numV);
                    s.addIntArg(ofToInt(texto[2]));
                    s.addIntArg(ofToInt(texto[3]));
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:POSX","posX") && texto.size() == 3){
                    string temp = "/posX " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/posX");
                    s.addIntArg(numV);
                    s.addIntArg(ofToInt(texto[2]));
                    osc.sendMessage(s);
                }
                if(texto[1] ==  XML.getValue("WORDS:NAME:POSY","posY") && texto.size() == 3){
                    string temp = "/posY " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/posY");
                    s.addIntArg(numV);
                    s.addIntArg(ofToInt(texto[2]));
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:ROT","rot") && texto.size() == 5){
                    string temp = "/rotX " + ofToString(numV) + " " + texto[2] + " " + texto[3] + " " + texto[4];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/rot");
                    s.addIntArg(numV);
                    s.addFloatArg(ofToFloat(texto[2]));
                    s.addFloatArg(ofToFloat(texto[3]));
                    s.addFloatArg(ofToFloat(texto[4]));
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:ROTX","rotX") && texto.size() == 3){
                    string temp = "/rotX " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/rotX");
                    s.addIntArg(numV);
                    s.addFloatArg(ofToFloat(texto[2]));
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:ROTY","rotY") && texto.size() == 3){
                    string temp = "/rotY " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/rotY");
                    s.addIntArg(numV);
                    s.addFloatArg(ofToFloat(texto[2]));
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:ROTZ","rotZ") && texto.size() == 3){
                    string temp = "/rotZ " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/rotZ");
                    s.addIntArg(numV);
                    s.addFloatArg(ofToFloat(texto[2]));
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:SIZE","size") && texto.size() == 4){
                    string temp = "/sizeX " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/size");
                    s.addIntArg(numV);
                    s.addIntArg(ofToInt(texto[2]));
                    s.addIntArg(ofToInt(texto[3]));
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:WIDTH","width") && texto.size() == 3){
                    string temp = "/width " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/width");
                    s.addIntArg(numV);
                    s.addIntArg(ofToInt(texto[2]));
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:HEIGHT","height") && texto.size() == 3){
                    string temp = "/height " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/height");
                    s.addIntArg(numV);
                    s.addIntArg(ofToInt(texto[2]));
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:SCALE","scale") && texto.size() == 4){
                    string temp = "/scale " + ofToString(numV) + " " + texto[2] + " " + texto[3];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/scale");
                    s.addIntArg(numV);
                    s.addFloatArg(ofToFloat(texto[2]));
                    s.addFloatArg(ofToFloat(texto[3]));
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:SPEED","speed") && texto.size() == 3){
                    string temp = "/speed " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/speed");
                    s.addIntArg(numV);
                    s.addFloatArg(ofToFloat(texto[2]));
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:PLAY","play") && texto.size() == 2){
                  string temp = "/play " + ofToString(numV);
                  ofLogNotice() << "CineVivo[send]: " << temp;
                  s.setAddress("/play");
                  s.addIntArg(numV);
                  osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:LOOPSTATE","loopState") && texto.size() == 3){
                  string temp = "/loopState " + ofToString(numV) + " " + texto[2];
                  ofLogNotice() << "CineVivo[send]: " << temp;
                  s.setAddress("/loopState");
                  s.addIntArg(numV);
                  s.addIntArg(ofToInt(texto[2]));
                  osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:STOP","stop") && texto.size() == 2){
                    string temp = "/stop " + ofToString(numV);
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/stop");
                    s.addIntArg(numV);
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:PAUSE","pause") && texto.size() == 2){
                    string temp = "/pause " + ofToString(numV);
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/pause");
                    s.addIntArg(numV);
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:SETFRAME","setFrame") && texto.size() == 3){
                    string temp = "/setFrame " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/setFrame");
                    s.addIntArg(ofToInt(texto[2]));
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:FILTER","filter") && texto.size() == 3){
                    string temp = "/filter " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/filter");
                    s.addIntArg(numV);
                    s.addIntArg(ofToInt(texto[2]));
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:FILTERM","filterMode") && texto.size() == 3){
                    string temp = "/filterMode " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/filterMode");
                    s.addIntArg(numV);
                    s.addIntArg(ofToInt(texto[2]));
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:SHADER","shader") && texto.size() == 3){
                    string temp = "/shader " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/shader");
                    s.addIntArg(numV);
                    s.addStringArg(texto[2]);
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:BLUR","blur") && texto.size() == 3){
                    string temp = "/blur " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/blur");
                    s.addIntArg(numV);
                    s.addIntArg(ofToFloat(texto[2]));
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:CHROMA","chroma") && texto.size() == 5){
                    string temp = "/chroma " + ofToString(numV) + " " + texto[2] + " " + texto[3] + " " + texto[4];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/chroma");
                    s.addIntArg(numV);
                    s.addIntArg(ofToInt(texto[2]));
                    s.addIntArg(ofToInt(texto[3]));
                    s.addIntArg(ofToInt(texto[4]));
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:CHROMA","chroma") && texto.size() == 2){
                    string temp = "/chroma " + ofToString(numV);
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/chroma");
                    s.addIntArg(numV);
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:RANDOM","random") && texto.size() == 2){
                    string temp = "/random " + ofToString(numV);
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/random");
                    s.addIntArg(numV);
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:BLEND","blend") && texto.size() == 4){
                    string temp = "/blend " + ofToString(numV) + " " + texto[2] + " " + texto[3];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/blend");
                    s.addIntArg(numV);
                    s.addIntArg(ofToInt(texto[2]));
                    s.addIntArg(ofToInt(texto[3]));
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:FEEDBACK","feedback") && texto.size() == 3){
                    string temp = "/feedback " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/feedback");
                    s.addIntArg(numV);
                    s.addIntArg(ofToInt(texto[2]));
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:COLOR","color") && texto.size() == 5){
                    string temp = "/color " + ofToString(numV) + " " + texto[2] + " " + texto[3] + " " + texto[4];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/color");
                    s.addIntArg(numV);
                    s.addIntArg(ofToInt(texto[2]));
                    s.addIntArg(ofToInt(texto[3]));
                    s.addIntArg(ofToInt(texto[4]));
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:OPACITY","opacity") && texto.size() == 3){
                    string temp = "/opacity " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/opacity");
                    s.addIntArg(numV);
                    s.addIntArg(ofToInt(texto[2]));
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:FIT","fit") && texto.size() == 2){
                    string temp = "/fit " + ofToString(numV);
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/fit");
                    s.addIntArg(numV);
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:FHORIZONTAL","fitHorizontal") && texto.size() == 2){
                    string temp = "/fitHorizontal " + ofToString(numV);
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/fitHorizontal");
                    s.addIntArg(numV);
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:FVERTICAL","fitVertical") && texto.size() == 2){
                    string temp = "/fitVertical " + ofToString(numV);
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/fitVertical");
                    s.addIntArg(numV);
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:POINTS","points") && texto.size() == 10){
                    string temp = "/points " + ofToString(numV);
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/points");
                    s.addIntArg(numV);
                    for(int i = 2; i < 10; i++){
                        s.addIntArg(ofToInt(texto[i]));
                    }
                    osc.sendMessage(s);
                }
                if(texto[1] == XML.getValue("WORDS:NAME:SYPHON","syphon") && texto.size() == 3){
                    string temp = "/syphon " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/syphon");
                    s.addIntArg(numV);
                    s.addIntArg(ofToInt(texto[2]));
                    osc.sendMessage(s);
                }
            }
        }
        texto.clear();
    }
}

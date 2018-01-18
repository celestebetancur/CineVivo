#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    ofSetWindowShape(640, 480);
    ofSetWindowTitle("CineVivo");
    ofSetVerticalSync(false);
    
    XML.load ("xml/OSCConf.xml");
    //----------------------OSC setup -------------------------
    portOut = XML.getValue("PORT:NAME:OUT",5613);
    sender.setup("127.0.0.1",portOut);
    //OSC in
    portIn = XML.getValue("PORT:NAME:IN",5612);
    reciever.setup(portIn);
    
    //--------------------filters setup -----------------------
    
    ambientLight = 155;
    ofDisableArbTex();
    ofEnableSmoothing();
    ofEnableAlphaBlending();
    lightC.setAreaLight(200, 200);
    ofSetGlobalAmbientColor(ambientLight);

    //-----------------------------------------------------------
    
    numVideosLoaded = 0;
    backgroundColor = ofColor(0,0,0);
    
    for(int i = 0; i < LIM; i++){

        shader[i].load("shadersGL2/shader");

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
        one[i].set(0,0);
        pix[i].allocate(WIDTH, HEIGHT, OF_PIXELS_RGBA);
        texVideo[i].allocate(pix[i]);
        rectMode[i] = 0;
        mask[i].allocate(WIDTH, HEIGHT, OF_IMAGE_COLOR_ALPHA);
        maskedIndex[i] == 0;
        currentFilter[i] = 0;
        shaderOn[i] = false;
        shaderLoaded[i] = false;
        //-------------------Syphon--------------------------------
        syphonDir[i].setup();
        syphonClient[i].setup();
        syphonDirId[i] = -1;
        syphonON[i] = false;
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    
    while (reciever.hasWaitingMessages()){
        
        ofxOscMessage m;
        reciever.getNextMessage(&m);
        
        for(int i  = 0; i < m.getNumArgs(); i++){
            if (m.getAddress() == "/windowShape"  &&  m.getNumArgs() == 2){
                ofSetWindowShape(m.getArgAsInt(0), m.getArgAsInt(1));
            }
            if (m.getAddress() == "/backColor" && m.getNumArgs() == 3){
                backgroundColor = ofColor(m.getArgAsInt(1),m.getArgAsInt(2),m.getArgAsInt(3));
            }
            if (m.getAddress() == "/backColor" && m.getNumArgs() == 1){
                backgroundColor = ofColor(m.getArgAsInt(1));
            }
            if (m.getAddress() == "/fullscreen" &&  m.getNumArgs() == 1){
                if(m.getArgAsInt(0) == 1)
                    fullScreen = true;
                if(m.getArgAsInt(0) == 0)
                    fullScreen = false;
            }
            if (m.getAddress() == "/clean"){
                cam.close();
                numVideosLoaded = 0;
                for(int i = 0; i < LIM; i++){
                    videoLC[i].stop();
                    vIndex[i] = 0;
                    vIndexPlaying[i] = false;
                    syphonON[i] = false;
                    camON[i] = false;
                    model3DOn[i] = false;
                    vX[i] = 0;
                    vY[i] = 0;
                    vRotX[i] = 0;
                    vRotY[i] = 0;
                    vRotZ[i] = 0;
                    vW[i] = 0;
                    vH[i] = 0;
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
                if (m.getAddress() == "/load"  &&  m.getNumArgs() == 2){
                    if (m.getArgAsString(1) == "camera"){
                        int n = m.getArgAsInt(0);
                        cam.setup(640, 480);
                        if(cam.isInitialized()){
                            vIndex[n] = 0;
                            syphonON[n] = false;
                            vIndexPlaying[n] = false;
                            model3DOn[n] = false;
                            vW[n] = cam.getWidth();
                            vH[n] = cam.getHeight();
                            camON[n] = true;
                            //fbo[n].allocate(vW[n],vH[n]);
                            four[n].set(ofPoint(0,vH[n]));
                            three[n].set(ofPoint( vW[n] ,vH[n]));
                            two[n].set(ofPoint(vW[n],0));
                            pix[n].allocate(cam.getWidth() , cam.getHeight(), OF_PIXELS_RGB);
                            texVideo[n].allocate(pix[n]);
                            
                            if(shaderLoaded[n] == false){
                            // simple filter chain
                            
                            FilterChain * charcoal = new FilterChain(cam.getWidth(), cam.getHeight(), "Charcoal");
                            //charcoal->addFilter(new BilateralFilter(cam.getWidth(), cam.getHeight(), 4, 4));
                            charcoal->addFilter(new GaussianBlurFilter(cam.getWidth(), cam.getHeight(), 2.f ));
                            charcoal->addFilter(new DoGFilter(cam.getWidth(), cam.getHeight(), 12, 1.2, 8, 0.99, 4));
                            filters[n].push_back(charcoal);
                            
                            // Basic filter examples
                            
                            filters[n].push_back(new HalftoneFilter(cam.getWidth(), cam.getHeight(), 0.01));
                            filters[n].push_back(new CrosshatchFilter(cam.getWidth(), cam.getHeight()));
                            filters[n].push_back(new KuwaharaFilter(6));
                            filters[n].push_back(new SobelEdgeDetectionFilter(cam.getWidth(), cam.getHeight()));
                            filters[n].push_back(new BilateralFilter(cam.getWidth(), cam.getHeight()));
                            filters[n].push_back(new SketchFilter(cam.getWidth(), cam.getHeight()));
                            filters[n].push_back(new DilationFilter(cam.getWidth(), cam.getHeight()));
                            filters[n].push_back(new PerlinPixellationFilter(cam.getWidth(), cam.getHeight()));
                            filters[n].push_back(new XYDerivativeFilter(cam.getWidth(), cam.getHeight()));
                            filters[n].push_back(new ZoomBlurFilter());
                            filters[n].push_back(new EmbossFilter(cam.getWidth(), cam.getHeight(), 2.f));
                            filters[n].push_back(new SmoothToonFilter(cam.getWidth(), cam.getHeight()));
                            filters[n].push_back(new TiltShiftFilter(cam.getTexture()));
                            filters[n].push_back(new VoronoiFilter(cam.getTexture()));
                            filters[n].push_back(new CGAColorspaceFilter());
                            filters[n].push_back(new ErosionFilter(cam.getWidth(), cam.getHeight()));
                            filters[n].push_back(new LookupFilter(cam.getWidth(), cam.getHeight(), "img/lookup_amatorka.png"));
                            filters[n].push_back(new LookupFilter(cam.getWidth(),cam.getHeight(), "img/lookup_miss_etikate.png"));
                            filters[n].push_back(new LookupFilter(cam.getWidth(), cam.getHeight(), "img/lookup_soft_elegance_1.png"));
                            filters[n].push_back(new VignetteFilter());
                            filters[n].push_back(new PosterizeFilter(8));
                            filters[n].push_back(new LaplacianFilter(cam.getWidth(), cam.getHeight(), ofVec2f(1, 1)));
                            filters[n].push_back(new PixelateFilter(cam.getWidth(), cam.getHeight()));
                            filters[n].push_back(new HarrisCornerDetectionFilter(cam.getTexture()));
                            filters[n].push_back(new MotionDetectionFilter(cam.getTexture()));
                            filters[n].push_back(new LowPassFilter(cam.getWidth(), cam.getHeight(), 0.9));
                            
                            // blending examples
                            
                            ofImage wes = ofImage("img/wes.jpg");
                            ChromaKeyBlendFilter * chroma = new ChromaKeyBlendFilter(ofVec3f(0.f, 1.f, 0.f), 0.4);
                            chroma->setSecondTexture(wes.getTexture());
                            filters[n].push_back(chroma);
                            
                            filters[n].push_back(new DisplacementFilter("img/mandel.jpg", cam.getWidth(), cam.getHeight(), 25.f));
                            filters[n].push_back(new PoissonBlendFilter(wes.getTexture(), cam.getWidth(),cam.getHeight(), 2.0));
                            filters[n].push_back(new DisplacementFilter("img/glass/3.jpg", cam.getWidth(), cam.getHeight(), 40.0));
                            filters[n].push_back(new ExclusionBlendFilter(wes.getTexture()));
                            
                            // Convolution filters
                            
                            Abstract3x3ConvolutionFilter * convolutionFilter1 = new Abstract3x3ConvolutionFilter(cam.getWidth(),cam.getHeight());
                            convolutionFilter1->setMatrix(-1, 0, 1, -2, 0, 2, -1, 0, 1);
                            filters[n].push_back(convolutionFilter1);
                            
                            Abstract3x3ConvolutionFilter * convolutionFilter2 = new Abstract3x3ConvolutionFilter(cam.getWidth(),cam.getHeight());
                            convolutionFilter2->setMatrix(4, 4, 4, 4, -32, 4, 4,  4, 4);
                            filters[n].push_back(convolutionFilter2);
                            
                            Abstract3x3ConvolutionFilter * convolutionFilter3 = new Abstract3x3ConvolutionFilter(cam.getWidth(),cam.getHeight());
                            convolutionFilter3->setMatrix(1.2,  1.2, 1.2, 1.2, -9.0, 1.2, 1.2,  1.2, 1.2);
                            filters[n].push_back(convolutionFilter3);
                            
                            //  daisy-chain a bunch of filters
                            
                            FilterChain * foggyTexturedGlassChain = new FilterChain(cam.getWidth(),cam.getHeight(), "Weird Glass");
                            foggyTexturedGlassChain->addFilter(new PerlinPixellationFilter(cam.getWidth(),cam.getHeight(), 13.f));
                            foggyTexturedGlassChain->addFilter(new EmbossFilter(cam.getWidth(),cam.getHeight(), 0.5));
                            foggyTexturedGlassChain->addFilter(new GaussianBlurFilter(cam.getWidth(),cam.getHeight(), 3.f));
                            filters[n].push_back(foggyTexturedGlassChain);
                            
                            // unimaginative filter chain
                            
                            FilterChain * watercolorChain = new FilterChain(cam.getWidth(),cam.getHeight(), "Monet");
                            watercolorChain->addFilter(new KuwaharaFilter(9));
                            watercolorChain->addFilter(new LookupFilter(cam.getWidth(),cam.getHeight(), "img/lookup_miss_etikate.png"));
                            watercolorChain->addFilter(new BilateralFilter(cam.getWidth(),cam.getHeight()));
                            watercolorChain->addFilter(new PoissonBlendFilter("img/canvas_texture.jpg",cam.getWidth(),cam.getHeight(), 2.0));
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
                        //string temp = "videos/" + m.getArgAsString(1);
                        string temp = "videos/" + m.getArgAsString(1) + ".mov";
                        int n = m.getArgAsInt(0);
                        videoLC[n].load(temp);
                        videoLC[n].play();
                        if(videoLC[n].isLoaded()){
                            camON[n] = false;
                            syphonON[n] = false;
                            model3DOn[n] = false;
                            vIndex[n] = 1;
                            vIndexPlaying[n] = true;
                            vW[n] = videoLC[n].getWidth();
                            vH[n] = videoLC[n].getHeight();
                            //fbo[n].allocate(vW[n],vH[n]);
                            four[n].set(ofPoint(0,vH[n]));
                            three[n].set(ofPoint( vW[n] ,vH[n]));
                            two[n].set(ofPoint(vW[n],0));
                            pix[n].allocate(vW[n] , vH[n], OF_PIXELS_RGBA);
                            texVideo[n].allocate(pix[n]);
                            
                            if(shaderLoaded[n] == false){
                            
                            FilterChain * charcoal = new FilterChain(videoLC[n].getWidth(), videoLC[n].getHeight(), "Charcoal");
                            //charcoal->addFilter(new BilateralFilter(videoLC[n].getWidth(), videoLC[n].getHeight(), 4, 4));
                            charcoal->addFilter(new GaussianBlurFilter(videoLC[n].getWidth(), videoLC[n].getHeight(), 2.f ));
                            charcoal->addFilter(new DoGFilter(videoLC[n].getWidth(), videoLC[n].getHeight(), 12, 1.2, 8, 0.99, 4));
                            filters[n].push_back(charcoal);
                            
                            // Basic filter examples
                            
                            filters[n].push_back(new HalftoneFilter(videoLC[n].getWidth(), videoLC[n].getHeight(), 0.01));
                            filters[n].push_back(new CrosshatchFilter(videoLC[n].getWidth(), videoLC[n].getHeight()));
                            filters[n].push_back(new KuwaharaFilter(6));
                            filters[n].push_back(new SobelEdgeDetectionFilter(videoLC[n].getWidth(), videoLC[n].getHeight()));
                            filters[n].push_back(new BilateralFilter(videoLC[n].getWidth(), videoLC[n].getHeight()));
                            filters[n].push_back(new SketchFilter(videoLC[n].getWidth(), videoLC[n].getHeight()));
                            filters[n].push_back(new DilationFilter(videoLC[n].getWidth(),videoLC[n].getHeight()));
                            filters[n].push_back(new PerlinPixellationFilter(videoLC[n].getWidth(), videoLC[n].getHeight()));
                            filters[n].push_back(new XYDerivativeFilter(videoLC[n].getWidth(), videoLC[n].getHeight()));
                            filters[n].push_back(new ZoomBlurFilter());
                            filters[n].push_back(new EmbossFilter(videoLC[n].getWidth(), videoLC[n].getHeight(), 2.f));
                            filters[n].push_back(new SmoothToonFilter(videoLC[n].getWidth(), videoLC[n].getHeight()));
                            filters[n].push_back(new TiltShiftFilter(videoLC[n].getTexture()));
                            filters[n].push_back(new VoronoiFilter(videoLC[n].getTexture()));
                            filters[n].push_back(new CGAColorspaceFilter());
                            filters[n].push_back(new ErosionFilter(videoLC[n].getWidth(), videoLC[n].getHeight()));
                            filters[n].push_back(new LookupFilter(videoLC[n].getWidth(), videoLC[n].getHeight(), "img/lookup_amatorka.png"));
                            filters[n].push_back(new LookupFilter(videoLC[n].getWidth(),videoLC[n].getHeight(), "img/lookup_miss_etikate.png"));
                            filters[n].push_back(new LookupFilter(videoLC[n].getWidth(), videoLC[n].getHeight(), "img/lookup_soft_elegance_1.png"));
                            filters[n].push_back(new VignetteFilter());
                            filters[n].push_back(new PosterizeFilter(8));
                            filters[n].push_back(new LaplacianFilter(videoLC[n].getWidth(), videoLC[n].getHeight(), ofVec2f(1, 1)));
                            filters[n].push_back(new PixelateFilter(videoLC[n].getWidth(), videoLC[n].getHeight()));
                            filters[n].push_back(new HarrisCornerDetectionFilter(videoLC[n].getTexture()));
                            filters[n].push_back(new MotionDetectionFilter(videoLC[n].getTexture()));
                            filters[n].push_back(new LowPassFilter(videoLC[n].getWidth(), videoLC[n].getHeight(), 0.9));
                            
                            // blending examples
                            
                            ofImage wes = ofImage("img/wes.jpg");
                            ChromaKeyBlendFilter * chroma = new ChromaKeyBlendFilter(ofVec3f(0.f, 1.f, 0.f), 0.4);
                            chroma->setSecondTexture(wes.getTexture());
                            filters[n].push_back(chroma);
                            
                            filters[n].push_back(new DisplacementFilter("img/mandel.jpg", videoLC[n].getWidth(), videoLC[n].getHeight(), 25.f));
                            filters[n].push_back(new PoissonBlendFilter(wes.getTexture(), videoLC[n].getWidth(),videoLC[n].getHeight(), 2.0));
                            filters[n].push_back(new DisplacementFilter("img/glass/3.jpg", videoLC[n].getWidth(), videoLC[n].getHeight(), 40.0));
                            filters[n].push_back(new ExclusionBlendFilter(wes.getTexture()));
                            
                            // Convolution filters
                            
                            Abstract3x3ConvolutionFilter * convolutionFilter1 = new Abstract3x3ConvolutionFilter(videoLC[n].getWidth(),videoLC[n].getHeight());
                            convolutionFilter1->setMatrix(-1, 0, 1, -2, 0, 2, -1, 0, 1);
                            filters[n].push_back(convolutionFilter1);
                            
                            Abstract3x3ConvolutionFilter * convolutionFilter2 = new Abstract3x3ConvolutionFilter(videoLC[n].getWidth(),videoLC[n].getHeight());
                            convolutionFilter2->setMatrix(4, 4, 4, 4, -32, 4, 4,  4, 4);
                            filters[n].push_back(convolutionFilter2);
                            
                            Abstract3x3ConvolutionFilter * convolutionFilter3 = new Abstract3x3ConvolutionFilter(videoLC[n].getWidth(),videoLC[n].getHeight());
                            convolutionFilter3->setMatrix(1.2,  1.2, 1.2, 1.2, -9.0, 1.2, 1.2,  1.2, 1.2);
                            filters[n].push_back(convolutionFilter3);
                            
                            //  daisy-chain a bunch of filters
                            
                            FilterChain * foggyTexturedGlassChain = new FilterChain(videoLC[n].getWidth(),videoLC[n].getHeight(), "Weird Glass");
                            foggyTexturedGlassChain->addFilter(new PerlinPixellationFilter(videoLC[n].getWidth(),videoLC[n].getHeight(), 13.f));
                            foggyTexturedGlassChain->addFilter(new EmbossFilter(videoLC[n].getWidth(),videoLC[n].getHeight(), 0.5));
                            foggyTexturedGlassChain->addFilter(new GaussianBlurFilter(videoLC[n].getWidth(),videoLC[n].getHeight(), 3.f));
                            filters[n].push_back(foggyTexturedGlassChain);
                            
                            // unimaginative filter chain
                            
                            FilterChain * watercolorChain = new FilterChain(videoLC[n].getWidth(),videoLC[n].getHeight(), "Monet");
                            watercolorChain->addFilter(new KuwaharaFilter(9));
                            watercolorChain->addFilter(new LookupFilter(videoLC[n].getWidth(),videoLC[n].getHeight(), "img/lookup_miss_etikate.png"));
                            watercolorChain->addFilter(new BilateralFilter(videoLC[n].getWidth(),videoLC[n].getHeight()));
                            watercolorChain->addFilter(new PoissonBlendFilter("img/canvas_texture.jpg",videoLC[n].getWidth(),videoLC[n].getHeight(), 2.0));
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
                    }
                }
                if (m.getAddress() == "/worldCenter"  &&  m.getNumArgs() == 3){
                    worldCenterX[m.getArgAsInt(0)] = m.getArgAsInt(1);
                    worldCenterY[m.getArgAsInt(0)] = m.getArgAsInt(2);
                }
                if (m.getAddress() == "/free"  &&  m.getNumArgs() == 1){
                    camON[m.getArgAsInt(0)] = false;
                    model3DOn[m.getArgAsInt(0)] = false;
                    videoLC[m.getArgAsInt(0)].stop();
                    vIndex[m.getArgAsInt(0)] = 0;
                    vX[m.getArgAsInt(0)] = 0;
                    vY[m.getArgAsInt(0)] = 0;
                    vRotX[m.getArgAsInt(0)] = 0;
                    vRotY[m.getArgAsInt(0)] = 0;
                    vRotZ[m.getArgAsInt(0)] = 0;
                    vW[m.getArgAsInt(0)] = videoLC[m.getArgAsInt(0)].getWidth();
                    vH[m.getArgAsInt(0)] = videoLC[m.getArgAsInt(0)].getHeight();
                    vSpeed[m.getArgAsInt(0)] = 1;
                    vOpacity[m.getArgAsInt(0)] = 255;
                    vColor[m.getArgAsInt(0)] = ofColor(255,255,255);
                    maskedIndex[m.getArgAsInt(0)] = 0;
                    shaderLoaded[m.getArgAsInt(0)] = false;
                    filters[m.getArgAsInt(0)].clear();
                }
                if (m.getAddress() == "/load3D"  &&  m.getNumArgs() == 2){
                    camON[m.getArgAsInt(0)] = false;
                    videoLC[m.getArgAsInt(0)].stop();
                    vIndex[m.getArgAsInt(0)] = 0;
                    vX[m.getArgAsInt(0)] = 0;
                    vY[m.getArgAsInt(0)] = 0;
                    vRotX[m.getArgAsInt(0)] = 0;
                    vRotY[m.getArgAsInt(0)] = 0;
                    vRotZ[m.getArgAsInt(0)] = 0;
                    vW[m.getArgAsInt(0)] = videoLC[m.getArgAsInt(0)].getWidth();
                    vH[m.getArgAsInt(0)] = videoLC[m.getArgAsInt(0)].getHeight();
                    vSpeed[m.getArgAsInt(0)] = 1;
                    vOpacity[m.getArgAsInt(0)] = 255;
                    vColor[m.getArgAsInt(0)] = ofColor(255,255,255);
                    maskedIndex[m.getArgAsInt(0)] = 0;
                    shaderLoaded[m.getArgAsInt(0)] = false;
                    filters[m.getArgAsInt(0)].clear();
                    string temp = "models/" + m.getArgAsString(1);
                    model3DOn[m.getArgAsInt(0)] = true;
                    models3D[m.getArgAsInt(0)].loadModel(temp,false);
                    models3D[m.getArgAsInt(0)].setPosition(ofGetWidth()*0.5,ofGetHeight()*0.5,0);
                    models3D[m.getArgAsInt(0)].setLoopStateForAllAnimations(OF_LOOP_NORMAL);
                    models3D[m.getArgAsInt(0)].playAllAnimations();
                }
                if (m.getAddress() == "/mask"  &&  m.getNumArgs() == 2){
                    maskedIndex[m.getArgAsInt(0)] = 1;
                    string temp = "mask/" + m.getArgAsString(1) + ".png";
                    mask[m.getArgAsInt(0)].load(temp);
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
                if (m.getAddress() == "/rot" && m.getNumArgs() == 3){
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
                if (m.getAddress() == "/shader"){
                    if(syphonON[m.getArgAsInt(0)] == 1)
                        ofLogNotice() << "CineVivo[notice]: Shaders can not be applied to syphon inputs";
                    else{
                        if(m.getArgAsInt(1) == 1)
                            shaderOn[m.getArgAsInt(0)] = true;
                        if(m.getArgAsInt(1) == 0)
                            shaderOn[m.getArgAsInt(0)] = false;
                    }
                }
                if (m.getAddress() == "/shaderMode"){
                    
                    if(m.getArgAsInt(1) < filters[m.getArgAsInt(0)].size()){
                        currentFilter[m.getArgAsInt(0)] = m.getArgAsInt(1);
                    } else {
                        ofLogNotice() << "Cinevivo[error]: shaderMode " + ofToString(m.getArgAsInt(1)) + " is not available";
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
                    shaderOn[n] = false;
                }
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
    int c = 0;
    for(int i = 0; i < LIM; i++){
        if(vIndexPlaying[i] == true){
            c++;
            videoLC[i].update();
            if(videoLC[i].isFrameNew()){
                pix[i] = videoLC[i].getPixels();
                texVideo[i].loadData(pix[i]);
            }
        }
        if(camON[i] == true){
            c++;
            cam.update();
            if( cam.isFrameNew()){
                pix[i] = cam.getPixels();
                texVideo[i].loadData(pix[i]);
            }
        }
        if(model3DOn[i] == true){
            c++;
            models3D[i].update();
        }
    }
    numVideosLoaded = c;
}
//--------------------------------------------------------------
void ofApp::draw(){
    
    ofSetFullscreen(fullScreen);
    ofSetBackgroundColor(backgroundColor);
    
    for(int i = 0; i < LIM; i++){
        if(vIndex[i] == 1 || camON[i] == 1 || syphonON[i] == 1 || model3DOn[i] == 1){
            ofPushMatrix();
            ofTranslate(worldCenterX[i],worldCenterY[i]);
            ofPushMatrix();
            ofRotateX(vRotX[i]);
            ofRotateY(vRotY[i]);
            ofRotateZ(vRotZ[i]);
            if(rectMode[i] == 0)
                ofTranslate(0,0);
            if(rectMode[i] == 1){
                ofTranslate(-vW[i]/2,-vH[i]/2);
            }
            ofSetColor(vColor[i],vOpacity[i]);
            ofTranslate(vX[i],vY[i]);
            ofScale(vScaleX[i],vScaleY[i]);
            videoLC[i].setSpeed(vSpeed[i]);
            if(shaderOn[i] == true){
                //shader[i].begin();
                ofPushMatrix();
                ofScale(-1, 1);
                ofTranslate(-vW[i], 0);
                filters[i][currentFilter[i]]->begin();
            }
            //fbo[i].begin();
            if(shaderOn[i] == true){
                //shader[i].setUniform1f("alpha", sin(ofGetElapsedTimef()));
            }
            if(vIndexPlaying[i] == true){
                texVideo[i].draw(one[i], two[i], three[i], four[i]);
            }
            if(camON[i] == true){
                //cam.getTexture().draw(one[i],two[i],three[i],four[i]);
                texVideo[i].draw(one[i], two[i], three[i], four[i]);
            }
            if(syphonON[i] == true && syphonDir[i].isValidIndex(syphonDirId[i])){
                syphonClient[i].bind();
                syphonClient[i].getTexture().draw(one[i],two[i],three[i],four[i]);
                syphonClient[i].unbind();
            }
            if(maskedIndex[i] == 1 && mask[i].isAllocated()){
                mask[i].getTexture().draw(one[i], two[i], three[i], four[i]);
            }
            if(shaderOn[i] == true){
                //shader[i].end();
                filters[i][currentFilter[i]]->end();
                ofPopMatrix();
            }
            if(model3DOn[i] == true){
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
            //fbo[i].end();
            //fbo[i].draw(0, 0);
            ofPopMatrix();
            ofPopMatrix();
        }
    }
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

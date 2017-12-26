#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetWindowShape(600, 400);
    ofSetWindowTitle("CineVivo");
    ofSetBackgroundColor(0);
    
    ofSetVerticalSync(false);
    
    //OSC out
    portOut = XML.getValue("PORT:NAME:OUT",5613);
    sender.setup("127.0.0.1",portOut);
    //OSC in
    XML.load ("OSCConf.xml");
    portIn = XML.getValue("PORT:NAME:IN",5612);
    reciever.setup(portIn);
    
    //-----------------------------------
    
    ambientLight = 155;
    models3D.loadModel("rggtrn.obj",false);
    models3D.setPosition(ofGetWidth()*0.5,ofGetHeight()*0.5,0);
    ofDisableArbTex();
    lightC.setAreaLight(200, 200);
    ofSetGlobalAmbientColor(ambientLight);
    
    //-----------------------------------
    
    numVideosLoaded = 0;
    
    for(int i = 0; i < LIM; i++){
#ifdef TARGET_OPENGLES
        shader.load("shadersES2/shader");
#else
        if(ofIsGLProgrammableRenderer()){
            shader[i].load("shadersGL3/shader");
        }else{
            shader[i].load("shadersGL2/shader");
        }
#endif
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
        shaderOn[i] = false;
        camON[i] = false;
        one[i].set(0,0);
        pix[i].allocate(WIDTH, HEIGHT, OF_PIXELS_RGBA);
        texVideo[i].allocate(pix[i]);
        rectMode[i] = 0;
        mask[i].allocate(WIDTH, HEIGHT, OF_IMAGE_COLOR_ALPHA);
        maskedIndex[i] == 0;
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    
    while (reciever.hasWaitingMessages()){
        
        ofxOscMessage m;
        reciever.getNextMessage(&m);
        
        for(int i  = 0; i < m.getNumArgs(); i++){
            ofLogNotice() << m.getNumArgs();
            if (m.getAddress() == "/windowShape"  &&  m.getNumArgs() == 2){
                ofSetWindowShape(m.getArgAsInt(0), m.getArgAsInt(1));
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
                }
            }
            if(m.getArgAsInt(0) >= 0 &&  m.getArgAsInt(0) < LIM){
                if (m.getAddress() == "/load"  &&  m.getNumArgs() == 2){
                    if (m.getArgAsString(1) == "camera"){
                        int n = m.getArgAsInt(0);
                        if(cam.isInitialized()){
                            cam.setup(cam.getWidth(), cam.getHeight());
                            vIndex[n] = 1;
                            vIndexPlaying[n] = 1;
                            vW[n] = cam.getWidth();
                            vH[n] = cam.getHeight();
                            camON[n] = true;
                            fbo[n].allocate(vW[n],vH[n]);
                            four[n].set(ofPoint(0,vH[n]));
                            three[n].set(ofPoint( vW[n] ,vH[n]));
                            two[n].set(ofPoint(vW[n],0));
                            pix[n].allocate(cam.getWidth() , cam.getHeight(), OF_PIXELS_RGB);
                            texVideo[n].allocate(pix[n]);
                        }
                    } else {
                        string temp = "videos/" + m.getArgAsString(1);
                        int n = m.getArgAsInt(0);
                        vIndex[n] = 1;
                        vIndexPlaying[n] = 1;
                        videoLC[n].load(temp);
                        if(videoLC[n].isLoaded()){
                            videoLC[n].play();
                            vW[n] = videoLC[n].getWidth();
                            vH[n] = videoLC[n].getHeight();
                            fbo[n].allocate(vW[n],vH[n]);
                            four[n].set(ofPoint(0,vH[n]));
                            three[n].set(ofPoint( vW[n] ,vH[n]));
                            two[n].set(ofPoint(vW[n],0));
                            pix[n].allocate(vW[0] , vH[0], OF_PIXELS_RGBA);
                            texVideo[n].allocate(pix[n]);
                        }
                    }
                }
                if (m.getAddress() == "/free"  &&  m.getNumArgs() == 1){
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
                    ofLogNotice() << "stop";
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
                /*if (m.getAddress() == "/shader"){
                    if(m.getArgAsInt(1) == 1)
                        shaderOn[m.getArgAsInt(0)] = true;
                    if(m.getArgAsInt(1) == 0)
                        shaderOn[m.getArgAsInt(0)] = false;
                }*/
                if (m.getAddress() == "/fit" && m.getNumArgs() == 1){
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
                        if(vIndexPlaying[i] == 1) {
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
                        if(vIndexPlaying[i] == 1) {
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
        if(vIndexPlaying[i] != 0){
            videoLC[i].update();
            pix[i] = videoLC[i].getPixels();
            texVideo[i].loadData(pix[i]);
            c++;
        }
        if(camON[i] != false){
            cam.update();
            c++;
        }
    }
    numVideosLoaded = c;
}
//--------------------------------------------------------------
void ofApp::draw(){
    
    ofSetFullscreen(fullScreen);
    
    for(int i = 0; i < LIM; i++){
        if(vIndex[i] != 0){
            if(rectMode[i] == 0)
                ofSetRectMode(OF_RECTMODE_CORNER);
            if(rectMode[i] == 1)
                ofSetRectMode(OF_RECTMODE_CENTER);
            ofSetColor(vColor[i],vOpacity[i]);
            ofPushMatrix();
            ofTranslate(vX[i],vY[i]);
            ofRotateX(vRotX[i]);
            ofRotateY(vRotY[i]);
            ofRotateZ(vRotZ[i]);
            ofScale(vScaleX[i],vScaleY[i]);
            videoLC[i].setSpeed(vSpeed[i]);
            if(shaderOn[i] == true){
                shader[i].begin();
            }
            fbo[i].begin();
            if(shaderOn[i] == true){
                shader[i].setUniform1f("alpha", sin(ofGetElapsedTimef()));
            }
            if(camON[i] == false){
                texVideo[i].draw(one[i], two[i], three[i], four[i]);
            }
            if(camON[i] == true)
                cam.getTexture().draw(one[i],two[i],three[i],four[i]);
            fbo[i].end();
            fbo[i].draw(0, 0);
            if(shaderOn[i] == true){
                shader[i].end();
            }
            if(maskedIndex[i] == 1 && mask[i].isAllocated()){
                mask[i].getTexture().draw(one[i], two[i], three[i], four[i]);
            }
            ofPopMatrix();
        }
    }
    /*ofSetColor(200,10,235);
     ofEnableBlendMode(OF_BLENDMODE_ALPHA);
     ofEnableDepthTest();
     light.enable();
     ofEnableSeparateSpecularLight();
     ofPushMatrix();
     ofTranslate(models3D.getPosition().x + 300,models3D.getPosition().y + 200,(sin(ofGetElapsedTimef()) * -100)-400);
     ofRotateY(180);
     ofRotateX(-115);
     models3D.drawFaces();
     ofPopMatrix();
     ofDisableDepthTest();
     ofDisableBlendMode();
     ofDisableSeparateSpecularLight();*/
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
void ofApp::exit() {
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}


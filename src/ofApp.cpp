#include "ofApp.h"
//#include <iostream>

//using namespace std;

//--------------------------------------------------------------
/*void ofApp::setupPreview(){
    ofBackground(60,60,60);
    ofSetWindowTitle("Previews");
}
//--------------------------------------------------------------
void ofApp::drawPreview(ofEventArgs & args){

}*/

//--------------------------------------------------------------
void ofApp::setupOut(){
    ofSetBackgroundColor(0);
    ofSetWindowTitle("Output");
    server.setName("Syphon");
}
//--------------------------------------------------------------
void ofApp::drawOut(ofEventArgs & args){
    
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    wi = ofGetWidth();
    WI = ofGetScreenWidth();
    he = ofGetHeight();
    HE = ofGetScreenHeight();
    
    ofScale(wi/WI,he/HE);
    if(but1A.state == true ){
        ofSetColor(colorPrevA);
        for(int i = 1; i < div+1; i++) {
            for(int j = (W/div)*(i-1); j < (W/div)*i; j++) {
                textures[j].draw(j+i+move[i-1],up[i-1],1,H);
            }
        }
    }
    if(but1B.state == true ){
        ofSetColor(colorPrevB);
        for(int i = 1; i < div1+1; i++) {
            for(int j = (W1/div1)*(i-1); j < (W1/div1)*i; j++) {
                textures1[j].draw(j+i+move1[i-1],up1[i-1],1,H1);
            }
        }
    }
    server.publishScreen();
}
//--------------------------------------------------------------
void ofApp::testGui(ofEventArgs & args){

}
//--------------------------------------------------------------
void ofApp::setupGui(){
    ofSetBackgroundColor(0);
    ofSetWindowTitle("Preview A");
}
//--------------------------------------------------------------
void ofApp::setupGuiB(){
    ofSetBackgroundColor(0);
    ofSetWindowTitle("Preview B");
}
//--------------------------------------------------------------
void ofApp::drawGui(ofEventArgs & args){
    ofScale(.23,.23);
    colorPrevA = ofColor(slider[0].value,slider[1].value,slider[2].value, slider[3].value);
    ofSetColor(colorPrevA);
    for(int i = 1; i < div+1; i++) {
        ofSetColor(colorPrevA);
        for(int j = (W/div)*(i-1); j < (W/div)*i; j++) {
            textures[j].draw(j+i+move[i-1],up[i-1],1,H);
        }
    }
}
//--------------------------------------------------------------
void ofApp::drawGuiB(ofEventArgs & args){
    ofScale(.23,.23);
    colorPrevB = ofColor(sliderB[0].value,sliderB[1].value,sliderB[2].value, sliderB[3].value);
    ofSetColor(colorPrevB);
    for(int i = 1; i < div1+1; i++) {
        ofSetColor(colorPrevB);
        for(int j = (W1/div1)*(i-1); j < (W1/div1)*i; j++) {
            textures1[j].draw(j+i+move1[i-1],up1[i-1],1,H1);
        }
    }
}
//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowTitle("CineVivo - Main control");
    ofSetWindowShape(WIDTH * .77, HEIGHT * .85);
    font.load("font.ttf",12);
    
    ofBackground(60,60,60);
    
    videos.allowExt("mov");
    videos.allowExt("mp4");
    
    videos.listDir("videos");

    /* MAPPING 
     */
    scaleX = 1;
    scaleY = 1;
    
    toMove = 1;
    toMove1 = 1;
    div = 1;
    div1 = 1;
    divOn = 48;
    div1On = 56;
    multiply = 1;
    mstate = 68;
    distance = 0;
    
    video.setPixelFormat(OF_PIXELS_RGBA);
    video.load("videos/1.mov");
    video.play();
    video1.setPixelFormat(OF_PIXELS_RGBA);
    video1.load("videos/2.mov");
    video1.play();
    
    W = video.getWidth();
    H = video.getHeight();
    W1 = video1.getWidth();
    H1 = video1.getHeight();
    
    pixels.allocate(W,H,OF_PIXELS_RGBA);
    texture.allocate(pixels);
    pixels1.allocate(W1,H1,OF_PIXELS_RGBA);
    texture1.allocate(pixels1);
    
    for(int i = 0; i < W; i++) {
        Pixels[i].allocate(1, H, OF_PIXELS_RGBA);
        textures[i].allocate(Pixels[i]);
    }
    for(int i = 0; i < W1; i++) {
        Pixels1[i].allocate(1, H1, OF_PIXELS_RGBA);
        textures1[i].allocate(Pixels1[i]);
    }
    
    for(int i = 0; i < 16; i++) {
        move[i] = distance;
        move1[i] = distance;
        up[i] = distance;
        up1[i] = 185;
    }
   
    //------------------------------------------
    slider[0].setColor(255,0,0,255);
    slider[1].setColor(0,255,0,255);
    slider[2].setColor(0,0,255,255);
    slider[3].setColor(127,127,127,255);
    
    sliderB[0].setColor(255,0,0,255);
    sliderB[1].setColor(0,255,0,255);
    sliderB[2].setColor(0,0,255,255);
    sliderB[3].setColor(127,127,127,255);
    
    colorPrevA = ofColor(255,255,255,255);
    colorPrevB = ofColor(255,255,255,255);
    
    for(int i = 0; i < 4; i++){
        slider[i].ySlide = 227;
        sliderB[i].ySlide = 454;
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    
    for(int i = 0; i < 4; i++){
        slider[i].onSlideV();
        sliderB[i].onSlideV();
    }
    video.update();
    video1.update();
    
    if(video.isFrameNew() && video1.isFrameNew()) {
        r = video.getPixels().getChannel(0);
        g = video.getPixels().getChannel(1);
        b = video.getPixels().getChannel(2);
        a = video.getPixels().getChannel(3);
        
        R = video1.getPixels().getChannel(0);
        G = video1.getPixels().getChannel(1);
        B = video1.getPixels().getChannel(2);
        A = video1.getPixels().getChannel(3);
        int c = 0;
        for(int i = 0; i < video.getPixels().size(); i += 4) {
            pixels[i] = r[c];
            pixels[i+1] = g[c];
            pixels[i+2] = b[c];
            pixels[i+3] = a[c];
            c++;
        }
        c = 0;
        for(int i = 0; i < video1.getPixels().size(); i += 4) {
            pixels1[i] = R[c];
            pixels1[i+1] = G[c];
            pixels1[i+2] = B[c];
            pixels1[i+3] = A[c];
            c++;
        }
        c = 0;
        for(int i = 0; i < H*4; i += 4) {
            for(int j = 0; j < W; j++) {
                for(int k = 0; k < 4; k++) {
                    Pixels[j][k+i] = pixels[c];
                    c++;
                }
            }
        }
        for(int i = 0; i < W; i++) {
            textures[i].loadData(Pixels[i]);
        }
        c = 0;
        for(int i = 0; i < H1*4; i += 4) {
            for(int j = 0; j < W1; j++) {
                for(int k = 0; k < 4; k++) {
                    Pixels1[j][k+i] = pixels1[c];
                    c++;
                }
            }
        }
        for(int i = 0; i < W1; i++) {
            textures1[i].loadData(Pixels1[i]);
        }
        texture.loadData(pixels);
        texture1.loadData(pixels1);
    }
    
    for(int i = 0; i < 16; i++) {
        if(i == (div-1)){
            butDiv[i].state = true;
        }
        else{
            butDiv[i].state = false;
        }
    }
    for(int i = 0; i < 16; i++) {
        if(i == (div1-1)){
            butDiv1[i].state = true;
        }
        else{
            butDiv1[i].state = false;
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){

    ofPushMatrix();
    ofTranslate(15,36);
    int c = 0;
    float W = WIDTH * 0.77;
    float H = HEIGHT * 0.25;
    
    /*for(int i = 0; i < 8; i++){
        for(int j = 0; j < 2; j++){
            ofNoFill();
            ofSetColor(111,111,111);
            ofSetLineWidth(1);
            ofDrawRectangle((((W/90)*i)+(W/9)*i)-1,(((H/8)*j)+(H/3)*j)-1,W/9+2,H/2.5+2);
            ofSetColor(255);
            previews[c].draw(((W/90)*i)+(W/9)*i,((H/8)*j)+(H/3)*j,W/9,H/2.5);
            ofFill();
            buttons[c].set(((W/90)*i)+(W/9)*i,((H/8)*j)+(H/3)*j,W/9,H/2.5);
            c++;
        }
    }*/
    c = 0;
    ofPopMatrix();
    ofSetColor(157,157,157);
    font.drawString("CineVivo BETA v. 0.01", 25, 25);
    
    float localW = ofGetWidth();
    float localH = ofGetHeight();
    
    for(int i = 0; i < 4; i++){
        slider[i].drawSliderV(localW/16 * (i+1),localH/3, localW/30, localH/4, 0);
        sliderB[i].drawSliderV(localW/16 * (i+1),localH/3 * 2, localW/30, localH/4, 0);
    }
    for(int i = 0; i < 8; i++){
        butDiv[i].button(localW/32 * (i+1)+430, localH/3, 20, 20);
        butDiv[i+8].button(localW/32 * (i+1)+675, localH/3, 20, 20);
    }
    for(int i = 0; i <= (div-1); i++){
         butMov[i].button(localW/32 * (i+1)+430, localH/3 + 26, 20, 20);
    }
    for(int i = 0; i < 8; i++){
        butDiv1[i].button(localW/32 * (i+1)+430, localH/3 * 2, 20, 20);
        butDiv1[i+8].button(localW/32 * (i+1)+675, localH/3 * 2, 20, 20);
    }
    for(int i = 0; i <= (div1-1); i++){
        butMov1[i].button(localW/32 * (i+1)+430, localH/3 * 2 + 26, 20, 20);
    }
    
    but1A.button(300,localH/3,30,30);
    font.drawString("Run", 300, localH/3 + 50);
    but2A.button(350,localH/3,30,30);
    font.drawString("Load", 346, localH/3 + 50);
    but3A.button(400,localH/3,30,30);
    font.drawString("Fx", 403, localH/3 + 50);
    
    but1B.button(300,localH/3 * 2,30,30);
    font.drawString("Run", 300, (localH/3*2) + 50);
    but2B.button(350,localH/3 * 2,30,30);
    font.drawString("Load", 346, (localH/3*2) + 50);
    but3B.button(400,localH/3 * 2,30,30);
    font.drawString("Fx", 403, (localH/3*2) + 50);
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
        case OF_KEY_UP:
            for(int i = 0; i < 16; i++){
                if(butMov[i].state == true){
                    up[i] -= 10;
                }
                if(butMov1[i].state == true){
                    up1[i] -= 10;
                }
            }
            break;
        case OF_KEY_DOWN:
            for(int i = 0; i < 16; i++){
                if(butMov[i].state == true){
                    up[i] += 10;
                }
                if(butMov1[i].state == true){
                    up1[i] += 10;
                }
            }
            break;
        case OF_KEY_LEFT:
            for(int i = 0; i < 16; i++){
                if(butMov[i].state == true){
                    move[i] -= 10;
                }
                if(butMov1[i].state == true){
                    move1[i] -= 10;
                }
            }
            break;
        case OF_KEY_RIGHT:
            for(int i = 0; i < 16; i++){
                if(butMov[i].state == true){
                    move[i] += 10;
                }
                if(butMov1[i].state == true){
                    move1[i] += 10;
                }
            }
            break;
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
    /*for(int i = 0; i < PREVIEWS; i++){
        if(buttons[i].onClick(ofGetMouseX(),ofGetMouseY())) {
            if(but2A.state == true){
                currentA = i;
            }
            if(but2B.state == true){
                currentB = i;
            }
        }
    }*/

    if(but1A.onClick(x,y)){
        but1A.state = !but1A.state;
    }
    if(but2A.onClick(x,y)){
        but2A.state = !but2A.state;
    }
    if(but3A.onClick(x,y)){
        but3A.state = !but3A.state;
    }
    
    if(but1B.onClick(x,y)){
        but1B.state = !but1B.state;
    }
    if(but2B.onClick(x,y)){
        but2B.state = !but2B.state;
    }
    if(but3B.onClick(x,y)){
        but3B.state = !but3B.state;
    }
    
    but1A.click(ofColor(111,111,111,255));
    but2A.click(ofColor(111,111,111,255));
    but3A.click(ofColor(111,111,111,255));
    but1B.click(ofColor(111,111,111,255));
    but2B.click(ofColor(111,111,111,255));
    but3B.click(ofColor(111,111,111,255));
    
    for(int i = 0; i < 16; i++){
        butDiv[i].click(ofColor(111,111,111,255));
        butDiv1[i].click(ofColor(111,111,111,255));
        butMov[i].click(ofColor(111,111,111,255));
        butMov1[i].click(ofColor(111,111,111,255));
        if(butDiv[i].onClick(x,y)){
            div = i+1;
        }
        if(butDiv1[i].onClick(x,y)){
            div1 = i+1;
        }
        if(butMov[i].onClick(x,y)){
            butMov[i].state = !butMov[i].state;
        }
        if(butMov1[i].onClick(x,y)){
            butMov1[i].state = !butMov1[i].state;
        }
    }
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
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    FILE = std::accumulate(begin(dragInfo.files),end(dragInfo.files),FILE);
    if (but2A.state == true) {
        video.stop();
        video.load(FILE);
        video.play();
        W = video.getWidth();
        H = video.getHeight();
        pixels.allocate(W,H,OF_PIXELS_RGBA);
        texture.allocate(pixels);
        for(int i = 0; i < W; i++) {
            Pixels[i].allocate(1, H, OF_PIXELS_RGBA);
            textures[i].allocate(Pixels[i]);
        }
    }
    if (but2B.state == true) {
        video1.stop();
        video1.load(FILE);
        video1.play();
        W1 = video1.getWidth();
        H1 = video1.getHeight();
        pixels1.allocate(W1,H1,OF_PIXELS_RGBA);
        texture1.allocate(pixels);
        for(int i = 0; i < W1; i++) {
            Pixels1[i].allocate(1, H1, OF_PIXELS_RGBA);
            textures1[i].allocate(Pixels1[i]);
        }
    }
    FILE.clear();
    dragInfo.files.clear();
}

#include "GuiApp.h"
#include <stdlib.h>

//--------------------------------------------------------------
void GuiApp::setup() {
    
    ofSetFrameRate(30);
    ofBackground(0);
    ofSetWindowTitle("CineVivo - Live Coding");
    
    ofSetLogLevel("ofxGLEditor", OF_LOG_NOTICE);
    
    // setup the global editor font before you do anything!
    ofxEditor::loadFont("fonts/font.ttf", 20);
    
    // set a custom Repl banner & prompt (do this before setup())
    ofxRepl::setReplBanner("CineVivo");
    ofxRepl::setReplPrompt("~> ");
    
    // setup editor with event listener
    editor.setup(this);
    
    colorScheme.loadFile("colorScheme.xml");
    syntax.loadFile("CVSyntax.xml");
    
    editor.setColorScheme(&colorScheme);
    editor.getSettings().addSyntax(&syntax);
    //editor.getSettings().printSyntaxes();
    
    // make some room for the bottom editor info text
    editor.resize(ofGetWidth(), ofGetHeight()-ofxEditor::getCharHeight());
    
    // change multi editor settings, see ofxEditorSettings.h for details
    //editor.getSettings().setTextColor(ofColor::red); // main text color
    //editor.getSettings().setCursorColor(ofColor::blue); // current pos cursor
    //editor.getSettings().setAlpha(0.5); // main text, cursor, & highlight alpha
    
    // other settings
    editor.setLineWrapping(true);
    editor.setLineNumbers(true);
    editor.setAutoFocus(false);
    
    // move the cursor to a specific line
    //editor.setCurrentLine(4);
    //ofLogNotice() << "current line: " << editor.getCurrentLine();
    
    osc.setup("127.0.0.1",5612);
    
}

//--------------------------------------------------------------
void GuiApp::draw() {
    
    editor.draw();
    
    // draw current editor info using the same font as the editor
    if(!editor.isHidden() && editor.getCurrentEditor() > 0) {
        
        int bottom = ofGetHeight()-ofxEditor::getCharHeight();
        int right = ofGetWidth()-ofxEditor::getCharWidth()*7; // should be enough room
        //ofSetColor(ofColor::gray);
        
        // draw the current editor num
        editor.drawString("Editor: "+ofToString(editor.getCurrentEditor()),
                          0, bottom);
        
        // draw the current line & line pos
        editor.drawString(ofToString(editor.getCurrentLine()+1)+","+
                          ofToString(editor.getCurrentLinePos()),
                          right, bottom);
    }
}

//--------------------------------------------------------------
void GuiApp::keyPressed(int key) {
    
    // note: when using CTRL as the modifier key, CTRL + key might be an ascii
    // control char so check for both the char 'f' and 6, the ascii value for
    // CTRL + f, see also: http://ascii-table.com/control-chars.php
    bool modifier = ofGetKeyPressed(ofxEditor::getSuperAsModifier() ? OF_KEY_SUPER : OF_KEY_CONTROL);
    if(modifier) {
        switch(key) {
            case 'f': case 6:
                ofToggleFullscreen();
                return;
            case 'l': case 12:
                editor.setLineWrapping(!editor.getLineWrapping());
                return;
            case 'n': case 14:
                editor.setLineNumbers(!editor.getLineNumbers());
                return;
            case 'k': case 26:
                editor.setAutoFocus(!editor.getAutoFocus());
                return;
        }
    }
    
    // see ofxGLEditor.h for key commands
    editor.keyPressed(key);
}

//--------------------------------------------------------------
void GuiApp::windowResized(int w, int h) {
    // make some room for the bottom editor info text
    editor.resize(w, h-ofxEditor::getCharHeight());
}

//--------------------------------------------------------------
void GuiApp::openFileEvent(int &whichEditor) {
    // received an editor open via CTRL/Super + o
    
    ofLogNotice() << "received open event for editor " << whichEditor
    << " with filename " << editor.getEditorFilename(whichEditor);
}

//--------------------------------------------------------------
void GuiApp::saveFileEvent(int &whichEditor) {
    // received an editor save via CTRL/Super + s or CTRL/Super + d
    
    ofLogNotice() << "received save event for editor " << whichEditor
    << " with filename " << editor.getEditorFilename(whichEditor);
}

//--------------------------------------------------------------
void GuiApp::executeScriptEvent(int &whichEditor) {
    // received on editor CTRL/Super + e
    
    string txt = editor.getText(whichEditor);
    lineas = ofSplitString(txt, "\n");
    
    for(int i = 0; i < lineas.size();i++){
        ofxOscMessage s;
        if(lineas[i].size() != 0){
            texto = ofSplitString(lineas[i], " ");
            if(texto[0] == "windowShape" && texto.size() == 3){
                string temp = "/windowShape " + texto[1] + " " + texto[2];
                ofLogNotice() << "CineVivo[send]: " << temp;
                s.setAddress("/windowShape");
                s.addIntArg(ofToInt(texto[1]));
                s.addIntArg(ofToInt(texto[2]));
                osc.sendMessage(s);
            }
            if(texto[0] == "fullscreen" && texto.size() == 2){
                string temp = "/fullscreen " + texto[1];
                ofLogNotice() << "CineVivo[send]: " << temp;
                s.setAddress("/fullscreen");
                s.addIntArg(ofToInt(texto[1]));
                osc.sendMessage(s);
            }
            if(texto[0] == "clean" && texto.size() == 1){
                string temp = "/clean ";
                ofLogNotice() << "CineVivo[send]: " << temp;
                s.setAddress("/clean");
                s.addIntArg(0);
                osc.sendMessage(s);
            }
            if(texto[0] == "fitAllHorizontal" && texto.size() == 1){
                string temp = "/fitAllHorizontal ";
                ofLogNotice() << "CineVivo[send]: " << temp;
                s.setAddress("/fitAllHorizontal");
                s.addIntArg(0);
                osc.sendMessage(s);
            }
            if(texto[0] == "fitAllVertical" && texto.size() == 1){
                string temp = "/fitAllVertical ";
                ofLogNotice() << "CineVivo[send]: " << temp;
                s.setAddress("/fitAllVertical");
                s.addIntArg(0);
                osc.sendMessage(s);
            }
            
            int numV = -1;
            for(int i = 0; i < 10;i++){
                if(ofIsStringInString(digit[i], texto[0]))
                    numV = ofToInt(texto[0]);
            }
            
            if(numV >= 0 && numV <= 7){
                if(texto[1] == "load" && texto.size() == 3){
                    string temp = "/load " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/load");
                    s.addIntArg(numV);
                    s.addStringArg(texto[2]);
                    osc.sendMessage(s);
                }
                if(texto[1] == "free"&& texto.size() == 2){
                    string temp = "/free " + ofToString(numV);
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/free");
                    s.addIntArg(numV);
                    osc.sendMessage(s);
                }
                if(texto[1] == "mask" && texto.size() == 3){
                    string temp = "/mask " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/mask");
                    s.addIntArg(numV);
                    s.addStringArg(texto[2]);
                    osc.sendMessage(s);
                }
                if(texto[1] == "rectMode" && texto.size() == 3){
                    string temp = "/rectMode " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/rectMode");
                    s.addIntArg(numV);
                    s.addIntArg(ofToInt(texto[2]));
                    osc.sendMessage(s);
                }
                if(texto[1] == "pos" && texto.size() == 4){
                    string temp = "/pos " + ofToString(numV) + " " + texto[2] + " " + texto[3];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/pos");
                    s.addIntArg(numV);
                    s.addIntArg(ofToInt(texto[2]));
                    s.addIntArg(ofToInt(texto[3]));
                    osc.sendMessage(s);
                }
                if(texto[1] == "posX" && texto.size() == 3){
                    string temp = "/posX " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/posX");
                    s.addIntArg(numV);
                    s.addIntArg(ofToInt(texto[2]));
                    osc.sendMessage(s);
                }
                if(texto[1] == "posY" && texto.size() == 3){
                    string temp = "/posY " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/posY");
                    s.addIntArg(numV);
                    s.addIntArg(ofToInt(texto[2]));
                    osc.sendMessage(s);
                }
                if(texto[1] == "rot" && texto.size() == 5){
                    string temp = "/rotX " + ofToString(numV) + " " + texto[2] + " " + texto[3] + " " + texto[4];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/rot");
                    s.addIntArg(numV);
                    s.addFloatArg(ofToFloat(texto[2]));
                    s.addFloatArg(ofToFloat(texto[3]));
                    s.addFloatArg(ofToFloat(texto[4]));
                    osc.sendMessage(s);
                }
                if(texto[1] == "rotX" && texto.size() == 3){
                    string temp = "/rotX " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/rotX");
                    s.addIntArg(numV);
                    s.addFloatArg(ofToFloat(texto[2]));
                    osc.sendMessage(s);
                }
                if(texto[1] == "rotY" && texto.size() == 3){
                    string temp = "/rotY " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/rotY");
                    s.addIntArg(numV);
                    s.addFloatArg(ofToFloat(texto[2]));
                    osc.sendMessage(s);
                }
                if(texto[1] == "rotZ" && texto.size() == 3){
                    string temp = "/rotZ " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/rotZ");
                    s.addIntArg(numV);
                    s.addFloatArg(ofToFloat(texto[2]));
                    osc.sendMessage(s);
                }
                if(texto[1] == "size" && texto.size() == 4){
                    string temp = "/sizeX " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/size");
                    s.addIntArg(numV);
                    s.addIntArg(ofToInt(texto[2]));
                    s.addIntArg(ofToInt(texto[3]));
                    osc.sendMessage(s);
                }
                if(texto[1] == "width" && texto.size() == 3){
                    string temp = "/width " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/width");
                    s.addIntArg(numV);
                    s.addIntArg(ofToInt(texto[2]));
                    osc.sendMessage(s);
                }
                if(texto[1] == "height" && texto.size() == 3){
                    string temp = "/height " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/height");
                    s.addIntArg(numV);
                    s.addIntArg(ofToInt(texto[2]));
                    osc.sendMessage(s);
                }
                if(texto[1] == "scale" && texto.size() == 4){
                    string temp = "/scale " + ofToString(numV) + " " + texto[2] + " " + texto[3];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/scale");
                    s.addIntArg(numV);
                    s.addFloatArg(ofToFloat(texto[2]));
                    s.addFloatArg(ofToFloat(texto[3]));
                    osc.sendMessage(s);
                }
                if(texto[1] == "speed" && texto.size() == 3){
                    string temp = "/speed " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/speed");
                    s.addIntArg(numV);
                    s.addFloatArg(ofToFloat(texto[2]));
                    osc.sendMessage(s);
                }
                if(texto[1] == "play" && texto.size() == 2){
                    string temp = "/play " + ofToString(numV);
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/play");
                    s.addIntArg(numV);
                    osc.sendMessage(s);
                }
                if(texto[1] == "stop" && texto.size() == 2){
                    string temp = "/stop " + ofToString(numV);
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/stop");
                    s.addIntArg(numV);
                    osc.sendMessage(s);
                }
                if(texto[1] == "pause" && texto.size() == 2){
                    string temp = "/pause " + ofToString(numV);
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/pause");
                    s.addIntArg(numV);
                    osc.sendMessage(s);
                }
                if(texto[1] == "setFrame" && texto.size() == 3){
                    string temp = "/setFrame " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/setFrame");
                    s.addIntArg(ofToInt(texto[2]));
                    osc.sendMessage(s);
                }
                if(texto[1] == "color" && texto.size() == 5){
                    string temp = "/color " + ofToString(numV) + " " + texto[2] + " " + texto[3] + " " + texto[4];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/color");
                    s.addIntArg(numV);
                    s.addIntArg(ofToInt(texto[2]));
                    s.addIntArg(ofToInt(texto[3]));
                    s.addIntArg(ofToInt(texto[4]));
                    osc.sendMessage(s);
                }
                if(texto[1] == "opacity" && texto.size() == 3){
                    string temp = "/opacity " + ofToString(numV) + " " + texto[2];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/opacity");
                    s.addIntArg(numV);
                    s.addIntArg(ofToInt(texto[2]));
                    osc.sendMessage(s);
                }
                if(texto[1] == "fit" && texto.size() == 2){
                    string temp = "/fit " + ofToString(numV) + " " + texto[0];
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/fit");
                    s.addIntArg(numV);
                    s.addIntArg(ofToInt(texto[0]));
                    osc.sendMessage(s);
                }
                if(texto[1] == "fitHorizontal" && texto.size() == 2){
                    string temp = "/fitHorizontal " + ofToString(numV);
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/fitHorizontal");
                    s.addIntArg(numV);
                    osc.sendMessage(s);
                }
                if(texto[1] == "fitVertical" && texto.size() == 2){
                    string temp = "/fitVertical " + ofToString(numV);
                    ofLogNotice() << "CineVivo[send]: " << temp;
                    s.setAddress("/fitVertical");
                    s.addIntArg(numV);
                    osc.sendMessage(s);
                }
            }
        }
    }
}

//--------------------------------------------------------------
void GuiApp::evalReplEvent(const string &text) {
    
    if(text == "list-videos"){
        videosFolder.listDir("videos");
        string temp;
        for(int i = 0; i < videosFolder.size(); i++){
            temp = temp + " " + videosFolder.getName(i);
        }
        editor.evalReplReturn(temp);
    }
    if(text == "list-mask"){
        maskFolder.listDir("mask");
        string temp;
        for(int i = 0; i < maskFolder.size(); i++){
            temp = temp + " " + maskFolder.getName(i);
        }
        editor.evalReplReturn(temp);
    }
    if(text == "help"){
        editor.openFile("help.txt", 9);
        editor.evalReplReturn("Help opened in buffer 9");
    }
    if(text == "keywords"){
        editor.openFile("keywords.txt", 9);
        editor.evalReplReturn("Keywords reference opened in buffer 9");
    }
}

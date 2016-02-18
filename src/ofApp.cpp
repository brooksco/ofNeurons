#include "ofApp.h"
#include "nMesh.h"

// Resize factor
int rFactor = 12;

// Edge detection distance
float hy = 1;

bool one = false;
bool two = true;
bool three = false;
bool four = false;
bool five = false;

// Canvas center vector
ofVec3f cCenter;

// Screen diagonal
float screenH;

ofSoundPlayer musicPlayer;
float soundNoise;


//--------------------------------------------------------------
void ofApp::setup(){
    bool succ = true;
    
    // Load image
    succ = image.load("bear.jpg");
    int newWidth = image.getWidth() / rFactor;
    int newHeight = image.getHeight() / rFactor;
    
    image.resize(newWidth, newHeight);
    
    if (!succ) {
        cerr << "loading image failed ...\n";
    }
    
    
    cCenter = ofVec3f(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2, 0.0);
    
    // Set screen diagonal
    screenH = sqrt(pow(ofGetWindowWidth(), 2) +  (pow(ofGetWindowHeight(), 2)));
    
    bool below = false;
    bool above = false;
    
    float intensityThreshold = 140.0;
    int w = image.getWidth();
    int h = image.getHeight();
    
    cout << "Width" << endl;
    cout << w << endl;
    cout << "Height" << endl;
    cout << h << endl;
    
    for (int x = 0; x < w; x++) {
        // Need to skip the last row, figure out why later
        for (int y = 0; y < (h - 1); y++) {
            ofColor c = image.getColor(x, y);
            float intensity = c.getLightness();
            
            
            if (intensity >= intensityThreshold) {
                ofVec3f pos(x, y, 0.0);
                
                // Need to go through and see if we're near an edge
                
                float a = 0;
                float o = 0;
                below = false;
                above = false;
                bool skip = false;
                
                for (int i = 0; i <= 360; i += 90) {
                    // Horizontal value
                    a = pos.x + (hy * cos(ofDegToRad(i)));
                    o = pos.y + (hy * sin(ofDegToRad(i)));
                    
                    // If we're before the 0, adjust
                    if (a < 0) {
                        //                        skip = true;
                        a = 0;
                        
                        // If we're outside the width range, adjust
                    } else if (a > (w)) {
                        //                        skip = true;
                        //                        a = (w * rFactor);
                        a = w;
                    }
                    
                    // If we're below the 0, adjust
                    if (o < 0) {
                        //                        skip = true;
                        o = 0;
                        
                        // wrong
                        // If...this is all weird, there's some issue at the bottom of the image
                        // } else if (o > (h - hy)) {
                    } else if (o > (h)) {
                        //                        skip = true;
                        //                        o = (h * rFactor);
                        o = h;
                    }
                    
                    // Get the color then get the lightness value
                    ofColor cc = image.getColor(a, o);
                    float intensityI = cc.getLightness();
                    
                    // If the it's lighter than the threshold, mark it as "above"
                    if (intensityI > intensityThreshold) {
                        above = true;
                        
                        // If it's below the threshold, mark it as "below"
                    } else if (intensityI <= intensityThreshold) {
                        below = true;
                    }
                    
                    
                }
                
                // If values both above & below threshold are present, we're by an edge
                if ((below == true) && (above == true)) {
                    // Add it to the points for the prototype
                    prototypePoints.push_back((pos * rFactor));
                }
                
            }
        }
    }
    
    // Create the first mesh
    nMesh mesh1 = nMesh(prototypePoints, 1);
    nMeshes.push_back(mesh1);

    
//    soundPlayer.load("chime.wav");
    soundPlayer.load("recordstatic_single.wav");
    soundPlayer.setMultiPlay(true);
    
    staticPlayer.load("sndhairshort.wav");
//    staticPlayer.load("recordstatic.mp3");
    staticPlayer.setMultiPlay(true);
    
//    musicPlayer.load("tunnelsandplanes.mp3"); // pretty good!
    musicPlayer.load("whereismymind.mp3");
    musicPlayer.setSpeed(.5);
    musicPlayer.setLoop(true);
    
    mute = true;
    
    if (mute == false) {
        musicPlayer.play();
        musicPlayer.setPosition(ofRandom(0, 1.0));

    }

    soundNoise = 0;
    
}

//--------------------------------------------------------------
void ofApp::update(){
    for (int i = 0; i < nMeshes.size(); i++) {
        
        nMeshes[i].update();
        
        if (nMeshes[i].alphaFactor == 0) {
            nMeshes.erase(nMeshes.begin() + i);
            //            delete * nMeshes[i];
            
            //            cout << nMeshes.size() << endl;
        }
    }
    
    
    
    if (nMeshes.size() < 40) {
        
        int type = 3;
        
        if (ofRandom(10) > 6) {
            type = 1;
            
        } else if (ofRandom(10) > 7) {
            type = 2;
        }
        
        
        nMesh newMesh = nMesh(prototypePoints, type);
        nMeshes.push_back(newMesh);
    }
    
    soundNoise += .001;

    float soundNoise = ofNoise(soundNoise);
    musicPlayer.setVolume(ofNoise(soundNoise) * .5);
    
    musicPlayer.setSpeed(ofMap(soundNoise, 0, 1, .1, 1.0));

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0, 0, 0);
    //    ofColor(0, 0, 0, 100);
    
    for (int i = 0; i < nMeshes.size(); i++) {
        if (one == true) {
            nMeshes[i].mesh.setMode(OF_PRIMITIVE_POINTS);
            
        } else if (two == true) {
            nMeshes[i].mesh.setMode(OF_PRIMITIVE_LINE_LOOP);
            
        } else if (three == true) {
            nMeshes[i].mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
            
        } else if (four == true) {
            nMeshes[i].mesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
            
        } else if (five == true) {
            nMeshes[i].mesh.setMode(OF_PRIMITIVE_LINES_ADJACENCY);
            
        } else {
            nMeshes[i].mesh.setMode(OF_PRIMITIVE_POINTS);
        }
        
        if (nMeshes[i].type == 1) {
            nMeshes[i].mesh.setMode(OF_PRIMITIVE_LINE_LOOP);
            
        } else if (nMeshes[i].type == 2) {
            nMeshes[i].mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
            
        } else if (nMeshes[i].type == 3) {
            nMeshes[i].mesh.setMode(OF_PRIMITIVE_POINTS);
        }
        
        
        
        nMeshes[i].mesh.draw();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if (key == '1') {
        one = !one;
        two = false;
        three = false;
        four = false;
        five = false;
    }
    
    if (key == '2') {
        two = !two;
        one = false;
        three = false;
        four = false;
        five = false;
    }
    
    if (key == '3') {
        three = !three;
        one = false;
        two = false;
        four = false;
        five = false;
    }
    
    if (key == '4') {
        four = !four;
        one = false;
        two = false;
        three = false;
        five = false;
    }
    
    if (key == '5') {
        five = !five;
        one = false;
        two = false;
        three = false;
        four = false;
    }
    
    if (key == 'm') {
        mute = !mute;
        
        if (mute == false) {
            cout << "Unmuted" << endl;
            musicPlayer.play();
            
        } else {
            cout << "Muted" << endl;
            musicPlayer.stop();
            soundPlayer.stop();
            staticPlayer.stop();
        }
        
    }
    
    if (key == '=') {
        nMesh newMesh = nMesh(prototypePoints, ofRandom(3));
        nMeshes.push_back(newMesh);
        
        cout << nMeshes.size() << endl;
    }
    
    if (key == '-') {
        if (nMeshes.size() >= 1) {
            nMeshes.pop_back();
        }
        
        cout << nMeshes.size() << endl;
    }
    
    if (key == OF_KEY_RIGHT) {
        for (int i = 0; i < prototypePoints.size(); i++) {
            prototypePoints[i].x = (prototypePoints[i].x + 1);
        }
        
    }
    
    if (key == OF_KEY_LEFT) {
        for (int i = 0; i < prototypePoints.size(); i++) {
            prototypePoints[i].x = (prototypePoints[i].x - 1);
        }
        
    }
    
    if (key == OF_KEY_UP) {
        for (int i = 0; i < prototypePoints.size(); i++) {
            prototypePoints[i].y = (prototypePoints[i].y - 1);
        }
        
    }
    
    if (key == OF_KEY_DOWN) {
        for (int i = 0; i < prototypePoints.size(); i++) {
            prototypePoints[i].y = (prototypePoints[i].y + 1);
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
    
}

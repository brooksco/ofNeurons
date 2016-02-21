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

bool bSoundToggle;
bool bMusicToggle;

bool showGui = true;

// Canvas center vector
ofVec3f cCenter;

// Screen diagonal
float screenH;

ofSoundPlayer musicPlayer;
float soundNoise;

bool activeSensing = true;

int drawCenter = 0;


//--------------------------------------------------------------
void ofApp::setup(){
    
    //    cCenter = ofVec3f(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2, 0.0);
    cCenter = ofVec3f(640 / 2, 480 / 2, 0.0);
    
    // Set screen diagonal
    //    screenH = sqrt(pow(ofGetWindowWidth(), 2) + (pow(ofGetWindowHeight(), 2)));
    screenH = sqrt(pow(640, 2) + (pow(480, 2)));
    // Kinect setup
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    // enable depth->video image calibration
    kinect.setRegistration(true);
    kinect.init();
    //    kinect.init(true); // shows infrared instead of RGB video image
    //kinect.init(false, false); // disable video image (faster fps)
    kinect.open();
    
    // Allocate space for openCV images
    colorImg.allocate(kinect.width, kinect.height);
    grayImage.allocate(kinect.width, kinect.height);
    grayThreshNear.allocate(kinect.width, kinect.height);
    grayThreshFar.allocate(kinect.width, kinect.height);
    edgeImage.allocate(kinect.width, kinect.height);
    
    
    ofSetFrameRate(60);
    bKinectOpen = true ;
    
    int guiWidth = 300;
    float totalArea = kinect.width * kinect.height;
    
    
    
    
    gui.setDefaultWidth(guiWidth);
    gui.setup();
    gui.setPosition(800, 20);
    
    // Depth group
    depthGroup.setup("Kinect depth range");
    depthGroup.add(nearDepthSlider.setup("near", 255, 0, 255));
    depthGroup.add(farDepthSlider.setup("far", 215, 0, 255));
    
    gui.add(&depthGroup);
    depthGroup.setWidthElements(guiWidth);
    
    // Blob group
    blobGroup.setup("Blob size");
    blobGroup.add(minBlobSlider.setup("min size", totalArea / 20, 0, totalArea));
    blobGroup.add(maxBlobSlider.setup("max size", totalArea / 2, 0, totalArea));
    
    gui.add(&blobGroup);
    blobGroup.setWidthElements(guiWidth);
    
    // Position group
    posGroup.setup("Position");
    posGroup.add(xPosSlider.setup("x", 0, -kinect.width / 2, kinect.width / 2));
    posGroup.add(yPosSlider.setup("y", 0, -kinect.height / 2, kinect.height / 2));
    posGroup.add(zPosSlider.setup("z", 0, -1000, 1000));
    
    gui.add(&posGroup);
    posGroup.setWidthElements(guiWidth);
    
    // Center position group
    centerPosGroup.setup("Center position");
    centerPosGroup.add(xCenterPosSlider.setup("center x", kinect.width / 2, 0, kinect.width));
    centerPosGroup.add(yCenterPosSlider.setup("center y", kinect.height / 2, 0, kinect.height));
    
    gui.add(&centerPosGroup);
    centerPosGroup.setWidthElements(guiWidth);
    
    // Misc group
    miscGroup.setup("Misc.");
    miscGroup.add(motorSlider.setup("motor angle", 12, -30.0, 30.0));
    miscGroup.add(calibrateViewToggle.setup("calibrate views", true));
    miscGroup.add(easyCamToggle.setup("easyCam", false));
    
    bSoundToggle = false;
    
    miscGroup.add(soundToggle.setup("sound", bSoundToggle));
    miscGroup.add(musicToggle.setup("music", false));
    
    gui.add(&miscGroup);
    miscGroup.setWidthElements(guiWidth);
    
    // Load previous settings
    gui.loadFromFile("settings.xml");
    
    
    // Music handling
    
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
    
    if (musicToggle == true) {
        musicPlayer.play();
        musicPlayer.setPosition(ofRandom(0, 1.0));
        
    }
    
    soundNoise = 0;
    
}

//--------------------------------------------------------------
void ofApp::update() {
    
    // Update from GUI control values
    //    bThreshWithOpenCV = toggle;
    minBlobSize = minBlobSlider;
    maxBlobSize = maxBlobSlider;
    nearThreshold = nearDepthSlider;
    farThreshold = farDepthSlider;
    
    pointCloudMinZ = minPointSlider;
    pointCloudMaxZ = maxPointSlider;
    
    cCenter.x = xCenterPosSlider;
    cCenter.y = yCenterPosSlider;
    
    kinect.setCameraTiltAngle(motorSlider);
    
    // Handle toggling sounds and music
    
    if ((soundToggle == false) && (bSoundToggle == true)) {
        bSoundToggle = false;
        
    } else if ((soundToggle == true) && (bSoundToggle == false)) {
        bSoundToggle = true;
    }
    
    if ((musicToggle == false) && (bMusicToggle == true)) {
        bMusicToggle = false;
        musicPlayer.stop();
        
    } else if ((musicToggle == true) && (bMusicToggle == false)) {
        bMusicToggle = true;
        musicPlayer.play();
        musicPlayer.setPosition(ofRandom(0, 1.0));
    }
    
    
    ofSetWindowTitle( "ofNeurons w/ Kinect - FPS:"+ ofToString( ofGetElapsedTimef() ) ) ;
    kinect.update();
    
    
    // there is a new frame and we are connected
    if (kinect.isFrameNew()) {
        
        // load grayscale depth image from the kinect source
        grayImage.setFromPixels(kinect.getDepthPixels());
        
        // we do two thresholds - one for the far plane and one for the near plane
        // we then do a cvAnd to get the pixels which are a union of the two thresholds
        
        grayThreshNear = grayImage;
        grayThreshFar = grayImage;
        grayThreshNear.threshold(nearThreshold, true);
        grayThreshFar.threshold(farThreshold);
        cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
        
        
        // update the cv images
        grayImage.flagImageChanged();
        
        
        // Find contours
        contourFinder.findContours(grayImage, minBlobSize , maxBlobSize , 20, false);
        
        
        // DEFINITELY going to need to alter this so it isn't such a mess to make so many new ones all at once
        
        // this can move further out later
        if (nMeshes.size() < 40) {
            // Play around with this later...
            
            int newCount = ofRandom(1, 10);
            for (int j = nMeshes.size(); j < newCount; j++) {
                
                for (int i = 0; i < contourFinder.blobs.size(); i++) {
                    int type = 3;
                    
                    if (ofRandom(10) > 6) {
                        type = 1;
                        
                    } else if (ofRandom(10) > 7) {
                        type = 2;
                    }
                    
                    // NEED TO store a vector since there may be multiple blobs, and have protype points be based on that vector
                    
                    // Copy current points to protype
                    prototypePoints = contourFinder.blobs[i].pts;
                    
                    // This is awful but shit...let's see how bad it makes it
                    for (int jj = 0; jj < prototypePoints.size(); jj++) {
                        // Pull kinect z data at the spot
                        ofVec3f kVec = kinect.getWorldCoordinateAt(prototypePoints[jj].x, prototypePoints[jj].y);
                        // 500 is about 2 feet, 1000 is about 10 feet. Max is 2048, min is 0

                        prototypePoints[jj].z = ofMap(kVec.z, 500, 1000, -400, 400);
                    }
                    

                    // Create a new nMesh, adding the points and setting the type
                    nMesh newMesh = nMesh(prototypePoints, type);
                    nMeshes.push_back(newMesh);
                    
                }
            }
            
        } else {
            // Might want to do a more gradual adding of meshes if it's over some threshold
        }
        
        
    }
    
    
    for (int i = 0; i < nMeshes.size(); i++) {
        
        nMeshes[i].update();
        
        if (nMeshes[i].alphaFactor == 0) {
            nMeshes.erase(nMeshes.begin() + i);
            //            delete * nMeshes[i];
            
            //            cout << nMeshes.size() << endl;
        }
    }
    
    // Check to make sure we have points
    if (prototypePoints.size() > 0) {
        if (nMeshes.size() < 20) {
            
            int type = 3;
            
            if (ofRandom(10) > 6) {
                type = 1;
                
            } else if (ofRandom(10) > 7) {
                type = 2;
            }
            
            
            nMesh newMesh = nMesh(prototypePoints, type);
            nMeshes.push_back(newMesh);
        }

    }
    
    
    if (calibrateViewToggle == true) {
        // Run canny edge detection and stuff it in the edge image
        cvCanny(grayImage.getCvImage(), edgeImage.getCvImage(), 0, 0);
        
        edgeImage.flagImageChanged();
        
    }
    
    soundNoise += .001;
    
    float soundNoise = ofNoise(soundNoise);
    musicPlayer.setVolume(ofNoise(soundNoise) * .5);
    
    musicPlayer.setSpeed(ofMap(soundNoise, 0, 1, .1, 1.0));
    
//    easyCam.setPosition(-ofGetWidth()/2, -ofGetHeight()/2, 0);
//    easyCam.setTarget(ofVec3f(1, -1, 1));
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0, 0, 0);
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    glPointSize(2);
    
    
    ofPushMatrix();
    
    if (easyCamToggle == true) {
        easyCam.begin();
        ofScale(1,-1,1);
        ofTranslate(-ofGetWidth()/2, -ofGetHeight()/2);
    }

    // For now make the assumption that the projection is always landscape format
    //    ofScale((float) ofGetWindowWidth() / 640, (float) ofGetWindowHeight() / 480);
    float scaleRatio = ofGetWindowWidth() / 640;
    ofScale(scaleRatio, scaleRatio);
    
    // Translate based on sliders and on initially centering the height
    ofTranslate(xPosSlider, yPosSlider -(((kinect.height / 2) * scaleRatio) - (ofGetWindowHeight() / 2)) / 4, zPosSlider);
    
    for (int i = 0; i < nMeshes.size(); i++) {
        
        if (nMeshes[i].type == 1) {
            nMeshes[i].mesh.setMode(OF_PRIMITIVE_LINE_LOOP);
            
        } else if (nMeshes[i].type == 2) {
            nMeshes[i].mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
            
        } else if (nMeshes[i].type == 3) {
            nMeshes[i].mesh.setMode(OF_PRIMITIVE_POINTS);
        }
        
        nMeshes[i].mesh.draw();
    }
    
    if (calibrateViewToggle == true) {
        ofFill();
        ofSetColor(255, 255, 255, 127);
        ofDrawBox(cCenter.x - 5, cCenter.y - 5, 0, 10, 10, 10);
    }
    
    if (easyCamToggle == true) {
        easyCam.end();
    }
    
    ofPopMatrix();
    
    
    // If we're calibrating, draw the box at the center vector so we can move it easily
    if (calibrateViewToggle == true) {
        
        // draw from the live kinect
        kinect.drawDepth(10, 10, 400, 300);
        kinect.draw((ofGetWindowWidth() - 410), 10, 400, 300);
        
        grayImage.draw(10, (ofGetWindowHeight() - 310), 400, 300);
        contourFinder.draw(10, (ofGetWindowHeight() - 310), 400, 300);
        
        edgeImage.draw((ofGetWindowWidth() - 410), (ofGetWindowHeight() - 310), 400, 300);
    }
    
    
    if (showGui) {
        gui.draw();
    }
    
}

void ofApp::exit() {
    gui.saveToFile("settings.xml");
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if (key == OF_KEY_RETURN) {
        showGui = !showGui;
    }
    
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


// Old


//    bool succ = true;
//
//    // Load image
//    succ = image.load("bear.jpg");
//    int newWidth = image.getWidth() / rFactor;
//    int newHeight = image.getHeight() / rFactor;
//
//    image.resize(newWidth, newHeight);
//
//    if (!succ) {
//        cerr << "loading image failed ...\n";
//    }
//
//

//
//    bool below = false;
//    bool above = false;
//
//    float intensityThreshold = 140.0;
//    int w = image.getWidth();
//    int h = image.getHeight();
//
//    cout << "Width" << endl;
//    cout << w << endl;
//    cout << "Height" << endl;
//    cout << h << endl;
//
//    for (int x = 0; x < w; x++) {
//        // Need to skip the last row, figure out why later
//        for (int y = 0; y < (h - 1); y++) {
//            ofColor c = image.getColor(x, y);
//            float intensity = c.getLightness();
//
//
//            if (intensity >= intensityThreshold) {
//                ofVec3f pos(x, y, 0.0);
//
//                // Need to go through and see if we're near an edge
//
//                float a = 0;
//                float o = 0;
//                below = false;
//                above = false;
//                bool skip = false;
//
//                for (int i = 0; i <= 360; i += 90) {
//                    // Horizontal value
//                    a = pos.x + (hy * cos(ofDegToRad(i)));
//                    o = pos.y + (hy * sin(ofDegToRad(i)));
//
//                    // If we're before the 0, adjust
//                    if (a < 0) {
//                        //                        skip = true;
//                        a = 0;
//
//                        // If we're outside the width range, adjust
//                    } else if (a > (w)) {
//                        //                        skip = true;
//                        //                        a = (w * rFactor);
//                        a = w;
//                    }
//
//                    // If we're below the 0, adjust
//                    if (o < 0) {
//                        //                        skip = true;
//                        o = 0;
//
//                        // wrong
//                        // If...this is all weird, there's some issue at the bottom of the image
//                        // } else if (o > (h - hy)) {
//                    } else if (o > (h)) {
//                        //                        skip = true;
//                        //                        o = (h * rFactor);
//                        o = h;
//                    }
//
//                    // Get the color then get the lightness value
//                    ofColor cc = image.getColor(a, o);
//                    float intensityI = cc.getLightness();
//
//                    // If the it's lighter than the threshold, mark it as "above"
//                    if (intensityI > intensityThreshold) {
//                        above = true;
//
//                        // If it's below the threshold, mark it as "below"
//                    } else if (intensityI <= intensityThreshold) {
//                        below = true;
//                    }
//
//
//                }
//
//                // If values both above & below threshold are present, we're by an edge
//                if ((below == true) && (above == true)) {
//                    // Add it to the points for the prototype
//                    prototypePoints.push_back((pos * rFactor));
//                }
//
//            }
//        }
//    }
//
//    // Create the first mesh
//    nMesh mesh1 = nMesh(prototypePoints, 1);
//    nMeshes.push_back(mesh1);
//
//





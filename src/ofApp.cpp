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
    //        screenH = sqrt(pow(ofGetWindowWidth(), 2) + (pow(ofGetWindowHeight(), 2)));
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
    gui.setPosition(900, 20);
    
    // Depth group
    depthGroup.setup("Kinect depth range");
    depthGroup.add(nearDepthSlider.setup("near", 255, 0, 255));
    depthGroup.add(farDepthSlider.setup("far", 215, 0, 255));
    
    gui.add(&depthGroup);
    depthGroup.setWidthElements(guiWidth);
    
    // Blob group
    blobGroup.setup("Blob size");
    blobGroup.add(minBlobSlider.setup("min size", totalArea / 20, 0, totalArea / 4));
    blobGroup.add(maxBlobSlider.setup("max size", totalArea / 4, 0, totalArea / 4));
    
    gui.add(&blobGroup);
    blobGroup.setWidthElements(guiWidth);
    
    // Position group
    posGroup.setup("Position");
    posGroup.add(xPosSlider.setup("x", 0, -kinect.width / 2, kinect.width / 2));
    posGroup.add(yPosSlider.setup("y", 0, -kinect.height / 2, kinect.height / 2));
    posGroup.add(zPosSlider.setup("z", 0, -2000, 1000));
    
    gui.add(&posGroup);
    posGroup.setWidthElements(guiWidth);
    
    // Center position group
    centerPosGroup.setup("Center position");
    centerPosGroup.add(xCenterPosSlider.setup("center x", kinect.width / 2, 0, kinect.width));
    centerPosGroup.add(yCenterPosSlider.setup("center y", kinect.height / 2, 0, kinect.height));
    
    gui.add(&centerPosGroup);
    centerPosGroup.setWidthElements(guiWidth);
    
    // Point cloud position group
    cloudPosGroup.setup("Point cloud position");
    cloudPosGroup.add(xCloudPosSlider.setup("point cloud x", 0, -1000, 1000));
    cloudPosGroup.add(yCloudPosSlider.setup("point cloud y", 0, -1000, 1000));
    cloudPosGroup.add(zCloudPosSlider.setup("point cloud z", 0, -2000, 2000));
    
    gui.add(&cloudPosGroup);
    cloudPosGroup.setWidthElements(guiWidth);
    
    
//    // Blur group
//    // setup(int width, int height, int radius = 32, float shape = .2, int passes = 1, float downsample = .5);
//    blurGroup.setup("Blur");
//    blurGroup.add(blurRadiusSlider.setup("blur radius", 40, 1, 80));
//    blurGroup.add(blurShapeSlider.setup("blur shape", .2, .1, 10));
//    blurGroup.add(blurPassesSlider.setup("blur passes", 1, 1, 10));
//    blurGroup.add(blurDownsampleSlider.setup("blur downsample", .5, .1, 1));
//    blurGroup.add(blurRefreshButton.setup("blur refresh"));
//    
//    gui.add(&blurGroup);
//    blurGroup.setWidthElements(guiWidth);
    
    // Misc group
    miscGroup.setup("Misc.");
    miscGroup.add(motorSlider.setup("motor angle", 12, -30.0, 30.0));
    miscGroup.add(calibrateViewToggle.setup("calibrate views", true));
    miscGroup.add(activeKinectToggle.setup("active sensing", true));
    miscGroup.add(easyCamToggle.setup("easyCam", false));
    miscGroup.add(blurToggle.setup("bloom", false));
    miscGroup.add(pointCloudToggle.setup("pointCloud", false));
    // Mesh number
    miscGroup.add(meshNumberSlider.setup("number of meshes", 10, 0, 100));
    
    bSoundToggle = false;
    
    miscGroup.add(soundToggle.setup("sound", bSoundToggle));
    miscGroup.add(musicToggle.setup("music", false));
    
    gui.add(&miscGroup);
    miscGroup.setWidthElements(guiWidth);
    
    // Load previous settings
    gui.loadFromFile("settings.xml");
    
    
    // Blur setup
    // setup(int width, int height, int radius = 32, float shape = .2, int passes = 1, float downsample = .5);
//    blur.setup(ofGetWindowWidth(), ofGetWindowHeight(), blurRadiusSlider, blurShapeSlider, blurPassesSlider, blurDownsampleSlider);
    blur.setup(ofGetWindowWidth(), ofGetWindowHeight(), 50, .2, 1, .1);
    
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
    
    // If kinect sensing is supposed to be active
    if (activeKinectToggle == true) {
        
        // There is a new frame, we're connected
        if (kinect.isFrameNew()) {
            
            // Load grayscale depth image from the kinect
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
            
            // Lets try to dump the sculpture data into it's own mesh
            ofPixels pixels = grayImage.getPixels();
            modelMesh.clear();
            modelMesh.setMode(OF_PRIMITIVE_POINTS);
            int step = 2;
            for (int y = 0; y < 480; y += step) {
                for (int x = 0; x < 640; x += step) {
                    //            if (kinect.getDistanceAt(x, y) > 0) {
                    // Check if the pixel is at least a certain brightness
                    
                    int value = pixels[y * 640 + x];
                    
                    if ((int)pixels[y * 640 + x] > 10) {
                        
                        ofVec3f vertex = kinect.getWorldCoordinateAt(x, y);
                        vertex.z = ofMap(vertex.z, 200, 1000, 400, -100);
//                        vertex.z = 100;
                        vertex.x = x;
                        vertex.y = y;
//                        ofVec3f vertex = ofVec3f(x, y, 0);
                        
                        modelMesh.addVertex(vertex);
                        
                        //Offset the color here
                        ofColor col = kinect.getColorAt(x, y);
                        col = ofColor(col.r, col.g, col.b, 128);
//                                            ofColor col = ofColor(255, 255, 255); // + offset ;
                        
                        modelMesh.addColor( col );
                        
                    }
                }
            }
            
            
            if (nMeshes.size() < meshNumberSlider) {
                // Probably ought to be played around with
                int oldCount = nMeshes.size();
                int newCount = ofRandom(1, 5);
                
                for (int j = oldCount; j < (newCount + oldCount); j++) {
                    
                    // Mmmm maybe there's a better way to clear this, maybe not
                    vecPrototypePoints.clear();
                    
                    for (int i = 0; i < contourFinder.blobs.size(); i++) {
                        int type = 3;
                        
                        if (ofRandom(10) > 6) {
                            type = 1;
                            
                        } else if (ofRandom(10) > 7) {
                            type = 2;
                        }
                        
                        // Copy current points to protype
                        prototypePoints = contourFinder.blobs[i].pts;
                        // Push protype back onto vector
                        vecPrototypePoints.push_back(contourFinder.blobs[i].pts);
                        
                        // This is awful but shit...let's see how bad it makes it
                        for (int jj = 0; jj < prototypePoints.size(); jj++) {
                            // Pull kinect z data at the spot
                            ofVec3f kVec = kinect.getWorldCoordinateAt(prototypePoints[jj].x, prototypePoints[jj].y);
                            // 500 is about 2 feet, 1000 is about 10 feet. Max is 2048, min is 0
                            
                            float deep = kVec.z;
                            
                            prototypePoints[jj].z = ofMap(kVec.z, 500, 2000, 0, 400);
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
        
    } else {
        // The kinect is not actively sensing, so build from prototype if available only
        
        // Check to make sure we have points
        if (prototypePoints.size() > 0) {
            
            // If we have less meshes than some amount, add them the easy way
            if (nMeshes.size() < meshNumberSlider) {
                
                int oldCount = nMeshes.size();
                int newCount = ofRandom(1, 5);
                
                for (int j = oldCount; j < (newCount + oldCount); j++) {
                    
                    
                    int type = 3;
                    
                    if (ofRandom(10) > 6) {
                        type = 1;
                        
                    } else if (ofRandom(10) > 7) {
                        type = 2;
                    }
                    
                    for (int i = 0; i < vecPrototypePoints.size(); i++) {
                        nMesh newMesh = nMesh(vecPrototypePoints[i], type);
                        nMeshes.push_back(newMesh);
                    }
                }
                
            }
            
        }
        
    }
    
    // Update the meshes
    for (int i = 0; i < nMeshes.size(); i++) {
        nMeshes[i].update();
        
        // If the actionP is completed...delete it
        if (nMeshes[i].actionPComplete == true) {
            nMeshes.erase(nMeshes.begin() + i);
        }
    }
    
    // If we're in calibration view...
    if (calibrateViewToggle == true) {
        // Run canny edge detection and stuff it in the edge image
        cvCanny(grayImage.getCvImage(), edgeImage.getCvImage(), 0, 0);
        
        edgeImage.flagImageChanged();
    }
    
    
    // Update music playback
    soundNoise += .001;
    float soundNoise = ofNoise(soundNoise);
    
    musicPlayer.setVolume(ofNoise(soundNoise) * .5);
    musicPlayer.setSpeed(ofMap(soundNoise, 0, 1, .1, 1.0));
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0, 0, 0);
    glPointSize(2);
    
    if (blurToggle == true) {
//        if (blurRefreshButton == true) {
//            blur.setup(ofGetWindowWidth(), ofGetWindowHeight(), blurRadiusSlider, blurShapeSlider, blurPassesSlider, blurDownsampleSlider);
//        }
        
        blur.begin();
    }
    
    ofPushMatrix();
    
    if (easyCamToggle == true) {
        easyCam.begin();
        ofScale(1, -1, 1);
        ofTranslate(-ofGetWidth() / 2, -ofGetHeight() / 2);
    }

    // For now make the assumption that the projection is always landscape format
    //    ofScale((float) ofGetWindowWidth() / 640, (float) ofGetWindowHeight() / 480);
    float scaleRatio = ofGetWindowWidth() / 640;
    ofScale(scaleRatio, scaleRatio);
    
    // Translate based on sliders and on initially centering the height
    ofTranslate(xPosSlider, yPosSlider -(((kinect.height / 2) * scaleRatio) - (ofGetWindowHeight() / 2)) / 4, zPosSlider);
    
    
    ofBackground(0, 0, 0);
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    
    // Go through all the meshes, set the mesh display modes, and draw them
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

    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    
    // Draw the model back as a point cloud
    if (pointCloudToggle == true) {
        ofPushMatrix();
        
        // Translate based on sliders
        ofTranslate(xCloudPosSlider, yCloudPosSlider, zCloudPosSlider);
        
        modelMesh.draw();
        ofPopMatrix();
    }
    
    // If we're calibrating, draw the box at the center vector so we can move it easily
    if (calibrateViewToggle == true) {
        ofFill();
        ofSetColor(255, 255, 255, 127);
        ofDrawBox(cCenter.x - 5, cCenter.y - 5, 0, 10, 10, 10);
    }
    
    if (easyCamToggle == true) {
        easyCam.end();
    }
    
    ofPopMatrix();
    
    if (blurToggle == true) {
        blur.end();
        blur.draw();
    }
    
    
    // If we're blurring, do it ALL OVER AGAIN to draw non-blurred stuff
    if (blurToggle == true) {
        
        ofPushMatrix();
        
        if (easyCamToggle == true) {
            easyCam.begin();
            ofScale(1, -1, 1);
            ofTranslate(-ofGetWidth()/2, -ofGetHeight()/2);
        }
        
        // For now make the assumption that the projection is always landscape format
        //    ofScale((float) ofGetWindowWidth() / 640, (float) ofGetWindowHeight() / 480);
        scaleRatio = ofGetWindowWidth() / 640;
        ofScale(scaleRatio, scaleRatio);
        
        // Translate based on sliders and on initially centering the height
        ofTranslate(xPosSlider, yPosSlider -(((kinect.height / 2) * scaleRatio) - (ofGetWindowHeight() / 2)) / 4, zPosSlider);
        
        ofEnableBlendMode(OF_BLENDMODE_ADD);
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
        
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        
        // Draw the model back as a point cloud
        if (pointCloudToggle == true) {
            ofPushMatrix();
            
            // Translate based on sliders
            ofTranslate(xCloudPosSlider, yCloudPosSlider, zCloudPosSlider);
            
            modelMesh.draw();
            ofPopMatrix();
        }
        
        if (easyCamToggle == true) {
            easyCam.end();
        }
        
        ofPopMatrix();
        
    } // End ALL OVER AGAIN
    
    
    
    
    // If we're calibrating, draw the different displays
    if (calibrateViewToggle == true) {
        // Draw from the live kinect
        kinect.drawDepth(10, 10, 400, 300);
        kinect.draw((ofGetWindowWidth() - 410), 10, 400, 300);
        
        grayImage.draw(10, (ofGetWindowHeight() - 310), 400, 300);
        contourFinder.draw(10, (ofGetWindowHeight() - 310), 400, 300);
        
        edgeImage.draw((ofGetWindowWidth() - 410), (ofGetWindowHeight() - 310), 400, 300);
        
        ofDrawBitmapString("Number of meshes: " + std::to_string(nMeshes.size()), ((ofGetWindowWidth() / 2) - 60), 20);
    }
    
    // Show the GUI if we want it
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
    blur.setup(ofGetWindowWidth(), ofGetWindowHeight(), 50, .2, 1, .1);
//    blur.setup(ofGetWindowWidth(), ofGetWindowHeight(), blurRadiusSlider, blurShapeSlider, blurPassesSlider, blurDownsampleSlider);

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





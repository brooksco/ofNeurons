//
//  nMesh.cpp
//  ofNeurons
//
//  Created by Colin Brooks on 2/10/16.
//
//

#include "ofApp.h"
#include "nMesh.h"

float initialAlpha = .1;
ofSoundPlayer soundPlayer;
ofSoundPlayer staticPlayer;

//bool bSoundToggle;
bool mute;

// Constructor
nMesh::nMesh(vector<ofVec3f> prototypePoints, int type) {
    this->actionP = false;
    this->actionPComplete = false;
    this->type = type;
    
    // Type 3 is dots
    if (type == 3) {
        this->displacementScale = ofRandom(.5, 1.5);
//        this->alphaFactor = ofRandom(.1, 1); //.2;
        this->alphaFactor = ofRandom(50, 150); //.2;
        
    } else {
        // Non dot
        this->displacementScale = ofRandom(1.5, 4.0);
        this->alphaFactor = ofRandom(10, 15);
//        this->alphaFactor = ofRandom(.1, 1); //.1;
    }
    
    
    vector<int> usedPoints;
    
    ofVec3f current = prototypePoints[0];
    
    
    // No reMesh
    for (int i = 0; i < prototypePoints.size(); i++) {
        
        ofVec3f current = prototypePoints[i];
        
        // This maybe should be played with
        current.x = current.x + ofRandom(-12.0, 12.0);
        current.y = current.y + ofRandom(-12.0, 12.0);
        
        mesh.addVertex(current);
        
        if (type == 3) {
            mesh.addColor(ofColor(ofRandom(155), ofRandom(255), 255, ofRandom(100, 200)));
            
        } else {
            mesh.addColor(ofColor(ofRandom(200,255), ofRandom(200,255), ofRandom(200,255), 127));
        }
        
        
        offsets.push_back(ofVec3f(ofRandom(0,100000), ofRandom(0,100000), ofRandom(0,100000)));

    }
    
}


void nMesh::update() {
    
    int numVerts = mesh.getNumVertices();
//    for (int i = 0; i < numVerts; ++i) {
    for (int i = 0; i < numVerts; i++) {
        ofVec3f vert = mesh.getVertex(i);
        
        float time = ofGetElapsedTimef();
        float timeScale = 5.0;
        
        ofVec3f timeOffsets = offsets[i];
        
        float xGrowth = 0;
        float yGrowth = 0;
        
        float growthFactor = 2;
        
        // If an actionP is happening OR if we hit the right random value
        if ((ofRandom(100000) >= 99999) || (actionP == true)) {
            
            if (actionP == false) {
                // First time through, actionP is starting
                actionPStart = ofGetFrameNum();
                actionP = true;
                displacementScale = displacementScale * 1.25;
                
//                // Using the r value as storage for a value
//                // Safe to go through all of them since it won't get back here
//                for (int j = 0; j < numVerts; j++) {
//                    ofColor old = mesh.getColor(j);
//                    mesh.setColor(j, ofColor(old.r, old.g, old.b, old.r));
//                }
                
                // Handle sounds
                if (bSoundToggle == true) {
                    
                    if (type == 1) {
                        soundPlayer.setSpeed(ofRandom(0.05, 1));
                        soundPlayer.setVolume(0.4f);
                        soundPlayer.play();
                        
                    } else if (type == 2) {
                        soundPlayer.setSpeed(ofRandom(0.01, .25));
                        soundPlayer.setVolume(1.0f);
                        soundPlayer.play();
                        
                    } else if (type == 3) {
//                        staticPlayer.setSpeed(.4);
//                        staticPlayer.setVolume(.025f);
//                        staticPlayer.play();
                    }
                }
            } // End actionP == false
            
            int frameDiff = ofGetFrameNum() - actionPStart;
            
            if (type == 3) {
                alphaFactor = ofMap(frameDiff, 0, 480, 200, 0); // used to be 1 instead of 255

            } else {
                // 180 frames, 60fps, so ~3 sec
                alphaFactor = ofMap(frameDiff, 0, 180, 60, 0); // used to be 1 instead of 255
            }
            
            if (vert.x < cCenter.x) {
                // Quadrant II
                if (vert.y < cCenter.y) {
                    
                    // Get dimensions of right triangle from center of window
                    float tempX = cCenter.x - vert.x;
                    float tempY = cCenter.y - vert.y;
                    // Calculate the angle
                    float tempAngle = atan(tempY / tempX);
                    // Calculate the hypotenuse
                    float tempH = tempY / sin(tempAngle);
                    
                    // Calculate growthFactor
                    growthFactor = ofMap(tempH, 0, screenH, 3, 0);
//                    growthFactor = growthCalcHelper(vert, ofVec3f(0, 0, 0));
                    
                    // Add to to the hypotenuse, and recalulate the new x/y points
                    tempH += growthFactor;
                    float newTempX = tempH * cos(tempAngle);
                    float newTempY = tempH * sin(tempAngle);
                    
                    // Take the difference between the old and new x/y points as the amount to grow
                    xGrowth = (newTempX - tempX) * -1;
                    yGrowth = (newTempY - tempY) * -1;
                    
                } else {
                    // Quadrant III
                    
                    // Get dimensions of right triangle from center of window
                    float tempX = cCenter.x - vert.x;
                    float tempY = vert.y - cCenter.y;
                    // Calculate the angle
                    float tempAngle = atan(tempY / tempX);
                    // Calculate the hypotenuse
                    float tempH = tempY / sin(tempAngle);
                    
                    // Calculate growthFactor
                    growthFactor = ofMap(tempH, 0, screenH, 3, 0);
//                    growthFactor = growthCalcHelper(vert, ofVec3f(0, (cCenter.y * 2), 0));
                    
                    // Add to to the hypotenuse, and recalulate the new x/y points
                    tempH += growthFactor;
                    float newTempX = tempH * cos(tempAngle);
                    float newTempY = tempH * sin(tempAngle);
                    
                    // Take the difference between the old and new x/y points as the amount to grow
                    xGrowth = (newTempX - tempX) * -1;
                    yGrowth = (newTempY - tempY);
                }
                
            } else {
                // Quadrant I
                if (vert.y < cCenter.y) {
                    
                    // Get dimensions of right triangle from center of window
                    float tempX = vert.x - cCenter.x;
                    float tempY = cCenter.y - vert.y;
                    // Calculate the angle
                    float tempAngle = atan(tempY / tempX);
                    // Calculate the hypotenuse
                    float tempH = tempY / sin(tempAngle);
                    
                    // Calculate growthFactor
                    growthFactor = ofMap(tempH, 0, screenH, 3, 0);
//                    growthFactor = growthCalcHelper(vert, ofVec3f((cCenter.x * 2), 0, 0));
                    
                    // Add to to the hypotenuse, and recalulate the new x/y points
                    tempH += growthFactor;
                    float newTempX = tempH * cos(tempAngle);
                    float newTempY = tempH * sin(tempAngle);
                    
                    // Take the difference between the old and new x/y points as the amount to grow
                    xGrowth = (newTempX - tempX);
                    yGrowth = (newTempY - tempY) * -1;
                    
                } else {
                    // Quadrant IV
                    
                    // Get dimensions of right triangle from center of window
                    float tempX = vert.x - cCenter.x;
                    float tempY = vert.y - cCenter.y;
                    // Calculate the angle
                    float tempAngle = atan(tempY / tempX);
                    // Calculate the hypotenuse
                    float tempH = tempY / sin(tempAngle);
                    
                    // Calculate growthFactor
                    growthFactor = ofMap(tempH, 0, screenH, 3, 0);
//                    growthFactor = growthCalcHelper(vert, ofVec3f((cCenter.x * 2), (cCenter.y * 2), 0));
                    
                    // Add to to the hypotenuse, and recalulate the new x/y points
                    tempH += growthFactor;
                    float newTempX = tempH * cos(tempAngle);
                    float newTempY = tempH * sin(tempAngle);
                    
                    // Take the difference between the old and new x/y points as the amount to grow
                    xGrowth = (newTempX - tempX);
                    yGrowth = (newTempY - tempY);
                }
            }
            
            
            
        }// End actionP
        
        if (type == 3) {
            xGrowth = xGrowth / 4;
            yGrowth = yGrowth / 4;
        }
        
        
        vert.x += (ofSignedNoise(time*timeScale+timeOffsets.x)) * displacementScale + xGrowth;
        vert.y += (ofSignedNoise(time*timeScale+timeOffsets.y)) * displacementScale + yGrowth;
        vert.z += (ofSignedNoise(time*timeScale+timeOffsets.z)) * displacementScale;
        mesh.setVertex(i, vert);

        float distance = vert.distance(cCenter);
        
        float alpha = 1;
        
        // If its the dot type
        if (type == 3) {
//            alpha = ofMap(distance, 0, screenH / 2, 255, 0);
            alpha = 255;
//            // Maintain old color but set new alpha value
            ofColor old = mesh.getColor(i);
            mesh.setColor(i, ofColor(old.r, old.g, old.b, (alphaFactor) / 2));
            
        } else {
            ofColor old = mesh.getColor(i);
//            alpha = ofMap(distance, 0, screenH / 2, 200, 0);
//            
//            if (alpha < 0) {
//                alpha = 0;
//            }
//            mesh.setColor(i, ofColor(0, 255, 255, (alpha * alphaFactor)));
//            mesh.setColor(i, ofColor(0, 255, 255, alpha));
            mesh.setColor(i, ofColor(0, 255, old.b, alphaFactor));

            
            if (ofRandom(10) > 9) {
                mesh.setColor(i, ofColor(255, 0, old.b, 60));
                
            } else if (ofRandom(10) > 8) {
                mesh.setColor(i, ofColor(255, 0, old.b, 0));
            }
            
        }
        
        if (alphaFactor <= 0) {
            actionPComplete = true;
        }
        
//        if (distance == screenH / 4) {
//            actionPComplete = true;
//        }
        
    }
    
}

float nMesh::growthCalcHelper(ofVec3f vert, ofVec3f distantVert) {
    float tempDistance = vert.distance(distantVert);
    float growth = ofMap(tempDistance, 0, screenH, 0, 10);
    
    return growth;
}




void nMesh::reMesh(ofVec3f cPoint, ofMesh cMesh, vector<ofVec3f> cPoints, vector<int> uPoints) {
    vector<int> usedPoints;
    
    ofVec3f current = cPoints[0];
    
    reMeshHelper(current, mesh, cPoints, usedPoints);
}
// End confusion


void nMesh::reMeshHelper(ofVec3f cPoint, ofMesh cMesh, vector<ofVec3f> cPoints, vector<int> uPoints) {
    if (uPoints.size() < cPoints.size() - 1) {
        
        ofVec3f closest = ofVec3f(10000, 10000, 10000);
        ofVec3f sClosest = ofVec3f(10000, 10000, 10000);
        int closestIndex = -1;
        int sClosestIndex = -1;
        
        // Find closest
        for (int j = 0; j < cPoints.size(); j++) {
            
            if ( !(std::find(uPoints.begin(), uPoints.end(), j) != uPoints.end()) ) {
                
                if (cPoint.distance(cPoints[j]) < cPoint.distance(closest)) {
                    
                    if ((sClosestIndex == -1) && (closestIndex != -1)) {
                        sClosest = closest;
                        sClosestIndex = closestIndex;
                    }
                    
                    closest = cPoints[j];
                    closestIndex = j;
                    
                    
                } else if (cPoint.distance(cPoints[j]) < cPoint.distance(sClosest)) {
                    sClosest = cPoints[j];
                    sClosestIndex = j;
                }
                
            }
        } // end for j
        
        // This maybe should be played with
        cPoint.x = cPoint.x + ofRandom(-12.0, 12.0);
        cPoint.y = cPoint.y + ofRandom(-12.0, 12.0);
        
        mesh.addVertex(cPoint);
        mesh.addColor(ofColor(255, 255, 255, 127));
        offsets.push_back(ofVec3f(ofRandom(0,100000), ofRandom(0,100000), ofRandom(0,100000)));
        
        
        uPoints.push_back(closestIndex);
        reMeshHelper(closest, mesh, cPoints, uPoints);
        
    }
    
}

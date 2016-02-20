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

nMesh::nMesh(ofMesh originalMesh) {
    // Copy the prototype mesh, might want to muss it up a bit after this
    this->mesh = originalMesh;
    this->actionP = false;
    this->alphaFactor = initialAlpha;
    
    this->mesh.setMode(OF_PRIMITIVE_POINTS);
    //    this->mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    
    // Setup offsets? points?
    int numVerts = mesh.getNumVertices();
    for (int i = 0; i < numVerts; ++i) {
        ofVec3f vert = mesh.getVertex(i);
        points.push_back(vert);
        offsets.push_back(ofVec3f(ofRandom(0,100000), ofRandom(0,100000), ofRandom(0,100000)));
    }
    
}

// Not sure if I want a constructor, or the reMesh function
nMesh::nMesh(vector<ofVec3f> prototypePoints, int type) {
    this->actionP = false;
    this->alphaFactor = initialAlpha;
    this->type = type;
    
    if (type == 3) {
        this->displacementScale = ofRandom(.5, 1.5);
        
    } else {
        this->displacementScale = ofRandom(1.5, 4.0);
    }
    
    
    vector<int> usedPoints;
    
    ofVec3f current = prototypePoints[0];
    
    reMesh(current, mesh, prototypePoints, usedPoints);
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
        
        // This is mad iffy
        if (closestIndex != -1) {
            //            mesh.addIndex(closestIndex);
            
            //            if (sClosestIndex != -1) {
            //                mesh.addIndex(sClosestIndex);
            //            }
        }
        
        
        uPoints.push_back(closestIndex);
        reMeshHelper(closest, mesh, cPoints, uPoints);
        
    }
    
}

void nMesh::update() {
    
    int numVerts = mesh.getNumVertices();
    for (int i = 0; i < numVerts; ++i) {
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
                actionPStart = ofGetFrameNum();
                actionP = true;
                displacementScale = displacementScale * 1.25;
                alphaFactor = 1;
                
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
            }
            
            int frameDiff = ofGetFrameNum() - actionPStart;
            
            if (type == 3) {
                alphaFactor = ofMap(frameDiff, 0, 960, 1, 0);
                
            } else {
                // 180 frames, 60fps, so ~3 sec
                alphaFactor = ofMap(frameDiff, 0, 180, 1, 0);
            }
            
            if (vert.x < (ofGetWindowWidth() / 2)) {
                // Quadrant II
                if (vert.y < (ofGetWindowHeight() / 2)) {
                    
                    // Get dimensions of right triangle from center of window
                    float tempX = (ofGetWindowWidth() / 2) - vert.x;
                    float tempY = (ofGetWindowHeight() / 2) - vert.y;
                    // Calculate the angle
                    float tempAngle = atan(tempY / tempX);
                    // Calculate the hypotenuse
                    float tempH = tempY / sin(tempAngle);
                    
                    // Calculate growthFactor
                    growthFactor = growthCalcHelper(vert, ofVec3f(0, 0, 0));
                    
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
                    float tempX = (ofGetWindowWidth() / 2) - vert.x;
                    float tempY = vert.y - (ofGetWindowHeight() / 2);
                    // Calculate the angle
                    float tempAngle = atan(tempY / tempX);
                    // Calculate the hypotenuse
                    float tempH = tempY / sin(tempAngle);
                    
                    // Calculate growthFactor
                    growthFactor = growthCalcHelper(vert, ofVec3f(0, ofGetWindowHeight(), 0));
                    
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
                if (vert.y < (ofGetWindowHeight() / 2)) {
                    
                    // Get dimensions of right triangle from center of window
                    float tempX = vert.x - (ofGetWindowWidth() / 2);
                    float tempY = (ofGetWindowHeight() / 2) - vert.y;
                    // Calculate the angle
                    float tempAngle = atan(tempY / tempX);
                    // Calculate the hypotenuse
                    float tempH = tempY / sin(tempAngle);
                    
                    // Calculate growthFactor
                    growthFactor = growthCalcHelper(vert, ofVec3f(ofGetWindowWidth(), 0, 0));
                    
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
                    float tempX = vert.x - (ofGetWindowWidth() / 2);
                    float tempY = vert.y - (ofGetWindowHeight() / 2);
                    // Calculate the angle
                    float tempAngle = atan(tempY / tempX);
                    // Calculate the hypotenuse
                    float tempH = tempY / sin(tempAngle);
                    
                    // Calculate growthFactor
                    growthFactor = growthCalcHelper(vert, ofVec3f(ofGetWindowWidth(), ofGetWindowHeight(), 0));
                    
                    // Add to to the hypotenuse, and recalulate the new x/y points
                    tempH += growthFactor;
                    float newTempX = tempH * cos(tempAngle);
                    float newTempY = tempH * sin(tempAngle);
                    
                    // Take the difference between the old and new x/y points as the amount to grow
                    xGrowth = (newTempX - tempX);
                    yGrowth = (newTempY - tempY);
                }
            }
        }
        
        if (type == 3) {
            xGrowth = xGrowth / 4;
            yGrowth = yGrowth / 4;
        }
        
        
        vert.x += (ofSignedNoise(time*timeScale+timeOffsets.x)) * displacementScale + xGrowth;
        vert.y += (ofSignedNoise(time*timeScale+timeOffsets.y)) * displacementScale + yGrowth;
        vert.z += (ofSignedNoise(time*timeScale+timeOffsets.z)) * displacementScale;
        mesh.setVertex(i, vert);
        
//        vert.x += 1;
//        mesh.setVertex(i, vert);
        
        
        float distance = vert.distance(cCenter);
        float alpha = ofMap(distance, 0, screenH, 255, 0);
        
        if (type == 3) {
            mesh.setColor(i, ofColor(0, 255, 255, (alpha * alphaFactor) / 2));
            
        } else {
            mesh.setColor(i, ofColor(0, 255, 255, (alpha * alphaFactor)));
            
            if (ofRandom(10) > 9) {
                mesh.setColor(i, ofColor(255, 0, 255, 60));
                
            } else if (ofRandom(10) > 8) {
                mesh.setColor(i, ofColor(255, 0, 255, 0));
            }
            
        }
        
    }
    
}

float nMesh::growthCalcHelper(ofVec3f vert, ofVec3f distantVert) {
    float tempDistance = vert.distance(distantVert);
    float growth = ofMap(tempDistance, 0, screenH / 1, 0, 10);
    
    return growth;
}

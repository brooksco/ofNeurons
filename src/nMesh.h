//
//  nMesh.h
//  ofNeurons
//
//  Created by Colin Brooks on 2/10/16.
//
//

#ifndef nMesh_h
#define nMesh_h

#include "ofMain.h"

class nMesh {
    
public:
    
    // Constructors
    nMesh(ofMesh prototypeMesh);
    nMesh(vector<ofVec3f> prototypePoints, int type);
    
    // Methods
    void reMesh(ofVec3f cPoint, ofMesh cMesh, vector<ofVec3f> cPoints, vector<int> uPoints);
    void update();
    
    // Variables
    ofMesh mesh;
    vector<ofVec3f> points;
    vector<ofVec3f> offsets;
    bool actionP;
    int actionPStart;
    
    float alphaFactor;
    float displacementScale;
    
    int type; // To be used for styling?
    
private:
    
    // Methods
    void reMeshHelper(ofVec3f cPoint, ofMesh cMesh, vector<ofVec3f> cPoints, vector<int> uPoints);
    
    float growthCalcHelper(ofVec3f vert, ofVec3f distantVert);
    
};

#endif /* nMesh_h */
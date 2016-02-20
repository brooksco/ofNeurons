#pragma once

#include "ofMain.h"
#include "nMesh.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxGUI.h"

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    vector<ofVec3f> offsets;
    vector<ofVec3f> prototypePoints;
    vector<nMesh> nMeshes;
    
    ofMesh prototypeMesh;
    ofImage image;
    
    // Kinect
    ofxKinect kinect;
    bool bKinectOpen;
    int angle;
    
    
    // OpenCV
    ofxCvColorImage colorImg;
    
    ofxCvGrayscaleImage grayImage; // grayscale depth image
    ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
    ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
    
    ofxCvGrayscaleImage edgeImage;
    
    ofxCvContourFinder contourFinder;
    
    float minBlobSize, maxBlobSize ;
    float pointCloudMinZ, pointCloudMaxZ ;
    
    bool bThreshWithOpenCV;
    
    int nearThreshold;
    int farThreshold;
    
    vector<ofMesh> meshes;
    
    // GUI
    ofxPanel gui;
    
    ofxFloatSlider nearDepthSlider;
    ofxFloatSlider farDepthSlider;
    ofxFloatSlider minBlobSlider;
    ofxFloatSlider maxBlobSlider;
    
    ofxFloatSlider motorSlider;
    
    ofxToggle calibrateViewToggle;
    ofxToggle contourToggle;
    ofxToggle pointsToggle;
    
    ofxToggle soundToggle;
    ofxToggle musicToggle;
    
    ofxFloatSlider minPointSlider;
    ofxFloatSlider maxPointSlider;
    
    
};


// Canvas center vector
extern ofVec3f cCenter;
extern ofSoundPlayer soundPlayer;
extern ofSoundPlayer staticPlayer;
extern bool mute;

extern bool bSoundToggle;
extern bool bMusicToggle;

// Screen diagonal
extern float screenH;
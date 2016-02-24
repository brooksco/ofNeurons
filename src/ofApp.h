#pragma once

#include "ofMain.h"
#include "nMesh.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxGUI.h"
#include "ofxXmlSettings.h"
#include "ofxBlur.h"

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
    
    void exit();
    
    ofEasyCam easyCam;
    
    vector<ofVec3f> offsets;
    vector<ofVec3f> prototypePoints;
    vector<vector<ofVec3f>> vecPrototypePoints;
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
    
    // GUI groups
    ofxGuiGroup depthGroup;
    ofxGuiGroup blobGroup;
    ofxGuiGroup posGroup;
    ofxGuiGroup centerPosGroup;
    ofxGuiGroup blurGroup;
    ofxGuiGroup miscGroup;
    
    
    // GUI sliders
    ofxFloatSlider nearDepthSlider;
    ofxFloatSlider farDepthSlider;
    
    ofxFloatSlider minBlobSlider;
    ofxFloatSlider maxBlobSlider;
    
    ofxFloatSlider motorSlider;
    
    ofxFloatSlider minPointSlider;
    ofxFloatSlider maxPointSlider;
    
    ofxIntSlider xPosSlider;
    ofxIntSlider yPosSlider;
    ofxIntSlider zPosSlider;
    
    ofxIntSlider meshNumberSlider;
    
    ofxIntSlider xCenterPosSlider;
    ofxIntSlider yCenterPosSlider;
    
    // setup(int width, int height, int radius = 32, float shape = .2, int passes = 1, float downsample = .5);
    ofxIntSlider blurRadiusSlider;
    ofxFloatSlider blurShapeSlider;
    ofxIntSlider blurPassesSlider;
    ofxFloatSlider blurDownsampleSlider;
    

    // GUI toggles
    ofxToggle activeKinectToggle;
    ofxToggle calibrateViewToggle;
    
    ofxToggle soundToggle;
    ofxToggle musicToggle;
    
    ofxToggle easyCamToggle;
    ofxToggle blurToggle;
    
    // GUI buttons;
    ofxButton blurRefreshButton;
    
    // XML settings
    ofxXmlSettings settings;
    
    // Blur
    ofxBlur blur;

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
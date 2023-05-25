#pragma once

#include "ofMain.h"
#include "ofxLeapMotion2.h"
#include "ofxATK.hpp"
#include "ofxGui.h"
#include "Synth.h"


#include "Particle.h"
#define MAX_GRAINS 64

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    //void customDrawSlider();
    void exit();
    void audioOut(float* buffer, int bufferSize, int nChannels);


    
    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    
    float lowestX, lowestY, highestY, lowestZ, highestZ;
    
    SoundFile* sound1;
    SoundFile* sound2;
    SoundFile* sound3;
    SoundFile* sound4;
    
    Reverb* reverb;

//ofATK impulseGenerator
    ImpulseGenerator* impulse;//outputs every sample either 0 or 1
    
    Synth* grains[64];

    bool displayGUI;
    ofxPanel gui;
    ofxFloatSlider emission;
    ofxFloatSlider amplitude;
    ofxFloatSlider duration;
    ofxFloatSlider xSlider;
    ofxFloatSlider ySlider;// x y means cross-fading between sounds
    ofxFloatSlider leftHandOpen;
    ofxFloatSlider rightHandOpen;
    ofxFloatSlider pitch;
    ofxFloatSlider reverbSlider;

    //flashing light
    ofFbo fbo;
    ofShader shader;
    //float amplitude;
    float threshold;
    float flashFrequency;

    // Particle System
    vector<vector<Particle>> particleSystems;
    const int numParticlesPerFinger = 30;
    
    //leap motion
    ofxLeapMotion leap;
    vector <ofxLeapMotionSimpleHand> simpleHands;
    
    vector <int> fingersFound;
    ofEasyCam cam;
    bool leapActive;
};




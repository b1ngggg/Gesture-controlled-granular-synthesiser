#pragma once

#include "ofMain.h"

class Particle {
public:
    Particle(ofPoint pos);
    void update();
    void draw();

private:
    ofPoint position;
    ofPoint velocity;
    float size;
};

#include "Particle.h"

Particle::Particle(ofPoint pos) {
    position = pos;
    velocity = ofPoint(ofRandom(-1, 1), ofRandom(-1, 1), ofRandom(-1, 1));
    size = 2;
}

void Particle::update() {
    position += velocity;
}

void Particle::draw() {
    ofSetColor(255, 0, 0, 150);
    ofDrawSphere(position, size);
}

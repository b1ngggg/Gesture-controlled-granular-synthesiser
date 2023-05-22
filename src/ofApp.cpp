#include "ofApp.h"

//--------------------------------------------------------------


void ofApp::setup(){
    
    leapActive = true;
    sound1 = new SoundFile(ofToDataPath("piano2.wav"));
    sound2 = new SoundFile(ofToDataPath("search.wav"));
    sound3 = new SoundFile(ofToDataPath("deep.wav"));
    sound4 = new SoundFile(ofToDataPath("noise.wav"));



    impulse = new ImpulseGenerator(4); //the number is devided by 1000, for example if it is 4, then you will get 1 pulse every 250 milliseconds
    

    for (int i = 0; i < MAX_GRAINS; i++) {
        grains[i] = new Synth(sound1,sound2,sound3,sound4);

    }
    reverb = new Reverb();

    //ui
    displayGUI = true;
    gui.setup();
    gui.add(emission.setup("EmissionRate", 40, 1, 1000));
    gui.add(amplitude.setup("Amplitude", 0, 0.0, 1.0));
    gui.add(duration.setup("Duration", 40, 0.1, 40));
    gui.add(pitch.setup("Pitch", 1, 0.0, 16));
    gui.add(reverbSlider.setup("reverb", 0.05, 0.0, 0.3));
    gui.add(xSlider.setup("X", 1.5, 0.0, 3.0));
    gui.add(ySlider.setup("Y", 0.5, 0.0, 1.0));
    gui.add(leftHandOpen.setup("Left Hand Open", 0.0, 0.0, 1.0));
    gui.add(rightHandOpen.setup("Right Hand Open", 0.0, 0.0, 1.0));

    // Your existing setup code
    
    // Set up the shader
    shader.load("shaders/flash.vert", "shaders/flash.frag");

    // Set up the FBO
    ofFbo::Settings settings;
    settings.width = ofGetWidth();
    settings.height = ofGetHeight();
    settings.internalformat = GL_RGBA;
    fbo.allocate(settings);

    // Initialize amplitude and threshold variables
    //mplitude = 0.0;
    threshold = 0.5; // Adjust this value to control the minimum amplitude for the flashing effect
    flashFrequency = 10.0;
    
    ofSoundStreamSetup(2, 0, ATKSettings::sampleRate, ATKSettings::bufferSize, 4);
    //audio setup using ATK lib 48000 and 512
    
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    leap.open();
    // Initialize particle systems
    particleSystems.resize(5 * 2); // 5 fingers for 2 hands
        
    // keep app receiving data from leap motion even when it's in the background
    leap.setReceiveBackgroundFrames(true);
    
    cam.setOrientation(ofPoint(-20, 0, 0));
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    
    
}


//--------------------------------------------------------------
void ofApp::update(){
    
    int a = 0;
    for (int i = 0; i < MAX_GRAINS; i++) {
        if (grains[i]->getActive()) {
            a++;
        }
        
        grains[i]->setMainAmplitude(amplitude);
        grains[i]->setMainDuration(duration);
        grains[i]->setLeftHandOpen(leftHandOpen);
        grains[i]->setRightHandOpen(rightHandOpen);
        grains[i]->setMainPitch(pitch);
        grains[i]->setX(xSlider);
        grains[i]->setY(ySlider);

    }
   
    
    


    impulse->setFreq(emission);
    impulse->setBurstMasking(rightHandOpen*0.5);
    impulse->setPulseDeviation(rightHandOpen);

    ofxPanel gui;
    //ofxFloatSlider emission;


    
    fingersFound.clear();
    simpleHands = leap.getSimpleHands();
    

    
    float distanceLeft = 25;
    float distanceRight = 25;
    if( leap.isFrameNew() && simpleHands.size() ){
        
        leap.setMappingX(-230, 230, -ofGetWidth()/2, ofGetWidth()/2);
        leap.setMappingY(90, 490, -ofGetHeight()/2, ofGetHeight()/2);
        leap.setMappingZ(-150, 150, -200, 200);
        
        fingerType fingerTypes[] = {THUMB, INDEX, MIDDLE, RING, PINKY};
        
        for(int i = 0; i < simpleHands.size(); i++){
            for (int f=0; f<5; f++) {
                int id = simpleHands[i].fingers[ fingerTypes[f] ].id;
                ofPoint mcp = simpleHands[i].fingers[ fingerTypes[f] ].mcp; // metacarpal
                ofPoint pip = simpleHands[i].fingers[ fingerTypes[f] ].pip; // proximal
                ofPoint dip = simpleHands[i].fingers[ fingerTypes[f] ].dip; // distal
                ofPoint tip = simpleHands[i].fingers[ fingerTypes[f] ].tip; // fingertip
                fingersFound.push_back(id);
                }
            }
        
        for (int i = 0; i < simpleHands.size(); i++) {
            for (int f = 0; f < 5; f++) {
                // ...

                ofPoint tip = simpleHands[i].fingers[fingerTypes[f]].tip;  // fingertip

                // Add new particle to the corresponding particle system
                particleSystems[5 * i + f].push_back(Particle(tip));

                // Limit the number of particles per system
                if (particleSystems[5 * i + f].size() > numParticlesPerFinger) {
                    particleSystems[5 * i + f].erase(particleSystems[5 * i + f].begin());
                }

                // Update particles
                for (Particle &p : particleSystems[5 * i + f]) {
                    p.update();
                }
            }
        }

    leap.markFrameAsOld();
        
        if(leapActive){
            if(simpleHands.size()>1){
                if(simpleHands[0].handPos.x < lowestX){
                    lowestX = simpleHands[0].handPos.x;
                }
                if(simpleHands[0].handPos.x < lowestY){
                    lowestY = simpleHands[0].handPos.y;
                }
                if(simpleHands[0].handPos.x > highestY){
                    highestY = simpleHands[0].handPos.y;
                }
                if(simpleHands[0].handPos.x < lowestZ){
                    lowestZ = simpleHands[0].handPos.z;
                }
                if(simpleHands[0].handPos.x < highestZ){
                    highestZ = simpleHands[0].handPos.z;
                }
                
            
                int left;
                if(simpleHands[0].fingers[fingerTypes[4]].tip.x < simpleHands[0].fingers[fingerTypes[0]].tip.x){
                    left = 0;
                }else{
                    left = 1;
                }
                
                    
                    
            xSlider = ofMap(simpleHands[left].handPos.x, -600, -20, 0.0, 3.0, true);
            ySlider = ofMap(simpleHands[left].handPos.y, -350, 200,0.0,1.0, true);
            reverbSlider = ofMap(simpleHands[left].handPos.z, -220, 300,0.3,0.0, true);
            
                int right;
                if(simpleHands[0].fingers[fingerTypes[4]].tip.x > simpleHands[0].fingers[fingerTypes[0]].tip.x){
                    right = 0;
                }else{
                    right = 1;
                }
                
            amplitude = ofMap(simpleHands[right].handPos.y, -350, 200,0.0,1.0, true);
            emission = ofMap(simpleHands[right].handPos.x, 50, 800,0.0,1000, true);
            //duration = ofMap(simpleHands[right].handPos.z, -166, 254,3,100, true);
            duration = 1.f / ofMap(simpleHands[right].handPos.z, -166, 254,0.01,0.333, true);
            
                
            distanceLeft = ofDist(simpleHands[left].fingers[fingerTypes[0]].tip.x,
                                      simpleHands[left].fingers[fingerTypes[0]].tip.y,
                                      simpleHands[left].fingers[fingerTypes[0]].tip.z,
                                      simpleHands[left].fingers[fingerTypes[1]].tip.x,
                                      simpleHands[left].fingers[fingerTypes[1]].tip.y,
                                      simpleHands[left].fingers[fingerTypes[1]].tip.z);
                
            leftHandOpen = ofMap(distanceLeft, 35,180,0.0 ,1.0, true);
                
            distanceRight = ofDist(simpleHands[right].fingers[fingerTypes[0]].tip.x,
                                      simpleHands[right].fingers[fingerTypes[0]].tip.y,
                                      simpleHands[right].fingers[fingerTypes[0]].tip.z,
                                      simpleHands[right].fingers[fingerTypes[1]].tip.x,
                                      simpleHands[right].fingers[fingerTypes[1]].tip.y,
                                      simpleHands[right].fingers[fingerTypes[1]].tip.z);
                
            rightHandOpen = ofMap(distanceRight, 35,180,0.0 ,1.0, true);
            pitch = ofMap(distanceRight, 35,189,1.0 ,16.0, true);
            }else{
                amplitude = fmax(amplitude - 0.3, 0);
                emission = fmax(emission - 200, 0);
            }
    }
        }

}
void ofApp::audioOut(float* buffer, int bufferSize, int nChannels){
    
        for(int i=0; i< bufferSize; i++){
            
            //Audio thread
            float currentSample = 0;

            impulse->process();

            //start a new grain using the impulse generator
            if (impulse->getSample() == 1) {
                for (int i = 0; i < MAX_GRAINS; i++) {
                    if (grains[i]->getActive() == false) {
                        //start new grain
                        grains[i]->begin();
                        break;
                    }
                }

            }

            for (int i = 0; i < MAX_GRAINS; i++) {
                grains[i]->process();
                currentSample += grains[i]->getSample();
            }

        reverb->process(currentSample);
        currentSample = (currentSample * (0.3 - reverbSlider)) + (reverb->getSample() * reverbSlider);
            //currentSample += reverb->getSample() * 0.1;
            //float currentSample = impulse->getSample()-0.5;
            buffer[i * nChannels + 0] = currentSample;
            buffer[i * nChannels + 1] = currentSample;
            //A stereo granulator

        }
}

//--------------------------------------------------------------
void ofApp::draw(){

    // Draw the visual effect with the shader
        ofBackground(0);
    ofEnableBlendMode(OF_BLENDMODE_ADD);


//--------------------DYNAMIC CIRCLE
    // Dynamic Circle
    float circleSize = ofMap(duration, 0.1, 40, 10, 400);
    ofColor circleColor;
    circleColor.setHsb(ofMap(pitch, 0.0, 16.0, 0, 255), 255, 255, ofMap(amplitude, 0.0, 1.0, 0, 255));
    
    ofPushMatrix();
    ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
    ofSetColor(circleColor);
    int numVertices = ofMap(emission, 1, 1000, 3, 20);
    ofBeginShape();
    for (int i = 0; i < numVertices; i++) {
        float angle = ofMap(i, 0, numVertices, 0, TWO_PI);
        float xOffset = circleSize * cos(angle);
        float yOffset = circleSize * sin(angle);
        float x = xOffset + ofRandom(-circleSize * 0.1, circleSize * 0.1);
        float y = yOffset + ofRandom(-circleSize * 0.1, circleSize * 0.1);
        ofVertex(x, y);
    }
    ofEndShape(true);
    ofPopMatrix();


    // Lines radiating outwards
    int numLines = ofMap(emission, 1, 1000, 1, 100);
    float angleStep = 360.0 / numLines;
    

    ofPushMatrix();
    ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
    ofSetColor(circleColor);
    for(int i = 0; i < numLines; i++) {
        ofPushMatrix();
        ofRotate(angleStep * i);
        ofDrawLine(0, 0, circleSize, 0);
        ofPopMatrix();
    }
    ofPopMatrix();

    // Reverb effect
    float reverbCircleSize = ofMap(reverbSlider, 0.0, 0.3, 0, 300);
    ofSetColor(255, 255, 255, 255);
    ofNoFill();
    ofSetLineWidth(5);
    ofDrawCircle(ofGetWidth() / 2, ofGetHeight() / 2, reverbCircleSize);
    ofFill();
    ofDisableBlendMode();
    
    
    //--------------------DYNAMIC CIRCLE additional
    // Rectangles scaling with amplitude and affected by pitch
    float rectWidth = ofMap(amplitude, 0.0, 1.0, 10, 50);
    float rectHeight = ofMap(amplitude, 0.0, 1.0, 10, 200);
    float pitchFactor = ofMap(pitch, 0.0, 16.0, 0.5, 2.0);
    ofSetColor(circleColor);

    for (int i = 0; i < numLines; i++) {
        ofPushMatrix();
        ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
        ofRotate(angleStep * i);
        ofDrawRectangle(-rectWidth / 2 * pitchFactor, circleSize, rectWidth * pitchFactor, rectHeight * pitchFactor);
        ofPopMatrix();
    }


    
    // Animated waveform affected by emission and duration
    std::vector<float> frequencyData; // You should populate this vector with frequency data from the sound
    int numSamples = frequencyData.size();
    float waveStep = (float)ofGetWidth() / (float)numSamples;
    float waveHeight = ofMap(duration, 0.1, 40, 50, 300);

    ofSetColor(circleColor);
    ofBeginShape();
    for (int i = 0; i < numSamples; i++) {
        float x = i * waveStep;
        float y = ofMap(frequencyData[i], 0.0, 1.0, ofGetHeight() / 2 + waveHeight / 2, ofGetHeight() / 2 - waveHeight / 2);
        y *= ofMap(emission, 1, 1000, 0.5, 2.0); // Scale waveform based on emission
        ofVertex(x, y);
    }
    ofEndShape();

    

    //--------------------DYNAMIC CIRCLE end
    
        ofSetColor(200);
        ofDrawBitmapString(("Press H to hide the GUI"), 400, 20);
        ofDrawBitmapString(("Press SPACE to deactive leapMotion"), 400, 40);

        cam.begin();
        
        ofPushMatrix();
        ofRotate(90, 0, 0, 1);
        ofSetColor(20);
        //ofDrawGridPlane(800, 20, false);
        ofPopMatrix();
        
    for (int i = 0; i < simpleHands.size(); i++){
        bool isLeft = simpleHands[i].isLeft;
        ofPoint handPos = simpleHands[i].handPos;
        ofPoint handNormal = simpleHands[i].handNormal;
        
        // Define some random values for the color range
        float hueMin = ofRandom(0, 50);
        float hueMax = ofRandom(200, 255);
        float saturationMin = ofRandom(100, 150);
        float saturationMax = ofRandom(200, 255);
        float brightnessMin = ofRandom(50, 100);
        float brightnessMax = ofRandom(200, 255);
        
        // Set hand color based on amplitude and pitch
        float handHue = ofMap(pitch, 0.0, 16.0, hueMin, hueMax);
        float handSaturation = ofMap(amplitude, 0.0, 1.0, saturationMin, saturationMax);
        float handBrightness = ofMap(duration, 0.1, 40, brightnessMin, brightnessMax);
        ofColor handColor = ofColor::fromHsb(handHue, handSaturation, handBrightness);

        
        ofDrawSphere(handPos.x, handPos.y, handPos.z, 20);
        fingerType fingerTypes[] = {THUMB, INDEX, MIDDLE, RING, PINKY};

            
            ofSetColor(0, 0, 255);
            ofDrawSphere(handPos.x, handPos.y, handPos.z, 20);
            ofSetColor(255, 255, 0);
            ofDrawArrow(handPos, handPos + 100*handNormal);
            float fingerSaturation = ofRandom(100, 200);
            float fingerBrightness = ofRandom(100, 200);
            ofColor fingerColor = ofColor::fromHsb(handHue, fingerSaturation, fingerBrightness);
            ofColor jointColor = ofColor::fromHsb(handHue, fingerSaturation, fingerBrightness);
            float jointBrightness = ofMap(reverbSlider, 0.0, 0.3, 0.0, 255.0);
            float opennessBrightness = ofMap((isLeft ? leftHandOpen : rightHandOpen), 0.0, 1.0, 0.0, 255.0);
            
            for (int f=0; f<5; f++) {
                ofPoint mcp = simpleHands[i].fingers[ fingerTypes[f] ].mcp;  // metacarpal
                ofPoint pip = simpleHands[i].fingers[ fingerTypes[f] ].pip;  // proximal
                ofPoint dip = simpleHands[i].fingers[ fingerTypes[f] ].dip;  // distal
                ofPoint tip = simpleHands[i].fingers[ fingerTypes[f] ].tip;  // fingertip
                
                // Set finger color based on duration and pitch
                float fingerHue = ofMap(pitch, 0.0, 16.0, 0.0, 255.0);
                ofColor fingerColor = ofColor::fromHsb(fingerHue, 255.0, fingerBrightness);
                ofSetColor(fingerColor);

                ofDrawSphere(mcp.x, mcp.y, mcp.z, 12);
                ofDrawSphere(pip.x, pip.y, pip.z, 12);
                ofDrawSphere(dip.x, dip.y, dip.z, 12);
                ofDrawSphere(tip.x, tip.y, tip.z, 12);

                // Set joint color based on reverb and hand openness
                float jointHue = ofMap(reverbSlider, 0.0, 0.3, 0.0, 255.0);
                ofColor jointColor = ofColor::fromHsb(jointHue, 255.0, jointBrightness);
                ofSetColor(jointColor);
                ofSetLineWidth(20);
                ofLine(mcp.x, mcp.y, mcp.z, pip.x, pip.y, pip.z);
                ofLine(pip.x, pip.y, pip.z, dip.x, dip.y, dip.z);
                ofLine(dip.x, dip.y, dip.z, tip.x, tip.y, tip.z);
            }
            
        }
            for (int i = 0; i < simpleHands.size(); i++) {
                // ...
    
                for (int f = 0; f < 5; f++) {
                    // ...
    
                    // Draw particles
         
                    for (Particle &p : particleSystems[5 * i + f]) {
                        p.draw();
                    }
                }
            }
        
        cam.end();
                
            
    // Apply the flashing light effect if the amplitude is above the threshold
    float flashAmplitude = 0.0;
    if (amplitude > threshold) {
        flashAmplitude = (1 + sin(ofGetElapsedTimef() * flashFrequency * pitch * TWO_PI)) * 0.5; // CHANGED
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        shader.begin();
        ofSetColor(139, 0, 0, 150);
        shader.setUniform1f("amplitude", flashAmplitude);
        shader.setUniform3f("flashColor", 1.0, 1.0, 1.0); // Set the flash color (white in this case)
        ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
        shader.end();
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    }

    if (amplitude > threshold) {
        shader.end();
    }

    if (displayGUI) {
        ofSetDepthTest(false);
        gui.draw();
        //customDrawSlider();
    }
        
    }

void ofApp::keyPressed(int key){
    if(key == 'h'){
        displayGUI = !displayGUI;
    }else if(key == ' '){
        leapActive = !leapActive;
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
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}

//--------------------------------------------------------------
void ofApp::exit(){
    // let's close down Leap and kill the controller
    ofSoundStream();
    leap.close();
     //close the soundstream
}

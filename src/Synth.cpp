
#include "Synth.h"

void Synth::begin(){
	active = true;

	//amp and duration will be decided by the size of the hands opened
	//mainAmplitude = ofRandom(1,1);
	//using ofClamp to prevent clipping, audio thread
	amplitude = ofClamp((mainAmplitude + ofRandom(-leftHand, leftHand)),0, 2);
	//mainDuration = ofRandom(20, 300);
	duration = (mainDuration + ofRandom(-leftHand, leftHand));

	//speed = ofRandom(0.5, 4);
	//half speed down an octave, twice a speed up an octave
	speed = ofClamp((mainSpeed + ofRandom(-leftHand, leftHand)), 0, 16);



	float tempFreq = (1000 / duration);
	eValue = 0;//reset
	phasor->setPhase(0); //phasor starting at 0 set to the right period lengths and the right frequency
	phasor->setFreq(tempFreq);


	//when the hands are in middle and completely opened, it can play anywhere the soundfile
    x = ofClamp(xMain+ofRandom(rightHand * -0.5,rightHand * 0.5), 0 , 3);
    y = ofClamp(yMain+ofRandom(rightHand * -0.5,rightHand * 0.5), 0 , 1);
	
	//soundPlayer->setLoopMode(0);//0: off, 1: standard looping, 2: ping-pong
	for (int i=0; i < 2; i++) {
		soundPlayers[(int)ofClamp(int(x) + i, 0, 3)]->setLoopEnabled(false);
		soundPlayers[(int)ofClamp(int(x) + i, 0, 3)]->play(speed);
		soundPlayers[(int)ofClamp(int(x) + i, 0, 3)]->setLocation(y);

	}
}

void Synth::process() {
	//let the process start when grain alive
	if (active) {
		soundPlayers[int(x)]->process();
        
		if (x < 3) {
			soundPlayers[int(x) + 1] ->process();
		}
		float sample1 = soundPlayers[int(x)]->getSample() * (1 - (x - int(x)));
		float sample2;
		if (x < 3) {
			sample2 = soundPlayers[int(x)]->getSample() * (x - int(x));
		}else {
			sample2 = sample1 * (x - int(x));
		}

		phasor->process();
		//currentSample = (sample1 + sample2) * amplitude * triWindow(phasor->getSample());//triangle window get rid of the clip sound
             currentSample = (sample1 + sample2) * amplitude * skewWindow(phasor->getSample(),0.9);
        
		//make the grain sound with every impulse
		if (phasor->getSample() - eValue < 0) {
			currentSample = 0;
			end();
		}
		eValue = phasor->getSample();
	}
	else {
		currentSample = 0;
	}
}

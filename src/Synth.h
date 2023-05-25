#pragma once

#ifndef Synth_hpp
#define Synth_hpp

#include <stdio.h>
#include "ofMain.h"
#include "ofxATK.hpp"

class Synth {

public:

	Synth(SoundFile* _sound1, SoundFile* _sound2, SoundFile* _sound3, SoundFile* _sound4) {

		sound1 = _sound1;
		sound2 = _sound2;
		sound3 = _sound3;
		sound4 = _sound4;

		soundPlayers[0] = new SoundPlayer(sound1);
		soundPlayers[1] = new SoundPlayer(sound2);
		soundPlayers[2] = new SoundPlayer(sound3);
		soundPlayers[3] = new SoundPlayer(sound4);

		active = false;
		phasor = new TPhasor(1);
	}

	void begin();//start the grain
	void end() {
		active = false;
	}//end the grain

	void process();
	float getSample() {
		return currentSample;
	}

	bool getActive() {
		return active;
	}
	void setMainDuration(float newDuration) {
		mainDuration = newDuration;
	}

	void setMainAmplitude(float newAmplitude) {
		mainAmplitude = newAmplitude;
	}

	void setLeftHandOpen(float newOpen) {
		leftHand = newOpen;
	}

	void setRightHandOpen(float newOpen) {
		rightHand = newOpen;
	}

	void setMainPitch(float newSpeed) {
		mainSpeed = newSpeed;
	}

	void setX(float newX) {
		xMain = newX;
	}
	
	void setY(float newY) {
		yMain = newY;
	}




private:
	SoundFile* sound1;
	SoundFile* sound2;
	SoundFile* sound3;
	SoundFile* sound4;
	SoundPlayer* soundPlayers[4];

	float currentSample;
	float position;

	//create some stochastic, main means a value which is the center, and the value change based on that value.
	//For example if set mainDuration to 200, the value will be up to 250 and down to 150, so 200 is the center.
	float mainDuration;
	float duration;
	float mainAmplitude;
	float amplitude;


	//this is to reduce the clipping sound
	//when you change the x location which switches the sound file
	float mainSpeed;
	float speed;



	float leftHand;
	float rightHand;

	float xMain;
	float x;

	float yMain;
	float y;

	bool active;

	float eValue;//edge detect

	//phasor
	TPhasor* phasor;

};

#endif /*Synth_hpp*/

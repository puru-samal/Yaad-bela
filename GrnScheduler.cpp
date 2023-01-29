#include <Bela.h>
#include <cmath>
#include <stdlib.h> 
#include <libraries/math_neon/math_neon.h>
#include "GrnScheduler.h"

// Class constructor
GrnScheduler::GrnScheduler(int t, GrnSrc& s, GUIData& g) : Src(s), Gui(g), tapNum(t) {}

// Class destructor
GrnScheduler::~GrnScheduler(){}

// Call in setup
void GrnScheduler::setup(){
	this->grnTrig   = 0;
	this->readPtr   = 0;
	this->currVoice = -1;
}

// Set individual params
void GrnScheduler::setParam(tapParam t){
	switch(t) {
		float target;
		
		case delayTime:
			// One pole filter transition
			// Problem: this->sz doesnt quite reach target (asymptote)
			// Solution: Set target to be a bit higher or lower, and stop once actual target is reached
			target = (this->Gui.getTapParam(delayTime, this->tapNum) * this->Src.sampleRate) * 0.001; //mstosamps
			if (this->prev_dtime > target) {
				this->dtime =  min(target, (target + 1) * (1 - this->alpha) + this->prev_dtime * this->alpha);
				this->prev_dtime = this->dtime;
			} else if (this->prev_dtime < target) {
				this->dtime =  max(target, (target - 1) * (1 - this->alpha) + this->prev_dtime * this->alpha);
				this->prev_dtime = this->dtime;
			}
			break;
		
		case grnSz:
			// One pole filter transition
			// Problem: this->sz doesnt quite reach target (asymptote)
			// Solution: Set target to be a bit higher or lower, and stop once actual target is reached
			target = (this->Gui.getTapParam(grnSz, this->tapNum) * this->Src.sampleRate) * 0.001; //mstosamps
			if (this->prev_sz > target) {
				this->sz =  min(target, (target + 1) * (1 - this->alpha) + this->prev_sz * this->alpha);
				this->prev_sz = this->sz;
			} else if (this->prev_sz < target) {
				this->sz =  max(target, (target - 1) * (1 - this->alpha) + this->prev_sz * this->alpha);
				this->prev_sz = this->sz;
			}
			break;
	
		case plyRt:
			{
				//Increment-based transition
				float noise = 2.0 * (float)rand() / (float)RAND_MAX - 1.0;
				float pch = noise * 0.5 * this->Gui.getGlobalParam(detune) + this->Gui.getTapParam(plyRt, this->tapNum);
				this->rt = (this->grnTrig == 1) ? powf_neon(2, pch/12) : this->rt;
				break;
			}
		case plyDir:
			target = (int) this->Gui.getTapParam(plyDir, this->tapNum);
			if (target == 0){
				this->dir = (this->voiceIsActive[0] && this->grnTrig) ? ((rand() > RAND_MAX/2) ? -1 : 1) : this->dir;	
			} else {
				this->dir = target;
			}
			break;
		
		case grnEnv:
			this->env = (int) this->Gui.getTapParam(grnEnv, this->tapNum);
			break;
		
		case tapLPF:
			this->lpf = this->Gui.getTapParam(tapLPF, this->tapNum);
			break;
		
		case tapHPF:
			this->hpf = this->Gui.getTapParam(tapHPF, this->tapNum);
			break;
		
	};
}

// Set all params
void GrnScheduler::setAllParams() {
	setParam(delayTime);
	setParam(grnSz);
	setParam(plyRt);
	setParam(plyDir);
	setParam(grnEnv);
	setParam(tapLPF);
	setParam(tapHPF);
}

// Increments read pointer, Checks for grain triggers, Each tap has 8 grain voices
void GrnScheduler::tapScheduler(){
	// Read Pointer
	this->readPtr = this->Src.writePtr - this->dtime;
	if(this->readPtr > this->Src.numFrames)
		this->readPtr -= this->Src.numFrames;
	else if(this->readPtr < 0)
		this->readPtr += this->Src.numFrames;
	
	// Grain Trigger
	if ((int) this->sz == 0) {
		this->grnTrig = -1; // Avoid divide by zero
		this->sz = 512.0;
	} else {
		this->grnTrig =  ((int) fmodf_neon(this->Src.writePtr, this->sz) == 0) ? 1 : 0; // 1 when sz is hit, 0 when not
	}
	
	// Voice Incrementor based in gTrig
	this->currVoice = (this->grnTrig) == 1 ? (this->currVoice + 1) % this->maxVoices : this->currVoice;
	
	// Voice Schedulers
	for (unsigned int voice = 0; voice < this->maxVoices; voice++){
		voiceScheduler(voice);
	}
}

// Sets per-voice state, generates grain and envelope pointers 
void GrnScheduler::voiceScheduler(int voiceNum){
	// Set Voice State
	this->voiceIsActive[voiceNum] = (this->currVoice == voiceNum) ? 1 : 0;
	
	// Voice Trigger
	int trig = (this->voiceIsActive[voiceNum] && this->grnTrig);
	
	// Sample the read pointer when trigger is recieved and this voice is active
	this->samp_readPtr[voiceNum] = trig ? this->readPtr : this->samp_readPtr[voiceNum];
	
	float offset = (this->rt - 1) * this->sz;
	
	if(this->dir == 1) {
		offset = constrain(offset, 0, this->Src.numFrames - this->dtime); 
		offset = this->samp_readPtr[voiceNum] - offset;
		
		if(offset > this->Src.numFrames)
			offset -= this->Src.numFrames;
		else if(offset < 0)
			offset += this->Src.numFrames;
		
		this->grnCounter[voiceNum] = trig ? 0 : this->grnCounter[voiceNum] + this->rt;
		this->grnCounter[voiceNum] = constrain(this->grnCounter[voiceNum], 0, this->rt * this->sz);
	}
	else if (this->dir == -1) {
		offset = this->samp_readPtr[voiceNum] + this->dtime;
		
		if(offset > this->Src.numFrames)
			offset -= this->Src.numFrames;
		else if(offset < 0)
			offset += this->Src.numFrames;
		
		this->grnCounter[voiceNum] = trig ? 0 : this->grnCounter[voiceNum] - this->rt;
		this->grnCounter[voiceNum] = constrain(this->grnCounter[voiceNum], -1.0 * this->rt * this->sz, 0);
	}
	
	this->envPtr[voiceNum] = trig ? 0 : this->envPtr[voiceNum] + 1;
	this->envPtr[voiceNum] = min(this->envPtr[voiceNum], this->sz);
	
	this->grnPtr[voiceNum] = offset + this->grnCounter[voiceNum];
	if(this->grnPtr[voiceNum] > this->Src.numFrames)
		this->grnPtr[voiceNum] -= this->Src.numFrames;
	else if(offset < 0)
		this->grnPtr[voiceNum] += this->Src.numFrames;

}

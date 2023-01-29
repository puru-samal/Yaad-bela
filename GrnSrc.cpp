#include <cmath>
#include <vector>
#include "GrnSrc.h"

constexpr float GrnSrc::maxDelay;

// Initialize Grain Source
GrnSrc::GrnSrc(int numChans, float sR) {
	setup(numChans, sR);
}

// Destructor
GrnSrc::~GrnSrc(){}

// Called in setup()
void GrnSrc::setup(int numChans, float sR) {
	this->sampleRate = sR;
	this->numChannels = numChans;
	this->delayBuf.resize(this->numChannels);
	for (unsigned int channel = 0; channel < this->numChannels; channel++) {
		this->delayBuf[channel].resize(this->maxDelay * this->sampleRate);
	}
	this->numFrames = this->delayBuf[0].size();
}

// Begin incrementing write pointer
void GrnSrc::process(int bypass_state){
		this->writePtr = (bypass_state == 0) ? 0.0 : 
						((++this->writePtr >= this->numFrames) ? 0.0f : this->writePtr);
}

// Linearly-interpolated buffer write - - idx: frameNumber, channel: 0(left), 1(right), input: float
void GrnSrc::bufWr(float idx, int channel, float input){
	if (idx < 0)
		idx += this->numFrames;
	else if (idx > this->numFrames)
		idx -= this->numFrames;
		
	int x0 = floor(idx);
	if ((idx - x0) == 0.0f){
		this->delayBuf[channel][x0] = input;
	} else {
		int x1 = x0 + 1;
		x1 = (x1 >= numFrames) ? 0 : x1;
		float fracAbove = idx - x0;
		float fracAbove_ = 1/fracAbove;
		float fracBelow = 1.0 - fracAbove;
		this->delayBuf[channel][x1] = (input - fracBelow * this->delayBuf[channel][x0]) * fracAbove_;
	}
}

// Linearly-interpolated buffer write - idx: frameNumber, channel: 0(left), 1(right).
float GrnSrc::bufRd(float idx, int channel){
	if (idx < 0)
		idx += this->numFrames;
	else if (idx > this->numFrames)
		idx -= this->numFrames;
		
	int x0 = floor(idx);
	int x1 = x0 + 1;
	x1 = (x1 >= numFrames) ? 0 : x1;
	float fracAbove = idx - x0;
	float fracBelow = 1.0 - fracAbove;
	float out = fracBelow * this->delayBuf[channel][x0] 
				+ fracAbove * this->delayBuf[channel][x1];
	return out;
}

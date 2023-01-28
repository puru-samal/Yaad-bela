#include <vector>
#include "Granulator.h"

// Class constructor
Granulator::Granulator(GrnScheduler& s) : scheduler(s) {}

// Class destructor
Granulator::~Granulator(){}

// Called in setup()
void Granulator::setup(){
	this->grnOut.resize(this->scheduler.Src.numChannels);
	for(unsigned int channel = 0; channel < this->scheduler.Src.numChannels; channel++) {
		this->grnOut[channel].resize(this->scheduler.maxVoices);
	}
}

//  Generates grain output. Stored in this->grnOut[channel][voice]
void Granulator::process(){
	for (unsigned int channel = 0; channel < this->scheduler.Src.numChannels; channel++) {
		for (unsigned int voice = 0; voice < this->scheduler.maxVoices; voice++) {
			this->grnOut[channel][voice] 
								= this->scheduler.Src.bufRd(this->scheduler.grnPtr[voice], channel);
		}
	}
}



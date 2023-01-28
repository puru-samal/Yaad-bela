#pragma once
#include <vector>
#include "GrnScheduler.h"

class Granulator {
	/*
	- Has a reference to a tap scheduler
	- Creates grains from source based on the scheduler
	- Per-voice grain output is stored in grnOut
	- Stored in split format. Can be accessed with grnOut[channel][voice]
	*/
	public:
	GrnScheduler& scheduler; // Reference to scheduler
	std::vector<std::vector<float>> grnOut; // Contains grain out for each channel and voice
	
	Granulator(GrnScheduler& s);
	~Granulator();
	
	void setup();
	void process();
	
	private:
	
};
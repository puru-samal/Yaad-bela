#pragma once
#include <vector>
#include "GrnScheduler.h"

class GrnEnv {
	/*
	- Has a reference to the tap scheduler
	- Generates per-voice grain envelope
	- Envelope can be accessed via this->envOut[voice]
	*/
	public:
	static constexpr int tableLength = 512;
	GrnScheduler& scheduler;   // Reference to tap scheduler
	std::vector<float> envOut; // Contains envelope output for each voice
	
	std::vector<float> bell  = std::vector<float>(tableLength);
	std::vector<float> perc  = std::vector<float>(tableLength);
	std::vector<float> swell = std::vector<float>(tableLength);
	
	GrnEnv(GrnScheduler& s);
	~GrnEnv();
	
	void setup();
	void process();
	float cosineBell(float phase);
	float cosinePerc(float phase);
	float cosineSwell(float phase);
	float setLUT_Bell(int phase, int LUT_sz);
	float setLUT_Perc(int phase, int LUT_sz);
	float setLUT_Swell(int phase, int LUT_sz);
	
	
	private:
	
};
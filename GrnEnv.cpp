#include <cmath>
#include <libraries/math_neon/math_neon.h>
#include "GrnEnv.h"

// Class constructor
GrnEnv::GrnEnv(GrnScheduler& s) : scheduler(s) {}

// Class destructor
GrnEnv::~GrnEnv(){}

// Call in setup
void GrnEnv::setup(){
	this->envOut.resize(this->scheduler.maxVoices);
	
	// Set grn envelopes in LUT's
	for (unsigned int n = 0; n < this->tableLength; n++){
		this->bell[n]  = setLUT_Bell(n, this->tableLength);
		this->perc[n]  = setLUT_Perc(n, this->tableLength);
		this->swell[n] = setLUT_Swell(n, this->tableLength);
	}
}

// Generates envelope output. LUT approach used. Output stored and can be accesed through envOut->voice
void GrnEnv::process(){
	for (unsigned int voice = 0; voice < this->scheduler.maxVoices; voice++){
		
		float idx = (this->scheduler.envPtr[voice] / this->scheduler.sz) * this->tableLength;
		int x0 = floor(idx);
		int x1 = x0 + 1;
		x1 = (x1 >= this->tableLength) ? 0 : x1;
		float fracAbove = idx - x0;
		float fracBelow = 1.0 - fracAbove;

		switch(this->scheduler.env) {
			case -1:
				this->envOut[voice] = fracBelow * this->swell[x0] + fracAbove * this->swell[x1];
				break;
			case 0:
				this->envOut[voice] = fracBelow * this->bell[x0] + fracAbove * this->bell[x1];
				break;
			case 1:
				this->envOut[voice] = fracBelow * this->perc[x0] + fracAbove * this->perc[x1];;
				break;
			default:
				this->envOut[voice] = 0.0f;
				break;
		}
	}
}

// Cosine bell envelope: phase -> envPtr (accessed from scheduler). Computationally expensive!
float GrnEnv::cosineBell(float phase) {
	float envOut = 0.0f;
	float atkDur = this->scheduler.sz * 0.5;
	float relDur = this->scheduler.sz * 0.5;
	
	float atkDur_ = 1/atkDur;
	float relDur_ = 1/relDur;
	
	if (phase <= atkDur) {
		envOut = (1.0 - cosf_neon(M_PI * phase * atkDur_)) * 0.5;
	}
	else if (phase > atkDur && phase < (this->scheduler.sz - relDur)) {
		envOut = 1.0f;
	}
	else if (phase >= (this->scheduler.sz - relDur)) {
		envOut = (1.0 - cosf_neon(M_PI * phase * relDur_)) * 0.5;
	}
	
	return envOut;
}

// Cosine perc envelope: phase -> envPtr (accessed from scheduler). Computationally expensive!
float GrnEnv::cosinePerc(float phase) {
	float envOut = 0.0f;
	float atkDur = this->scheduler.sz * 0.125;
	float relDur = this->scheduler.sz * 0.875;
	
	float atkDur_ = 1/atkDur;
	float relDur_ = 1/relDur;
	
	if (phase <= atkDur) {
		envOut = (1.0 - cosf_neon(M_PI * phase * atkDur_)) * 0.5;
	}
	else if (phase > atkDur && phase < (this->scheduler.sz - relDur)) {
		envOut = 1.0f;
	}
	else if (phase >= (scheduler.sz - relDur)) {
		envOut = (1.0 - cosf_neon(M_PI * relDur_ * (phase - this->scheduler.sz))) * 0.5; 
	}

	return envOut;
}

// Cosine swell envelope: phase -> envPtr (accessed from scheduler) 
float GrnEnv::cosineSwell(float phase) {
	float envOut = 0.0f;
	float atkDur = this->scheduler.sz * 0.875;
	float relDur = this->scheduler.sz * 0.125;
	
	float atkDur_ = 1/atkDur;
	float relDur_ = 1/relDur;
	
	if (phase <= atkDur) {
		envOut = (1.0 - cosf_neon(M_PI * phase * atkDur_)) * 0.5;
	}
	else if (phase > atkDur && phase < (this->scheduler.sz - relDur)) {
		envOut = 1.0f;
	}
	else if (phase >= (scheduler.sz - relDur)) {
		envOut = (1.0 - cosf_neon(M_PI * relDur_ * (phase - this->scheduler.sz))) * 0.5;
	}

	return envOut;
}

// Load cosine bell envelope into LUT
float GrnEnv::setLUT_Bell(int phase, int LUT_sz) {
	float envOut = 0.0f;
	float atkDur = (int) LUT_sz * 0.5;
	float relDur = (int) LUT_sz * 0.5;
	
	float atkDur_ = 1/atkDur;
	float relDur_ = 1/relDur;
	
	if (phase <= atkDur) {
		envOut = (1.0 - cosf_neon(M_PI * phase * atkDur_)) * 0.5;
	}
	else if (phase > atkDur && phase < (LUT_sz - relDur)) {
		envOut = 1.0f;
	}
	else if (phase >= (LUT_sz - relDur)) {
		envOut = (1.0 - cosf_neon(M_PI * phase * relDur_)) * 0.5;
	}
	
	return envOut;
	
}

// Load cosine perc envelope into LUT	
float GrnEnv::setLUT_Perc(int phase, int LUT_sz) {
	float envOut = 0.0f;
	float atkDur = (int) LUT_sz * 0.125;
	float relDur = (int) LUT_sz * 0.875;
	
	float atkDur_ = 1/atkDur;
	float relDur_ = 1/relDur;
	
	if (phase <= atkDur) {
		envOut = (1.0 - cosf_neon(M_PI * phase * atkDur_)) * 0.5;
	}
	else if (phase > atkDur && phase < (LUT_sz - relDur)) {
		envOut = 1.0f;
	}
	else if (phase >= (scheduler.sz - relDur)) {
		envOut = (1.0 - cosf_neon(M_PI * relDur_ * (phase - LUT_sz))) * 0.5; 
	}

	return envOut;
	
}

// Load cosine swell envelope into LUT
float GrnEnv::setLUT_Swell(int phase, int LUT_sz) {
	float envOut = 0.0f;
	float atkDur = (int) LUT_sz * 0.875;
	float relDur = (int) LUT_sz * 0.125;
	
	float atkDur_ = 1/atkDur;
	float relDur_ = 1/relDur;
	
	if (phase <= atkDur) {
		envOut = (1.0 - cosf_neon(M_PI * phase * atkDur_)) * 0.5;
	}
	else if (phase > atkDur && phase < (LUT_sz - relDur)) {
		envOut = 1.0f;
	}
	else if (phase >= (scheduler.sz - relDur)) {
		envOut = (1.0 - cosf_neon(M_PI * relDur_ * (phase - LUT_sz))) * 0.5;
	}

	return envOut;
	
}

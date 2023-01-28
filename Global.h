#pragma once
#include "GUIData.h"
#include "param.h"

class Global {
	public:
	GUIData& Gui; // Reference to global gui data
	
	// Global parameters
	int _bypass;
	int _tempo;
	float _feedback;
	float _dryWet;
	float _spread;
	float _detune;
	int _nTaps;
	float _inLvl;
	float _outLvl;
	
	// Prev values (for one-pole filter)
	float alpha = 0.99;
	float prev_dryWet;
	float prev_feedback;
	float prev_spread;
	float prev_detune;
	float prev_inLvl;
	float prev_outLvl;
	
	Global(GUIData& g);
	~Global();
	
	void setGlobalParam(globalParam gP);
	void setGlobalParams();
	
	private:
	
};
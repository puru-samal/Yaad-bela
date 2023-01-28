#pragma once 
#include "param.h"

class GUIData {
	/*
	- An interface for accessing data from the GUIData
	- Gets global parameters: Bypass, Tempo, Feedback, Dry/Wet, Spread, Detune, nTaps, In_Lvl, Out_Lvl
	- Gets tap parameters: Delay time, grain size, playback rate, playback direction, grain envelope, LPF Cutoff, HPF Cutoff
	*/
	public:
		float* params;
		int maxTaps = 8;

		GUIData();
		~GUIData();
		
		void process(float* p);
		float getGlobalParam(globalParam p);
		float getTapParam(tapParam t, int tapNum);
		
		
	private:
	
};


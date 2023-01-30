#pragma once
#include <vector>
#include "GrnSrc.h"
#include "GUIData.h"
#include "Global.h"
#include "GrnScheduler.h"
#include "GrnEnv.h"
#include "Granulator.h"

class GrnTap {
	public:
	/*
	- Communicates with the GUIData class and Global class
	- Provides a one-stop interface for schedulers, granulators and envelopes
	- Gets the output of taps
	*/
	
	int tapNum;   // Current Tap number [1...8]
	GrnSrc& Src;  // Reference to grain source
 	GUIData& Gui; // Reference to data from GUI
 	Global& Glob;
	GrnScheduler scheduler; // Grain scheduler object
	GrnEnv env;             // Grain envelope object
	Granulator grn;         // Granulator object
	float pan;              // Panning
	
	GrnTap(int t, GrnSrc& s, GUIData& g, Global& glb);
	~GrnTap();
	
	void setup();
	void setParams();
	void process(); // Starts scheduler, granulator and envelope processing
	float out(int channel); // Get tap output
	
	private:
	
};
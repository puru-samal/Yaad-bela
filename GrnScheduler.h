#pragma once 
#include "GrnSrc.h"
#include "GUIData.h"
#include "param.h"

class GrnScheduler {
	/*
	- Communicates with the GUIData class
	- Schedules the creation of grains
	- Generates read pointers for granulator
	- Generates read pointers for envelope
	*/
	
	public:
	static constexpr int maxVoices = 1; // Each Tap has 2 grain voices
	GrnSrc& Src;  // Reference to global grain source
	GUIData& Gui; // Reference to global gui data
	
	float readPtr;  	  // Read Pointer
	unsigned int grnTrig; // Grain Trigger
	float samp_readPtr[maxVoices]  = {0.0f}; // Sample readPtr when trigger is recieved for each voice
	float grnCounter[maxVoices]    = {0.0f}; // A grain counter that increments when trigger is recieved
	float grnPtr[maxVoices]        = {0.0f}; // Per-voice read pointer for the granulator
	float envPtr[maxVoices]        = {0.0f}; // Per-voice read pointer for the envelopes
	int tapNum;     // Current Tap number [1...8]
	int currVoice;  // Current active voice [0..7]
	int voiceIsActive[maxVoices] = {0};   // Voice state, 1 if voice is active
	
	// Tap Params
	float dtime; // Delay Time (in samples)
	float sz;    // Grain Size (in samples)
	float rt;    // Playback Rate (in semitones[-24..24])
	int dir;     // Playback Direction (1: Fwd, -1: Rev, 0: Rnd)
	int env;     // Grain Envelope (1: Perc, -1: Swell, 0: Bell)
	float lpf;   // LPF Cutoff (20hz - 20kHz)
	float hpf;   // HPF Cutoff (20hz - 20kHz)
	int kill;
	
	// Prev values (for one-pole filter)
	float alpha = 0.99; // Affects speed of transition of parameter (0.99->slow, 0.9999->fast)
	float prev_dtime;
	float prev_sz;
	float prev_rt;
	
	GrnScheduler(int t, GrnSrc& s, GUIData& g);
	~GrnScheduler();
	
	void setup();
	void setParam(tapParam t); 
	void setAllParams();       
	void tapScheduler();      
	void voiceScheduler(int voiceNum); // Per-voice scheduler
	
	private:

};
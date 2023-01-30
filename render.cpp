/*
Yaad: A variable multitap granular delay effect.
Author: Purusottam Samal
*/
#include <Bela.h>
#include <libraries/Gui/Gui.h>
#include <libraries/Scope/Scope.h>
#include <libraries/AudioFile/AudioFile.h>
#include <vector>
#include "Global.h"
#include "GrnTap.h"

// Maximum Number of Taps
int MAX_TAPS = 6;

Scope scope;
Gui gui;

GrnSrc Src;
GUIData Interface;
Global Glob(Interface);

std::vector<float> TapsOut(2, 0); // Holds output from taps

// Tap objects
GrnTap Tap1(1, Src, Interface, Glob);
GrnTap Tap2(2, Src, Interface, Glob);
GrnTap Tap3(3, Src, Interface, Glob);
GrnTap Tap4(4, Src, Interface, Glob);
GrnTap Tap5(5, Src, Interface, Glob);
GrnTap Tap6(6, Src, Interface, Glob);

// Butterworth coefficients for low-pass filter @ 8000Hz
float a0 = 0.1772443606634904;
float a1 = 0.3544887213269808;
float a2 = 0.1772443606634904;
float a3 = -0.5087156198145868;
float a4 = 0.2176930624685485;

// Previous two input and output values for each channel (required for applying the filter)
std::vector<float> x_1(2, 0);
std::vector<float> x_2(2, 0);
std::vector<float> y_1(2, 0);
std::vector<float> y_2(2, 0);

// Dummy ptr for test signal
int elapsedSamps;

// Test signal
std::string fileName = "pluck.wav";
int startFrame = 0;
int endFrame = 22050;
std::vector<std::vector<float>> sampleData;

bool setup(BelaContext *context, void *userData)
{
	// Set up GUI 
	gui.setup(context->projectName);
	gui.setBuffer('f', 65);
	
	// Load Audio File
	sampleData = AudioFileUtilities::load(fileName, endFrame - startFrame, startFrame);
	
	// Setup 
	Src.setup(2, context->audioSampleRate);
	Tap1.setup();
	Tap2.setup();
	Tap3.setup();
	Tap4.setup();
	Tap5.setup();
	Tap6.setup();
	
	// Debugging
	scope.setup(8, context->audioSampleRate);
	elapsedSamps = 0;
	
	return true;
}

void render(BelaContext *context, void *userData) // Called each block
{	
	// GUI Data 
	DataBuffer& guiData = gui.getDataBuffer(0);
	Interface.process(guiData.getAsFloat());
	Glob.setGlobalParams();
	
	
	if (gui.isConnected()) {
		for (unsigned int n = 0; n < context->audioFrames; n++){
			

			if(++elapsedSamps > endFrame)
				elapsedSamps = startFrame;
			
			Src.process(Glob._bypass);
			
			// Set params from GUI
			Tap1.setParams();
			Tap2.setParams();
			Tap3.setParams();
			Tap4.setParams();
			Tap5.setParams();
			Tap6.setParams();
			
			// Begin processing taps (scheduler, granulator, envelope)
			Tap1.process();
			Tap2.process();
			Tap3.process();
			Tap4.process();
			Tap5.process();
			Tap6.process();
		
			for (unsigned int channel = 0; channel < context->audioOutChannels; channel++){
				
				// Audio input
				float in = sampleData[channel][elapsedSamps] * Glob._inLvl;
				
				// Tap output
				TapsOut[channel] = Tap1.out(channel) 
								 + Tap2.out(channel) 
								 + Tap3.out(channel) 
								 + Tap4.out(channel)
								 + Tap5.out(channel)
								 + Tap6.out(channel);
				
				// Input + scaledfeedback * tapOutput -> Biquad LP Filter (8000Hz) -> Buffer write
				int scl = (Glob._nTaps == 0) ? 1 : Glob._nTaps;
				float bufIn = in + (Glob._feedback/scl) * TapsOut[channel];
				
				// Biquad LPF 
				float tempIn = bufIn; 
				
				bufIn = a0 * bufIn
					  + a1 * x_1[channel]
					  + a2 * x_2[channel]
					  + a3 * Src.bufRd(Src.writePtr - 1, channel)
					  + a4 * Src.bufRd(Src.writePtr - 2, channel);
					  
				x_2[channel] = x_1[channel];
        		x_1[channel] = tempIn;
        		y_2[channel] = y_1[channel];
    			y_1[channel] = bufIn;
				
				// Write into buffer
				Src.bufWr(Src.writePtr, channel, bufIn);
				
				// Output
				float out = (1.0f - Glob._dryWet) * in + Glob._dryWet * TapsOut[channel];
				
				// Write output to output device
		    	audioWrite(context, n, channel, out * Glob._outLvl);
				
			}
		}
	}
}

void cleanup(BelaContext *context, void *userData)
{

}


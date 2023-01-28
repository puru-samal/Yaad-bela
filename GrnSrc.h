#pragma once 
#include <vector>

/*
TO DO:
- Implement Bypass functionality, stop writing to buffer when Bypass == 1.
- (Optional) Implement an option of cubic hermite interpolation for buffer read and buffer writes.
*/

class GrnSrc {
	/*
	- Handles audio input
	- Maintains a circular delay buffer
	- Increments the write pointer 
	- Read from and writes to the delay buffer
	*/
	public:
		static constexpr float maxDelay = 10.0f; // Maximum allowed delaytime
		std::vector<std::vector<float>> delayBuf; // Delay Buffer
		float sampleRate; // Sample Rate, set from BelaContext
		int numFrames;   
		int numChannels;
		float writePtr = 0.0f; // Write Pointer
		
		GrnSrc(){}; // Default Constructor
		GrnSrc(int numChannels, float sampleRate);
		~GrnSrc();
		
		void setup(int numChannels, float sampleRate);
		void process(int bypass_state);
		void bufWr(float idx, int channel, float input);
		float bufRd(float idx, int channel);
	
	private:
		
};
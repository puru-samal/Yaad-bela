#include <Bela.h>
#include "Global.h"

// Constructor
Global::Global(GUIData& g) : Gui(g) {}

// Destructor
Global::~Global(){}

// Set individual params
void Global::setGlobalParam(globalParam gP){
	switch(gP) {
		float target;
		
		case bypass:
			target = this->Gui.getGlobalParam(bypass);
			this->_bypass = (int) target;
			break;
		
		case tempo:
			target = this->Gui.getGlobalParam(tempo);
			this->_tempo = (int) target;
			break;
		
		case feedback:
			target = this->Gui.getGlobalParam(feedback);
			if (this->prev_feedback > target) {
				this->_feedback =  min(target, (target + 1) * (1 - this->alpha) + this->prev_feedback * this->alpha);
				this->prev_feedback = this->_feedback;
			} else if (this->prev_feedback < target) {
				this->_feedback =  max(target, (target - 1) * (1 - this->alpha) + this->prev_feedback * this->alpha);
				this->prev_feedback = this->_feedback;
			}
			break;
		
		case dryWet:
			target = this->Gui.getGlobalParam(dryWet);
			if (this->prev_dryWet > target) {
				this->_dryWet =  min(target, (target + 1) * (1 - this->alpha) + this->prev_dryWet * this->alpha);
				this->prev_dryWet = this->_dryWet;
			} else if (this->prev_dryWet < target) {
				this->_dryWet =  max(target, (target - 1) * (1 - this->alpha) + this->prev_dryWet * this->alpha);
				this->prev_dryWet = this->_dryWet;
			}
			break;
		
		case spread:
			target = this->Gui.getGlobalParam(spread);
			if (this->prev_spread > target) {
				this->_spread =  min(target, (target + 1) * (1 - this->alpha) + this->prev_spread * this->alpha);
				this->prev_spread = this->_spread;
			} else if (this->prev_spread < target) {
				this->_spread =  max(target, (target - 1) * (1 - this->alpha) + this->prev_spread * this->alpha);
				this->prev_spread = this->_spread;
			}
			break;
		
		case detune:
			target = this->Gui.getGlobalParam(detune);
			if (this->prev_detune > target) {
				this->_detune =  min(target, (target + 1) * (1 - this->alpha) + this->prev_detune * this->alpha);
				this->prev_detune = this->_detune;
			} else if (this->prev_detune < target) {
				this->_detune =  max(target, (target - 1) * (1 - this->alpha) + this->prev_detune * this->alpha);
				this->prev_detune = this->_detune;
			}
			break;
		
		case nTaps:
			target = this->Gui.getGlobalParam(nTaps);
			this->_nTaps = (int) target;
			break;
		
		case inLvl:
			target = this->Gui.getGlobalParam(inLvl);
			if (this->prev_inLvl > target) {
				this->_inLvl =  min(target, (target + 1) * (1 - this->alpha) + this->prev_inLvl * this->alpha);
				this->prev_inLvl = this->_inLvl;
			} else if (this->prev_inLvl < target) {
				this->_inLvl =  max(target, (target - 1) * (1 - this->alpha) + this->prev_inLvl * this->alpha);
				this->prev_inLvl = this->_inLvl;
			}
			break;
		
		case outLvl:
			target = this->Gui.getGlobalParam(outLvl);
			if (this->prev_outLvl > target) {
				this->_outLvl =  min(target, (target + 1) * (1 - this->alpha) + this->prev_outLvl * this->alpha);
				this->prev_outLvl = this->_outLvl;
			} else if (this->prev_outLvl < target) {
				this->_outLvl =  max(target, (target - 1) * (1 - this->alpha) + this->prev_outLvl * this->alpha);
				this->prev_outLvl = this->_outLvl;
			}
			break;
	}	
}

// Set all params
void  Global::setGlobalParams() {
	setGlobalParam(bypass);
	setGlobalParam(tempo);
	setGlobalParam(feedback);
	setGlobalParam(dryWet);
	setGlobalParam(spread);
	setGlobalParam(detune);
	setGlobalParam(nTaps);
	setGlobalParam(inLvl);
	setGlobalParam(outLvl);
}



#include "param.h"
#include "GUIData.h"

// Class constructor
GUIData::GUIData(){};

// Class destructor
GUIData::~GUIData(){};

// To run in render, gets parameter from GUI
void GUIData::process(float* p){
	this->params = p;
}

// Gets global parameter based on enum argument
float GUIData::getGlobalParam(globalParam p){
	return this->params[(int) p];
}

// Gets global parameter based on enum argument and tap num
float GUIData::getTapParam(tapParam t, int tapNum){
	// tapNum - [1...8]
	return this->params[tapNum * (this->maxTaps - 1)  + 2 + (int) t];
}
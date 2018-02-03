#include <iostream>
#include <string.h>
#include <Windows.h>
#include "Leap.h"
//#include <Vector>


class SampleListener : public Leap::Listener {
public:
	void onConnect(const Leap::Controller&);
	void onFrame(const Leap::Controller&);

};

const std::string fingerNames[] = { "Thumb", "Index", "Middle", "Ring", "Pinky" };
const std::string boneNames[] = { "Metacarpal", "Proximal", "Middle", "Distal" };
const std::string stateNames[] = { "STATE_INVALID", "STATE_START", "STATE_UPDATE", "STATE_END" };

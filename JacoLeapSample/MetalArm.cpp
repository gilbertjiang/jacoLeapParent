#include <iostream>
#include <string.h>
#include <Windows.h>
#include "Leap.h"
#include <cstring>
#include "Lib_Examples\CommunicationLayerWindows.h"
#include "Lib_Examples\CommandLayer.h"
#include <conio.h>
#include "Lib_Examples\KinovaTypes.h"
#include <ctime>
//#include "leapTrack.h"
//#include "jacoControl.h"

//A handle to the API.
HINSTANCE commandLayer_handle;

//Function pointers to the functions we need
int(*MyInitAPI)();
int(*MyCloseAPI)();
int(*MySendBasicTrajectory)(TrajectoryPoint command);
int(*MyGetDevices)(KinovaDevice devices[MAX_KINOVA_DEVICE], int &result);
int(*MySetActiveDevice)(KinovaDevice device);
int(*MyMoveHome)();
int(*MyInitFingers)();
int(*MyGetCartesianCommand)(CartesianPosition &);

const std::string fingerNames[] = { "Thumb", "Index", "Middle", "Ring", "Pinky" };
const std::string boneNames[] = { "Metacarpal", "Proximal", "Middle", "Distal" };
const std::string stateNames[] = { "STATE_INVALID", "STATE_START", "STATE_UPDATE", "STATE_END" };

class LeapTracker{
	Leap::Vector palmPosition;
	Leap::Vector handAngle;
	Leap::Vector fingerAngle;
public:
	void getLeapData(const Leap::Controller&);
	Leap::Vector getpalmPosition();
	Leap::Vector gethandAngle();
	Leap::Vector getfingerAngle();

};

Leap::Vector LeapTracker::getpalmPosition()
{
	return palmPosition;
}

Leap::Vector LeapTracker::gethandAngle()
{
	return handAngle;
}

Leap::Vector LeapTracker::getfingerAngle()
{
	return fingerAngle;
}

void LeapTracker::getLeapData(const Leap::Controller& controller)
{
	const Leap::Frame frame = controller.frame();

	Leap::HandList hands = frame.hands();
	for (Leap::HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl) {
		// Get the first hand
		const Leap::Hand hand = *hl;
		const Leap::Vector normal = hand.palmNormal();
		const Leap::Vector direction = hand.direction();
		const Leap::Vector handPosition = hand.palmPosition();
		//std::cout << "hello world" << std::endl;
		// Calculate the hand's pitch, roll, and yaw angles
		/*std::cout << std::string(2, ' ') << "pitch: " << direction.pitch() * Leap::RAD_TO_DEG << " degrees, "
			<< "roll: " << normal.roll() * Leap::RAD_TO_DEG << " degrees, "
			<< "yaw: " << direction.yaw() * Leap::RAD_TO_DEG << " degrees" << std::endl;*/

		palmPosition = hand.palmPosition();
		handAngle = Leap::Vector(normal.roll(), direction.pitch(), direction.yaw()); //in leap coordinate

		Leap::Vector fingerTipPos0;
		Leap::Vector fingerTipPos1;
		Leap::Vector fingerTipPos2;
		// Get fingers
		Leap::FingerList fingers = hand.fingers();
		Leap::FingerList::const_iterator fl = fingers.begin();
		Leap::Finger finger = *fl;
		fingerTipPos0 = finger.tipPosition();
		++fl;
		finger = *fl;
		fingerTipPos1 = finger.tipPosition();
		++fl;
		finger = *fl;
		fingerTipPos2 = finger.tipPosition();

		float dist1 = sqrt((fingerTipPos0[0] - fingerTipPos1[0])*(fingerTipPos0[0] - fingerTipPos1[0])
			+ (fingerTipPos0[1] - fingerTipPos1[1])*(fingerTipPos0[1] - fingerTipPos1[1])
			+ (fingerTipPos0[2] - fingerTipPos1[2])*(fingerTipPos0[2] - fingerTipPos1[2]));
		
		float dist2 = sqrt((fingerTipPos0[0] - fingerTipPos2[0])*(fingerTipPos0[0] - fingerTipPos2[0])
			+ (fingerTipPos0[1] - fingerTipPos2[1])*(fingerTipPos0[1] - fingerTipPos2[1])
			+ (fingerTipPos0[2] - fingerTipPos2[2])*(fingerTipPos0[2] - fingerTipPos2[2]));
		
		std::cout << "tip position0: " << fingerTipPos0 << std::endl;
		std::cout << "tip position1: " << fingerTipPos1 << std::endl;
		std::cout << "tip position2: " << fingerTipPos2 << std::endl;
		std::cout << "dist1: " << dist1 << std::endl;
		std::cout << "dist2: " << dist1 << std::endl;

		if (dist1 < 50 && dist2 < 50)
			fingerAngle = Leap::Vector(0,0,0);
		else
			fingerAngle = Leap::Vector(180,180,180);

	}
}

int main(int argc, char** argv) {
	std::cout << "Press Enter to start Leap Tracking..." << std::endl;

	std::cin.get();

	/* Leap Code*/
	Leap::Controller controller;
	/*SampleListener listener;
	controller.addListener(listener);*/
	LeapTracker sampleTracker;

	float leapHandPosition[3];
	float leapHandAngle[3];
	float leapFingerAngle[3];
	time_t tstart, tend;
	
	/* Jaco Code*/
	//load the API.
	commandLayer_handle = LoadLibrary(L"CommandLayerWindows.dll");

	CartesianPosition currentCommand;

	int programResult = 0;

	//We load the functions from the library (Under Windows, use GetProcAddress)
	MyInitAPI = (int(*)()) GetProcAddress(commandLayer_handle, "InitAPI");
	MyCloseAPI = (int(*)()) GetProcAddress(commandLayer_handle, "CloseAPI");
	MyMoveHome = (int(*)()) GetProcAddress(commandLayer_handle, "MoveHome");
	MyInitFingers = (int(*)()) GetProcAddress(commandLayer_handle, "InitFingers");
	MyGetDevices = (int(*)(KinovaDevice devices[MAX_KINOVA_DEVICE], int &result)) GetProcAddress(commandLayer_handle, "GetDevices");
	MySetActiveDevice = (int(*)(KinovaDevice devices)) GetProcAddress(commandLayer_handle, "SetActiveDevice");
	MySendBasicTrajectory = (int(*)(TrajectoryPoint)) GetProcAddress(commandLayer_handle, "SendBasicTrajectory");
	MyGetCartesianCommand = (int(*)(CartesianPosition &)) GetProcAddress(commandLayer_handle, "GetCartesianCommand");

	//Verify that all functions has been loaded correctly
	if ((MyInitAPI == NULL) || (MyCloseAPI == NULL) || (MySendBasicTrajectory == NULL) ||
		(MyGetDevices == NULL) || (MySetActiveDevice == NULL) || (MyGetCartesianCommand == NULL) ||
		(MyMoveHome == NULL) || (MyInitFingers == NULL))

	{
		std::cout << "* * *  E R R O R   D U R I N G   I N I T I A L I Z A T I O N  * * *" << std::endl;
		programResult = 0;
	}
	else
	{
		std::cout << "I N I T I A L I Z A T I O N   C O M P L E T E D" << std::endl;

		int result = (*MyInitAPI)();

		std::cout << "Initialization's result :" << result << std::endl;

		KinovaDevice list[MAX_KINOVA_DEVICE];

		int devicesCount = MyGetDevices(list, result);

		for (int i = 0; i < devicesCount; i++)
		{
			std::cout << "Found a robot on the USB bus (" << list[i].SerialNumber << ")" << std::endl;

			//Setting the current device as the active device.
			MySetActiveDevice(list[i]);

			std::cout << "Send the robot to HOME position" << std::endl;
			MyMoveHome();

			std::cout << "Initializing the fingers" << std::endl;
			MyInitFingers();

			TrajectoryPoint pointToSend;
			pointToSend.InitStruct();

			std::cout << "Starting Leap Tracking in 3 seconds" << std::endl;
			Sleep(3000);
			
			// will operate the robot for 5 seconds
			for (int i = 0; i < 750; i++)
			{
				sampleTracker.getLeapData(controller);

				for (int i = 0; i < 3; i++)
				{
					leapHandPosition[i] = sampleTracker.getpalmPosition()[i];
					leapHandAngle[i] = sampleTracker.gethandAngle()[i];
					leapFingerAngle[i] = sampleTracker.gethandAngle()[i];
				}

				//We specify that this point will be an CARTESIAN position.
				pointToSend.Position.Type = CARTESIAN_POSITION;

				//We get the actual Cartesian command of the robot.
				MyGetCartesianCommand(currentCommand);

				// position in meters
				pointToSend.Position.CartesianPosition.X = leapHandPosition[0] + 454; //X_leap +454
				pointToSend.Position.CartesianPosition.Y = -1 * leapHandPosition[3]; //-Z_leap
				pointToSend.Position.CartesianPosition.Z = leapHandPosition[2]+729-200; // Y_leap+729-200

				// position in radians
				pointToSend.Position.CartesianPosition.ThetaX = -1 * leapHandAngle[2]; //negative yaw / handAngle[2]
				pointToSend.Position.CartesianPosition.ThetaY = leapHandAngle[1]; //pitch / handAngle[1]
				pointToSend.Position.CartesianPosition.ThetaZ = -1 * leapHandAngle[0]; //negative roll / handAngle[0]

				// position of fingeres in degrees
				pointToSend.Position.Fingers.Finger1 = leapFingerAngle[0];
				pointToSend.Position.Fingers.Finger2 = leapFingerAngle[1];
				pointToSend.Position.Fingers.Finger3 = leapFingerAngle[2];

				std::cout << "*********************************" << std::endl;
				std::cout << "Sending the first point to the robot." << std::endl;
				MySendBasicTrajectory(pointToSend);

				Sleep(5);

				std::cout << "*********************************" << std::endl << std::endl << std::endl;
			}
			
		}

		std::cout << std::endl << "C L O S I N G   A P I" << std::endl;
		result = (*MyCloseAPI)();
		programResult = 1;
	}

	FreeLibrary(commandLayer_handle);

	std::cout << "Press Enter to end..." << std::endl;
	std::cin.get();
	
	/* Jaco Code end*/
	return programResult;
	
}
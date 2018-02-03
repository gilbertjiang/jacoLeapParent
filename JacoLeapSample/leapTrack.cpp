#include "leapTrack.h"

void SampleListener::onConnect(const Leap::Controller& controller) {
	std::cout << "Connected" << std::endl;
}

void SampleListener::onFrame(const Leap::Controller& controller) {
	const Leap::Frame frame = controller.frame();
	//std::cout << "Frame id: " << frame.id()
	//	//<< ", timestamp: " << frame.timestamp()
	//	<< ", hands: " << frame.hands().count()
	//	<< ", fingers: " << frame.fingers().count()
	//	<< 
	//	<< std::endl;
	//std::cout << " hands: " << frame.hands().count() << std::endl;

	Leap::HandList hands = frame.hands();

	for (Leap::HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl) {
		// Get the first hand
		const Leap::Hand hand = *hl;
		//	std::string handType = hand.isLeft() ? "Left hand" : "Right hand";
		//	std::cout << std::string(2, ' ') << handType << ", id: " << hand.id()
		//		<< ", palm position: " << hand.palmPosition() << std::endl;
		// Get the hand's normal vector and direction
		const Leap::Vector normal = hand.palmNormal();
		const Leap::Vector direction = hand.direction();
		const Leap::Vector handPosition = hand.palmPosition();

		// Calculate the hand's pitch, roll, and yaw angles
		std::cout << std::string(2, ' ') << "pitch: " << direction.pitch() * Leap::RAD_TO_DEG << " degrees, "
			<< "roll: " << normal.roll() * Leap::RAD_TO_DEG << " degrees, "
			<< "yaw: " << direction.yaw() * Leap::RAD_TO_DEG << " degrees" << std::endl;

		palmPosition = hand.palmPosition();
		handAngle = Leap::Vector(hand.direction().yaw(), hand.direction().pitch(), hand.direction().roll()); //thetaX, thetaY, theta Z

		//// Get the Arm bone
		//Arm arm = hand.arm();
		//std::cout << std::string(2, ' ') << "Arm direction: " << arm.direction()
		//	<< " wrist position: " << arm.wristPosition()
		//	<< " elbow position: " << arm.elbowPosition() << std::endl;

		// Get fingers
		const Leap::FingerList fingers = hand.fingers();
		for (Leap::FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); ++fl)
		{
			const Leap::Finger finger = *fl;
			std::cout << std::string(4, ' ') << fingerNames[finger.type()]
				<< " finger, id: " << finger.id()
				//<< ", length: " << finger.length()
				//<< "mm, width: " << finger.width()
				<< ", tip position: " << finger.tipPosition()
				<< std::endl;
			//Sleep(17);
		}
		//	// Get finger bones
		//	for (int b = 0; b < 4; ++b) {
		//		Bone::Type boneType = static_cast<Bone::Type>(b);
		//		Bone bone = finger.bone(boneType);
		//		std::cout << std::string(6, ' ') << boneNames[boneType]
		//			<< " bone, start: " << bone.prevJoint()
		//			<< ", end: " << bone.nextJoint()
		//			<< ", direction: " << bone.direction() << std::endl;
		//	}
	}
}
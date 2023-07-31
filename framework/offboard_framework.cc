#include <Offboard.hh>
#include <joystick/Joystick.hh>
#include <serial/Serial.hh>

#include <linux/joystick.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <iostream>
#include <sstream>
#include <chrono>

int main(int argc, char **argv)
{

    Joystick js("/dev/input/js0");
    Serial s("/dev/ttyACM0");
    std::stringstream ss;

    s.setBaudrate(Serial::Baudrate::B_115200);
    js.start();

    motor_ctls ctls;

    uint8_t RX_BUF[sizeof(motor_ctls)];

    while (1)
    {
    	const auto data = js.get();
    	const auto axes = data.first;

    	ctls.thrust = axes[THRUST_INDEX];
    	ctls.roll = axes[ROLL_INDEX];
    	ctls.pitch = axes[PITCH_INDEX];
    	ctls.yaw = axes[YAW_INDEX];

    	// Write
    	s.write((void *) &ctls, sizeof(motor_ctls));
    	ss << "\n\n\tThrust: " << std::to_string(ctls.thrust)
    	   << "\n\tRoll: " << std::to_string(ctls.roll)
    	   << "\n\tPitch: " << std::to_string(ctls.pitch)
    	   << "\n\tYaw: " << std::to_string(ctls.yaw);
    	std::cout << ss.str() << std::endl;
    	ss.str(std::string{});

    	// Read
    	auto ret = s.read((void *) &RX_BUF, sizeof(motor_ctls));

	if (ret != sizeof(motor_ctls))
	    std::cout << "read(): returned with " << ret << " bytes."  << std::endl;
    }

    js.stop();
    return 0;
}

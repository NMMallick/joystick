#include <Offboard.hh>
#include <joystick/Joystick.hh>
#include <serial/Serial.hh>

#include <linux/joystick.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <csignal>

// So the signal handler can reach it
Joystick *jsPtr;
Serial *sPtr;

void handler(int signum)
{
    jsPtr->close();
    sPtr->close();

    exit(signum);
}

int main(int argc, char **argv)
{
    Joystick js("/dev/input/js0");
    Serial s("/dev/ttyACM0");

    // Set these pointers so the signal handler can find them
    sPtr = &s;
    jsPtr = &js;

    signal(SIGINT, handler);

    // s.setBaudrate(115200);
    js.start();

    motor_ctls ctls;
    uint8_t RX_BUF[sizeof(motor_ctls)] = {0};

    auto printdata = [] (const motor_ctls &input)
		     {
			 std::stringstream ss;
			 ss << "\n\n\tThrust: " << std::to_string(input.thrust)
			    << "\n\tRoll: " << std::to_string(input.roll)
			    << "\n\tPitch: " << std::to_string(input.pitch)
			    << "\n\tYaw: " << std::to_string(input.yaw);
			 std::cout << ss.str() << std::endl;
			 ss.str(std::string{});
		     };
    auto printhex = [] (const uint8_t *buf, const size_t size)
		    {
			std::stringstream ss;
			ss << std::hex;
			for (size_t i = 0; i < size; i++)
			{
			    if (i % 4 == 0 && i != 0)
				ss << std::endl << std::setw(2) << std::setfill('0') << (int) buf[i] << " ";
			    else
				ss << std::setw(2) << std::setfill('0') << (int) buf[i] << " ";
			}

			std::cout << ss.str() << std::endl;
		    };

    while (1)
    {
    	const auto data = js.get();
    	const auto axes = data.first;

    	ctls.thrust = axes[THRUST_INDEX];
    	ctls.roll = axes[ROLL_INDEX];
    	ctls.pitch = axes[PITCH_INDEX];
    	ctls.yaw = axes[YAW_INDEX];

    	// Write
    	s.write(reinterpret_cast<uint8_t*>(&ctls), sizeof(RX_BUF));

    	// Read
    	auto ret = s.read((uint8_t *) &RX_BUF, sizeof(RX_BUF));

	if (ret != sizeof(RX_BUF))
	{
	    std::cout << "did not get enough data back" << std::endl;
	    continue;
	}

	const auto res = reinterpret_cast<const motor_ctls *>(&RX_BUF);
	printdata(*res);

    }

    js.stop();
    return 0;
}

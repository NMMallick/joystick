#include <Offboard.hh>
#include <joystick/Joystick.hh>

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
    std::stringstream ss;

    js.start();

    while (1)
    {

    	const auto data = js.get();
    	const auto axes = data.first;
    	const auto buttons = data.second;

    	// Print output
    	ss << "\n\n\tThrust: " << std::to_string(axes[THRUST_INDEX])
    	   << "\n\tRoll: " << std::to_string(axes[ROLL_INDEX])
    	   << "\n\tPitch: " << std::to_string(axes[PITCH_INDEX])
    	   << "\n\tYaw: " << std::to_string(axes[YAW_INDEX]);

    	std::cout << ss.str();
    	ss.str(std::string());

    }

    js.stop();
    return 0;
}

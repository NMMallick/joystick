#include <Offboard.hh>
#include <joystick/Joystick.hh>

#include <linux/joystick.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <iostream>
#include <sstream>

int main(int argc, char **argv)
{

	// Open up the joystick device file
	const char *device = "/dev/input/js0";
	int js = open(device, O_NONBLOCK | O_RDONLY);

	struct js_event event;

	// get size n shit
	const auto aSize = get_axis_count(js);
	const auto bSize = get_button_count(js);

	auto ctls = joy_ctls(aSize, bSize);

	// Verify that bad things aren't happening
	if (js == -1)
	{
		perror("Could not open joystick");
	}

	std::stringstream ss;

	while(1)
	{
		while (read(js, &event, sizeof(event)) > 0)
		{
			switch (event.type)
			{
			case JS_EVENT_BUTTON:
			case JS_EVENT_AXIS:
				parse_event(&event, ctls);
				break;
			default:
				break;
			}
		}

		if (errno != EAGAIN && errno != 0)
		{
			fprintf(stderr, "Error reading joystick (%d)\n", errno);
			fprintf(stderr, "\t%s\n", strerror(errno));
			break;
		}

		// Save output
		ss << "\n\n\tThrust: " << std::to_string(ctls.axes[THRUST_INDEX])
		   << "\n\tRoll: " << std::to_string(ctls.axes[ROLL_INDEX])
		   << "\n\tPitch: " << std::to_string(ctls.axes[PITCH_INDEX])
		   << "\n\tYaw: " << std::to_string(ctls.axes[YAW_INDEX]);

		std::cout << ss.str();
		ss.str(std::string());
	}

	return 0;
}

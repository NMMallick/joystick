/**
 * See also:
 * https://www.kernel.org/doc/Documentation/input/joystick-api.txt
 */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>

#include <linux/joystick.h>
#include <joystick/Joystick.hh>

// Joystick class definitions (start)
Joystick::Joystick(const std::string &port) :
    done(true)
{
    fd = open(port.c_str(), O_NONBLOCK | O_RDONLY);

    // Check to see if we've opened the device file
    if (fd == -1)
    {
	std::cerr << strerror(errno) << std::endl;
	throw std::runtime_error("Could not open device file : " + port );
    }

    // Initialize the input container struct
    inputs = Joystick::Input(get_axis_count(fd), get_button_count(fd));

}

void Joystick::start()
{
    // Important to capture by reference since we want
    // NO copies of this class
    thr = std::thread([&]() {
			   while(1)
			   {
			       // Check to see if we are being told to wrap up
			       {
				   Joystick::Lock lk(flag_mutex_);
				   if (done)
				       break;
			       }

			       // TODO: Read data and write it to our container class

			   }
		       });
}

void Joystick::stop()
{

}

Joystick::Pair get()
{

}

// Joystick class definitions (end)

// joystick definitions (start)
int read_js_event(int fd, struct js_event *event)
{
    ssize_t bytes;

    bytes = read(fd, event, sizeof(*event));

    if (bytes == sizeof(*event))
        return 0;

    /* Error, could not read full event. */
    return -1;
}

size_t get_axis_count(int fd)
{
    __u8 axes;

    if (ioctl(fd, JSIOCGAXES, &axes) == -1)
        return 0;

    return axes;
}

size_t get_button_count(int fd)
{
    __u8 buttons;
    if (ioctl(fd, JSIOCGBUTTONS, &buttons) == -1)
        return 0;

    return buttons;
}

void parse_event(struct js_event *event, joy_input &input)
{
    size_t idx = event->number;

    if (event->type == JS_EVENT_BUTTON)
		ctl.buttons[idx] = event->value;

    if (event->type == JS_EVENT_AXIS)
    {
		float val = event->value/TOTAL_CLICKS;

		// Dead region
		input.axes[idx] = (abs(val) < __DEADZONE__) ? 0.0 : val;
    }
}
// joystick definitions (end)

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
#include <iostream>

#include <linux/joystick.h>
#include <joystick/Joystick.hh>

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

void parse_event(struct js_event *event, JoyInput &input)
{
    size_t idx = event->number;

    if (event->type == JS_EVENT_BUTTON)
	input.btns[idx] = event->value;

    if (event->type == JS_EVENT_AXIS)
    {
	float val = event->value/TOTAL_CLICKS;

	// Dead region
	input.axs[idx] = (abs(val) < __DEADZONE__) ? 0.0 : val;
    }
}
// joystick definitions (end)

// Joystick input definitions (start)
JoyInput::JoyInput(size_t a, size_t b)
{
    axs = std::vector<float>(a, 0.0);
    btns = std::vector<float>(b, 0.0);
}

std::vector<float> JoyInput::axes()
{
    return std::vector<float>(axs);
}

std::vector<float> JoyInput::buttons()
{
    return std::vector<float>(btns);
}
// Joyst input definitions (end)

// Joystick class definitions (start)
Joystick::Joystick(const std::string &port) :
    done(true)
{
    // Checking hardware concurrency since this depends on it
    if (std::thread::hardware_concurrency() < 2)
	throw std::runtime_error("This platform does not support hardware concurrency");

    fd = open(port.c_str(), O_NONBLOCK | O_RDONLY);

    // Check to see if we've opened the device file
    if (fd == -1)
    {
	std::cerr << strerror(errno) << std::endl;
	throw std::runtime_error("Could not open device file : " + port );
    }

    // Initialize the input container struct
    inputs = JoyInput(get_axis_count(fd), get_button_count(fd));
}

Joystick::~Joystick()
{
    stop();
}

void Joystick::start()
{
    // Important to capture by reference since we want
    // NO copies of this class
    auto fn = [&]() {
		  while(1)
		  {
		      // Check to see if we are being told to wrap up
		      {
			  Joystick::Lock lk(flag_mutex_);
			  if (done)
			      break;
		      }

		      {
			  while (read(fd, &event, sizeof(event)) > 0)
			  {
			      switch (event.type)
			      {
			      case JS_EVENT_BUTTON:
			      case JS_EVENT_AXIS:
			      {
				  Joystick::Lock lk(data_mutex_);
				  parse_event(&event, inputs);
				  break;
			      }
			      default:
				  break;
			      }
			  }

			  if (errno != EAGAIN && errno != 0)
			  {
			      // TODO: figure out what to do if we we have
			      // a problem reading from the joy device
			      break;
			  }
		      }
		  }
	      };

    // Start the thread
    thr = std::thread(std::move(fn));

    // Set the flag so we know the thread is running
    Joystick::Lock lk(flag_mutex_);
    done = false;
}

void Joystick::stop()
{
    // Set the done flag to true so the threads
    // knows to stop
    {
	Joystick::Lock lk(flag_mutex_);
	if (!done)
	    done = true;
    }

    // Wait for the thread to join
    if (thr.joinable())
	thr.join();
}

Joystick::Pair Joystick::get()
{
    Joystick::Lock lk(data_mutex_);
    return std::make_pair(inputs.axes(), inputs.buttons());
}
// Joystick class definitions (end)

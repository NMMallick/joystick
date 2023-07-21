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

void parse_event(struct js_event *event, joy_ctls &ctl)
{
    size_t idx = event->number;

    if (event->type == JS_EVENT_BUTTON)
		ctl.buttons[idx] = event->value;

    if (event->type == JS_EVENT_AXIS)
    {
		float val = event->value/TOTAL_CLICKS;

		// Dead region
		ctl.axes[idx] = (abs(val) < __DEADZONE__) ? 0.0 : val;
    }
}
// joystick definitions (end)

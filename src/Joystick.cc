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
#include <Offboard.hh>

/* Struct for XBOX Controller */
// TODO - looking to get rid of this
typedef struct xbox_ctls
{
    // Axes
    float axes[AXIS_COUNT];
    float buttons[BUTTON_COUNT];
} xbox_ctls;

/**
 * Reads a joystick event from the joystick device.
 *
 * Returns 0 on success. Otherwise -1 is returned.
 */
int read_js_event(int fd, struct js_event *event)
{
    ssize_t bytes;

    bytes = read(fd, event, sizeof(*event));

    if (bytes == sizeof(*event))
        return 0;

    /* Error, could not read full event. */
    return -1;
}

/**
 * Returns the number of axes on the controller or 0 if an error occurs.
 */
size_t get_axis_count(int fd)
{
    __u8 axes;

    if (ioctl(fd, JSIOCGAXES, &axes) == -1)
        return 0;

    return axes;
}

/**
 * Returns the number of buttons on the controller or 0 if an error occurs.
 */
size_t get_button_count(int fd)
{
    __u8 buttons;
    if (ioctl(fd, JSIOCGBUTTONS, &buttons) == -1)
        return 0;

    return buttons;
}

size_t parse_event(struct js_event *event, input_ctl_t &ctl)
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

    return idx;
}

int main(int argc, char **argv)
{
    const char *device;
    struct js_event event;
    int js;
    size_t axis;

    xbox_ctls xb {
		.axes={0.0},
		.buttons={0.0}
	};
	
	motor_ctls ctls {
		.thrust = 0.0;
		.yaw = 0.0;
		.roll = 0.0;
		.pitch = 0.0;
	};
	
    // Open up the device file
    device = "/dev/input/js0";
    js = open(device, O_NONBLOCK | O_RDONLY);

    if (js == -1)
    {
        perror("Could not open joystick");
		return -1;
    }

    // Print out the number of axes on the joystick
    size_t ax_count = get_axis_count(js);
    size_t bt_count = get_button_count(js);

    // Verify the axis count
    if (ax_count != AXIS_COUNT)
    {
		fprintf(stderr, "Invalid axis count (%ld != %d)\n", ax_count, AXIS_COUNT);
		return -1;
    }

    // Verify the button count
    if (bt_count != BUTTON_COUNT)
    {
		fprintf(stderr, "Invalid button count (%ld != %d)\n", bt_count, BUTTON_COUNT);
		return -1;
    }

    printf("Reading events .. \n");

    while (1)
    {
		while (read(js, &event, sizeof(event)) > 0)
		{
			switch (event.type)
			{
			case JS_EVENT_BUTTON:
			case JS_EVENT_AXIS:
				parse_event(&event, xb);
				break;
	    	default:
				/* Ignore init events. */
				break;
			}
		}

		if (errno != EAGAIN && errno != 0)
		{
			fprintf(stderr, "Error reading joystick (%d)\n", errno);
			fprintf(stderr, "\t%s\n", strerror(errno));
			break;
		}

		// save output
		ctls.thrust = xb.axes[THRUST_INDEX];
		ctls.yaw = xb.axes[YAW_INDEX];
		ctls.roll = xb.axes[ROLL_INDEX];
		ctls.pitch = xb.axes[PITCH_INDEX];

		
		// print axes
		// fprintf(stdout, "Axes:\t\t[");
		// for (int i = 0; i < AXIS_COUNT; i++)
		// {
		// 	fprintf(stdout, " %f ", xb.axes[i]);
		// }
		
		// fprintf(stdout, "]\r");
		// fflush(stdout);
    }

    close(js);
    return 0;
}

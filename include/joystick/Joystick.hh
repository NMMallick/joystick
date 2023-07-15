#pragma once

#include <linux/joystick.h>
#include <vector>

// Struct Definitions
typedef struct joy_ctls
{
	joy_ctls(size_t a, size_t b)
	{
		axes.reserve(a);
		buttons.reserve(b);

		std::fill(axes.begin(), axes.end(), 0.0);
		std::fill(buttons.begin(), buttons.end(), 0.0);
	}
	
	// Axes
	std::vector<float> axes;
	// Buttons
	std::vector<float> buttons;
	
} joy_ctls;

#define TOTAL_CLICKS 32767.0
#define __DEADZONE__ 0.6

/**
 * @brief Reads a joystick event from an open joystick device file
 * @param fd An open joystick file descriptor
 * @param event Joystick event structure data is copied to
 * @return 0 on success, -1 otherwise
 */
int read_js_event(int fd, struct js_event *event);

/**
 * @brief Returns the number of axes on the controller
 * @param fd An open joystick file descriptor returned by open()
 * @return number of axes, 0 if an error occurs
 */
size_t get_axis_count(int fd);

/**
 * @brief Returns the number of buttons on the controller
 * @param fd An open joystick file descriptor returned by open()
 * @return number of buttons, 0 if an error occurs
 */
size_t get_button_count(int fd);

/**
 * @brief Parse the joystick event and copies data into joy_ctls struct
 * @param event Joystick object which the event is copied to
 * @param ctl Joystick control object which the event param is copied to
 * @returns the event
 */
void parse_event(struct js_event *event, joy_ctls &ctl);
   
	


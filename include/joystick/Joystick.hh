#pragma once

#include <linux/joystick.h>
#include <vector>
#include <mutex>
#include <thread>
#include <utility>

// Struct Definitions
typedef struct joy_input
{
	joy_input(size_t a, size_t b)
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

} joy_input;

#define TOTAL_CLICKS 32767.0
#define __DEADZONE__ 0.6

class Joystick
{
    using Lock = std::lock_guard<std::mutex>;
    using Mutex = std::mutex;
    using Pair = std::pair<std::vector, std::vector>;
    using Input = joy_input;

public:
    // Constructor
    Joystick(const std::string &port);

    // We don't want to enable copy constructor
    // and assignment operator since this class will
    // be threaded and open/close devic files
    Joystick() = delete;
    Joystick(const Joystick &cp) = delete;
    Joystick& operator=(const Joystick &rhs) = delete;

    // Start and stop reading the joystick driver
    void start();
    void stop();

    // Get the data
    Pair get();

protected:

    Mutex data_mutex_, flag_mutex_;
    Input inputs;
    bool done;

private:
    std::thread thr;
    int fd;
    js_event event;

};

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
void parse_event(struct js_event *event, joy_input &input);

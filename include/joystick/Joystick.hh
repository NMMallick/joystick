#pragma once

#include <linux/joystick.h>
#include <vector>
#include <mutex>
#include <thread>
#include <utility>

#define TOTAL_CLICKS 32767.0
#define __DEADZONE__ 800

class JoyInput;

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
void parse_event(struct js_event *event, JoyInput &input);

// JoyInput Object
class JoyInput
{
public:
    // Constructor
    JoyInput() = default;
    JoyInput(size_t a, size_t b);

    // Returns copies of the data in both vectors
    std::vector<float> axes();
    std::vector<bool> buttons();

    // Sets the values of the buffers
    void setAxis(const unsigned long &index, const float &value);
    void setButton(const unsigned long &index, const float &value);

private:
    friend void parse_event(struct js_event *, JoyInput &);

    // Axes
    std::vector<float> axs;
    // Buttons
    std::vector<bool> btns;

};


class Joystick
{
    using Lock = std::lock_guard<std::mutex>;
    using Mutex = std::mutex;
    using Pair = std::pair<std::vector<float>, std::vector<bool>>;

public:
    // Constructor/Destructor
    Joystick(const std::string &port);
    ~Joystick();

    // We don't want to enable copy constructor
    // and assignment operator since this class will
    // be threaded and open/close device files
    Joystick() = delete;
    Joystick(const Joystick &cp) = delete;
    Joystick& operator=(const Joystick &rhs) = delete;

    // Start and stop reading the joystick driver
    void start();
    void stop();
    void close();

    // Get the data
    Pair get();

    void setAxisValue(const unsigned long &index, const bool &value)
    {
	inputs.setAxis(index, value);
    }

    void setButtonValue(const unsigned long &index, const bool &value)
    {
	inputs.setButton(index, value);
    }

protected:
    Mutex data_mutex_, flag_mutex_;
    JoyInput inputs;
    bool done;

private:
    std::thread thr;
    int fd;
    js_event event;

};

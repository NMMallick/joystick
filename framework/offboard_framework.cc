#include <Offboard.hh>

#include <joystick/Joystick.hh>
#include <serial/Serial.hh>

#include <iostream>
#include <iomanip>
#include <sstream>

#include <csignal>

// So the signal handler can reach it
Joystick *jsPtr = nullptr;
serial::Serial *sPtr = nullptr;

void handler(int signum)
{
    if (jsPtr != nullptr)
	jsPtr->close();

    if (sPtr != nullptr)
	sPtr->close();

    exit(signum);
}


void printdata(const MotorCtls &input)
{
    std::stringstream ss;
    ss << "\n\n\tThrust: " << std::to_string(input.thrust)
       << "\n\tRoll: " << std::to_string(input.roll)
       << "\n\tPitch: " << std::to_string(input.pitch)
       << "\n\tYaw: " << std::to_string(input.yaw);
    std::cout << ss.str() << std::endl;
    ss.str(std::string{});
};

void printhex(const uint8_t *buf, const size_t size)
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

int main(int argc, char **argv)
{
    Joystick js("/dev/input/js0");
    serial::Serial s("/dev/ttyACM0");
    const auto buffer = serial::create_buffer<MotorCtls>();

    // Set these pointers so the signal handler can find them
    sPtr = &s;
    jsPtr = &js;
    signal(SIGINT, handler);
    js.start();

    MotorCtls ctls;

    // Set the default values for yaw since
    // JS only reports data on changes
    js.setAxisValue(YAW_CCW_INDEX, -1.0);
    js.setAxisValue(YAW_CW_INDEX, -1.0);

    while (1)
    {
    	const auto data = js.get();
    	const auto axes = data.first;

    	ctls.thrust = -(axes[THRUST_INDEX]-1)/2.;
    	ctls.roll = axes[ROLL_INDEX];
    	ctls.pitch = -axes[PITCH_INDEX];
    	ctls.yaw = (axes[YAW_CCW_INDEX] - axes[YAW_CW_INDEX])/2.;

	buffer->setData(ctls);

    	// Write
	s.write(buffer.get());

    	// Read
	auto ret = s.read(buffer.get());

	if (ret != buffer->size())
	{
	    std::cout << "did not get enough data back" << std::endl;
	    continue;
	}

	const auto res = buffer->getData();
	printdata(res);
    }

    js.stop();
    return 0;
}

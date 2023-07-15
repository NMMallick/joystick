#pragma once

#include <termios.h>
#include <inttypes.h>

class Serial
{
	// Only support 3 rates for now
	// enum struct Baudrate
	// {
	// 	B4800,
	// 	B9600,
	// 	B115200
	// };
	using Baudrate = size_t;
	
public:
	
	Serial(const char *port);
	~Serial();
	
	// Don't want to make copy of any instance since
	// we are opening platform device files
	Serial(const Serial &cp) = delete;
	Serial &operator=(const Serial &cp) = delete;

	// Setter for control modes (c_lflag)
	
	// Setter for input modes (c_iflag)

	// Settor for output modes (c_oflag)

	// Set baud rate 
	void setBaudrate(const Serial::Baudrate &br);
	
	// Read
	
	// Writes

	// Helpers
	bool isReady();
	
private:
	// open a device file
	void openDevFile(const char *devFile);
	
	int serial_port_;
	struct termios tty;

	// setup flags that validate if the port
	// is ready to read/write to 
	uint8_t setup;

	// Defines for verifying setup
#define PORT_OPENED      0x01
#define BAUD_RATE_SET    0x02
#define CTL_MODE_SET     0x04
#define INPUT_MODE_SET   0x08
#define OUTPUT_MODE_SET  0x10
#define PORT_READY       0x1f
	
};

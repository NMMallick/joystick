
#include <time.h>

// C library headers
#include <stdio.h>
#include <string.h>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

// c++stdlib
#include <iostream>
#include <chrono>

// Serial header
#include <serial/Serial.hh>

// Serial.hh definitions (start)

Serial::Serial(const char *port)
{
    // clean setup variable
    setup &= ~0xff;

    openDevFile(port);

    if (!(setup & PORT_OPENED))
	throw std::runtime_error("Serial(): cannot open serial port");

    // ##### Control Modes #####
    // - PARITY BIT - NOTE check to see if we need to check the parity bit on the other end of the
    // serial comm. We will disable since this is most common and what is be used in the
    // motivation of this class
    tty.c_cflag &= ~PARENB;

    // - STOP BIT -
    // Use only one stop bit in comms
    tty.c_cflag &= ~CSTOPB;

    // - NUM BITS/BYTE -
    // Set the number of bits per byte (most common - 8)
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;

    // - HARDWARE FLOW CONTROL -
    // Disable RST/CTS hardware flow control
    tty.c_cflag &= ~CRTSCTS;

    // - CREAD & CLOCAL -
    // Setting CLOCAL disables modem-specific signal lines such as carrier detect. It also
    // prevents the controlling process from getting sent a SIGHUP signal when a modem disconnect is
    // detected, which is usually a good thing here. Setting CREAD allows us to read data (desired)
    tty.c_cflag |= CREAD | CLOCAL;
    setup |= CTL_MODE_SET;

    // ###### LOCAL MODES #####
    // - CANONICAL MODE -
    // Setting canonical mode triggers input processing only when a new line character is recieved.
    // This is mainly used for line-by-line editing since chars like backspace are used to edit
    // the current line of text. This could cause bytes to go missing when sending raw data, so
    // we will turn it off.
    tty.c_lflag &= ~ICANON;

    // - ECHO -
    // If this is set, sent characters are echo'd back. This may be dependent on canonical mode, i.e.
    // when canon is disabled, echo may do nothing (would be valiant to check)
    tty.c_lflag &= ~ECHO;// Disable echo
    tty.c_lflag &= ~ECHOE;// Disable erasure
    tty.c_lflag &= ~ECHONL;// Disable new line

    // - Signal Characters -
    // We disable the interpretation of signal characts (INT, QUIT, SUSP). Again, raw data only so this
    // will be disabled
    tty.c_lflag &= ~ISIG;
    setup |= LOCAL_MODE_SET;

    // ##### Input Modes #####
    // - Software Flow Control -
    // disables software flow control
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);

    // - Byte Handling -
    // Other byte handling fields. Disabling to keep the serial stream raw
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
    setup |= INPUT_MODE_SET;

    // ##### Output Modes ######
    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

    setup |= OUTPUT_MODE_SET;

    // ##### VMIN and VTIME #####
    // Read any available data immediately
    tty.c_cc[VTIME] = 0;
    tty.c_cc[VMIN] = 0;


    // ##### Save Termios #####
    // Save tty settings and check for error
    if (tcsetattr(serial_port_, TCSANOW, &tty) != 0)
    {
	printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
    }
}

Serial::~Serial()
{
    // If the port is opened then we close it
    if (setup & PORT_OPENED)
	close(serial_port_);
}

void Serial::openDevFile(const char *devFile)
{
    serial_port_ = ::open(devFile, O_RDWR | O_NOCTTY | O_NONBLOCK);

    if (serial_port_ < 0)
    {
	std::cerr << "open() returned with : " << strerror(errno) << std::endl;
	return;
    }

    // Read in existing settings, and handle any error
    // NOTE: This is important! POSIX states that the struct passed to tcsetattr()
    // must have been initialized with a call to tcgetattr() overwise behaviour
    // is undefined
    if (tcgetattr(serial_port_, &tty) != 0)
    {
	std::cerr << "tcgetattr() returned with : " << + strerror(errno) << std::endl;
	return;
    }

    // ##### Save Termios #####
    // Save tty settings and check for error
    if (tcsetattr(serial_port_, TCSANOW, &tty) != 0)
    {
	printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
    }

    setup |= PORT_OPENED;
}

void Serial::setBaudrate(const Serial::Baudrate &br)
{
    // TODO: make this a bit more dynamic by independently
    // setting the output and input baudrates. For now, set
    // both for prototyping
    const auto baudrate = static_cast<unsigned>(br);
    cfsetspeed(&tty, baudrate);

    // Verify the baudrate
    if (cfgetispeed(&tty) == baudrate
	&& cfgetospeed(&tty) == baudrate)
	setup |= BAUD_RATE_SET;
    else
	setup &= ~BAUD_RATE_SET;
}

bool Serial::isReady()
{
    return (setup & PORT_READY) == PORT_READY;
}

size_t Serial::read(void *buf, size_t len, float timeOut)
{
    if (!isReady())
	return -1;

    using namespace std::chrono;
    size_t bytes_read = 0;

    auto start = steady_clock::now();
    duration<float> elapsed_time;

    while ((bytes_read != len) && (elapsed_time.count() < timeOut))
    {
    	int n = ::read(serial_port_, buf, len);
    	if (n != 0)
    	{
    	    bytes_read+=n;
    	    start = steady_clock::now();
    	}

    	elapsed_time = steady_clock::now() - start;
    }

    return bytes_read;
}

size_t Serial::write(void *buf, size_t len)
{
    if (!isReady())
	return -1;

    size_t n = 0;

    while (n < len)
    {
	n += ::write(serial_port_, buf + n, len - n);
    }

    return n;
}

// Serial.hh definitions (end)

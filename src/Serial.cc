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

    if (setup &= PORT_OPENED)
    {
	std::cerr << "Serial(): can't open serial port" << std::endl;
	return;
    }

    // ##### Control Modes #####
    // - PARITY BIT - NOTE check to see if we need to check
    // the parity bit on the other end of the
    // serial comm. We will disable
    tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
    // tty.c_cflag |= PARENB;  // Set parity bit, enabling parity

    // - CREAD & CLOCAL -
    // Setting CLOCAL disables modem-specific signal lines such as carrier detect. It also
    // prevents the controlling process from getting sent a SIGHUP signal when a modem disconnect is
    // detected, which is usually a good thing here. Setting CREAD allows us to read data (desired)
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

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
    // For now we will keep it disable but could use some more investigation into this
    // https://en.wikipedia.org/wiki/Software_flow_control
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl

    // - Byte Handling -
    // Other byte handling fields. Disabling to keep the serial stream raw
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

    setup |= INPUT_MODE_SET;

    // ##### Output Modes ######
    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
    // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT IN LINUX)
    // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT IN LINUX)

    setup |= OUTPUT_MODE_SET;

    // ##### VMIN and VTIME #####
    // https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/
    // This link has nice into section on how to use VMIN and VTIME

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
    setup &= 0x00;
    serial_port_ = open(devFile, O_RDWR);

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
    return setup == PORT_READY;
}

void Serial::read(uint8_t *buf, size_t len, float timeOut)
{
    using namespace std::chrono;
    size_t bytes_read = 0;

    auto start = steady_clock::now();
    auto end = steady_clock::now();
    duration<float> elapsed_time = end - start;

    while ((bytes_read != len) || (elapsed_time.count() > timeOut))
    {
	int n = ::read(serial_port_, buf, len);
	if (n != 0)
	{
	    bytes_read+=n;
	    start = steady_clock::now();
	}

	end = steady_clock::now();
    }
}

// Serial.hh definitions (end)

int outdated_main(void)
{


// struct termios {
// 	tcflag_t c_iflag;		/* input mode flags */
// 	tcflag_t c_oflag;		/* output mode flags */
// 	tcflag_t c_cflag;		/* control mode flags */
// 	tcflag_t c_lflag;		/* local mode flags */
// 	cc_t c_line;			/* line discipline */
// 	cc_t c_cc[NCCS];		/* control characters */
// };


    // // Writing
    // unsigned char msg[] = { 'F', 'o', 'c', 'k' };
    // write(serial_port, msg, 4);

    // // Reading
    // char rxbuf [4] = {0};
    // if (n < 0)
    // 	fprintf(stdout, "error baby\n");

    // fprintf(stdout, "%s\n", rxbuf);

    // printf("success!!!!\n");

    // close(serial_port);

    return 0;
}

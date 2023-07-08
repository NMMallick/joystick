// C library headers
#include <stdio.h>
#include <string.h>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

int main(void)
{
    int serial_port = open("/dev/ttyUSB0", O_RDWR);

    // Check for errors
    if (serial_port < 0)
    {
	printf("Error %i from open: %s\n", errno, strerror(errno));
    }

// Create new termios struct, we call it 'tty' for convention
// No need for "= {0}" at the end as we'll immediately write the existing
// config to this struct
    struct termios tty;

// Read in existing settings, and handle any error
// NOTE: This is important! POSIX states that the struct passed to tcsetattr()
// must have been initialized with a call to tcgetattr() overwise behaviour
// is undefined
    if(tcgetattr(serial_port, &tty) != 0)
    {
	printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
    }

// struct termios {
// 	tcflag_t c_iflag;		/* input mode flags */
// 	tcflag_t c_oflag;		/* output mode flags */
// 	tcflag_t c_cflag;		/* control mode flags */
// 	tcflag_t c_lflag;		/* local mode flags */
// 	cc_t c_line;			/* line discipline */
// 	cc_t c_cc[NCCS];		/* control characters */
// };

    // - PARITY BIT - NOTE check to see if we need to check
    // the parity bit on the other end of the
    // serial comm. We will disable
    tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
    // tty.c_cflag |= PARENB;  // Set parity bit, enabling parity

    // - STOP BIT - NOTE if this bit is set, two stop bits, else
    // only one. Most common is one
    tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
    // tty.c_cflag |= CSTOPB;  // Set stop field, two stop bits used in communication

    // - # BITS/BYTE - sets how many data bits are transmitted
    // across the serial port. Most common is 8
    tty.c_cflag &= ~CSIZE; // Clear all the size bits, then use one of the statements below
    // tty.c_cflag |= CS5; // 5 bits per byte
    // tty.c_cflag |= CS6; // 6 bits per byte
    // tty.c_cflag |= CS7; // 7 bits per byte
    tty.c_cflag |= CS8; // 8 bits per byte (most common)

    // - HW FLOW CTL - Sets the hardware RTS/CTS flow control. Used when two extra
    // wires are used between endpoints to signal when data is ready to be sent/recv.
    // The most common is to disable
    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    // tty.c_cflag |= CRTSCTS;  // Enable RTS/CTS hardware flow control

}

#ifndef OFFBOARD_HH_
#define OFFBOARD_HH_

#define TX_BUF_LEN 16
#define RX_BUF_LEN (TX_BUF_LEN)
#define DATA_READY 1

// Struct to translate YAW, PITCH, ROLL, THRUST
// from manual input
typedef struct motor_ctls
{
	float thrust,
		  yaw,
		  roll,
		  pitch;
} motor_ctls;

#if USE_LOGITECH

#define YAW_INDEX 0
#define THRUST_INDEX 1
#define ROLL_INDEX 3
#define PITCH_INDEX 4

#elif USE_XBOX_WIRED

#define YAW_INDEX 0
#define THRUST_INDEX 1
#define ROLL_INDEX 4
#define PITCH_INDEX 3

#elif USE_XBOX_WIRELESS

#define YAW_INDEX 0
#define THRUST_INDEX 1
#define ROLL_INDEX 2
#define PITCH_INDEX 3

#else

#error Controller define required

#endif


#define TOTAL_CLICKS 32767.0
#define __DEADZONE__ 0.6

#if USE_RAW_INPUT
uint8_t axis_index;
/*
 * -- AXES --
 * [0 - left joy (left/right)]
 * [1 - left joy (up/down)]
 * [2 - left trigger]
 * [3 - right joy (left/right)]
 * [4 - right joy (up/down)]
 * [5 - right trigger]
 * [6 - left pad (left/right)]
 * [7 - left pad (up/down)]
 */

#define LEFT_JOY_LR 0x00
#define LEFT_JOY_UD 0x01
#define LEFT_TRIGGER 0x02
#define RIGHT_JOY_LR 0x03
#define RIGHT_JOY_UD 0x04
#define RIGHT_TRIGGER 0x05
#define LEFT_PAD_LR 0x06
#define LEFT_PAD_UD 0x07

#define AXIS_COUNT 8

/*
 * -- BUTTONS --nn
 * [0 - A]
 * [1 - B]
 * [2 - X]
 * [3 - Y]
 * [4 - LB]
 * [5 - RB]
 * [6 - BACK]
 * [7 - START]
 * [8 - CENTER]
 * [9 - LEFT JOY]
 * [10 - RIGHT JOY]
 */

#define A_BUTTON 0x00
#define B_BUTTON 0x01
#define X_BUTTON 0x02
#define Y_BUTTON 0x03
#define LEFT_BUTTON 0x04
#define RIGHT_BUTTON 0x05
#define BACK_BUTTON 0x06
#define START_BUTTON 0x07
#define CENTER_BUTTON 0x08
#define LEFT_JOY_BUTTON 0x09
#define RIGHT_JOY_BUTTON 0x0a

#define BUTTON_COUNT 11

#endif // USE_RAW_INPUT

#endif

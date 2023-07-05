#pragma once

// Defines for Xbox controller
#if USE_XBOX

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

#endif

// Defines for logitech controller
#if USE_LOGITECH

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
#endif

#define TOTAL_CLICKS 32767.0
#define __DEADZONE__ 0.6

typedef struct input_ctl_t
{
    // Axes
    float axes[AXIS_COUNT];
    float buttons[BUTTON_COUNT];
} input_ctl_t;


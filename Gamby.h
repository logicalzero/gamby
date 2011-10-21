#ifndef Gamby_h
#define Gamby_h

#include "WProgram.h"


// ###########################################################################
//
// ###########################################################################

// Various LCD commands
#define	POWER_CONTROL		B00101111	// all power circuits ON (LCD controlled)
#define	DC_STEP_UP		B01100100	// 011001xx, xx select 3-5x boosting
#define	REGULATOR_RESISTOR	B00100000	// 00100xxx (center)
#define	SET_EVR_1		B10000001	// 2-byte instruction!
//#define	SET_EVR_2	B00011100	// XXxxxxxx, xxxxxx = value, XX = don't care. (center)
#define	SET_EVR_2		B00011011	// XXxxxxxx, xxxxxx = value, XX = don't care. (center)
#define	SET_DUTY_1		B01001000 	// 2-byte instruction!
#define	SET_DUTY_2		B01000000
#define	SET_BIAS		B01010001	// 01010xxx, xxx = bias. Should be 001 for 1/5 bias
#define	SET_OSC_ON		B10101011	// Starts internal oscillator
#define	SHL_SELECT		B11000000	// 1100xXXX, x = COM scanning direction. 0 normal, 1 reverse. X = don't care.
#define	ADC_SELECT		B10100000	// 1010000x

#define	REGULATOR_RESISTOR_VAL	0x01		// range 0x00 ~ 0x03 // resistor value '001' @ 3.3V and 3x boost seems to work nicely
//#define VOLUME_CONTROL_VAL	0x1C		// range 0x00 ~ 0x3F
#define	VOLUME_CONTROL_VAL	0x0A		// range 0x00 ~ 0x3F
#define	DISPLAY_INVERT_VAL	0x00		// range 0x00 ~ 0x01

#define	ALL_BLACK		B10100100	// 1010010x, x=1 forces all pixels on. 'Entire Display ON' in datasheet.
#define	DISPLAY_INVERT		B10100110	// 1010011x, x = invert. 0 normal, 1 inverse video.
#define	SET_POWER_SAVE		B10101000	// 1010100x, x = power save level (0=standby, 1=sleep)
#define	CLEAR_POWER_SAVE	B11100001
#define	SOFT_RESET		B11100010	// Initialize the internal functions

#define DISPLAY_POWER		B10101110	// B1010111x, 0=off, 1=on
#define SET_N_LINE_INVERSION_1	B01001100	// 2 byte; 
#define CLEAR_N_LINE_INVERSION	B11100100
#define SET_INITIAL_COM0_1	B01000100	// 2 byte, 2nd is 00XXxxxx; X = don't care
#define SET_INITIAL_LINE_1	B01000000	// 2 byte, 2nd is 0Xxxxxxx

#define	SET_PAGE_ADDR		B10110000	// 1011xxxx, xxxx = page addr.
//#define SET_COLUMN_ADDR_1	B00010000	// 2-byte! 00010xxx, xxx = Y[6..4]
#define	SET_COLUMN_ADDR_1	B00010010	// 2-byte! 00010xxx, xxx = Y[6..4] (32 added for offset to work inverted)
#define	SET_COLUMN_ADDR_2	B00000000	// 0000xxxx, xxxx = Y[3..0]


// Values to make code a little more readable
#define POST_COMMAND_DELAY	100 //50		// microseconds; arbitrarily chosen
#define BETWEEN_BIT_DELAY       5

#define SET_POWER_SAVE_STANDBY  SET_POWER_SAVE
#define SET_POWER_SAVE_SLEEP    SET_POWER_SAVE | 1
#define STANDBY			0		// Use w/ bitwise OR for power save mode
#define SLEEP			1		// Use w/ bitwise OR for power save mode
#define CLEAR			255		// Use w/ setNLineInversion()
#define OFF			0		// use w/ bitwise OR for DISPLAY_POWER, ALL_BLACK, etc.
#define ON			1		// use w/ bitwise OR for DISPLAY_POWER, ALL_BLACK, etc. 

#define COMMAND                 0
#define DATA                    1

// Screen geometry constants
#define NUM_COLUMNS		96              // GoldLCD horizontal pixels
#define NUM_ROWS		64              // GoldLCD vertical pixels
#define NUM_PAGES		NUM_ROWS/8      // A 'page' is an 8 px horizontal 'stripe'
#define TOTAL_PIXELS            NUM_COLUMNS * NUM_ROWS
#define ROW_PIXELS              8 * NUM_COLUMNS

#define FIRST_COL               0
#define LAST_COL                NUM_COLUMNS - 1

// Drawing modes
#define NORMAL         0    // dark text, light background
#define INVERSE        0xFF // light text, dark background


// ###########################################################################
//
// ###########################################################################

/**
 *
 */
class GambyTextMode {
 public:
  GambyTextMode();

 private:

};

/**
 *
 */
class GambyBlockMode {
 public:
  GambyBlockMode();

 private:

};


/**
 *
 */
class GambyGraphicsMode {
 public:
  GambyGraphicsMode();

 private:

};


#endif

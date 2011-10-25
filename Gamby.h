#ifndef Gamby_h
#define Gamby_h

#include "WProgram.h"
#include <avr/pgmspace.h>

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


// draw mode flags
// Maybe all the flags should be merged into an int.
#define FILL_BLACK_TRANSPARENT  B00000001
#define FILL_WHITE_TRANSPARENT  B00000010
#define FILL_NONE               B00000011  // Both colors transparent equals none.
#define FILL_MODE_INVERSE       B00000100
#define DRAW_BLACK_TRANSPARENT  B00001000
#define DRAW_WHITE_TRANSPARENT  B00010000
#define DRAW_NONE               B00011000  // Both colors transparent equals none.
#define DRAW_MODE_INVERSE       B00100000


// Fill patterns (4x4 pixel grids as 16b ints)
#define PATTERN_WHITE           0x0000
#define PATTERN_BLACK           0xFFFF
#define PATTERN_GRAY            0x5a5a  // B0101101001011010
#define PATTERN_DK_GRAY         0xfaf5  // B1111101011110101
#define PATTERN_LT_GRAY         0x050a  // B0000010100001010
#define PATTERN_DK_L_DIAGONAL   0xedb7  // B1110110110110111
#define PATTERN_LT_L_DIAGONAL   0x1248  // B0001001001001000
#define PATTERN_DK_R_DIAGONAL   0x7bde  // B0111101111011110
#define PATTERN_LT_R_DIAGONAL   0x8421  // B1000010000100001
#define PATTERN_DK_GRID_SOLID   0xeeef  // B1110111011101111
#define PATTERN_LT_GRID_SOLID   0x1110  // B0001000100010000
#define PATTERN_DK_GRID_DOTS    0xfafa  // B1111101011111010
#define PATTERN_LT_GRID_DOTS    0x0505  // B0000010100000101
#define PATTERN_CHECKER         0x33cc  // B0011001111001100
#define PATTERN_CHECKER_INV     0xcc33  // B1100110000110011

// ###########################################################################
//
// ###########################################################################

/**
 *
 */
class GambyBase {
 public:
  void init();
  void clockOut(byte);
  void clockOutBit(boolean);
  void sendCommand(byte);
  void sendCommand(byte, byte);
};

/**
 *
 */
class GambyTextMode: public GambyBase {
 public:
  GambyTextMode();

 private:

};

/**
 *
 */
class GambyBlockMode: public GambyBase {
 public:
  GambyBlockMode();

 private:

};

#define NUM_DIRTY_COLUMNS NUM_COLUMNS >> 3

/**
 *
 */
class GambyGraphicsMode: public GambyBase {
 public:
  GambyGraphicsMode();
  void update();
  void setPixel(byte, byte, int);
  void setPixel(byte, byte, boolean);
  boolean getPatternPixel(byte, byte, int);
  void setPos(byte, byte);
  void drawSprite(const prog_uchar *, byte, byte);
  void drawSprite(const prog_uchar *, const prog_uchar *, byte, byte);

  unsigned int drawPattern;
  unsigned int drawMode;

 private:
  byte offscreen[NUM_COLUMNS][NUM_PAGES];
  byte dirtyBits[NUM_DIRTY_COLUMNS];

  void updateBlock(byte, byte);

};


#endif

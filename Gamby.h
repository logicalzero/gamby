#ifndef Gamby_h
#define Gamby_h

#include "Arduino.h"
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/io.h> 
#include <inttypes.h>

// ###########################################################################
//
// ###########################################################################

// Values to make code a little more readable
#define POST_COMMAND_DELAY	100 //50	// microseconds; arbitrarily chosen
#define BETWEEN_BIT_DELAY	5

#define CLEAR			255		// Use w/ setNLineInversion()

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

// Text drawing modes
#define NORMAL			0    // dark text, light background
#define INVERSE			0xFF // light text, dark background


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

#define TAB_WIDTH	8

// Word wrap modes
#define WRAP_NONE	B00000001    // text extends off beyond the right side of the screen. Fastest.
#define WRAP_CHAR	B00000010    // text wraps at the character that would extend beyond screen width.
#define WRAP_WORD	B00000100    // text breaks on whitespace. Slowest.

// Screen scrolling modes. NORMAL is marginally slower than the other two.
#define SCROLL_NONE	B00001000    // text goes off the bottom of the screen.
#define SCROLL_NORMAL	B00010000    // the entire display scrolls vertically when the bottom of the display is reached.
#define SCROLL_WRAP	B00100000    // Text resumes on the top line of the display after reaching the bottom.

// Inputs
#define DPAD_UP		B10000000
#define DPAD_LEFT	B01000000
#define DPAD_RIGHT	B00100000
#define DPAD_DOWN	B00010000
#define BUTTON_1	B00001000
#define BUTTON_2	B00000100
#define BUTTON_3	B00000010
#define BUTTON_4	B00000001


// ###########################################################################
//
// ###########################################################################

/**
 * The base class for all of Gamby's "modes." This should be considered an 
 * abstract class; only its subclasses should be instantiated.
 */
class GambyBase {
 public:
  void init();
  void clockOut(byte);
  void clockOutBit(boolean);
  void sendCommand(byte);
  void sendCommand(byte, byte);
  void clearDisplay();
  void readInputs();
  void setPos(byte, byte);

  static byte inputs;            /**< The D-Pad and button states. Set by readInputs(). */
  static unsigned int drawMode;  /**<  */

 private:
  static unsigned int toneTimeRemaining;
};


/**
 * The lightest-weight Gamby mode; uses no offscreen buffer. Eight lines of
 * scrolling, wrapping text using a proportional font. Also supports drawing
 * 'icons:' 8 pixel high bitmaps of any width.
 */
class GambyTextMode: public GambyBase {
 public:
  GambyTextMode();
  void setPos(byte, byte);
  void setColumn(byte);
  byte getCharWidth(byte);
  byte getCharBaseline(byte);
  int getTextWidth(char *);
  int getTextWidth_P(const char *);
  void drawChar(char);
  void drawText(char *);
  void drawText_P(const char *);
  void drawIcon(const prog_uchar *);
  void clearLine();
  void clearScreen();
  void newline();
  void scroll(int);

  byte wrapMode;            /**< The D-Pad and button states. Set by readInputs(). */
  byte scrollMode;          /**<  */
  const prog_int32_t* font; /**<  */

 private:
  byte currentLine;
  byte currentColumn;
  int offset;

};

/**
 * The second-lightest of Gamby's modes. Breaks the display into a 24x16 grid
 * of 4px x 4px blocks, drawn from a palette of 16, requiring a much smaller
 * offscreen buffer than the full graphics mode. Also supports the drawing
 * of (non-overlapping) sprites. 
 */
class GambyBlockMode: public GambyBase {
 public:
  GambyBlockMode();
  void setBlock(byte, byte, byte);
  byte getBlock(byte, byte);

  const prog_uint16_t* palette; /**<  */
  byte offscreen[NUM_COLUMNS/4][NUM_PAGES];

 private:
};

#define NUM_DIRTY_COLUMNS NUM_COLUMNS >> 3

/**
 * The most complete of Gamby's modes. Supports drawing of masked
 * bitmap sprites, as well as lines and shapes using 4px x 4px 
 * patterns.
 */
class GambyGraphicsMode: public GambyBase {
 public:
  GambyGraphicsMode();
  void clearScreen();
  void update();
  void setPixel(byte, byte);
  void setPixel(byte, byte, boolean);
  boolean getPatternPixel(byte, byte);

  void line(int, int, int, int);
  void box(int, int, int, int);
  void rect(int, int, int, int);
  void circle(int, int, int);
  void disc(int, int, int);

  void drawSprite(byte, byte, const prog_uchar *);
  void drawSprite(byte, byte, const prog_uchar *, const prog_uchar *);

  void drawText(int, int, char *);
  void drawText_P(int, int, const char *);

  static unsigned int drawPattern; /**<  */

  byte offscreen[NUM_COLUMNS][NUM_PAGES];
  byte dirtyBits[NUM_DIRTY_COLUMNS];

 private:
  void updateBlock(byte, byte);
  void drawHline(int, int, int);
  void plot4points(int, int, int, int);
  void plot8points(int, int, int, int);


};


#endif

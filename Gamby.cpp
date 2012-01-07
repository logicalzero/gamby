#include "Arduino.h"
#include "Gamby.h"

#define SWAP(x,y) x0^=y0^=x0^=y0

/****************************************************************************
 * Pins
 ****************************************************************************/

const int LCD_SID =	8;	// Data
const int LCD_SCK =	10;	// Clock
const int LCD_RS  =	11;	// Register Select (LOW = command, HIGH = data)
const int LCD_RES =	12; 	// Reset (inverted)
const int LCD_CS  =	13;    	// Chip select (inverted)


/****************************************************************************
 * 
 ****************************************************************************/

#ifndef Gamby
#define Gamby

byte GambyBase::inputs = 0;


/**
 * init(): Initialize the GAMBY LCD.
 *
 */
void GambyBase::init() {
  // Direct port manipulation to set output on pins 8-12.  
//  DDRB = DDRB | B00011111;
  
  pinMode(LCD_SID, OUTPUT);
  pinMode(LCD_SCK, OUTPUT);
  pinMode(LCD_RS,  OUTPUT);
  pinMode(LCD_RES, OUTPUT);
  pinMode(LCD_CS,  OUTPUT);

  digitalWrite(LCD_RES, LOW);
  digitalWrite(LCD_CS, LOW);
  digitalWrite(LCD_RES, HIGH);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_RS, LOW);
  
  sendCommand(SOFT_RESET);
  sendCommand(SET_DUTY_1, SET_DUTY_2);
  sendCommand(SET_BIAS);

  sendCommand(SHL_SELECT | B00001000); //1100xXXX
  sendCommand(ADC_SELECT | 0x01);
  
  sendCommand(SET_OSC_ON);
  sendCommand(DC_STEP_UP);
  sendCommand(REGULATOR_RESISTOR | REGULATOR_RESISTOR_VAL); 
  sendCommand(SET_EVR_1, SET_EVR_2 | VOLUME_CONTROL_VAL);
  sendCommand(SET_BIAS);
  sendCommand(POWER_CONTROL);
  sendCommand(CLEAR_POWER_SAVE);
  sendCommand(DISPLAY_INVERT | DISPLAY_INVERT_VAL);
  
  sendCommand(DISPLAY_POWER | ON);

  digitalWrite(LCD_RS, DATA);
  digitalWrite(LCD_CS, LOW);

  // Set up inputs
  inputs = 0;
  digitalWrite(18, LOW);
  digitalWrite(19, LOW);

  // Set Pins 2-8 as input, activate pull-up resistors   
  DDRD = DDRD & B00000011;
  PORTD = PORTD | B11111100;
}


/**
 * clockOut(): Send a byte to the LCD.
 *
 * @param data: The byte to send
 */
void GambyBase::clockOut(byte data) {
  
  byte i;
  digitalWrite(LCD_SID, HIGH);
  digitalWrite(LCD_SCK, HIGH);
    
  for (i=0; i<8; i++) {
    digitalWrite(LCD_SID, (data & B10000000));	// clock out MSBit of data
    digitalWrite(LCD_SCK, LOW);
    digitalWrite(LCD_SCK, HIGH);
    data = data << 0x01;
  }

  // Clock and data pins idle high
  digitalWrite(LCD_SID, HIGH);
}


/**
 * clockOutBit(): Send a single bit to the LCD.
 * 
 * @param b: The bit to send
 */
void GambyBase::clockOutBit(boolean b) {
  digitalWrite(LCD_SID, HIGH);  // necessary?
  digitalWrite(LCD_SCK, HIGH);
  digitalWrite(LCD_SID, b);
  digitalWrite(LCD_SCK, LOW);
  digitalWrite(LCD_SCK, HIGH);
    
  // Clock and data pins idle high
  digitalWrite(LCD_SID, HIGH);
}


/**
 * sendCommand(): Send a single-byte command to the LCD.
 *
 * @param command: The command to send, i.e. one of the constants,
 */
void GambyBase::sendCommand (byte command) {
  digitalWrite(LCD_RS, COMMAND);
  digitalWrite(LCD_CS, LOW);
  clockOut(command);
  digitalWrite(LCD_CS, HIGH);
}


/**
 * sendCommand(): Send a two-byte command to the LCD.
 *
 * @param b1: The first byte
 * @param b2: The second byte
 */
void GambyBase::sendCommand(byte b1, byte b2) {
  digitalWrite(LCD_RS, COMMAND);
  digitalWrite(LCD_CS, LOW);
  clockOut(b1);
  clockOut(b2);
  digitalWrite(LCD_CS, HIGH);
}


/**
 * clearDisplay(): Erase the screen contents and place the cursor in the first
 * column of the first row.
 *
 */
void GambyBase::clearDisplay () {
  byte i,j;
  for (i=0; i < NUM_PAGES; i++) {
    sendCommand(SET_PAGE_ADDR | i);
    sendCommand(SET_COLUMN_ADDR_1, SET_COLUMN_ADDR_2);
    digitalWrite(LCD_RS, DATA);
    digitalWrite(LCD_CS, LOW);
    for (j = 0; j < NUM_COLUMNS; j++) {
      clockOut(0);
    }
    digitalWrite(LCD_CS, HIGH);
  }
}


/**
 * readInputs(): Read the state of the DPad and buttons, then set the object's
 * `inputs` variable.
 *
 */
void GambyBase::readInputs() {
  // Digital pin 19 (A5) INPUT, digital pin 18 (A4) OUTPUT
  DDRC = (DDRC & B11001111) | B00010000;
  delay(1); // Not waiting gets inconsistent results. TODO: Determine how long is enough.
  inputs = PIND >> 4;
  
  // Digital pin 18 (A4) INPUT, digital pin 19 (A5) OUTPUT
  DDRC = (DDRC & B11001111) | B00100000;  
  delay(1);
  
  inputs = ~((PIND &  B11110000) | inputs);
}


/**
 * setPos(): Set the column and page location at which the next data will be
 * displayed.
 *
 * @param col:
 * @param line:
 */
void GambyBase::setPos(byte col, byte line) {
  sendCommand(SET_PAGE_ADDR | line);
  sendCommand(SET_COLUMN_ADDR_1 + ((col >> 4) & B00000111), 
	      SET_COLUMN_ADDR_2 | (col & B00001111)); 
}


/****************************************************************************
 * 
 ****************************************************************************/

/**
 * Constructor.
 *
 */
GambyTextMode::GambyTextMode() {
  currentLine = 0;
  currentColumn = 0;
  offset = 0;

  wrapMode = WRAP_WORD;
  scrollMode = SCROLL_NORMAL;
}


/**
 * scroll(): 
 *
 * @param s: The number of lines to scroll, positive or negative.
 */
void GambyTextMode::scroll(int s) {
  offset += s;
  if (offset >= NUM_PAGES)
    offset -= NUM_PAGES;
  else if (offset < 0)
    offset += NUM_PAGES;

  sendCommand(SET_INITIAL_COM0_1, 
	      (byte)offset << 3);  // shift 3 places to multiply by 8.
}


/**
 * setColumn(): Set the horizontal position of the cursor.
 *
 * @param column:
 */
void GambyTextMode::setColumn (byte column) {
  sendCommand(SET_COLUMN_ADDR_1 + (column >> 4), 
	      B00001111 & column); // & to mask out high bits
}


/**
 * setPos(): Set the cursor's column and line (relative to current scrolling).
 *
 * @param col: The column (0 to 95)
 * @param line: The 8 pixel line (0 to 7)
 */
void GambyTextMode::setPos(byte col, byte line) {
  currentColumn = col;
  int l = line - offset;
  if (l < 0)
    l += NUM_PAGES;
  sendCommand(SET_PAGE_ADDR | (byte)l);
  sendCommand(SET_COLUMN_ADDR_1 + ((currentColumn >> 4) & B00000111), 
	      SET_COLUMN_ADDR_2 | (currentColumn & B00001111)); 
}


/**
 * getCharWidth(): Retrieves the width of a given character.
 *
 * @param idx:The actual index into font data (ASCII value - 32)
 * @return: The character's with in pixels
 */
byte GambyTextMode::getCharWidth(byte idx) {
  return (byte)(pgm_read_dword(&font[idx]) & 0x0F); 
}


/**
 * getCharBaseline(): Get the vertical offset of a character.
 *
 * @param idx: The actual index into font data (ASCII value - 32)
 * @return: The character's vertical offset
 */
byte GambyTextMode::getCharBaseline(byte idx) {
  return (byte)((pgm_read_dword(&font[idx]) >> 4) & 0x07);
}


/**
 * drawChar(): Draw a character at the current screen position.
 *
 * @param c: is the ASCII character to draw.
 * @param inverse: is either NORMAL or INVERSE.
 */
void GambyTextMode::drawChar(char c, int inverse) {
  byte idx = byte(c) - 32;  // index into font data array
  long d = (long)pgm_read_dword(&font[idx]);  // character data
  byte w = (byte)(d & 0x0F);  // character width
  byte b = (byte)((d >> 4) & 0x07); // character baseline offset

  byte i,j,k;

  if (w + currentColumn > NUM_COLUMNS) {
    newline();
  }
  digitalWrite(LCD_CS, LOW);
  digitalWrite(LCD_RS, DATA);
  for (i = 0; i < w; i++) {
    // fill to baseline
    for (k = 0; k < (2 - b); k++)
      clockOutBit(inverse);

    // draw column of font bitmap
    for (j = 0; j < 5; j++) {
      clockOutBit(((d >> 31) ^ inverse) & 1);	// clock out MSBit of data
      d = d << 0x01;
    }

    // fill out top of character.
    for (k = 0; k<= b; k++)
      clockOutBit(inverse);    
  }
  // Draw gap ('kerning') between letters, 1px wide.
  clockOut(inverse);
  digitalWrite(LCD_CS, HIGH);

  currentColumn += w + 1;
}


/**
 * drawText(): Write a string to the display.
 *
 * @param s: the string to draw.
 * @param inverse: either NORMAL or INVERSE (0x00 or 0xFF).
 */
void GambyTextMode::drawText (char* s, int inverse) {
  int c = 0;
  while (s[c] != '\0') {
    if (s[c] == '\n')
      newline();
    else
      drawChar(s[c], inverse);
    c++;
  }
}


/** 
 * drawText_P(): Write a PROGMEM string to the display.
 *
 * @param s: the PROGMEM address of the string to draw.
 * @param inverse: either NORMAL or INVERSE (0x00 or 0xFF).
 */
void GambyTextMode::drawText_P(const char *s, int inverse) {
  char c = pgm_read_byte(s);
  while (c != '\0') {
    if (c == '\n')
      newline();
    else
      drawChar(c, inverse);
    c = pgm_read_byte(++s);
  }
}


/**
 * clearLine(): Clears the current line right of the current column.
 *
 */
void GambyTextMode::clearLine () {
  byte j;
  digitalWrite(LCD_RS, DATA);
  digitalWrite(LCD_CS, LOW);
  for (j = currentColumn; j <= LAST_COL; j++) {
    clockOut(0);
  }
  // restore previous column.
  sendCommand(SET_COLUMN_ADDR_1 + ((currentColumn >> 4) & B00000111), B00001111 & currentColumn); // & to mask out high bits

  //sendCommand(SET_COLUMN_ADDR_1, SET_COLUMN_ADDR_2 | currentColumn);
  digitalWrite(LCD_CS, HIGH);
}


/**
 * clearScreen(): Text-specific clear screen; resets scrolling offset.
 *
 */
void GambyTextMode::clearScreen() {
  currentColumn = 0;
  currentLine = 0;
  offset = 0;
  clearDisplay();
}


/**
 * newline():
 * 
 */
void GambyTextMode::newline() {
  currentLine++;
  if (currentLine >= NUM_PAGES) {
    switch (scrollMode) {
    case SCROLL_WRAP:
      currentLine = 0;
      break;

    case SCROLL_NORMAL:
      scroll(-1);

    default:
      // both SCROLL_NORMAL and SCROLL_NONE set the current line to the last.
      currentLine = (NUM_PAGES - 1);
    }
  }
  sendCommand(SET_PAGE_ADDR | (currentLine - offset));
  sendCommand(SET_COLUMN_ADDR_1, SET_COLUMN_ADDR_2);
  currentColumn = 0;
  clearLine();

}


/****************************************************************************
 * 
 ****************************************************************************/

/**
 * Constructor.
 *
 */
GambyBlockMode::GambyBlockMode() {
}


/****************************************************************************
 * 
 ****************************************************************************/

#ifdef SKIP_THIS_STUFF

unsigned int GambyGraphicsMode::drawPattern;
unsigned int GambyGraphicsMode::drawMode;

/**
 * Constructor.
 *
 */
GambyGraphicsMode::GambyGraphicsMode() {
  drawMode = 0;
}


/**
 * setPixel(): Set a pixel explicitly on or off.
 *
 * @param x: The pixel's horizontal position
 * @param y: The pixel's vertical position
 * @param p: The pixel, either off (0/false) or on (1/true)
 */
void GambyGraphicsMode::setPixel(byte x, byte y, boolean p) {
  byte c = x >> 3;            // column of dirtyBits to mark changed (x/8)
  byte r = y >> 3;            // row (page) of display, row of dirtyBits (y/8)
  byte b = y & B00000111;     // Bit number (within page) to change; low three bits
  byte oldOffscreen = offscreen[x][r]; // keep old offscreen to avoid unnecessary update

  offscreen[x][r] = bitWrite(offscreen[x][r], b, !p);

  // set "dirty bit" (flag as updated) if anything actually changed
  if (oldOffscreen != offscreen[x][r]) {
    // set "dirty bit"
    //setBit(dirtyBits[c], r, true);
    dirtyBits[c] = dirtyBits[c] | (1 << r);
  }
}


/**
 * setPixel(): Set a pixel using the current `drawPattern`.
 * 
 * @param x: The pixel's horizontal position
 * @param y: The pixel's vertical position
 */
void GambyGraphicsMode::setPixel(byte x, byte y) {
  byte c = x >> 3;            // column of dirtyBits to mark changed (x/8)
  byte r = y >> 3;            // row (page) of display, row of dirtyBits (y/8)
  byte b = y & B00000111;     // Bit number (within page) to change; low three bits
  byte oldOffscreen = offscreen[x][r]; // keep old offscreen to avoid unnecessary update

  boolean p = getPatternPixel(x, y);
  
  if (drawMode & DRAW_WHITE_TRANSPARENT || drawMode & DRAW_BLACK_TRANSPARENT) {
    if ((drawMode & DRAW_WHITE_TRANSPARENT) && !p)
      return;
    if ((drawMode & DRAW_BLACK_TRANSPARENT) && p)
      return;
  }

  //boolean p = true;
  offscreen[x][r] = bitWrite(offscreen[x][r], b, p);

  // set "dirty bit" (flag as updated) if anything actually changed
  if (oldOffscreen != offscreen[x][r]) {
    // set "dirty bit"
    //setBit(dirtyBits[c], r, true);
    dirtyBits[c] = dirtyBits[c] | (1 << r);
  }
}


/**
 * update(): Redraws the changed portions of the LCD.
 *
 */
void GambyGraphicsMode::update() {
  int c, r;
  digitalWrite(LCD_CS, LOW);
  for (r = 0; r < NUM_PAGES; r++) {
    for (c = 0; c < NUM_DIRTY_COLUMNS; c++) {
      if (bitRead(dirtyBits[c], r)) {
        updateBlock(c, r);
      }
    }
  }
  // Mark all bits as 'clean'
  for (c = 0; c < NUM_DIRTY_COLUMNS; c++) {
    dirtyBits[c] = 0;
  }
}


/**
 * updateblock(): Redraws an 8x8 block from the offscreen image.
 * TODO: Roll into update()?
 *
 * @param c: Column of screen block to update (width / 8)
 * @param r: The row (page) to update
 */
void GambyGraphicsMode::updateBlock(byte c, byte r) {
  int i;
  int x = c << 3;
  setPos(x, r); // setPos() should already be defined.
  digitalWrite(LCD_RS, DATA);
  digitalWrite(LCD_CS, LOW);
  for (i = 0; i < 8; i++) {
    // should already be in 'data' mode
    clockOut(offscreen[x + i][r]);

  }
}


/**
 * getPatternPixel():  Get pixel from a 4x4 grid (a 16b int) for the given
 * screen coordinates. Uses the drawPattern variable.
 *
 * @param x: Horizontal position on screen.
 * @param y: Vertical position on screen.
 * @return: The pixel for the given coordinates in the given pattern (0 or 1).
 */
boolean GambyGraphicsMode::getPatternPixel (byte x, byte y) {
  int i;
  // Don't do all the pattern-lookup work for solids
  if (drawPattern == PATTERN_BLACK)
    return false; // i.e. 0
  if (drawPattern == PATTERN_WHITE)
    return true; // i.e. 1
  // coordinates in pattern taken from least two bits of screen coords
  i = (x & B00000011) | ((y & B00000011) << 2);
  // only final bit determines pixel
  return (((drawPattern >> i) & 1) == 1);
}


/**
 * drawSprite (plain version): Draw a bitmap graphic.
 *
 * @param spriteIdx: The address of the sprite (e.g. the constant's name) 
 * @param x: The sprite's horizontal position
 * @param y: The sprite's vertical position
 */
void GambyGraphicsMode::drawSprite(const prog_uchar *spriteIdx, byte x, byte y) {
  byte w = pgm_read_byte_near(spriteIdx);
  byte h = pgm_read_byte_near(++spriteIdx);

  byte this_byte;
  byte this_bit=8;
  for (byte i=0; i<w; i++) {
    for (byte j=h; j>0; j--) {
      if (this_bit == 8) {
        spriteIdx++;
        this_byte = ~pgm_read_byte_near(spriteIdx);
        this_bit=0;
      }
      byte b = (this_byte << this_bit) & B10000000;
      setPixel(x+i,y+j,b);
      this_bit++;
    }
  }
}


/**
 * drawSprite (masked version): Draw a sprite, using another sprite as a mask 
 * (i.e. alpha channel). Note: the mask must be the same dimensions as the
 * foreground sprite. 
 *
 * @param spriteIdx: The address of the foreground sprite (e.g. the constant's name) 
 * @param maskIdx: The address of the mask sprite (e.g. the constant's name) 
 * @param x: The sprite's horizontal position
 * @param y: The sprite's vertical position
 */
void GambyGraphicsMode::drawSprite(const prog_uchar *spriteIdx, const prog_uchar *maskIdx, byte x, byte y) {
  byte w = pgm_read_byte_near(spriteIdx);
  byte h = pgm_read_byte_near(++spriteIdx);

  maskIdx++;
  byte this_byte, mask_byte;
  byte this_bit=8;
  for (byte i=0; i<w; i++) {
    for (byte j=h; j>0; j--) {
      if (this_bit == 8) {
        this_byte = ~pgm_read_byte_near(++spriteIdx);
        mask_byte = ~pgm_read_byte_near(++maskIdx);
        this_bit=0;
      }
      //byte b = (this_byte << this_bit) & B10000000;
      if (!((mask_byte << this_bit) & B10000000))
        setPixel(x+i,y+j, (this_byte << this_bit) & B10000000);
        this_bit++;
    }
  }
}


/**
 * line(): Draw a single-pixel-wide line between two points. 
 * 
 * @param x0: Start horizontal position
 * @param y0: Start vertical postition
 * @param x1: End horizontal position
 * @param y1: End vertical position
 */
 void line(int x0, int y0, int x1, int y1) {
  // A variation of Bresenham's line algorithm, cribbed from Wikipedia
  // See: http://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
  
  // My addition: use simpler method if line is horizontal (y0==y1)
  if (y0 == y1) {
    // make sure x0 is smaller than x1if (x0 > x1)
      SWAP(x0, x1);
    drawHline(x0, x1, y0);
    return;
  }
  // My addition: use simpler method if line is vertical (x0==x1)
  if (x0 == x1) {
    // make sure y0 is smaller than y1
    if (y0 > y1)
      SWAP(y0, y1);
    for (byte i=y0; i<=y1; i++) 
      setPixel((byte)x0, i); 
    return;
  }

  // cribbed code starts here
  boolean steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    SWAP(x0, y0);
    SWAP(x1, y1);
  }
  if (x0 > x1) {
    SWAP(x0, x1);
    SWAP(y0, y1);
  }

  int deltax = x1 - x0;
  int deltay = abs(y1 - y0);
  int error = deltax >> 1;
  int ystep = 1;
  int y = y0;
  int x = x0;

  if (y0 >= y1)
    ystep = -1;
  for (x = x0; x < x1; x++) {
    if (steep)
      setPixel((byte)y,(byte)x);
    else
      setPixel((byte)x,(byte)y);
    error = error - deltay;
    if (error < 0) {
      y += ystep;
      error += deltax;
    }
  }
} 

///////////////////////////////////////////////////////////////////////////////////

/**
 * circle(): Draw a circle centered at the given coordinates.
 *
 * @param cx: The horizontal posiiton of the circle's center
 * @param cy: The vertical position of the circle's center
 * @param radius: The circle's radius
 *
 */
void GambyGraphicsMode::circle(int cx, int cy, int radius) {
  int error = -radius;
  int x = radius;
  int y = 0;

  if (radius > 0) { 
    // The following while loop may altered to 'while (x > y)' for a
    // performance benefit, as long as a call to 'plot4points' follows
    // the body of the loop. This allows for the elimination of the
    // '(x != y') test in 'plot8points', providing a further benefit.
    //
    // For the sake of clarity, this is not shown here.
    while (x >= y) {
      plot8points(cx, cy, x, y);
 
      error += y;
      ++y;
      error += y;

      // The following test may be implemented in assembly language in
      // most machines by testing the carry flag after adding 'y' to
      // the value of 'error' in the previous step, since 'error'
      // nominally has a negative value.
      if (error >= 0) {
          --x;
          error -= x;
          error -= x;
      }
    }
  }
}


/**
 * plot8points(): Draws eight points. Used by the circle drawing.
 */
void GambyGraphicsMode::plot8points(int cx, int cy, int x, int y) {
  plot4points(cx, cy, x, y);
  if (x != y) 
    plot4points(cx, cy, y, x);
}
 

/**
 * plot4points(): Draws four points. Used by the circle drawing.
 * The 4th point can be omitted if the radius is known to be nonzero.
 */
void GambyGraphicsMode::plot4points(int cx, int cy, int x, int y) {
  setPixel(cx + x, cy + y);
  if (x != 0)
    setPixel(cx - x, cy + y);
  if (y != 0) 
    setPixel(cx + x, cy - y);
  if (x != 0 && y != 0) 
    setPixel(cx - x, cy - y);
}


/**
 * plot4lines(): Not currently used; part of filled circle drawing.
 *
 */
void GambyGraphicsMode::plot4lines(int cx, int cy, int x, int y) {
  int xplus = cx + x;
  int yplus = cy + y;
  int xminus = cx - x;
  int yminus = cy - y;

  line(x, yplus, xplus, yplus);
  if (x != 0) 
    line(x, yplus, xminus, yplus);
  if (y != 0) 
    line(x, yminus, xplus, yminus);
}

///////////////////////////////////////////////////////////////////////////////////

/**
 * plot2lines(): Not currently used; part of filled circle drawing.
 * 
 */
void GambyGraphicsMode::plot2lines(int cx, int cy, int x, int y) {

  // One pixel long 'line'
  if (x == 0) {
    setPixel(x, cy+y);
    if (y != 0)
      setPixel(x, cy-y);
    return;
  }

  drawHline(cx-x, cx+x, cy+y);
  // Mirror vertically if not the centerline.
  if (y != 0) 
    drawHline(cx-x, cx+x, cy-y);
}

/**
 * drawHLine(): Private. Draw a simple horizontal line.
 *
 * @param x1: first X coordinate, must be lower than x2
 * @param x2: second X coordinate, must be greater than x1
 * @param y: the common y coordinate
 */
 void GambyGraphicsMode::drawHline(int x1, int x2, int y) {
  for(int i=x1; i<=x2; i++)
    setPixel((byte)i, (byte)y);
}

#endif
#endif

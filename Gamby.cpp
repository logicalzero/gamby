#include "Arduino.h"

#include "Gamby.h"
#include "lcd.h"
#include "patterns.h"

// Major to-do items:
// ~~~~~~~~~~~~~~~~~~
// TODO: Replace all bitRead()/bitWrite() calls w/ normal bitwise ops --
//    but only after everything works. Doesn't seem to have much of a benefit,
//    though... tests don't show a signficant difference, but may be wrong.
// TODO: Get rid of weird and unnecessary typecasting throughout the code, 
//    which was originally put in during testing and forgotten about.
// TODO: Refactor buffer in GambyGraphicsMode/GambyBlockMode to allocate
//    memory dynamically, allowing for a smaller display area to save RAM,
//    adding a lightweight score display, et cetera.
// TODO: Make GambyTextMode the base class? Partial display areas in other
//    two modes could use the basic text drawing.

// Other items:
// ~~~~~~~~~~~~
// TODO: Word-wrap in GambyTextMode. 
// TODO: Change name of GambyBase::clockOut() to something better. Possibly
//    writeByte(). Users may want to use this.

// TODO: Consider doing something to support high-bit ASCII in a reasonable
//    way. Possibly do something to skip over symbols, just use letters.


/****************************************************************************
 * Pins
 ****************************************************************************/

const int LCD_SID =	8;	// Data
const int LCD_SCK =	10;	// Clock
const int LCD_RS  =	11;	// Register Select (LOW = command, HIGH = data)
const int LCD_RES =	12; 	// Reset (inverted)
const int LCD_CS  =	13;    	// Chip select (inverted)

const byte BIT_SID = 	B00000001;
const byte BIT_SCK =	B00000100;
const byte BIT_RS  =	B00001000;
const byte BIT_RES =	B00010000;
const byte BIT_CS  =	B00100000;

/****************************************************************************
 * Macros
 ****************************************************************************/

/**
 * Swap: Used in line drawing, assumes a variable named 'swap' has been
 * declared, and it is a type compatible with both 'x' and 'y'.
 * Just makes the code a little cleaner-looking.
 */
#define SWAP(x,y) swap=x; x=y; y=swap

/****************************************************************************
 * 
 ****************************************************************************/


byte GambyBase::inputs = 0;
unsigned int GambyBase::drawMode = NORMAL;


/**
 * Initialize the GAMBY LCD.
 *
 */
void GambyBase::init() {
  // Direct port manipulation to set output on pins 8-12.  
  DDRB = DDRB | B00111101;

  /*  
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
  */

  PORTB = PORTB & B11000000;
  PORTB = PORTB | BIT_RES | BIT_RS;
  PORTB = PORTB & ~BIT_RS;

  sendCommand(SOFT_RESET);
  sendCommand(SET_DUTY_1, SET_DUTY_2);
  sendCommand(SET_BIAS);

  sendCommand(SHL_SELECT_REVERSE); //1100xXXX
  sendCommand(ADC_SELECT);
  
  sendCommand(SET_OSC_ON);
  sendCommand(DC_STEP_UP);
  sendCommand(REGULATOR_RESISTOR | REGULATOR_RESISTOR_VAL); 
  sendCommand(SET_EVR_1, SET_EVR_2 | VOLUME_CONTROL_VAL);
  sendCommand(SET_BIAS);
  sendCommand(POWER_CONTROL);
  sendCommand(POWER_SAVE_CLEAR);
  sendCommand(DISPLAY_INVERT_OFF);
  
  sendCommand(DISPLAY_POWER_ON);

  //  digitalWrite(LCD_RS, DATA); // HIGH
  //  digitalWrite(LCD_CS, LOW);
  PORTB = (PORTB & ~BIT_CS) | BIT_SID;

  // Set up inputs
  inputs = 0;
  //  digitalWrite(18, LOW);
  //  digitalWrite(19, LOW);
  PORTC = PORTC & B11001111;

  // Set Pins 2-8 as input, activate pull-up resistors   
  DDRD = DDRD & B00000011;
  PORTD = PORTD | B11111100;

  clearDisplay();
  setPos(0,0);
}


/**
 * Send a byte to the LCD.
 *
 * @param data: The byte to send
 */
void GambyBase::clockOut(byte data) {
  byte i;

  //  digitalWrite(LCD_SID, HIGH);
  //  digitalWrite(LCD_SCK, HIGH);
  PORTB = PORTB | BIT_SID | BIT_SCK;
    
  for (i=0; i<8; i++) {
    //    digitalWrite(LCD_SID, (data & B10000000));	// clock out MSBit of data
    //    digitalWrite(LCD_SCK, LOW);
    //    digitalWrite(LCD_SCK, HIGH);
    if (data & B10000000)
      PORTB = (PORTB & ~BIT_SCK) | BIT_SID;
    else
      PORTB = PORTB & ~(BIT_SCK | BIT_SID);
    PORTB = PORTB | BIT_SCK;
    data = data << 0x01;
  }

  // Clock and data pins idle high
  //  digitalWrite(LCD_SID, HIGH);
  PORTB = PORTB | BIT_SID;
}


/**
 * Send a single-byte command to the LCD.
 *
 * @param command: The command to send, typically one of the constants
 *    defined in ``lcd.h``.
 */
void GambyBase::sendCommand (byte command) {
  //  digitalWrite(LCD_RS, COMMAND);
  //  digitalWrite(LCD_CS, LOW);
  PORTB = PORTB & ~(BIT_RS | BIT_CS);
  clockOut(command);
  //  digitalWrite(LCD_CS, HIGH);
  PORTB |= BIT_CS;
}


/**
 * sendCommand(): Send a two-byte command to the LCD.
 *
 * @param b1: The first byte
 * @param b2: The second byte
 */
void GambyBase::sendCommand(byte b1, byte b2) {
  //  digitalWrite(LCD_RS, COMMAND);
  //  digitalWrite(LCD_CS, LOW);
  PORTB = PORTB & ~(BIT_RS | BIT_CS);
  clockOut(b1);
  clockOut(b2);
  //  digitalWrite(LCD_CS, HIGH);
  PORTB |= BIT_CS;
}


/**
 * Erase the screen contents and place the cursor in the first column of the
 * first page.
 *
 */
void GambyBase::clearDisplay () {
  byte i,j;
  for (i=0; i < NUM_PAGES; i++) {
    sendCommand(SET_PAGE_ADDR | i);
    sendCommand(SET_COLUMN_ADDR_1, SET_COLUMN_ADDR_2);
    //    digitalWrite(LCD_RS, DATA);
    //    digitalWrite(LCD_CS, LOW);
    PORTB = (PORTB | BIT_RS) & ~BIT_CS;
    for (j = 0; j < NUM_COLUMNS; j++) {
      clockOut(0);
    }
    //    digitalWrite(LCD_CS, HIGH);
    PORTB = PORTB | BIT_CS;
  }
}


/**
 * Read the state of the DPad and buttons, then set the object's ``inputs`` 
 * variable.
 *
 */
void GambyBase::readInputs() {
  // Digital pin 19 (A5) INPUT, digital pin 18 (A4) OUTPUT
  DDRC = (DDRC & B11001111) | B00010000;
  //  delay(5); // Not waiting gets inconsistent results. 
  // TODO: Determine how long is enough. This seems to be okay, still gets
  // weird sometimes.
  asm("nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop");
  asm("nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop");
  asm("nop \n nop \n nop \n nop");
  inputs = PIND >> 4;
  
  // Digital pin 18 (A4) INPUT, digital pin 19 (A5) OUTPUT
  DDRC = (DDRC & B11001111) | B00100000;  
  //  delay(5);
  asm("nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop");
  asm("nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop");
  asm("nop \n nop \n nop \n nop");
  inputs = ~((PIND &  B11110000) | inputs);
}


/**
 * Set the column and 'page' location at which the next data will be 
 * displayed.
 *
 * @param col: The horizontal position, 0-97.
 * @param line: The vertical 'page,' 0-7.
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
  init();
  currentLine = 0;
  currentColumn = 0;
  offset = 0;

  wrapMode = WRAP_WORD;
  scrollMode = SCROLL_NORMAL;
}


/**
 * Scroll the screen up (or down) by one or more lines. 
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
 * Set the horizontal position of the cursor.
 *
 * @param column:
 */
void GambyTextMode::setColumn (byte column) {
  sendCommand(SET_COLUMN_ADDR_1 + (column >> 4), 
	      B00001111 & column); // & to mask out high bits
}


/**
 * Set the cursor's column and line (relative to current scrolling).
 *
 * @param col: The column (0 to 95)
 * @param line: The vertical 8 pixel 'page' (0 to 7)
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
 * Retrieves the width of a given character.
 *
 * @param idx:The actual index into font data (ASCII value - 32)
 * @return: The character's with in pixels
 */
byte GambyTextMode::getCharWidth(byte idx) {
  return (byte)(pgm_read_dword(&font[idx]) & 0x0F); 
}


/**
 * Get the vertical offset of a character.
 *
 * @param idx: The actual index into font data (ASCII value - 32)
 * @return: The character's vertical offset
 */
byte GambyTextMode::getCharBaseline(byte idx) {
  return (byte)((pgm_read_dword(&font[idx]) >> 4) & 0x07);
}


/** Get the width (in pixels) of a string.
 *
 * @param s: the string to measure.
 */
int GambyTextMode::getTextWidth(char* s) {
  int width = 0;
  for (int i=0; s[i] != '\0'; i++) {
    char c = s[i]-32;
    if (c >= 0)
      width += getCharWidth(c) + 1; // width + gutter
  }
  // No gutter after last character, so subtract it.
  return width-1;
}


/** Get the width (in pixels) of a string in PROGMEM.
 *
 * @param s: the PROGMEM address of the string to measure.
 */
int GambyTextMode::getTextWidth_P(const char *s) {
  int width = 0;
  char c = pgm_read_byte_near(s);
  while (c != '\0') {
    c -= 32;
    if (c > 0)
      width += getCharWidth(c) + 1;
    c = pgm_read_byte_near(++s);
  }
  return width-1;
}


/**
 * Draw a character at the current screen position.
 *
 * @param c: is the ASCII character to draw.
 */
void GambyTextMode::drawChar(char c) {

  // Newline character. Bail.
  if (c == '\n') {
    newline();
    return;
  }

  //  digitalWrite(LCD_CS, LOW);
  //  digitalWrite(LCD_RS, DATA);
  PORTB = (PORTB & ~BIT_CS) | BIT_RS;

  if (c == '\t') {
    if (currentColumn + 8 > NUM_COLUMNS) {
      newline();
      return;
    }

    // else, scootch in.
    for (byte t = (((currentColumn + 1) & B11111000) + 8) - currentColumn; t > 0; t--)
      clockOut(drawMode);
    currentColumn = ((currentColumn + 1) & B11111000) + 8;
    return;
  }

  long d = (long)pgm_read_dword(&font[byte(c) - 32]);  // character data
  byte w = (byte)(d & 0x0F);  // character width
  byte b = (byte)((d >> 4) & 0x07); // character baseline offset

  byte j;
  byte col; // The current column of the character

  currentColumn += w + 1;
  if (currentColumn > NUM_COLUMNS) {
    newline();
  }

  for (; w > 0; --w) {
    col = 0;
   
    // generate column of font bitmap
    for (j = 0; j < 5; j++) {
      col = (col << 1) | ((d >> 31) & 1);
      d = d << 0x01;
    }

    // fill out top of character.
    col = col << (b+1);

    clockOut(col ^ drawMode);
  }

  // Draw gap ('kerning') between letters, 1px wide.
  clockOut(drawMode);
  //  digitalWrite(LCD_CS, HIGH);
  PORTB = PORTB | BIT_CS;
}


/**
 * Write a string to the display.
 *
 * @param s: the string to draw.
 */
void GambyTextMode::drawText (char* s) {
  for (int c=0; s[c] != '\0'; c++)
    drawChar(s[c]);
}


/** 
 * Write a PROGMEM string to the display.
 *
 * @param s: the PROGMEM address of the string to draw.
 */
void GambyTextMode::drawText_P(const char *s) {
  char c = pgm_read_byte_near(s);
  while (c != '\0') {
    drawChar(c);
    c = pgm_read_byte_near(++s);
  }
}


/**
 * Clears the current line right of the current column.
 *
 */
void GambyTextMode::clearLine () {
  byte j;
  //  digitalWrite(LCD_RS, DATA);
  //  digitalWrite(LCD_CS, LOW);
  PORTB = (PORTB & ~BIT_CS) | BIT_RS;
  for (j = currentColumn; j <= LAST_COL; j++) {
    clockOut(0);
  }
  // restore previous column.
  sendCommand(SET_COLUMN_ADDR_1 + ((currentColumn >> 4) & B00000111), B00001111 & currentColumn); // & to mask out high bits

  //sendCommand(SET_COLUMN_ADDR_1, SET_COLUMN_ADDR_2 | currentColumn);
  //  digitalWrite(LCD_CS, HIGH);
  PORTB = PORTB | BIT_CS;
}


/**
 * Text-specific clear screen; resets scrolling offset.
 *
 */
void GambyTextMode::clearScreen() {
  currentColumn = 0;
  currentLine = 0;
  offset = 0;
  clearDisplay();
}


/**
 * Move the cursor to the first column of the next line, scrolling the
 * display if necessary.
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


/** 
 * Draw an 8px high icon at the current position on screen.
 *
 * @param idx: 
 */
void GambyTextMode::drawIcon(const prog_uchar *idx) {
  PORTB = (PORTB & ~BIT_CS) | BIT_RS;
  byte w = pgm_read_byte_near(idx);
  currentColumn += w;
  for (; w > 0; w--) {
    clockOut(pgm_read_byte_near(++idx));
  }
}

/****************************************************************************
 * 
 ****************************************************************************/

/**
 * Constructor.
 *
 */
GambyBlockMode::GambyBlockMode() {
  init();
}


/**
 * Draw a block at a given location.
 *
 * @param x: The horizontal position. 
 * @param y: The vertical position.
 * @param idx: The index of the block to draw.
 */
void GambyBlockMode::setBlock(byte x, byte y, byte idx) {
  // TODO: Implement this!
}


/**
 * Retrieve the index of the block at the given location.
 *
 * @param x: The horizontal position. 
 * @param y: The vertical position.
 * @return: The index of the block at the given coordinates.
 */
byte GambyBlockMode::getBlock(byte x, byte y) {
  // TODO: Implement this!
}


/****************************************************************************
 * 
 ****************************************************************************/

unsigned int GambyGraphicsMode::drawPattern;

/**
 * Constructor.
 *
 */
GambyGraphicsMode::GambyGraphicsMode() {
  init();
  drawMode = 0;
}


/**
 *
 *
 */
void GambyGraphicsMode::clearScreen() {
  int r, c;
  for (r = 0; r < NUM_PAGES; r++) {
    for (c = 0; c < NUM_COLUMNS; c++) {
      offscreen[c][r] = 0;
    }
  }
  for (c = 0; c < NUM_DIRTY_COLUMNS; c++) {
    dirtyBits[c] = 0xFF;
  }
}


/**
 * Set a pixel explicitly on or off.
 *
 * @param x: The pixel's horizontal position
 * @param y: The pixel's vertical position
 * @param p: The pixel, either off (0/false) or on (1/true)
 */
void GambyGraphicsMode::setPixel(byte x, byte y, boolean p) {
  // negative ints cast to byte are 256+val
  if (x >= NUM_COLUMNS || y >= NUM_ROWS)
      return;

  byte c = x >> 3;            // column of dirtyBits to mark changed (x/8)
  byte r = y >> 3;            // row (page) of display, row of dirtyBits (y/8)
  byte b = y & B00000111;     // Bit number (within page) to change; low three bits
  byte oldOffscreen = offscreen[x][r]; // keep old offscreen to avoid unnecessary update

  // TODO: Replace bitWrite with bitwise ops
  offscreen[x][r] = bitWrite(offscreen[x][r], b, !p);

  // set "dirty bit" (flag as updated) if anything actually changed
  if (oldOffscreen != offscreen[x][r]) {
    // set "dirty bit"
    //setBit(dirtyBits[c], r, true);
    dirtyBits[c] = dirtyBits[c] | (1 << r);
  }
}


/**
 * Set a pixel using the current `drawPattern`.
 * 
 * @param x: The pixel's horizontal position
 * @param y: The pixel's vertical position
 */
void GambyGraphicsMode::setPixel(byte x, byte y) {
  // negative ints cast to byte are 256+val
  if (x >= NUM_COLUMNS || y >= NUM_ROWS)
      return;

  boolean p = getPatternPixel(x, y);

  if (drawMode & DRAW_WHITE_TRANSPARENT || drawMode & DRAW_BLACK_TRANSPARENT) {
    if ((drawMode & DRAW_WHITE_TRANSPARENT) && !p)
      return;
    if ((drawMode & DRAW_BLACK_TRANSPARENT) && p)
      return;
  }

  byte c = x >> 3;            // column of dirtyBits to mark changed (x/8)
  byte r = y >> 3;            // row (page) of display, row of dirtyBits (y/8)
  byte b = y & B00000111;     // Bit number (within page) to change; low three bits
  byte oldOffscreen = offscreen[x][r]; // keep old offscreen to avoid unnecessary update

  offscreen[x][r] = bitWrite(offscreen[x][r], b, p);
  //  offscreen[x][r] = (offscreen[x][r] & ~(1 << b)) | (p << b);

  // set "dirty bit" (flag as updated) if anything actually changed
  if (oldOffscreen != offscreen[x][r]) {
    dirtyBits[c] = dirtyBits[c] | (1 << r);
  }
  
}


/**
 * Redraws the changed portions of the LCD.
 *
 */
void GambyGraphicsMode::update() {
  int c, r;
  //  digitalWrite(LCD_CS, LOW);
  PORTB = PORTB & ~BIT_CS;
  for (r = 0; r < NUM_PAGES; r++) {
    for (c = 0; c < NUM_DIRTY_COLUMNS; c++) {
      //if (bitRead(dirtyBits[c], r)) {
      if (dirtyBits[c] & (1 << r)) {
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
 * Redraws an 8x8 block from the offscreen image.
 * TODO: Roll into update()?
 *
 * @param c: Column of screen block to update (width / 8)
 * @param r: The row (page) to update
 */
void GambyGraphicsMode::updateBlock(byte c, byte r) {
  int i;
  int x = c << 3;
  setPos(x, r); // setPos() should already be defined.
  //  digitalWrite(LCD_CS, LOW);
  //  digitalWrite(LCD_RS, DATA);
  PORTB = (PORTB & ~BIT_CS) | BIT_RS;
  for (i = 0; i < 8; i++) {
    // should already be in 'data' mode
    clockOut(offscreen[x + i][r]);

  }
}


/**
 * Get pixel from a 4x4 grid (a 16b int) for the given screen coordinates. 
 * Uses the drawPattern variable.
 *
 * @param x: Horizontal position on screen.
 * @param y: Vertical position on screen.
 * @return: The pixel for the given coordinates in the given pattern (0 or 1).
 */
boolean GambyGraphicsMode::getPatternPixel (byte x, byte y) {
  int i;
  // Don't do all the pattern-lookup work for solids
  if (drawPattern == PATTERN_BLACK)
    return true; // i.e. 1 - an on pixel
  if (drawPattern == PATTERN_WHITE)
    return false; // i.e. 0 - an off pixel
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
void GambyGraphicsMode::drawSprite(byte x, byte y, const prog_uchar *spriteIdx) {
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
void GambyGraphicsMode::drawSprite(byte x, byte y, const prog_uchar *spriteIdx, const prog_uchar *maskIdx) {
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
 * Draw a single-pixel-wide line between two points. 
 * 
 * @param x0: Start horizontal position
 * @param y0: Start vertical postition
 * @param x1: End horizontal position
 * @param y1: End vertical position
 */
void GambyGraphicsMode::line(int x0, int y0, int x1, int y1) {
  // A variation of Bresenham's line algorithm, cribbed from Wikipedia
  // See: http://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

  // The SWAP macro uses this variable. Don't remove/rename.
  int swap;

  // My addition: use simpler method if line is horizontal (y0==y1)
  if (y0 == y1) {
    // make sure x0 is smaller than x1
    if (x0 > x1) {
      SWAP(x0,x1);
    }
    drawHline(x0, x1, y0);
    return;
  }
  // My addition: use simpler method if line is vertical (x0==x1)
  if (x0 == x1) {
    // make sure y0 is smaller than y1
    if (y0 > y1) {
      SWAP(y0,y1);
    }
    drawVline(y0, y1, x0);
    return;
  }

  // cribbed code starts here
  boolean steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    SWAP(x0,y0);
    SWAP(x1,y1);
  }
  if (x0 > x1) {
    SWAP(x0,x1);
    SWAP(y0,y1);
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
 * Draw a circle centered at the given coordinates.
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
 * Draws four points. Used by the circle drawing.
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
 * Draw a filled circle using the current drawPattern.
 *
 * @param cx: The horizontal position of the disc's center
 * @param xy: The vertical position of the disc's center
 * @param radius: The disc's radius
 */
void GambyGraphicsMode::disc(int cx, int cy, int radius) {
  int error = -radius;
  int x = radius;
  int y = 0;

  if (radius > 0) {
    while (x >= y) {

      // TODO: This can probably be optimized.
      drawHline(cx-x, cx+x, cy+y);
      drawHline(cx-x, cx+x, cy-y);
      drawHline(cx-y, cx+y, cy+x);
      drawHline(cx-y, cx+y, cy-x);

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
 * Draw an empty rectangle, using the current drawPattern.
 *
 */
void GambyGraphicsMode::box(int x1, int y1, int x2, int y2) {
  // The SWAP macro uses this variable. Don't remove/rename.
  int swap;

  // Make sure Xs and Ys are ordered
  if (x1 > x2) {
    SWAP(x1,x2);
  }
  if (y1 > y2) {
    SWAP(y1,y2);
  }

  // Draw as 4 lines
  drawHline(x1, x2, y1);
  drawHline(x1, x2, y2);
  drawVline(y1, y2, x1);
  drawVline(y1, y2, x2);
}


/**
 * Draw a filled rectangle, using the current drawPattern.
 *
 */
void GambyGraphicsMode::rect(int x1, int y1, int x2, int y2) {
  // The SWAP macro uses this variable. Don't remove/rename.
  int swap;

  // Make sure Xs and Ys are ordered
  if (x1 > x2) {
    SWAP(x1,x2);
  }
  if (y1 > y2) {
    SWAP(y1,y2);
  }
  
  // Fill the rectangle, one pixel at a time.
  for (int x = x1; x <= x2; x++) {
    for (int y = y1; y <= y2; y++) {
      setPixel((int)x, (int)y);
    }
  }
}


/**
 * Private. Draw a simple horizontal line.
 *
 * @param x1: first X coordinate, must be lower than x2
 * @param x2: second X coordinate, must be greater than x1
 * @param y: the common y coordinate
 */
 void GambyGraphicsMode::drawHline(int x1, int x2, int y) {
  for(int i=x1; i<=x2; i++)
    setPixel((byte)i, (byte)y);
}

/**
 * Private. Draw a simple vertical line.
 *
 * @param y1: first Y coordinate, must be lower than y2
 * @param y2: second Y coordinate, must be greater than y1
 * @param x: the common y coordinate
 */
 void GambyGraphicsMode::drawVline(int y1, int y2, int x) {
  for (int i=y1; i<=y2; i++) 
    setPixel((byte)x, (byte)i); 
}

/**
 * Write a string to the display.
 *
 * @param x: The horizontal position at which to draw the text.
 * @param y: The vertical position at which to draw the text.
 * @param s: the string to draw.
 */
void GambyGraphicsMode::drawText (int x, int y, char* s) {
  // TODO: Implement this!
}


/** 
 * Write a PROGMEM string to the display.
 *
 * @param x: The horizontal position at which to draw the text.
 * @param y: The vertical position at which to draw the text.
 * @param s: the PROGMEM address of the string to draw.
 */
void GambyGraphicsMode::drawText_P(int x, int y, const char *s) {
  // TODO: Implement this!
}

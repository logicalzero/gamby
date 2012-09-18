#include "Arduino.h"

#include "Gamby.h"

// Major to-do items:
// ~~~~~~~~~~~~~~~~~~
// TODO: Replace all bitRead()/bitWrite() calls w/ normal bitwise ops --
//    but only after everything works. Doesn't seem to have much of a benefit,
//    though... tests don't show a signficant difference, but may be wrong.
// TODO: Get rid of weird and unnecessary typecasting throughout the code, 
//    which was originally put in during testing and forgotten about.
// TODO: Refactor buffer in GambyGraphicsMode/GambyBlockMode to allocate
//    memory dynamically, allowing for a smaller display area to save RAM,
//    adding a lightweight score display, et cetera. Longer-term.
// TODO: Make GambyTextMode the base class? Partial display areas in other
//    two modes could use the basic text drawing.

// Other items:
// ~~~~~~~~~~~~
// TODO: Word-wrap in GambyTextMode. 

// TODO: Consider doing something to support high-bit ASCII in a reasonable
//    way. Possibly do something to skip over symbols, just use letters.


/****************************************************************************
 * Macros
 ****************************************************************************/

/**
 * Swap: Used in line drawing, assumes a and b are of XOR-able type
 * thanks to http://graphics.stanford.edu/~seander/bithacks.html#SwappingValuesXOR
 */
#define SWAP(a,b) (((a) ^ (b)) && ((b) ^= (a) ^= (b), (a) ^= (b)))

/**
 * Macros to put LCD into 'data' or 'command' mode (i.e. setting CS and RS).
 * They make the code a bit more readable.
 */
#define COMMAND_MODE() PORTB = PORTB & ~(BIT_RS | BIT_CS);
#define DATA_MODE()    PORTB = (PORTB & ~BIT_CS) | BIT_RS;


/****************************************************************************
 * 
 ****************************************************************************/

byte GambyBase::inputs = 0;


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

  // Set up inputs

  inputs = B00000000;
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
void GambyBase::sendByte(byte data) {
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
 *    defined in `lcd.h`.
 */
void GambyBase::sendCommand (byte command) {
  COMMAND_MODE();
  sendByte(command);
  //  digitalWrite(LCD_CS, HIGH);
  PORTB |= BIT_CS;
}


/**
 * Send a two-byte command to the LCD.
 *
 * @param b1: The first byte
 * @param b2: The second byte
 */
void GambyBase::sendCommand(byte b1, byte b2) {
  COMMAND_MODE();
  sendByte(b1);
  sendByte(b2);
  //  digitalWrite(LCD_CS, HIGH);
  PORTB |= BIT_CS;
}


/**
 * Erase the screen contents and place the cursor in the first column of the
 * first page. If used in GambyGraphicsMode, only the screen is cleared;
 * the contents of the buffer are preserved and will be redrawn when
 * `update()` is called.
 *
 */
void GambyBase::clearDisplay () {
  byte i,j;
  for (i=0; i < NUM_PAGES; i++) {
    sendCommand(SET_PAGE_ADDR | i);
    sendCommand(SET_COLUMN_ADDR_1, SET_COLUMN_ADDR_2);
    DATA_MODE();
    for (j = 0; j < NUM_COLUMNS; j++) {
      sendByte(0);
    }
    //    digitalWrite(LCD_CS, HIGH);
    PORTB = PORTB | BIT_CS;
  }
  currentColumn = 0;
  currentPage = 0;
}


/**
 * Read the state of the DPad and buttons, then set the object's `inputs` 
 * variable.
 *
 */
void GambyBase::readInputs() {
  // Digital pin 19 (A5) INPUT, digital pin 18 (A4) OUTPUT
  DDRC = (DDRC & B11001111) | B00010000;
  //  delay(5); // Not waiting gets inconsistent results.
  // Do delay without delay(), which acts weird w/ interrupts.
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
  currentColumn = col;
  currentPage = line;
}


/** 
 * Draw an 8px high icon at the current position on screen. The icon itself
 * is stored in PROGMEM.
 *
 * @param icon: The icon's location in `PROGMEM` (e.g. the name of the 
 *    `PROGMEM` constant).
 */
void GambyBase::drawIcon(const prog_uchar *icon) {
  DATA_MODE();
  byte w = pgm_read_byte_near(icon);
  currentColumn += w;
  for (; w > 0; w--) {
    sendByte(pgm_read_byte_near(++icon));
  }
}

/** 
 * Draw an 8px high icon at the current position on screen. The icon itself
 * is stored in PROGMEM.
 *
 * @param icon  The icon's location in `PROGMEM` (e.g. the name of the 
 *    `PROGMEM` constant).
 * @param frame  The frame number, 0 to (total frames)-1
 */
void GambyBase::drawIcon(const prog_uchar *icon, byte frame) {
  DATA_MODE();
  byte w = pgm_read_byte_near(icon);
  icon += w * frame;
  currentColumn += w;
  for (; w > 0; w--) {
    sendByte(pgm_read_byte_near(++icon));
  }
}


/** Retrieves the width of a given character.
 *
 * @param idx  The actual index into font data (ASCII value - 32)
 * @return     The character's with in pixels
 */
byte GambyBase::getCharWidth(byte idx) {
  return (byte)(pgm_read_dword(&font[idx]) & 0x0F); 
}


/**
 * Get the vertical offset of a character.
 *
 * @param idx: The actual index into font data (ASCII value - 32)
 * @return: The character's vertical offset
 */
byte GambyBase::getCharBaseline(byte idx) {
  return (byte)((pgm_read_dword(&font[idx]) >> 4) & 0x07);
}


/** 
 * Get the width (in pixels) of a string.
 *
 * @param s: the string to measure.
 */
int GambyBase::getTextWidth(char* s) {
  int width = 0;
  for (int i=0; s[i] != '\0'; i++) {
    char c = s[i]-32;
    if (c >= 0)
      width += getCharWidth(c) + 1; // width + gutter
  }
  // No gutter after last character, so subtract it.
  return width-1;
}


/** 
 * Get the width (in pixels) of a string in `PROGMEM`.
 *
 * @param s: the `PROGMEM` address (e.g. the constant's name) of the string 
 *    to measure.
 */
int GambyBase::getTextWidth_P(const char *s) {
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



/****************************************************************************
 * 
 ****************************************************************************/

byte GambyTextMode::drawMode = TEXT_NORMAL;
byte GambyTextMode::scrollMode = SCROLL_NORMAL;
byte GambyTextMode::wrapMode = WRAP_CHAR;

/**
 * Constructor.
 *
 */
GambyTextMode::GambyTextMode() {
  init();
  currentPage = 0;
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
 * @param column  The horizontal position on screen (0-95).
 */
void GambyTextMode::setColumn (byte column) {
  sendCommand(SET_COLUMN_ADDR_1 + (column >> 4), 
	      B00001111 & column); // & to mask out high bits
}


/**
 * Set the cursor's column and line (relative to current scrolling).
 *
 * @param col   The column (0 to 95)
 * @param line  The vertical 8 pixel 'page' (0 to 7)
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

  DATA_MODE();

  if (c == '\t') {
    // If the tab will exceed the screen width, new line:
    if (currentColumn + 8 > NUM_COLUMNS) {
      newline();
      return;
    }

    // else, scootch in.
    for (byte t = (((currentColumn + 1) & B11111000) + 8) - currentColumn; t > 0; t--)
      sendByte(drawMode);
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

    sendByte(col ^ drawMode);
  }

  // Draw gap ('kerning') between letters, 1px wide.
  sendByte(drawMode);
  //  digitalWrite(LCD_CS, HIGH);
  PORTB = PORTB | BIT_CS;
}


/**
 * Write a string to the display.
 *
 * @param s: the string to draw.
 */
void GambyTextMode::print (char* s) {
  for (int c=0; s[c] != '\0'; c++)
    drawChar(s[c]);
}


/**
 * Write a string to the display, followed by a newline.
 * To save a little memory, instead of using both `println()` and 
 * `print()`, consider using only `print()` and 'manually' end your 
 * strings with a 'newline' character ('`\n`').
 *
 * @param s: the string to draw.
 */
void GambyTextMode::println(char* s) {
  for (int c=0; s[c] != '\0'; c++)
    drawChar(s[c]);
  newline();
}


/** 
 * Write a PROGMEM string to the display.
 *
 * @param s: the PROGMEM address (e.g. the name of the variable) of the 
 *     string to draw.
 */
void GambyTextMode::print_P(const char *s) {
  char c = pgm_read_byte_near(s);
  while (c != '\0') {
    drawChar(c);
    c = pgm_read_byte_near(++s);
  }
}

/** 
 * Write a PROGMEM string to the display.
 * To save a little memory, instead of using both `println_P()` and 
 * `print_P()`, consider using only `print_P()` and 'manually' end your 
 * strings with a 'newline' character ('`\n`').
 *
 * @param s: the PROGMEM address of the string to draw.
 */
void GambyTextMode::println_P(const char *s) {
  print_P(s);
  newline();
}


/**
 * Clears the current line from the current column to the right edge of the
 * screen.
 *
 */
void GambyTextMode::clearLine () {
  byte j;
  DATA_MODE();
  for (j = currentColumn; j <= LAST_COLUMN; j++) {
    sendByte(0);
  }
  // restore previous column.
  sendCommand(SET_COLUMN_ADDR_1 + ((currentColumn >> 4) & B00000111), 
	      B00001111 & currentColumn); // & to mask out high bits

  //sendCommand(SET_COLUMN_ADDR_1, SET_COLUMN_ADDR_2 | currentColumn);
  //  digitalWrite(LCD_CS, HIGH);
  PORTB = PORTB | BIT_CS;
}


/**
 * Text-specific clear screen; resets scrolling offset.
 *
 */
void GambyTextMode::clearScreen() {
  offset = 0;
  clearDisplay();
}


/**
 * Move the cursor to the first column of the next line, scrolling the
 * display if necessary.
 * 
 */
void GambyTextMode::newline() {
  currentPage++;
  if (currentPage >= NUM_PAGES) {
    switch (scrollMode) {
    case SCROLL_WRAP:
      currentPage = 0;
      break;

    case SCROLL_NORMAL:
      scroll(-1);

    default:
      // both SCROLL_NORMAL and SCROLL_NONE set the current line to the last.
      currentPage = (NUM_PAGES - 1);
    }
  }
  sendCommand(SET_PAGE_ADDR | (currentPage - offset));
  sendCommand(SET_COLUMN_ADDR_1, SET_COLUMN_ADDR_2);
  currentColumn = 0;
  clearLine();

}



/****************************************************************************
 * 
 ****************************************************************************/

/**
 * Constructor. Runs once when the mode's instance is created, initializing
 * the display.
 */
GambyBlockMode::GambyBlockMode() {
  byte i,j;
  for (i=0; i<NUM_BLOCK_COLUMNS; i++)
    for (j=0; j<NUM_PAGES; j++)
      offscreen[i][j] = 0;
  init();
}

/**
 *
 *
 */
void GambyBlockMode::clearScreen() {
  byte i,j;
  for (i=0; i<NUM_BLOCK_COLUMNS; i++)
    for (j=0; j<NUM_PAGES; j++)
      offscreen[i][j] = 0;
  clearDisplay();
}

/**
 * Retrieve the index (number) of the block at the given location.
 *
 * @param x  The horizontal position, 0 to 23. 
 * @param y  The vertical position, 0 to 15.
 * @return   The index of the block (0 to 15) at the given coordinates.
 */
byte GambyBlockMode::getBlock(byte x, byte y) {
  if (y & 1)
    // odd blocks shifted to the high four bits
    return (offscreen[x][y >> 1] >> 4) & B00001111;
  return offscreen[x][y >> 1] & B00001111;
}


/**
 * Set a block at a given location without immediately updating the screen. 
 * The change is made to the offscreen buffer; a separate call to update() 
 * is required for it to show up. Use this if you want to do a lot of drawing 
 * and only show the end results.
 *
 * @param x  The horizontal position, 0 to 23. 
 * @param y  The vertical position, 0 to 15.
 * @param block The index of the block to draw (0 to 15).
 */
void GambyBlockMode::setBlock(byte x, byte y, byte block) {

  // each page is two blocks high, each block index is 4 bits
  byte evenBlockIdx, oddBlockIdx;
  byte page = y >> 1;

  // Get the other block in that 8-pixel-high 'page'
  // Set the specified block in the offscreen
  if (y & 1) {
    // odd blocks shifted to the high four bits
    oddBlockIdx = block & B00001111;
    evenBlockIdx = offscreen[x][page] & B00001111;
    offscreen[x][page] = (oddBlockIdx << 4) | evenBlockIdx;
  } else {
    evenBlockIdx = block;
    oddBlockIdx = (offscreen[x][page] & B11110000);
    offscreen[x][page] = oddBlockIdx | evenBlockIdx;
  }
}
 

/**
 * Draw a block at a given location.
 *
 * @param x  The horizontal position, 0 to 23. 
 * @param y  The vertical position, 0 to 15.
 * @param block The index of the block to draw (0 to 15).
 */
void GambyBlockMode::drawBlock(byte x, byte y, byte block) {
  // TODO: Optimize this!
  
  // each page is two blocks high, each block index is 4 bits
  byte evenBlockIdx, oddBlockIdx;
  byte page = y >> 1;

  // Get the other block in that 8-pixel-high 'page'
  // Set the specified block in the offscreen
  if (y & 1) {
    // odd blocks shifted to the high four bits
    oddBlockIdx = block & B00001111;
    evenBlockIdx = offscreen[x][page] & B00001111;
    offscreen[x][page] = (oddBlockIdx << 4) | evenBlockIdx;
  } else {
    evenBlockIdx = block;
    oddBlockIdx = (offscreen[x][page] & B11110000);
    offscreen[x][page] = oddBlockIdx | evenBlockIdx;
    oddBlockIdx = oddBlockIdx >> 4;
  }

  // Do the drawing
  unsigned int oddBlock = pgm_read_word_near(palette + oddBlockIdx);
  unsigned int evenBlock = pgm_read_word_near(palette + evenBlockIdx);
  byte i;

  // Don't send set position command if already in the right place
  if ((currentPage != page) || (currentColumn != x)) {
    setPos(x<<2, page);
  }

  DATA_MODE();

  for (i=0; i<4; i++) {
    // Build a two-block set four bits at a time
    byte combo = (((oddBlock & B00001111) << 4) | (evenBlock & B00001111));
    sendByte(combo);
    oddBlock = oddBlock >> 4;
    evenBlock = evenBlock >> 4;
  }
  currentPage = page;
  currentColumn = x + 1;
}


/**
 * Redraw the entire screen. In `GambyBlockMode`, this will cause the entire
 * display to redraw, showing blocks set via `setBlock()`. 
 *
 */
void GambyBlockMode::update() {
  update(0,0,NUM_BLOCK_COLUMNS-1, NUM_BLOCK_ROWS-1);
}


/**
 * Redraw a specific portion of the display. When used with `setBlock()`, you
 * can update only the portion you know you have changed, which will be
 * faster.
 *
 * @param x1 The left edge of the region to update, 0 to 23.
 * @param y1 The top edge of the region to update, 0 to 15.
 * @param x2 The right edge of the region to update, x1 to 24.
 * @param y2 The bottom edge of the region to update y1 to 15.
 */
void GambyBlockMode::update(byte x1, byte y1, byte x2, byte y2) {
  // TODO: This can probably be optimized.

  byte x, y, i;
  unsigned int oddBlock, evenBlock;

  for (y = (y1 & ~1); y <= (y2 | 1); y += 2) {
    setPos(x1 << 2, y >> 1);
    DATA_MODE();
    for (x=x1; x<=x2; x++) {
      oddBlock = pgm_read_word_near(palette + ((offscreen[x][y >> 1] >> 4) & B00001111));
      evenBlock = pgm_read_word_near(palette + (offscreen[x][y >> 1] & B00001111));
      for (i=0; i<4; i++) {
	// Build a two-block set four bits at a time
	//byte combo = (((oddBlock & B00001111) << 4) | (evenBlock & B00001111));
	sendByte(((oddBlock & B00001111) << 4) | (evenBlock & B00001111));
	oddBlock = oddBlock >> 4;
	evenBlock = evenBlock >> 4;
      }
    }
  }
  setPos(0,0);
  currentPage = 0;
  currentColumn = 0;
}


/**
 * Draw a filled rectangle.
 *
 * @param x1 The left edge of the rectangle, 0 to 23.
 * @param y1 The top edge of the rectangle, 0 to 15.
 * @param x2 The right edge of the rectangle, x1 to 24.
 * @param y2 The bottom edge of the rectangle, y1 to 15.
 */
void GambyBlockMode::rect(byte x1, byte y1, byte x2, byte y2, byte block) {
  byte x,y;
  for (y=y1; y<=y2; y++)
    for (x=x1; x<=x2; x++)
      setBlock(x1, y1, block);
  update(x1, y1, x2, y2);
}


/**
 * Draw an unfilled rectangle.
 *
 * @param x1 The left edge of the box, 0 to 23.
 * @param y1 The top edge of the box, 0 to 15.
 * @param x2 The right edge of the box, x1 to 24.
 * @param y2 The bottom edge of the box, y1 to 15.
 */
void GambyBlockMode::box(byte x1, byte y1, byte x2, byte y2, byte block) {
  byte x,y;
  for (x=x1; x<=x2; x++) {
    setBlock(x,y1,block);
    setBlock(x,y2,block);
  }
  for (y=y1+1; y<y2; y++) {
    setBlock(x1,y,block);
    setBlock(x2,y,block);
  }
  update(x1, y1, x2, y2);
}


/****************************************************************************
 * 
 ****************************************************************************/

unsigned int GambyGraphicsMode::drawPattern;
byte GambyGraphicsMode::drawMode = DRAW_NORMAL;

/**
 * Constructor. Runs once when the mode's instance is created, initializing
 * the display.
 *
 */
GambyGraphicsMode::GambyGraphicsMode() {
  init();
  clearScreen();
  drawMode = 0;
}


/**
 * Clears the display buffer. The screen itself doesn't clear until
 * update() is called.
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
 * @param x: The pixel's horizontal position (0-95)
 * @param y: The pixel's vertical position (0-63)
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
 * @param x: The pixel's horizontal position (0-95)
 * @param y: The pixel's vertical position (0-63)
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
  byte i;
  c = c << 3;
  setPos(c, r); // setPos() should already be defined.
  DATA_MODE();
  for (i = 0; i < 8; i++) {
    sendByte(offscreen[c + i][r]);
  }
}


/**
 * Get pixel from a 4x4 pixel pattern (stored as a 16b int) for the given 
 * screen coordinates. The pattern is the drawPattern variable is used.
 *
 * @param x  Horizontal position on screen.
 * @param y  Vertical position on screen.
 * @return   The pixel for the given coordinates in the given pattern 
 *    (`true` or `false`).
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
 * @param sprite  The address of the sprite in `PROGMEM` (e.g. the 
 *     constant's name) 
 * @param x  The sprite's horizontal position
 * @param y  The sprite's vertical position
 */
void GambyGraphicsMode::drawSprite(byte x, byte y, const prog_uchar *sprite) {
  byte w = pgm_read_byte_near(sprite);
  byte h = pgm_read_byte_near(++sprite);

  byte this_byte;
  byte this_bit=8;
  for (byte i=0; i<w; i++) {
    for (byte j=h; j>0; j--) {
      if (this_bit == 8) {
        sprite++;
        this_byte = ~pgm_read_byte_near(sprite);
        this_bit=0;
      }
      setPixel(x+i,y+j,(this_byte << this_bit) & B10000000);
      this_bit++;
    }
  }
}


/**
 * drawSprite (plain version, with frame number): Draw a bitmap graphic.
 * The data should contain more than one frame; additional bitmap data
 * (minus the dimensions) should simply be appended to the first frame.
 *
 * @param x  The sprite's horizontal position
 * @param y  The sprite's vertical position
 * @param sprite  The address of the sprite in `PROGMEM` (e.g. the 
 *     constant's name) 
 * @param frame The frame number to draw (0 to (number of frames)-1)
 */
void GambyGraphicsMode::drawSprite(byte x, byte y, const prog_uchar *sprite, byte frame) {
  unsigned int w = pgm_read_byte_near(sprite);
  unsigned int h = pgm_read_byte_near(++sprite);

  byte this_byte;
  byte this_bit=8;

  // TODO: This can probably be optimized.
  if (frame > 0) {
    unsigned int pixelsPerFrame = h * w;
    // Round up to the nearest 8 bits
    sprite += ((pixelsPerFrame & 0x7 ? ((pixelsPerFrame &  0xfff8) + 8) : pixelsPerFrame) >> 3) * frame;
  }

  for (byte i=0; i<w; i++) {
    for (byte j=h; j>0; j--) {
      if (this_bit == 8) {
        sprite++;
        this_byte = ~pgm_read_byte_near(sprite);
        this_bit=0;
      }
      setPixel(x+i,y+j,(this_byte << this_bit) & B10000000);
      this_bit++;
    }
  }
}



/**
 * Masked version: a second sprite is used as a 'mask' to make portions
 * of the first transparent; the foreground sprite will only be drawn where
 * the mask has white pixels. The mask must be the same dimensions as the
 * foreground sprite. 
 *
 * @param x: The sprite's horizontal position
 * @param y: The sprite's vertical position
 * @param sprite: The address of the foreground sprite in `PROGMEM` 
 *     (e.g. the constant's name) 
 * @param mask: The address of the mask sprite in `PROGMEM` 
 *     (e.g. the constant's name) 
 */
void GambyGraphicsMode::drawSprite(byte x, byte y, const prog_uchar *sprite, const prog_uchar *mask) {
  byte w = pgm_read_byte_near(sprite);
  byte h = pgm_read_byte_near(++sprite);

  mask++;
  byte this_byte, mask_byte;
  byte this_bit=8;
  for (byte i=0; i<w; i++) {
    for (byte j=h; j>0; j--) {
      if (this_bit == 8) {
        this_byte = ~pgm_read_byte_near(++sprite);
        mask_byte = ~pgm_read_byte_near(++mask);
        this_bit=0;
      }
      if (!((mask_byte << this_bit) & B10000000))
        setPixel(x+i,y+j, (this_byte << this_bit) & B10000000);
        this_bit++;
    }
  }
}

/**
 * drawSprite (masked multi-frame version): a second sprite is used as a 
 * 'mask' to make portions of the first transparent; the foreground sprite
 * will only be drawn where the mask has white pixels. The mask must be 
 * the same dimensions as the foreground sprite. The sprite and/or mask
 * should should contain more than one frame; additional bitmap data
 * (minus the dimensions) should simply be appended to the first frame.
 * A single-frame sprite can be used for either; just use frame 0.
 *
 * @param x The sprite's horizontal position
 * @param y The sprite's vertical position
 * @param sprite The address of the foreground sprite in `PROGMEM` 
 *     (e.g. the constant's name) 
 * @param frame The foreground image's frame number to draw 
 *     (0 to (number of frames)-1)
 * @param mask The address of the mask sprite in `PROGMEM` 
 *     (e.g. the constant's name) 
 * @param maskFrame The mask image's frame number to draw 
 *     (0 to (number of frames)-1)
 */
void GambyGraphicsMode::drawSprite(byte x, byte y, const prog_uchar *sprite, byte frame, 
				   const prog_uchar *mask, byte maskFrame) {
  byte w = pgm_read_byte_near(sprite);
  byte h = pgm_read_byte_near(++sprite);

  mask++;
  byte this_byte, mask_byte;
  byte this_bit=8;

  // TODO: This can probably be optimized.
  if (frame > 0 || maskFrame > 0) {
    // Frame size in bits
    unsigned int frameSize = h * w;
    // Convert frame size to bytes, rounding up
    frameSize =  (frameSize & 0x7 ? ((frameSize &  0xfff8) + 8) : frameSize) >> 3;
    sprite += frame * frameSize;
    mask += maskFrame * frameSize;
  }

  for (byte i=0; i<w; i++) {
    for (byte j=h; j>0; j--) {
      if (this_bit == 8) {
        this_byte = ~pgm_read_byte_near(++sprite);
        mask_byte = ~pgm_read_byte_near(++mask);
        this_bit=0;
      }
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
  } else {
    setPixel(cx, cy);
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
 * @param cy: The vertical position of the disc's center
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
  } else {
    setPixel(cx, cy);
  }
}


/**
 * Draw an empty rectangle, using the current drawPattern.
 *
 * @param x1: The horizontal position of the box's first corner
 * @param y1: The vertical position of the box's first corner
 * @param x2: The horizontal position of the box's opposite corner
 * @param y2: The vertical position of the box's opposite corner
 */
void GambyGraphicsMode::box(int x1, int y1, int x2, int y2) {
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
 * @param x1: The horizontal position of the rectangle's first corner
 * @param y1: The vertical position of the rectangle's first corner
 * @param x2: The horizontal position of the rectangle's opposite corner
 * @param y2: The vertical position of the rectangle's opposite corner
 */
void GambyGraphicsMode::rect(int x1, int y1, int x2, int y2) {
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
 * Write a `PROGMEM` string to the display.
 *
 * @param x: The horizontal position at which to draw the text.
 * @param y: The vertical position at which to draw the text.
 * @param s: the address of the string to draw (e.g. the name of
 *    the `PROGMEM` constant).
 */
void GambyGraphicsMode::drawText_P(int x, int y, const char *s) {
  // TODO: Implement this!
}

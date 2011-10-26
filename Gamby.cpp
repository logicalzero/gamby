#include "WProgram.h"
#include "Gamby.h"


// ###########################################################################
//
// ###########################################################################

const int SID =	8;	// Data
const int SCK =	10;	// Clock
const int RS  =	11;	// Register Select (LOW = command, HIGH = data)
const int RES =	12; 	// Reset
const int CS  =	13;    	// Chip select (inverted)


// ###########################################################################
//
// ###########################################################################

/**
 * Initialize the GAMBY LCD.
 */
void GambyBase::init() {
  // Direct port manipulation to set output on pins 8-12.  
//  DDRB = DDRB | B00011111;
  
  pinMode(SID, OUTPUT);
  pinMode(SCK, OUTPUT);
  pinMode(RS,  OUTPUT);
  pinMode(RES, OUTPUT);
  pinMode(CS,  OUTPUT);

  digitalWrite(RES, LOW);
  digitalWrite(CS, LOW);
  digitalWrite(RES, HIGH);
  digitalWrite(RS, HIGH);
  digitalWrite(RS, LOW);
  
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
  
  //  clearDisplay();
  sendCommand(DISPLAY_POWER | ON);
  
  //  clearDisplay();

  digitalWrite(RS, DATA);
  digitalWrite(CS, LOW);
}


/**
 *
 */
void GambyBase::clockOut(byte data) {
  
  byte i;
  digitalWrite(SID, HIGH);
  digitalWrite(SCK, HIGH);
    
  for (i=0; i<8; i++) {
    digitalWrite(SID, (data & B10000000));	// clock out MSBit of data
    digitalWrite(SCK, LOW);
    digitalWrite(SCK, HIGH);
    data = data << 0x01;
  }

  // Clock and data pins idle high
  digitalWrite(SID, HIGH);
}


/**
 *
 */
void GambyBase::clockOutBit(boolean b) {
  digitalWrite(SID, HIGH);  // necessary?
  digitalWrite(SCK, HIGH);
  digitalWrite(SID, b);
  digitalWrite(SCK, LOW);
  digitalWrite(SCK, HIGH);
    
  // Clock and data pins idle high
  digitalWrite(SID, HIGH);
}


/**
 *
 */
void GambyBase::sendCommand (byte command) {
  digitalWrite(RS, COMMAND);
  digitalWrite(CS, LOW);
  clockOut(command); // shiftOut(SID, SCK, MSBFIRST, command);
  digitalWrite(CS, HIGH);
}


/**
 *
 */
void GambyBase::sendCommand(byte b1, byte b2) {
  digitalWrite(RS, COMMAND);
  digitalWrite(CS, LOW);
  clockOut(b1); //  shiftOut(SID, SCK, MSBFIRST, b1);
  clockOut(b2); //  shiftOut(SID, SCK, MSBFIRST, b2);
  digitalWrite(CS, HIGH);
}


/**
 *
 */
void GambyBase::clearDisplay () {
  byte i,j;
  for (i=0; i < NUM_PAGES; i++) {
    sendCommand(SET_PAGE_ADDR | i);
    sendCommand(SET_COLUMN_ADDR_1, SET_COLUMN_ADDR_2);
    digitalWrite(RS, DATA);
    digitalWrite(CS, LOW);
    for (j = 0; j < NUM_COLUMNS; j++) {
      clockOut(0);
    }
    digitalWrite(CS, HIGH);
  }
}

// ###########################################################################
//
// ###########################################################################

/**
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
 *
 */
void GambyTextMode::scroll(int s) {
  offset += s;
  if (offset >= NUM_PAGES)
    offset -= NUM_PAGES;
  else if (offset < 0)
    offset += NUM_PAGES;

  sendCommand(SET_INITIAL_COM0_1, (byte)offset << 3);  // shift 3 places to multiply by 8.
}


/**
 *
 */
void GambyTextMode::setColumn (byte column) {
  sendCommand(SET_COLUMN_ADDR_1 + (column >> 4), B00001111 & column); // & to mask out high bits
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
  sendCommand(SET_COLUMN_ADDR_1 + ((currentColumn >> 4) & B00000111), SET_COLUMN_ADDR_2 | (currentColumn & B00001111)); 
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
  digitalWrite(CS, LOW);
  digitalWrite(RS, DATA);
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
  digitalWrite(CS, HIGH);

  currentColumn += w + 1;
}


/**
 * drawText(): Write a sring to the display.
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
 * clearLine(): Clears the current line right of the current column.
 *
 */
void GambyTextMode::clearLine () {
  byte j;
  digitalWrite(RS, DATA);
  digitalWrite(CS, LOW);
  for (j = currentColumn; j <= LAST_COL; j++) {
    clockOut(0);
  }
  // restore previous column.
  sendCommand(SET_COLUMN_ADDR_1 + ((currentColumn >> 4) & B00000111), B00001111 & currentColumn); // & to mask out high bits

  //sendCommand(SET_COLUMN_ADDR_1, SET_COLUMN_ADDR_2 | currentColumn);
  digitalWrite(CS, HIGH);
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


// ###########################################################################
//
// ###########################################################################

/**
 *
 */
GambyBlockMode::GambyBlockMode() {
}


// ###########################################################################
//
// ###########################################################################

/**
 *
 */
GambyGraphicsMode::GambyGraphicsMode() {
  drawMode = 0;
}


/**
 *
 */
void GambyGraphicsMode::setPos(byte col, byte line) {
  sendCommand(SET_PAGE_ADDR | line);
  sendCommand(SET_COLUMN_ADDR_1 + ((col >> 4) & B00000111), SET_COLUMN_ADDR_2 | (col & B00001111)); 
}


/**
 *
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
 *
 */
void GambyGraphicsMode::setPixel(byte x, byte y, int pat) {
  byte c = x >> 3;            // column of dirtyBits to mark changed (x/8)
  byte r = y >> 3;            // row (page) of display, row of dirtyBits (y/8)
  byte b = y & B00000111;     // Bit number (within page) to change; low three bits
  byte oldOffscreen = offscreen[x][r]; // keep old offscreen to avoid unnecessary update

  boolean p = getPatternPixel(x, y, pat);
  
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
  digitalWrite(CS, LOW);
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
  digitalWrite(RS, DATA);
  digitalWrite(CS, LOW);
  for (i = 0; i < 8; i++) {
    // should already be in 'data' mode
    clockOut(offscreen[x + i][r]);

  }
}


/**
 * getPatternPixel():  Get pixel from a 4x4 grid (a 16b int) for the given
 * screen coordinates.
 *
 * @param x: Horizontal position on screen.
 * @param y: Vertical position on screen.
 * @param pattern: A 16b `int`, interpreted as a 4-by-4 bitmap.
 * @return: The pixel for the given coordinates in the given pattern (0 or 1).
 */
boolean GambyGraphicsMode::getPatternPixel (byte x, byte y, int pattern) {
  int i;
  // Don't do all the pattern-lookup work for solids
  if (pattern == PATTERN_BLACK)
    return false; // i.e. 0
  if (pattern == PATTERN_WHITE)
    return true; // i.e. 1
  // coordinates in pattern taken from least two bits of screen coords
  i = (x & B00000011) | ((y & B00000011) << 2);
  // only final bit determines pixel
  return (((pattern >> i) & 1) == 1);
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

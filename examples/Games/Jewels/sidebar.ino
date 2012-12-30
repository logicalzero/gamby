// Logo is 32x24 pixels
// we will draw it as three 32x8 icon rows

// [ - - - - - - ]  [ - - - - - - ]  [ - - - - - - ]  [ - - - - - - ]
// X X X X X X X X  X X X X X X X X  X X X X X X X X  X X X X X X X X
// X X X X X X X X  X X X X X X X X  X X X X X X X X  X X X . X X . X
// X X X X X X X X  X X X X X X X X  X X X X X X X .  X . X X . X X X
// X X X X X X X X  X X X X X X X X  X X X . X . X X  X X X X X . X . 
// X X X X X X X X  X X X X X X X .  X . X X X X X .  X . X . X X . X
// X X X X X X X X  X X X X . X X X  X X X . X . X X  . X . X . X . X
// X X . . . X X X  X X . X X . X .  X . X X . X X .  . . X . X . X .
// X . X X X . . X  . X X X X X X X  . X X . X X . X  X . . X . X . .

// X X . X . X X X  X X . X . X . X  X . . X . . X .  X . X . . . X . 
// X . . X . X . .  . . . . . . . .  . . . . . . . .  X . . . . . . .
// X X . X . X . X  X . X X X . X .  X X . . X X . .  X . . X X X . . 
// . X . X . . X .  . X . X . . X .  . X . X . . X .  X . X . . . . . 
// X X . X . X X X  X . . X . X X .  X . X X X X . .  X . . X X . . .
// . . . X . X . .  . . . . X X . X  X . X . . . . .  X . . . . X . . 
// X X . X . X . .  . X . . X . . X  . . X . . . X .  X . X . . X . .
// X . . X . . X X  X . X . X . . X  . . . X X X . X  X X . X X . . .

// . . X . . X . .  . . . . . . . .  . X . . . . . .  . . . . . . . . 
// . X . . . . X .  X . . X . . . .  . . . . . . . .  . . . X . . . . 
// . . . . . . . .  . . . . . X . .  . . . . . . . .  . . . X . . . . 
// . . . X . X . .  . . . . . . . .  . . . . . . . .  . . X X X . . . 
// . . . . . . . .  . X . . . . . .  . . . . . . . .  X X X . X X X . 
// X . . . X . . .  . . . . . . . .  . . . . . . . .  . . X X X . . . 
// . . . . . . . .  . . . . . . . .  . . . . . . . .  . . . X . . . . 
// . . . . . . . .  . . . . . . . .  . . . . . . . .  . . . X . . . . 

PROGMEM const unsigned char sidebar_top[65] = {32,
  B11111111, B01111111, B10111111, B10111111, B10111111, B01111111, B01111111, B11111111,
  B01111111, B11111111, B10111111, B11111111, B11011111, B10111111, B11111111, B10101111,
  B01111111, B10101111, B11111111, B01010111, B10111111, B11010111, B01111111, B10101011,
  B10011111, B00101011, B01011111, B10101101, B01011011, B10110111, B01001101, B00110111
};

PROGMEM const unsigned char sidebar_mid[65] = {32,
  B11010111, B01011101, B00000000, B11111111, B00000000, B01110111, B10011001, B10010101,
  B10010101, B01001001, B10000100, B00011101, B11100100, B00110001, B00011100, B11100001,
  B00110101, B00001100, B01110000, B10011001, B10010100, B10010100, B01001001, B10000000,
  B11111111, B10000000, B01001001, B10010100, B10010100, B01100100, B00000001, B00000000
};

PROGMEM const unsigned char sidebar_bot[65] = {32,
  B00100000, B00000010, B00000001, B00001000, B00100000, B00001001, B00000010, B00000000,
  B00000010, B00010000, B00000000, B00000010, B00000000, B00000100, B00000000, B00000000,
  B00000000, B00000001, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00010000, B00010000, B00111000, B11101110, B00111000, B00010000, B00010000, B00000000
};

void draw_sidebar()
{
  gamby.setPos(64,0);
  gamby.drawIcon(sidebar_top);
  gamby.setPos(64,1);
  gamby.drawIcon(sidebar_mid);
  gamby.setPos(64,2);
  gamby.drawIcon(sidebar_bot);
  draw_score();
}

PROGMEM const unsigned char digiticons[10][7] = {
  // zero
  // . X X X X . 
  // X X . . X X 
  // X X . X X X 
  // X X X . X X
  // X X . . X X 
  // X X . . X X
  // . X X X X .
  // . . . . . .
  {6, B00111110, B01111111, B01001001, B01000101, B01111111, B00111110},
  // one
  // . . X X . .
  // . . X X . . 
  // . X X X . .
  // . . X X . . 
  // . . X X . .
  // . . X X . .
  // X X X X X X
  // . . . . . . 
  {6, B01000000, B01000100, B01111111, B01111111, B01000000, B01000000},
  // TWO
  // . X X X X . 
  // X X . . X X
  // . . . . X X 
  // . . . . X X 
  // . . . X X .
  // . X X . . .
  // X X . . . . 
  // X X X X X X 
  // . . . . . .
  {6, B01100010, B01110011, B01010001, B01001001, B01001111, B01000110},
  // THREE
  // . X X X X .
  // X X . . X X 
  // . . . . X X 
  // . . X X X .
  // . . . . X X 
  // X X . . X X 
  // . X X X X . 
  // . . . . . .
  {6, B00100010, B01100011, B01001001, B01001001, B01111111, B00110110},
  // FOUR
  // . . . X X .
  // . . X X X .
  // . X . X X .
  // X . . X X .
  // X X X X X X
  // . . . X X .
  // . . . X X .
  // . . . . . .
  {6, B00011000, B00010100, B00010010, B01111111, B01111111, B00010000},
  // FIVE
  // X X X X X X
  // X X . . . .
  // X X X X X . 
  // . . . . X X 
  // . . . . X X 
  // X X . . X X 
  // . X X X X . 
  // . . . . . .
  {6, B0100111, B01100111, B01000101, B01000101, B01111101, B00111001},
  // SIX
  // . X X X X .
  // X X . . X X 
  // X X . . . . 
  // X X X X X . 
  // X X . . X X 
  // X X . . X X 
  // . X X X X . 
  // . . . . . . 
  {6, B00111110, B01111111, B01001001, B01001001, B01111011, B00110010},
  // SEVEN
  // X X X X X X 
  // X X . . X X 
  // . . . X X . 
  // . . X X . . 
  // . . X X . . 
  // . . X X . . 
  // . . X X . . 
  // . . . . . .
  {6, B00000011, B00000011, B01111001, B01111101, B00000111, B00000011},
  // EIGHT
  // . X X X X . 
  // X X . . X X 
  // X X . . X X 
  // . X X X X . 
  // X X . . X X 
  // X X . . X X
  // . X X X X .
  // . . . . . .
  {6, B00110110, B01111111, B01001001, B01001001, B01111111, B00110110},
  // NINE
  // . X X X X . 
  // X X . . X X 
  // X X . . X X 
  // . X X X X X 
  // . . . . X X 
  // X X . . X X 
  // . X X X X .
  // . . . . . .
  {6, B00100110, B01101111, B01001001, B01001001, B01111111, B00111110}
};


void draw_score()
{
  byte startPos = 94;
  // clear the line - it would be nice to use gamby.clearLine(), but that is only
  // implemented in text mode
  gamby.setPos(64, 5);
  DATA_MODE();
  for (unsigned i = 0; i < (94 - 64); i++) {
    gamby.sendByte(0);
  }
 
  unsigned local_score = score;
  do {
    unsigned digit = local_score % 10;
    const unsigned char *icon = digiticons[digit];
    byte char_width = pgm_read_byte_near(icon);
    startPos -= char_width;
    gamby.setPos(startPos, 5);
    gamby.drawIcon(icon);
    local_score = local_score / 10;
    startPos -= 1;
  } while (local_score > 0);
}

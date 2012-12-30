// Jewel icons as per Twain
// There are seven types of jewels in the game.
// we represent them as 8x8-pixel icons
// this allows us to present a game field of 8x8 icons, with 32 px on the side

PROGMEM const unsigned char jewels[8][9] = { 
// hexagon (orange)
// . . . X . . . .
// . . X . X . . .
// X X . . . X X .
// X . X X X . X .
// X X . . . X X .
// . . X . X . . .
// . . . X . . . .
// . . . . . . . . 
  {8, B00011100, B00010100, B00101010, B01001001, B00101010, B00010100, B00011100, B00000000},

// pentagon (blue)
// . X X X X X . .
// X X . . . X X .
// X . X X X . X .
// . X . . . X . .
// . X X . X X . .
// . . X . X . . .
// . . . X . . . .
// . . . . . . . .
  {8, B00000110, B00011011, B00110101, B01000101, B00110101, B00011011, B00000110, B00000000},

// circle (white)
// . . . X X . . .
// . X X . . X X .
// . X . . . . X .
// X . . . . X . X
// X . . . . X . X
// . X . X X . X .
// . X X . . X X .
// . . . X X . . .
  {8, B00011000, B01100110, B01000010, B10100001, B10100001, B01011010, B01100110, B00011000},

// triangle (purple)
// . . . X . . . .
// . . X . X . . .
// . . X . X . . .
// . X X . X X . .
// . X . X . X . .
// X . . X . . X .
// X X X X X X X .
// . . . . . . . .
  {8, B01100000, B01011000, B01001110, B01110001, B01001110, B01011000, B01100000, B00000000},

// diamond (yellow)
// . . . X . . . .
// . . X . X . . .
// . X . X . X . .
// X . X . . X X .
// . X . . X X . .
// . . X X X . . .
// . . . X . . . .
// . . . . . . . .
  {8, B00001000, B00010100, B00101010, B01100101, B00110010, B00011100, B00001000, B00000000},

// irregular octagon (red)
// . X X X X X . .
// X X . X . X X .
// X . . X . . X .
// X X X X X X X .
// X . . X . . X .
// X X . X . X X .
// . X X X X X . .
// . . . . . . . . 
  {8, B00111110, B01101011, B01001001, B01111111, B01001001, B01101011, B00111110, B00000000},


// regular octagon (green)
// . . X X X . . .
// . X X . X X . .
// X X X X X X X .
// X . X . X . X .
// X X X X X X X .
// . X X . X X . .
// . . X X X . . .
// . . . . . . . .
  {8, B00011100, B00110110, B01111111, B01010101, B01111111, B00110110, B00011100, B00000000},


// blank/empty
// . . . . . . . .
// . . . . . . . .
// . . . . . . . .
// . . . . . . . .
// . . . . . . . .
// . . . . . . . .
// . . . . . . . .
// . . . . . . . .
  {8, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000},
};

const unsigned char* get_jewel(unsigned id)
{
  return jewels[id];
}

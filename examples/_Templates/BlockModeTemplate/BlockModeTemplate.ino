/*
GAMBY BlockMode Template
~~~~~~~~~~~~~~~~~~~~~~~

A jump start on using Gamby to draw a 24x16 grid of 4x4 pixel 'blocks.' This 
skeletal sketch contains the bare minimum required to get Gamby running in 
BlockMode. Do a "Save As" to make a copy and fill it out with your own code!

by David R. Stokes (gamby@logicalzero.com) 2012-08-28
*/

#include <Gamby.h>
#include <avr/pgmspace.h>

// A sample palette of blocks, stored in program memory. You can replace one 
// or all of the individual blocks, but a palette can have no more than 16 
// blocks in it. Typically, you will have only one per sketch, but it is
// possible to create more than one and switch between them.
PROGMEM prog_uint16_t palette[] = {
    0x0000, //  0 All 'white' (block 0 should almost always be 0x0000) 
    0xffff, //  1 All 'black'
    0x5a5a, //  2 50% gray dither
    0xfaf5, //  3 75% gray dither
    0x050a, //  4 25% gray dither
    0xedb7, //  5 Light diagonal left line
    0x1248, //  6 Dark diagonal left line
    0x7bde, //  7 Light diagonal right line
    0x8421, //  8 Dark diagonal right line
    0x888f, //  9 Dark-on-light solid grid lines
    0x1110, // 10 Light-on-dark solid grid lines
    0xa080, // 11 Light-on-dark dotted grid lines
    0x5f1f, // 12 Dark-on-light dotted grid lines
    0x33cc, // 13 Checker pattern 
    0xcc33, // 14 Mirrored checker pattern
    0x0001  // 15 Single pixel (upper right)
};


// Each of Gamby's 'modes' are wrapped in a class, a self-contained unit
// that bundles together all the required functionality. To use a mode,
// you must first create an 'instance' of its class -- a sort of working 
// copy. Your sketch should only use one.
GambyBlockMode gamby;

void setup() {
  // Set the block palette. You generally need to do this only once, usually
  // just after initializing Gamby. You could, however, do this elsewhere in 
  // your sketch -- for example, if you had a role-playing game and had an 
  // 'indoor' and an 'outdoor' palette.
  gamby.palette = blocks;

  // That's all that's required! From here on, it's up to you. All of Gamby's
  // functionality is contained in the 'gamby' object, which you use like
  // this:
  gamby.drawBlock(0,0,1);
  gamby.drawBlock(2,0,1);
  gamby.drawBlock(1,1,1);
  gamby.drawBlock(0,2,1);
  gamby.drawBlock(2,2,1);
}

void loop () {
  // Your code here!
}


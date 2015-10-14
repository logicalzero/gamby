/*
Pattern Fill Demo
~~~~~~~~~~~~~~~~~

This is a simple demonstration of the use of fill patterns in
GambyGraphicsMode. It simply draws random circles using different patterns.

by David R. Stokes (gamby@logicalzero.com) 2012-08-31
*/


#include <Gamby.h>

// Create an array of patterns, using those pre-defined by the GAMBY library.
// Using an array allows the sketch to pick a pattern by number.
int patterns[] = {
  PATTERN_WHITE,
  PATTERN_BLACK,
  PATTERN_GRAY,
  PATTERN_DK_GRAY,
  PATTERN_LT_GRAY,
  PATTERN_DK_L_DIAGONAL,
  PATTERN_LT_L_DIAGONAL,
  PATTERN_DK_R_DIAGONAL,
  PATTERN_LT_R_DIAGONAL,
  PATTERN_DK_GRID_SOLID,
  PATTERN_LT_GRID_SOLID,
  PATTERN_DK_GRID_DOTS,
  PATTERN_LT_GRID_DOTS,
  PATTERN_CHECKER,
  PATTERN_CHECKER_INV
};

// Each of Gamby's 'modes' are wrapped in a class, a self-contained unit
// that bundles together all the required functionality. To use a mode,
// you must first create an 'instance' of its class -- a sort of working
// copy. Your sketch should only use one.
GambyGraphicsMode gamby;


void setup () {
  // Nothing needs to be done in setup(). The screen is cleared when the
  // GambyGraphicsMode object 'gamby' is created.
}


void loop () {
  // Choose a random pattern from the array of patterns we created
  int p = patterns[random(16)];

  // Choose a random position and radius for the circle to draw
  int x = random(0,NUM_COLUMNS);
  int y = random(0,NUM_ROWS);
  int r = random(8,20);

  // Set the pattern to use when drawing
  gamby.drawPattern = p;

  // Draw a disc (filled circle). It will use the pattern set in the previous
  // line. The drawing is done on the offscreen buffer (a 'scratchpad' where
  // the image is stored before it's displayed) and not immediately drawn on
  // the LCD.
  gamby.disc(x,y,r);

  // Set the drawPattern to solid black and draw a solid outline (unfilled)
  // for the circle. Again, this is done to the offscreen buffer and won't
  // show up until update() is called.
  gamby.drawPattern = PATTERN_BLACK;
  gamby.circle(x,y,r);

  // The circle and its outline has been drawn to the offscreen buffer and
  // won't get displayed until update() is called. This way, the circle and
  // its outline get drawn at the same time, instead of the disc and then the
  // circle outline.
  gamby.update();
}

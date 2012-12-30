/*
GAMBY TextMode Template
~~~~~~~~~~~~~~~~~~~~~~~

A jump start on using Gamby to display text. This skeletal sketch contains
the bare minimum required to get Gamby running in TextMode. Do a "Save As"
to make a copy and fill it out with your own code!

by David R. Stokes (gamby@logicalzero.com) 2012-07-06
*/

#include <Gamby.h>

// Bring in the font from the 'font' tab (font.ino)
extern const int32_t font[];

// Each of Gamby's 'modes' are wrapped in a class, a self-contained unit
// that bundles together all the required functionality. To use a mode,
// you must first create an 'instance' of its class -- a sort of working 
// copy. Your sketch should only use one.
GambyTextMode gamby;

void setup () {
  // Set the font. You generally need to do this only once, usually just after
  // initializing Gamby. You could, however, do this elsewhere in your 
  // sketch -- for example, if you wanted to change fonts on the fly.
  gamby.font = font;

  // That's all that's required! From here on, it's up to you. All of Gamby's
  // functionality is contained in the 'gamby' object, which you use like
  // this:
  gamby.println("Hello, GAMBY!");
}

void loop () {
  // Your code here!
}



/*
Retroboto Font
~~~~~~~~~~~~~~~~~~~~~~~

A retro-techy font. Copy the contents of the 'font' tab (font.ino)
to your own sketch, or just do a 'Save As' to start fresh!

by David R. Stokes (gamby@logicalzero.com) 2013-05-09
*/

#include <Gamby.h>

// Bring in the font from the 'retroboto_font_data' tab (retroboto_font_data.ino)
extern prog_int32_t font[];

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
  gamby.println("DOMO ARIGATO,");
  gamby.println("RETROBOTO!");
  gamby.println("0123456789?!*");
}

void loop () {
  // Your code here!
}



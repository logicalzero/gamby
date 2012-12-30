/*
Animated Sprite Demo
~~~~~~~~~~~~~~~~~~~~

A simple demonstration of multi-frame sprites. A pair of bouncing balls.
Each ball has four frames of animation. 

Controls:
  D-Pad Up/Down: Increase/decrease movement speed
  D-Pad Right/Left: Increase/decrease animation speed
  Any button: Pause the movement (while pressed)

by David R. Stokes (gamby@logicalzero.com) 2012-08-28
*/

#include <Gamby.h>
#include <avr/pgmspace.h>

// Two spinning balls, each with 4 frames of animation.
// The first two numbers are the dimensions, the rest is the image data.
PROGMEM const unsigned char ball1[] = {16, 16, 
  // Frame 0
    7, 224,  31, 248,  53, 252, 127, 158, 127, 106, 255,  21, 255, 195, 255,   9, 
  216, 255, 162, 255, 216, 255, 100, 254, 120, 242,  63,   4,  31,  24,   7, 224,
  // Frame 1
    7, 224,  31, 248,  53, 252,  94, 254, 117, 254, 223,  87, 255,   5, 255,  81, 
  255,   5, 255,   1, 223,   7, 116, 254,  72, 254,  50, 252,  24, 248,   7, 224,
  // Frame 2
    7, 224,  31, 184,  63, 220, 117, 246, 126, 254, 209, 255, 238, 255, 176, 255,
  255,   5, 255,   1, 255,   1, 127,   2, 111,  14,  48, 252,  26, 248,   7, 224,
  // Frame 3
    7, 224,  31, 248,  63,  44, 127, 214, 127,  42, 250, 253, 213, 255, 232, 255,
  210, 255, 172, 255, 240, 249, 127,   2, 127,   2,  63,   4,  31,  24,   7, 224
};

PROGMEM const unsigned char ball2[] = {16, 16, // width and height
  // Frame 0
    7, 224,  31, 248,  63, 252, 127, 126, 127, 214, 253,  43, 250, 213, 255,   3, 
  213, 171, 248,  85, 214, 171, 104,  86, 122, 162,  53,   4,  26,  24,   7, 224,
  // Frame 1
    7, 224,  31, 248,  63, 252, 127, 254, 122, 190, 255, 215, 255,  37, 245,  83, 
  254,   5, 245,   1, 250,   3, 100,  86, 122, 170,  52,  84,  26, 184,   7, 224,
  // Frame 2
    7, 224,  31, 248,  63, 252, 126, 222, 127, 254, 245,  87, 253, 175, 234,  85, 
  254, 131, 245,   9, 250,   1, 125,   2, 106,  10,  58,  84,  28, 184,   7, 224,
  // Frame 3
    7, 224,  31, 248,  63, 252, 127,  94, 127, 246, 253,  91, 246, 175, 221,  85, 
  242, 171, 236,  85, 242, 169, 125,   2, 122,   2,  61,   4,  26, 152,   7, 224
};

// A 'mask' used by both balls, enabling them to be drawn without filling
// out the entire square, which would cause one ball to clip the other if
// they got too close. It is only a single frame; the shape of the ball does
// not change as it animates. 
PROGMEM const unsigned char ballMask[] = {16, 16, 
    7, 224,  31, 248,  63, 252, 127, 254, 127, 254, 255, 255, 255, 255, 255, 255, 
  255, 255, 255, 255, 255, 255, 127, 254, 127, 254,  63, 252,  31, 248,   7, 224
};

// An 'eraser' sprite, which is all white. It is also only a single frame.
PROGMEM const unsigned char ballEraser[] = {16, 16, 
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};


const byte readInputTime = 50;  // The minimum number of milliseconds between input reads
const byte lastFrame = 3;       // The last frame number

byte frameTime = 75;  // The number of milliseconds between the balls' frames of animation
byte moveTime = 80;   // The number of milliseconds between the balls' movements

byte frame;             // The current frame number, used by both balls
byte ball1_x, ball1_y;  // The first ball's position
byte ball2_x, ball2_y;  // The second ball's position
int ball1_dx, ball1_dy; // The first ball's motion, added to position on each move
int ball2_dx, ball2_dy; // The second ball's motion, added to position on each move

unsigned long nextFrame; // The time at which to change to the next frame of animation
unsigned long nextMove;  // The time at which to next move the balls
unsigned long nextRead;  // The time at which to next read the inputs

GambyGraphicsMode gamby;

void setup() {
  ball1_dx = 2;
  ball1_dy = 3;
  ball2_dx = 2;
  ball2_dy = 4;

  ball1_x = random(2,LAST_COLUMN-18);
  ball1_y = random(2,LAST_ROW-18);
  ball2_x = random(2,LAST_COLUMN-18);
  ball2_y = random(2,LAST_ROW-18);
}

void loop() {
  unsigned long now = millis();
  boolean changed = false;

  // If enough time has passed and none of the buttons are pressed,
  // move the balls.
  if (now > nextMove && !(gamby.inputs & BUTTON_ANY)) {
    // Erase the balls in their old position.
    gamby.drawSprite(ball1_x, ball1_y, ballEraser, ballMask);
    gamby.drawSprite(ball2_x, ball2_y, ballEraser, ballMask);

    // Move each ball
    ball1_x += ball1_dx;
    ball1_y += ball1_dy;
    ball2_x += ball2_dx;
    ball2_y += ball2_dy;

    // If a ball hits any edge of the screen, reverse its direction
    // Also make a 'bump' noise.
    if (ball1_x >= LAST_COLUMN-16 || ball1_x < 1) {
      ball1_dx = -ball1_dx;
      tone(9,50,50);
    }
    if (ball1_y >= LAST_ROW-16 || ball1_y < 1) {
      ball1_dy = -ball1_dy;
      tone(9,100,50);
    }
    if (ball2_x >= LAST_COLUMN-16 || ball2_x < 1) {
      ball2_dx = -ball2_dx;
      tone(9,60,50);
    }
    if (ball2_y >= LAST_ROW-16 || ball2_y < 1) {
      ball2_dy = -ball2_dy;
      tone(9,160,50);
    }

    // Set the time for the next move
    nextMove = now + moveTime;
    
    changed = true;
  }

  // If enough time has passed, advance to the next frame of the animation.
  if (now > nextFrame) {
    frame++;
    // Loop back to 0 if the current frame number exceeds the number of frames. 
    if (frame > lastFrame)
      frame = 0;
      
    // Set the time for the next frame change
    nextFrame = now + frameTime;
    changed = true;
  }

//  changed = true;

  // Redraw the balls if they changed position or frame.
  // Even though update() only redraws the changed portions of the screen, drawing the
  // same thing over itself
  if (changed) {
    gamby.drawSprite(ball1_x, ball1_y, ball1, frame, ballMask,0);
    gamby.drawSprite(ball2_x, ball2_y, ball2, 3-frame, ballMask,0);
    gamby.update();
  }
  
  // If enough time has passed since the last time the inputs were read,
  // read them again.
  if (now > nextRead) {
    gamby.readInputs();
    if (gamby.inputs & DPAD_UP)
      moveTime--;
    else if (gamby.inputs & DPAD_DOWN)
      moveTime++;
    if (gamby.inputs & DPAD_RIGHT)
      frameTime--;
    else if (gamby.inputs & DPAD_LEFT)
      frameTime++;
      
    nextRead = now + readInputTime;
  }
}

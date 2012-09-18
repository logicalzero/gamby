#include <avr/pgmspace.h>

// The introductory 'splash screen' with the game's title.
// It is stored as 8-pixel-high 'icons' the full width of the LCD with a separate 'frame' for
// each piece of the larger image.
PROGMEM prog_uchar splashscreen[] = {96, 
  // Row 0 ("frame" 0)
  84, 0, 202, 228, 224, 50, 48, 114, 96, 226, 192, 0, 12, 158, 158, 12, 192, 224, 224, 0, 128, 224, 
  240, 240, 240, 0, 8, 144, 196, 232, 226, 196, 0, 132, 206, 206, 6, 0, 53, 48, 186, 153, 248, 241, 
  224, 2, 212, 192, 10, 148, 192, 218, 192, 10, 0, 10, 0, 2, 192, 224, 224, 48, 48, 112, 96, 224, 
  192, 0, 0, 96, 96, 112, 48, 240, 224, 200, 0, 248, 250, 240, 6, 192, 230, 240, 50, 132, 129, 202,
  192, 197, 128, 133, 16, 2, 84, 160,
  // Row 1 ("frame" 1)
  0, 0, 193, 227, 135, 135, 142, 206, 252, 252, 56, 0, 255, 255, 255, 0, 127, 255, 255, 126, 15, 3, 
  255, 255, 127, 62, 15, 3, 129, 255, 255, 127, 0, 127, 127, 127, 0, 60, 127, 127, 103, 227, 225, 
  255, 255, 0, 127, 127, 62, 15, 3, 63, 255, 254, 0, 64, 20, 8, 193, 227, 135, 135, 142, 206, 252, 
  252, 56, 0, 120, 254, 254, 207, 199, 195, 255, 255, 0, 3, 15, 255, 255, 255, 7, 0, 7, 143, 31, 
  28, 56, 57, 241, 243, 227, 0, 0, 130,
  // Row 2 ("frame" 2)
  84, 0, 42, 16, 1, 1, 1, 1, 128, 0, 160, 0, 161, 89, 0, 44, 16, 2, 4, 0, 0, 0, 0, 0, 0, 128, 0, 128, 
  99, 19, 67, 40, 16, 6, 16, 10, 5, 192, 242, 248, 248, 124, 60, 62, 126, 254, 254, 254, 254, 254, 
  254, 126, 62, 60, 124, 248, 248, 240, 192, 0, 33, 89, 1, 41, 20, 0, 2, 0, 0, 0, 0, 0, 1, 129, 15, 
  157, 88, 28, 79, 7, 83, 0, 24, 0, 11, 3, 6, 6, 6, 7, 3, 3, 128, 0, 160, 0,
  // Row 3 ("frame" 3)
  1, 0, 0, 0, 160, 0, 168, 66, 24, 70, 16, 5, 0, 0, 0, 0, 0, 0, 128, 16, 96, 8, 16, 6, 0, 2, 1, 0, 0, 
  0, 0, 248, 252, 158, 14, 6, 7, 255, 255, 7, 128, 248, 252, 236, 232, 0, 135, 255, 255, 135, 0, 248, 
  252, 236, 232, 128, 7, 255, 255, 7, 6, 14, 158, 252, 248, 0, 0, 144, 96, 8, 16, 6, 0, 2, 1, 0, 0, 
  0, 0, 128, 64, 0, 160, 80, 0, 40, 20, 0, 10, 4, 1, 4, 1, 132, 1, 128,
  // Row 4 ("frame" 4)
  0, 0, 128, 40, 66, 40, 147, 4, 1, 0, 0, 0, 0, 0, 0, 128, 40, 210, 9, 36, 1, 0, 128, 0, 160, 0, 104,
  0, 86, 0, 10, 1, 3, 251, 255, 158, 14, 7, 3, 2, 1, 225, 129, 3, 7, 15, 1, 4, 0, 1, 15, 7, 3, 1, 1,
  1, 2, 3, 7, 14, 158, 255, 251, 3, 129, 40, 82, 5, 0, 0, 128, 0, 160, 0, 104, 0, 172, 17, 2, 4, 1,
  0, 128, 64, 0, 160, 16, 64, 40, 84, 8, 34, 20, 2, 0, 0,
  // Row 5 ("frame" 5)
  0, 42, 0, 10, 5, 0, 0, 128, 0, 160, 16, 128, 0, 160, 8, 86, 32, 138, 85, 0, 168, 2, 52, 74, 21, 64,
  21, 0, 85, 0, 42, 0, 90, 0, 171, 7, 78, 28, 152, 56, 176, 48, 113, 99, 98, 98, 98, 98, 98, 98, 97,
  96, 96, 112, 48, 48, 56, 152, 28, 14, 135, 3, 160, 84, 9, 162, 85, 0, 168, 2, 52, 10, 85, 0, 85, 0,
  42, 20, 0, 170, 17, 140, 34, 84, 3, 84, 1, 164, 17, 192, 40, 128, 104, 0, 168, 0,
  // Row 6 ("frame" 6)
  0, 4, 1, 0, 2, 0, 2, 0, 2, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 5, 
  0, 2, 0, 5, 0, 5, 0, 2, 0, 5, 0, 2, 0, 0, 0, 0, 4, 0, 2, 0, 0, 2, 1, 4, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 4, 0, 2, 0, 5, 0, 5, 0, 5, 0, 2, 1, 0, 2, 1, 0, 0, 2
};


// The "Game Over" screen. It's only 16 pixels high and displayed in the middle of the LCD.
// Each 8 pixel stripe is a different 'frame.'
PROGMEM prog_uchar gameover[] = {
  96, 
  0, 0, 120, 252, 206, 134, 195, 247, 255, 127, 0, 0, 204, 204, 238, 230, 230, 126, 252, 248, 0, 248, 252, 252, 192, 240, 124, 254, 254, 254, 192, 224, 112, 56, 252, 252, 248, 0, 248, 252, 254, 14, 134, 134, 252, 248, 112, 0, 0, 0, 0, 0, 128, 192, 240, 48, 48, 48, 240, 224, 156, 124, 252, 240, 192, 0, 128, 240, 254, 127, 15, 194, 224, 240, 48, 48, 48, 224, 192, 128, 30, 254, 252, 248, 124, 30, 14, 6, 12, 12, 128, 254, 255, 15, 0, 0,
  0, 0, 24, 57, 113, 97, 97, 127, 127, 60, 0, 15, 31, 31, 25, 56, 120, 120, 127, 63, 0, 15, 31, 31, 15, 1, 0, 31, 31, 15, 7, 1, 0, 112, 127, 127, 15, 0, 3, 15, 31, 63, 51, 49, 49, 56, 24, 0, 0, 0, 0, 0, 15, 31, 63, 48, 48, 56, 63, 31, 7, 0, 1, 7, 15, 31, 31, 7, 1, 0, 0, 7, 31, 63, 126, 103, 99, 99, 113, 49, 0, 63, 63, 31, 0, 0, 0, 0, 0, 28, 61, 57, 1, 0, 0, 0
};


// The 'lives left' monkey icon.
// It consists of a 2 frame icon, frame 0 is the top half, 1 is the bottom.
PROGMEM prog_uchar monkeyIcon[] = {
  17, // icon width
  // Top half of monkey face ("frame" 0)
  192, 96, 48, 248, 60, 206, 230, 15, 255, 63, 207, 230, 14, 252, 48, 32, 192,
  // Bottom half of monkey face ("frame" 1)
  1, 31, 54, 99, 65, 201, 209, 147, 144, 145, 147, 201, 193, 65, 98, 63, 1
};


// The button images. This is a bit more complex: there are four buttons, each 3 lines tall,
// each with 2 states. Different 'frames' are used for each of these.
PROGMEM prog_uchar buttonIcons[] = {24,
  // button 1 'off' (frames 0-2)
  0, 0, 0, 0, 0, 0, 0, 0, 64, 96, 96, 224, 224, 224, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  // button 1 'on' (frames 3-5)
  0, 192, 224, 240, 248, 252, 254, 254, 190, 159, 159, 31, 31, 31, 255, 254, 254, 254, 252, 248, 240, 224, 192, 0,
  126, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 126,
  0, 3, 7, 15, 31, 63, 127, 127, 127, 255, 255, 248, 248, 248, 255, 127, 127, 127, 63, 31, 15, 7, 3, 0,
  // button 2 'off' (frames 6-8)
  0, 0, 0, 0, 0, 0, 0, 128, 192, 192, 224, 96, 96, 224, 224, 192, 128, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 1, 1, 129, 192, 224, 240, 120, 63, 31, 7, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 6, 7, 7, 7, 7, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0,
  // button 2 'on' (frames 9-11)
  0, 192, 224, 240, 248, 252, 254, 126, 62, 63, 31, 159, 159, 31, 31, 62, 126, 254, 252, 248, 240, 224, 192, 0,
  126, 255, 255, 255, 255, 255, 255, 254, 254, 126, 63, 31, 15, 135, 192, 224, 248, 255, 255, 255, 255, 255, 255, 126,
  0, 3, 7, 15, 31, 63, 127, 121, 120, 248, 248, 248, 249, 249, 249, 121, 121, 127, 63, 31, 15, 7, 3, 0,
  // button 3 'off' (frames 12-14)
  0, 0, 0, 0, 0, 0, 0, 0, 128, 192, 224, 96, 96, 224, 192, 128, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 128, 128, 128, 0, 24, 28, 63, 247, 243, 224, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 1, 3, 7, 7, 6, 6, 7, 3, 3, 1, 0, 0, 0, 0, 0, 0, 0,
  // button 3 'on' (frames 15-17)
  0, 192, 224, 240, 248, 252, 254, 254, 126, 63, 31, 159, 159, 31, 63, 126, 254, 254, 252, 248, 240, 224, 192, 0,
  126, 255, 255, 255, 255, 255, 255, 127, 127, 127, 255, 231, 227, 192, 8, 12, 31, 255, 255, 255, 255, 255, 255, 126,
  0, 3, 7, 15, 31, 63, 127, 126, 124, 248, 248, 249, 249, 248, 252, 124, 126, 127, 63, 31, 15, 7, 3, 0,
  // button 4 'off' (frames 18-20)
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 192, 224, 224, 224, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 224, 240, 252, 222, 207, 195, 255, 255, 255, 192, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  // button 4 'on' (frames 21-23)
  0, 192, 224, 240, 248, 252, 254, 254, 254, 255, 127, 63, 31, 31, 31, 254, 254, 254, 252, 248, 240, 224, 192, 0,
  126, 255, 255, 255, 255, 255, 31, 15, 3, 33, 48, 60, 0, 0, 0, 63, 255, 255, 255, 255, 255, 255, 255, 126,
  0, 3, 7, 15, 31, 63, 127, 127, 127, 255, 255, 255, 248, 248, 248, 127, 127, 127, 63, 31, 15, 7, 3, 0
};



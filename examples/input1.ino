#include <Gamby.h>
#include <avr/pgmspace.h>



GambyGraphicsMode gamby;

byte frameNum = 0;
int runs = 0;

void setup () {
  gamby = GambyGraphicsMode();
  
  gamby.clearScreen();
  gamby.update();
}


void loop () {
  int r, c;
  boolean b;
  
  for (r = 0; r < NUM_ROWS; r++) {
    for (c = 0; c < NUM_COLUMNS; c++) {
      gamby.setPixel(c, r, false);
    }
  }
  
  gamby.readInputs();
  
  drawButtons();
  
  gamby.update();
  delay(50);
}

void drawButtons() {
   if ((gamby.inputs & BUTTON_1) != 0) { //up
     gamby.rect(60, 20, 70, 30);
   }
   if ((gamby.inputs & BUTTON_2) != 0) { //right
     gamby.rect(70, 25, 80, 35);
   }
   if ((gamby.inputs & BUTTON_3) != 0) { //left
     gamby.rect(50, 25, 60, 35);
   }
   if ((gamby.inputs & BUTTON_4) != 0) { //down
     gamby.rect(60, 30, 70, 40);
   }
   if ((gamby.inputs & DPAD_UP) != 0) {
     gamby.rect(10, 20, 20, 30);
   }
   if ((gamby.inputs & DPAD_RIGHT) != 0) {
     gamby.rect(20, 25, 30, 35);
   }
   if ((gamby.inputs & DPAD_LEFT) != 0) {
     gamby.rect(0, 25, 10, 35);
   }
   if ((gamby.inputs & DPAD_DOWN) != 0) {
     gamby.rect(10, 30, 20, 40);
   }
}


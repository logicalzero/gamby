/*
GAMBY Graphics Mode Speed Test
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A simple sketch that exercises several of the GambyGraphicsMode
features and provides timing information. Library developers
should use this before and after they make optimizations to
existing code.

David R. Stokes (gamby@logicalzero.com) 9/11/2012
*/

#include <Gamby.h>
#include <avr/pgmspace.h>

const prog_uchar smiley8x8[] PROGMEM = {
  8, 8, 
  0x3c, 0x42, 0xad, 0xa1, 0xad, 0x91, 0x42, 0x3c
};

const prog_uchar smiley8x8_mask[] PROGMEM = {
  8, 8, 
  0x3c, 0x7e, 0xff, 0xff, 0xff, 0xff, 0x7e, 0x3c
};

int pats[] = {0x0000, 0xFFFF, 0x5a5a, 0xfaf5, 0x050a, 0xedb7, 0x1248, 0x7bde, 0x8421, 0xeeef, 0x1110, 0xfafa, 0x0505, 0x33cc, 0xcc33};

GambyGraphicsMode gamby;
unsigned long startTime;

int runs = 1;

void setup () {
  Serial.begin(19200);
  Serial.println("\nGambyGraphics_SpeedTest starting...");
}


void loop () {
  Serial.print("== run ");
  Serial.println(runs, DEC);
  
  gamby.clearScreen();
  setPixelTest1();
  delay(1000);
  
  gamby.clearScreen();
  setPixelTest2();
  delay(1000);
  
  gamby.clearScreen();
  concentricCircles1();
  delay(1000);
  
  gamby.clearScreen();
  moire(0xFFFF);
  delay(1000);
  
  gamby.clearScreen();
  moire(PATTERN_GRAY);
  delay(1000);
  
  gamby.clearScreen();
  randomCircles();
  delay(1000);
  
  gamby.clearScreen();
  randomRects();
  delay(1000);
  
  gamby.clearScreen();
  drawSpriteGrid(smiley8x8);
  delay(1000);  
  
  gamby.clearScreen();
  drawSpriteGrid(smiley8x8, smiley8x8_mask);
  delay(1000);
  
  runs++;
}


void setPixelTest1() {
  byte r,c; 
  Serial.print("setPixel(), explicit:\t");
  startTime = micros();
  for (r=0; r<NUM_ROWS; r++) {
    for(c=0; c<NUM_COLUMNS; c++) {
      gamby.setPixel(c,r,false);
    }
  }
  gamby.update();
  Serial.println(micros() - startTime, DEC);
}


void setPixelTest2() {
  byte r,c; 
  Serial.print("setPixel(), pattern:\t");
  startTime = micros();
  gamby.drawPattern = PATTERN_CHECKER;
  for (r=0; r<NUM_ROWS; r++) {
    for(c=0; c<NUM_COLUMNS; c++) {
      gamby.setPixel(c,r);
    }
  }
  gamby.update();
  Serial.println(micros() - startTime, DEC);
}


void concentricCircles1() {
  int p, r;
  Serial.print("concentric discs:\t");
  startTime = micros();
  for (r=NUM_ROWS>>1; r>1;r-=4) {
    p = p == 0 ? 0xFFFF : 0;
    gamby.drawPattern = p;
    gamby.disc(NUM_COLUMNS >> 1, NUM_ROWS >> 1, r);
  }    
  gamby.update();
  Serial.println(micros() - startTime, DEC);
}


void concentricCircles2() {
  int p, r;
  gamby.drawPattern = 0xffff;
  Serial.print("concentric discs:\t");
  startTime = micros();
  for (r=NUM_ROWS>>1; r>1;r-=4) {
    gamby.circle(NUM_COLUMNS >> 1, NUM_ROWS >> 1, r);
  }    
  gamby.update();
  Serial.println(micros() - startTime, DEC);
}


void moire(int pat) {
  int i;
  gamby.drawPattern = pat;
  Serial.print("moire, patterned:\t");
  startTime = micros();
  
  for (i=0; i < NUM_COLUMNS; i+=3) 
    gamby.line(NUM_COLUMNS >> 1, NUM_ROWS >> 1, i, 0);
  gamby.update();

  for (i=0; i < NUM_ROWS; i+=3) 
    gamby.line(NUM_COLUMNS >> 1, NUM_ROWS >> 1,NUM_COLUMNS-1,i);
  gamby.update();

  for (i=NUM_COLUMNS-1; i >= 0; i-=3) 
    gamby.line(i, NUM_ROWS-1, NUM_COLUMNS >> 1, NUM_ROWS >> 1);
  gamby.update();

  for (i=NUM_ROWS-1; i >= 0; i-=3) 
    gamby.line(NUM_COLUMNS >> 1, NUM_ROWS >> 1,0,i);
  gamby.update();

  Serial.println(micros() - startTime, DEC);
}


void randomCircles() {
  int p = PATTERN_BLACK;
  int x, y, r;
  Serial.print("randomCircles:\t\t");
  startTime = micros();
  for (int i=0; i<50; i++) {
    gamby.drawPattern = pats[p];
    x = random(0,NUM_COLUMNS);
    y = random(0,NUM_ROWS);
    r = random(8,20);
    gamby.disc(x,y,r);
    gamby.drawPattern = PATTERN_BLACK;
    gamby.circle(x,y,r);
    p = p < 14 ? p+1 : 0;
  }
  gamby.update();
  Serial.println(micros() - startTime, DEC);
  gamby.drawPattern=0xFFFF;
}


void randomRects() {
  int p = PATTERN_BLACK;
  int x, y, x2, y2;
  Serial.print("randomRects:\t\t");
  startTime = micros();
  for (int i=0; i<50; i++) {
    gamby.drawPattern = pats[p];
    x = random(0,NUM_COLUMNS-4);
    y = random(0,NUM_ROWS-4);
    x2 = random(x,NUM_COLUMNS);
    y2 = random(y,NUM_ROWS);
    gamby.rect(x,y,x2,y2);
    gamby.drawPattern = PATTERN_BLACK;
    gamby.box(x,y,x2,y2);
    p = p < 14 ? p+1 : 0;
  }
  gamby.update();
  Serial.println(micros() - startTime, DEC);
  gamby.drawPattern=0xFFFF;
}


void drawSpriteGrid(const prog_uchar *spriteIdx) {
  gamby.drawPattern = PATTERN_GRAY;
  gamby.rect(0,0,NUM_COLUMNS-1, NUM_ROWS-1);
  Serial.print("Sprite grid, plain:\t");
  startTime = micros();
  for (int i=2; i<NUM_ROWS-8; i+=10) {
    for (int j=2; j<NUM_COLUMNS-8; j+=10) {
      gamby.drawSprite(j, i, spriteIdx);
    }
  }
  gamby.update();
  Serial.println(micros() - startTime);
}


void drawSpriteGrid(const prog_uchar *spriteIdx, const prog_uchar *maskIdx) {
  gamby.drawPattern = PATTERN_GRAY;
  gamby.rect(0,0,NUM_COLUMNS-1, NUM_ROWS-1);
  Serial.print("Sprite grid, masked:\t");
  startTime = micros();
  for (int i=2; i<NUM_ROWS-8; i+=10) {
    for (int j=2; j<NUM_COLUMNS-8; j+=10) {
      gamby.drawSprite(j, i, spriteIdx, maskIdx);
    }
  }
  gamby.update();
  Serial.println(micros() - startTime);
}


/*
Simian Says - a simple memory game 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The game shows a sequence of buttons to press. Repeat the pattern to
score. The sequence gets one longer each time.

This is an example of GambyTextMode being used for something other
than text. GambyTextMode uses the least RAM of all the modes but is
still capable of some interesting stuff.

by David R. Stokes (gamby@logicalzero.com) 2012-09-18
*/

#include <Gamby.h>

// Bring in the font from the 'font' tab (font.ino)

// Bring in all the icons from the 'artwork' tab (artwork.ino)
extern prog_int32_t font[];
extern prog_uchar splashscreen[];
extern prog_uchar gameover[];
extern prog_uchar monkeyIcon[];
extern prog_uchar buttonIcons[];

// Some sets of numbers. These don't actually change.
int notes[] = {220, 247, 262, 294};  // The musical notes associated with each button
byte buttonX[] = {46, 62, 30, 46};   // The horizontal position of each button image
byte buttonY[] = {0, 2, 2, 4};       // The vertical positions, by 8-pixel text line


// Each of Gamby's 'modes' are wrapped in a class, a self-contained unit
// that bundles together all the required functionality. To use a mode,
// you must first create an 'instance' of its class -- a sort of working 
// copy. Your sketch should only use one.
GambyTextMode gamby;


unsigned long seed;
int sequenceLength = 0;
int highScore = 0;
boolean playing;
byte lives = 3;

void setup () {
  gamby.font = font;
  
  // Show the splash screen. It's an icon with 7 'frames',
  // each of which is one 8 pixel row of the large image.
  gamby.clearDisplay();
  for (byte i=0; i<7; i++) {
    gamby.setPos(0,i);
    gamby.drawIcon(splashscreen,i);
  }
  gamby.setPos(0,7);
  gamby.print("Press any button to begin!");
  waitForAnyButton();
  
  startGame();
}

void loop () {
  if (playing) {
    drawScreen();
    playSequence();
    if (getSequence()) {
      // The player got the sqeuence correct!
      // Increase the length and continue
      sequenceLength++;
    }
    else {
      // The player got the sequence wrong. 
      // Play a rude sound, deduct a life and continue.
      tone(9,100);
      delay(1000);
      noTone(9);
      lives--;
      if (lives == 0)
        gameOver();
    }
    // A little delay for some breathing room.
    delay(250);
  }
}


// Wait for the player to press any button or move the stick.
void waitForAnyButton() {
  gamby.readInputs();
  byte initialInputs = gamby.inputs;
  while (gamby.inputs == initialInputs) {
    delay(100);
    gamby.readInputs();
  }
}


// Draw the game screen: the buttons, the 'lives left' monkeys and the score.
void drawScreen() {
  byte i;
  gamby.clearScreen();
  
  // The 'lives left' monkeys
  for (i=0; i<lives; i++) {
    gamby.setPos(0,i*2);
    gamby.drawIcon(monkeyIcon,0);
    gamby.setPos(0,i*2+1);
    gamby.drawIcon(monkeyIcon,1);
  }
  
  // The buttons in their 'off' states
  for (i=0; i<4; i++)
    drawButton(i,false);
  
  // The score
  gamby.setPos(0,7);
  gamby.print("Score: ");
  gamby.print(sequenceLength-1, DEC);
}


void drawButton(byte button, boolean state) {
  gamby.setPos(buttonX[button],buttonY[button]);
  gamby.drawIcon(buttonIcons, (button*6+state*3));
  gamby.setPos(buttonX[button],buttonY[button]+1);
  gamby.drawIcon(buttonIcons, (button*6+state*3)+1);
  gamby.setPos(buttonX[button],buttonY[button]+2);
  gamby.drawIcon(buttonIcons, (button*6+state*3)+2);
}


// Start a new game!
void startGame() {
  // Pick a new random seed for the game.
  seed = micros();
  
  // The sequence starts at length 1.
  sequenceLength = 1;
  
  lives = 3;
  
  // Update the screen: draw the buttons, the score, and the 'lives left' monkeys.
  drawScreen();
  
  playing = true;
}


// Play the current sequence.
void playSequence() {
  byte n;
  // A series of random numbers is always the same for the same seed.
  randomSeed(seed);
  for (byte i=0; i<sequenceLength; i++) {
    n = random(4);
    drawButton(n, true);
    tone(9, notes[n]);
    delay(500);
    drawButton(n, false);
    noTone(9);
    delay(100);
  }
}


// Read a button, show it on the screen, and play its associated tone.
byte readButton() {
  byte lastInputs, button;
  gamby.readInputs();
  lastInputs = gamby.inputs;
  
  // Pause until a button is pressed.
  while(!(lastInputs & BUTTON_ANY)) {
    delay(50);
    gamby.readInputs();
    lastInputs = gamby.inputs;
  }
  
  // Turn the 'inputs' bit into a button number, 0-3
  switch (lastInputs) {
    case BUTTON_1:
      button = 0;
      break;
    case BUTTON_2:
      button = 1;
      break;
    case BUTTON_3:
      button = 2;
      break;
    case BUTTON_4:
      button = 3;
      break;
  }
  
  // Draw the button 'pressed' and start the button sound playing.
  drawButton(button, true);
  tone(9, notes[button]);

  // Wait until the button is released
  while(gamby.inputs & BUTTON_ANY) {
    delay(50);
    gamby.readInputs();
  }
  
  // Draw the button 'unpressed' and stop the sound.
  drawButton(button, false);
  noTone(9);
  
  return button;
}


// Read a sequence of buttons entered by the player.
// Return 'true' if the player got the sequence correct, 'false' if not.
boolean getSequence() {
  // This whole game takes advantage of the fact that random numbers aren't really random.
  // From the same seed, the sequence is the same.
  randomSeed(seed);
  for (byte i=0; i<sequenceLength; i++) {
   byte n = random(4);
   byte b = readButton();
   if (b != n)
     return false;
  }
  return true;
}


// Show the 'game over' screen and pause before restarting.
void gameOver() {
  playing = false;
  
  gamby.clearDisplay();
  gamby.setPos(0,3);
  gamby.drawIcon(gameover, 0);
  gamby.setPos(0,4);
  gamby.drawIcon(gameover,1);
  
  gamby.setPos(14,5);
  gamby.print("Your score: ");
  gamby.print(sequenceLength-1, DEC);
  
  gamby.setPos(16,6);
  gamby.print("High score: ");
  gamby.print(highScore, DEC);
  
  if (sequenceLength - 1 > highScore) {
    gamby.setPos(4,2);
    gamby.print("** New high score! **");
    highScore = sequenceLength - 1;
  }
  
  waitForAnyButton();
  startGame();
}

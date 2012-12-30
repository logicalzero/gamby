// JEWELS: A strangely familiar puzzle game
//
// by Mars Saxman (mars@redecho.org) 17 October 2012
//
//////////////////////////////////////////////////////////////////////////////
//
// Jewels fall from the top of an 8x8 grid. Pick a jewel with the d-pad, then
// use the buttons to swap it with one of its neighbors. Make a line of three or
// more matching jewels, and they will disappear, scoring you points; all the
// jewels above will fall down to take their place, with new ones coming in 
// from above the screen.
//
// Game ends when there are no more legal moves. You win when the score counter
// can no longer keep up with the results of your lucky and skilful play.
//



// to do:
// use a multiplier for multi-collapse moves
// make a suggestion after a long idle

#include <Gamby.h>
#include <avr/pgmspace.h>
#include <stdint.h>

class Tile
{
  public:
    Tile() : value(EMPTY_SQUARE) {}
    bool is_marked() const { return value & MARK_FLAG; }
    void set_mark() { if (in_bounds()) value |= MARK_FLAG; }
    void clear_mark() { if (in_bounds()) value &= (~MARK_FLAG); }
    bool is_empty() const { return (value & ID_MASK) == EMPTY_SQUARE; }
    byte jewel() const { return value & ID_MASK; }
    void randomize() { if (in_bounds()) value = random(0, 7); }
    void set_jewel(byte x) { if (in_bounds()) value = x;}
    void clear() { if (in_bounds()) value = EMPTY_SQUARE; }
    void exile() { value = EMPTY_SQUARE | OOB_FLAG; }
    bool in_bounds() const { return 0 == (value & OOB_FLAG); }
  private:
    static const int EMPTY_SQUARE = 7;
    static const int ID_MASK = 0x7;
    static const int MARK_FLAG = 0x8;
    static const int OOB_FLAG = 0x80;
    byte value;
};

class Board
{
  public:
    Board() { oob.exile(); }
    static const int width = 8;
    static const int height = 8;
    Tile &tile(int x, int y)
    {
      if (x < 0 || x >= width) return oob;
      if (y < 0 || y >= height) return oob;
      return tiles[x][y]; 
    }
    bool swap(int fromX, int fromY, int toX, int toY)
    {
      Tile &from = tile(fromX, fromY);
      Tile &to = tile(toX, toY);
      if (!from.in_bounds() || !to.in_bounds()) {
        return false;
      }
      Tile temp = from;
      from = to;
      to = temp;
    }
    bool match(int fromX, int fromY, int toX, int toY)
    {
      Tile &from = tile(fromX, fromY);
      Tile &to = tile(toX, toY);
      return (from.jewel() == to.jewel()) && from.in_bounds() && to.in_bounds();
    }
    bool contains_blanks() const
    {
      for (unsigned y = 0; y < height; y++) {
        for (unsigned x = 0; x < width; x++) {
          if (tiles[x][y].is_empty()) {
            return true;
          }
        }
      }
      return false;
    }
    void randomize()
    {
      // Make a random playing field with no tile runs already formed.
      // We do this by making sure each tile differs from at least one of its
      // leftward and upward neighbors.
      for (unsigned x = 0; x < width; x++) {
        for (unsigned y = 0; y < height; y++) {
          bool matches = false;
          do {
            tile(x, y).randomize();
            bool match_left = match(x, y, x-1, y) && match(x, y, x-2, y);
            bool match_up = match(x, y, x, y-1) && match(x, y, x, y-2);
            matches = match_left || match_up;
          } while (matches);
        }
      }
    }
    void clear_all_marks()
    {
      for (unsigned x = 0; x < width; x++) {
        for (unsigned y = 0; y < height; y++) {
          if (tiles[x][y].is_marked()) {
            tiles[x][y].clear_mark();
          }
        }
      }
    }
  private:
    Tile tiles[width][height];
    Tile oob;
};

class Blink
{
  public:
    bool check()
    {
      unsigned long now = millis();
      if (now - lastBlink < kBlinkInterval) {
        return false;
      }
      lastBlink = now;
      blinkState = !blinkState;
      return true;
    }
    void set()
    {
      // when moving the cursor, we want it to be visible
      // so we'll reset the blink cycle
      blinkState = true;
      lastBlink = millis();
    }
    void clear()
    {
      // when deleting rows, we want the animation to be clear
      // so we'll hide the cursor and reset the blink cycle
      blinkState = false;
      lastBlink = millis();
    }
    bool state() const { return blinkState; }
  private:
    bool blinkState;
    unsigned long lastBlink;
    static const int kBlinkInterval = 750;
};

class Cursor
{
  public:
    Cursor(Board &b) : board(b) {}
    byte x() const { return xPos; }
    byte y() const { return yPos; }
    void Move(int delta_x, int delta_y)
    {
      int new_x = xPos + delta_x;
      if (new_x < 0) new_x = 0;
      if (new_x >= board.width) new_x = board.width - 1;
      int new_y = yPos + delta_y;
      if (new_y < 0) new_y = 0;
      if (new_y >= board.height) new_y = board.height - 1;
      xPos = new_x;
      yPos = new_y;
    }
    bool Equals(byte x, byte y)
    {
      return x == xPos & y == yPos;
    }
    Blink &blink() { return blinker; }
  private:
    Blink blinker;
    Board &board;
    byte xPos;
    byte yPos;
};

// We'll need a gamby shield driver to manage the UI
GambyGraphicsMode gamby;
const int kColumnPixels = 8;

// We'll also need a game board to maintain state
Board board;

// Delay a certain amount of time between redraws to leave time for display
// of each animation frame.
const int kFrameDuration = 120;

// The cursor is what allows the player to interact with the game. It
// always has a certain position. It will blink at a fixed rate to make
// it easier to spot in the array of black & white pixels.
Cursor cursor(board);

// Score: deleting squares gains you points.
unsigned score = 0;

void blit_inverted(const unsigned char *icon)
{
  DATA_MODE();
  byte length = pgm_read_byte_near(icon);
  for (unsigned index = 0; index < length; index++) {
    byte b = pgm_read_byte_near(++icon);
    gamby.sendByte(~b);
  }
}

void draw_board()
{
  for (unsigned x = 0; x < board.width; x++) {
    for (unsigned y = 0; y < board.height; y++) {
      // We might get called in the middle of a run-deletion operation, so we have to
      // be careful about masking out the run mark.
      const unsigned char *icon = get_jewel(board.tile(x, y).jewel());
      gamby.setPos(x * kColumnPixels, y);
      bool inverted = false;
      inverted |= cursor.blink().state() && cursor.Equals(x, y);
      inverted |= board.tile(x, y).is_marked();
      if (inverted) {
        // Indicate that this jewel is currently selected.
        // We'll draw the same image, but we'll draw it inverse.
        blit_inverted(icon);
      } else {
        gamby.drawIcon(icon);
      }
    }
  }
}

bool mark_runs()
{
  // Search the board for consecutive gem runs.
  // Mark them by setting the MARK_FLAG.
  bool redraw = false;
  for (unsigned x = 0; x < board.width; x++) {
    for (unsigned y = 0; y < board.height; y++) {
      if (board.tile(x, y).is_empty()) {
        continue;
      }
      if (x < 6) {
        // Look for a run moving to the right
        if (board.match(x, y, x+1, y) && board.match(x, y, x+2, y)) {
          for (unsigned i = 0; i < board.width && board.match(x, y, x+i, y); i++) {
            board.tile(x + i, y).set_mark();
          }
          redraw = true;
        }
      }
      if (y < 6) {
        // Look for a run moving downward
        if (board.match(x, y, x, y+1) && board.match(x, y, x, y+2)) {
          for (unsigned i = 0; i < board.height && board.match(x, y, x, y+i); i++) {
            board.tile(x, y+i).set_mark();
          }
          redraw = true;
        }
      }
    }
  }
  return redraw;
}

bool delete_runs(bool invisible)
{
  // Iterate through the board looking for consecutive horizontal or vertical runs.
  // Delete them, leaving blank spaces.
  // Invisible mode is used during startup, to create a clean board state.
  bool redraw = false;
  // Now that we have marked all of the squares which are involved in runs, delete them.
  for (unsigned x = 0; x < board.width; x++) {
    for (unsigned y = 0; y < board.height; y++) {
      if (board.tile(x, y).is_marked()) {
        board.tile(x, y).clear();
        board.tile(x, y).set_mark();
        redraw = true;
        score++;
      }
    }
  }
  if (!invisible) draw_board();
  board.clear_all_marks();
  draw_score();
  return redraw;
}

bool shift_column(unsigned x)
{
  bool result = false;
  for (int y = 7; y >= 0; y--) {
    if (!board.tile(x, y).is_empty()) {
      continue;
    }
    byte thisone = board.tile(x, y).jewel();
    // This square is empty. Is there a square above it?
    // If so, swap with it. Otherwise, generate a new jewel.
    if (y > 0) {
      if (board.tile(x, y - 1).is_empty()) continue;
      board.swap(x, y, x, y-1);
    } else {
      board.tile(x, y).randomize();
    }
    return true;
  }
  return result;
}

bool shift_down()
{
  // Iterate across the game field from the left to the right.
  // For each column, starting from the bottom, look for empty squares.
  // If there is a non-empty square above, swap their places.
  // If the empty square is at the top of the screen, fill it with a random square.
  bool redraw = false;
  for (unsigned x = 0; x < board.width; x++) {
    bool redraw_this = shift_column(x);
    redraw |= redraw_this;
  }
  if (redraw) {
    cursor.blink().clear();
  }
  return redraw;
}

void animate_swap_horizontal(byte leftX, byte bothY, bool holdHighlight)
{
  // We are exchanging the tile at leftX,bothY with the tile at leftX+1,bothY.
  // Do a cute little animation thingy where the two tiles switch places.
  const unsigned char *left_icon = get_jewel(board.tile(leftX, bothY).jewel());
  const unsigned char *right_icon = get_jewel(board.tile(leftX+1, bothY).jewel());
  byte local_left[16];
  for (unsigned i = 0; i < 8; i++) {
    local_left[i] = pgm_read_byte_near(left_icon + i + 1); 
    local_left[i+8] = 0;
  }
  byte local_right[16];
  for (unsigned i = 0; i < 8; i++) {
    local_right[i] = 0;
    local_right[i+8] = pgm_read_byte_near(right_icon + i + 1);
  }
  
  for (int stage = 0; stage < 9; stage++) {
    gamby.setPos(leftX * kColumnPixels, bothY);
    DATA_MODE();
    for (int x = 0; x < 16; x++) {
      byte blit = 0;
      blit |= local_left[(x - stage) & 0x0F];
      blit |= local_right[(x + stage) & 0x0F];
      if (stage == 8 && holdHighlight) blit = ~blit;
      gamby.sendByte( blit );
    }
    delay(kFrameDuration>>1);
  }
  delay(kFrameDuration>>1);
}

void animate_swap_vertical(byte bothX, byte topY, bool holdHighlight)
{
  // Exchange the tile at bothX, topY with the tile at bothX, topY+1.
  const unsigned char *top_icon = get_jewel(board.tile(bothX, topY).jewel());
  const unsigned char *bot_icon = get_jewel(board.tile(bothX, topY+1).jewel());
  byte local_top[8];
  for (unsigned i = 0; i < 8; i++) local_top[i] = pgm_read_byte_near(top_icon + i + 1);
  byte local_bot[8];
  for (unsigned i = 0; i < 8; i++) local_bot[i] = pgm_read_byte_near(bot_icon + i + 1);

  for (int stage = 0; stage < 9; stage++) {
    gamby.setPos(bothX * kColumnPixels, topY);
    DATA_MODE();
    for (int x = 0; x < 8; x++) {
      byte blit = 0;
      blit |= local_top[x] << stage;
      blit |= local_bot[x] << (8 - stage);
      if (stage == 8 && holdHighlight) blit = ~blit;
      gamby.sendByte(blit);
    }
    gamby.setPos(bothX * kColumnPixels, topY+1);
    DATA_MODE();
    for (int x = 0; x < 8; x++) {
      byte blit = 0;
      blit |= local_top[x] >> (8 - stage);
      blit |= local_bot[x] >> stage;
      if (stage == 8 && holdHighlight) blit = ~blit;
      gamby.sendByte(blit);
    }
    delay(kFrameDuration >> 1);
  }
  delay(kFrameDuration >> 1 );
}

void wait_for_button_release()
{
  // Wait for the player to release the swap button.
  do {
    gamby.readInputs();
  } while (gamby.inputs & (BUTTON_1 | BUTTON_2 | BUTTON_3 | BUTTON_4));
}

bool swap_tiles_vertical(byte targetX, byte targetY)
{
  animate_swap_vertical(cursor.x(), min(targetY, cursor.y()), true);
  wait_for_button_release();  
  
  // Swap the values. 
  board.swap(cursor.x(), cursor.y(), targetX, targetY);

  // The swap should have created some deletable runs. Go find them.
  bool madeRuns = mark_runs();
  if (madeRuns) {
    cursor.blink().clear();
  } else {
    // If the swap did not create any runs, it was illegal, and we will revert it.
    animate_swap_vertical(cursor.x(), min(targetY, cursor.y()), false);
    board.swap(cursor.x(), cursor.y(), targetX, targetY);
  }
  return true;
}

bool swap_tiles_horizontal(byte targetX, byte targetY)
{
  animate_swap_horizontal(min(targetX, cursor.x()), cursor.y(), true);
  wait_for_button_release();  

  // Swap the values. 
  board.swap(cursor.x(), cursor.y(), targetX, targetY);

  // The swap should have created some deletable runs. Go find them.
  bool madeRuns = mark_runs();
  if (madeRuns) {
    cursor.blink().clear();
  } else {
    // If the swap did not create any runs, it was illegal, and we will revert it.
    animate_swap_horizontal(min(targetX, cursor.x()), cursor.y(), false);
    board.swap(cursor.x(), cursor.y(), targetX, targetY);
  }
  return true;
}

bool check_inputs()
{
  gamby.readInputs();
  bool redraw = false;
  
  // See if the player wants to swap two tiles.
  // Only one swap is possible at a time.
  if (gamby.inputs & BUTTON_1 && cursor.y() > 0) {
    // up
    return swap_tiles_vertical(cursor.x(), cursor.y() - 1);
  }
  else if (gamby.inputs & BUTTON_2 && cursor.x() < 7) {
    // right
    return swap_tiles_horizontal(cursor.x() + 1, cursor.y());
  }
  else if (gamby.inputs & BUTTON_3 && cursor.x() > 0) {
    // left
    return swap_tiles_horizontal(cursor.x() - 1, cursor.y());
  }
  else if (gamby.inputs & BUTTON_4 && cursor.y() < 7) {
    // down  
    return swap_tiles_vertical(cursor.x(), cursor.y() + 1);
  }
  
  // See if the player wants to move the cursor.
  // Multiple cursor directions are possible at once.
  if (gamby.inputs & DPAD_UP) cursor.Move(0, -1);
  if (gamby.inputs & DPAD_DOWN) cursor.Move(0, 1);
  if (gamby.inputs & DPAD_LEFT) cursor.Move(-1, 0);
  if (gamby.inputs & DPAD_RIGHT) cursor.Move(1, 0);
  if (gamby.inputs & (DPAD_UP | DPAD_DOWN | DPAD_LEFT | DPAD_RIGHT)) {
    delay(kFrameDuration);
    cursor.blink().set();
    redraw = true;
  }
  return redraw;
}

void init_board()
{
  board.randomize();
  score = 0;
}

void setup() {
  // Seed the random number generator with noise from an unused analog input
  randomSeed(analogRead(0));
  // Set up the initial board state and draw.
  init_board();
  draw_board();
  draw_sidebar();
}

void loop () 
{
  bool redraw = false;
  redraw |= shift_down();
  if (!board.contains_blanks()) {
    redraw |= mark_runs();
    redraw |= delete_runs(false);
  }
  redraw |= cursor.blink().check();
  redraw |= check_inputs();
  redraw |= check_end();
  if (redraw) {
    draw_board();
    // Delay after each redraw to make animations visible.
    // This also discourages needless redraws.
    delay(kFrameDuration);
    gamby.setPos(0,0);
  }
}

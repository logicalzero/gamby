// Is the game over?
// If so, we draw a pretty but disappointing animation

void dissolve_away()
{
  // Clear the board in a pretty fashion, giving the user a moment to feel
  // like something has just happened before we reset the board and give
  // them a new game to play.
  byte coordinates[64];
  coordinates[0] = 0;
  for (unsigned i = 1; i < 64; i++) {
    byte j = random(0,i+1);
    coordinates[i] = coordinates[j];
    coordinates[j] = i;
  }
  Tile dummy;
  for (unsigned i = 0; i < 64; i++) {
    byte x = coordinates[i] & 0x07;
    byte y = (coordinates[i] >> 3) & 0x07;
    if (board.tile(x,y).is_empty()) continue;
    gamby.setPos(x * kColumnPixels, y);
    gamby.drawIcon(get_jewel(dummy.jewel()));
    delay(kFrameDuration);
  }
}

// Loss image is 64 pixels wide by 16 pixels tall, split into two icons:
// [------] [------] [------] [------] [------] [------] [------] [------]
// ........ ........ ........ ........ ........ ........ ........ ........
// XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX
// X.X.X.X. X.X.X.X. X.X.X.X. X.X.X.X. X.X.X.X. X.X.X.X. X.X.X.X. X.X.X.X.
// ........ ........ ........ ........ ........ ........ ........ ........
// ...XXX.. ...X.... X...X..X XXXX.... .....XXX ...X...X ..XXXXX. .XXXX...
// ..X...X. ..X.X... XX.XX..X ........ ....X... X..X...X ..X..... .X...X..
// ..X..... .X...X.. X.X.X..X ........ ....X... X..X...X ..X..... .X...X..
// ..X..... .X...X.. X.X.X..X XX...... ....X... X...X.X. ..XXX... .XXXX...

// ..X..XX. .XXXXX.. X...X..X ........ ....X... X...X.X. ..X..... .X.X....
// ..X...X. .X...X.. X...X..X ........ ....X... X....X.. ..X..... .X..X...
// ...XXXX. .X...X.. X...X..X XXXX.... .....XXX .....X.. ..XXXXX. .X...X..
// ........ ........ ........ ........ ........ ........ ........ ........
// .X.X.X.X .X.X.X.X .X.X.X.X .X.X.X.X .X.X.X.X .X.X.X.X .X.X.X.X .X.X.X.X
// XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX
// ........ ........ ........ ........ ........ ........ ........ ........
// ........ ........ ........ ........ ........ ........ ........ ........

PROGMEM const unsigned char loss_display_top[65] = {64,
  B00000110, B00000010, B11100110, B00010010, B00010110, B00010010, B00100110, B00000010,
  B00000110, B11000010, B00100110, B00010010, B00100110, B11000010, B00000110, B00000010,
  B11110110, B00100010, B11000110, B00100010, B11110110, B00000010, B00000110, B11110010,
  B10010110, B10010010, B00010110, B00010010, B00000110, B00000010, B00000110, B00000010,
  B00000110, B00000010, B00000110, B00000010, B11100110, B00010010, B00010110, B00010010,
  B11100110, B00000010, B00000110, B01110010, B10000110, B00000010, B10000110, B01110010,
  B00000110, B00000010, B11110110, B10010010, B10010110, B00010010, B00010110, B00000010,
  B00000110, B11110010, B10010110, B10010010, B10010110, B01100010, B00000110, B00000010
};
PROGMEM const unsigned char loss_display_bottom[65] = {64,
  B00100000, B00110000, B00100011, B00110100, B00100100, B00110101, B00100111, B00110000,
  B00100000, B00110111, B00100001, B00110001, B00100001, B00110111, B00100000, B00110000,
  B00100111, B00110000, B00100000, B00110000, B00100111, B00110000, B00100000, B00110111,
  B00100100, B00110100, B00100100, B00110100, B00100000, B00110000, B00100000, B00110000,
  B00100000, B00110000, B00100000, B00110000, B00100011, B00110100, B00100100, B00110100,
  B00100011, B00110000, B00100000, B00110000, B00100001, B00110110, B00100001, B00110000,
  B00100000, B00110000, B00100111, B00110100, B00100100, B00110100, B00100100, B00110000,
  B00100000, B00110111, B00100000, B00110001, B00100010, B00110100, B00100000, B00110000
};

void do_loss_animation()
{
  // Draw the board in its final state, with the last move completed
  draw_board();
  // Draw the banner that reads "GAME OVER" and give the user a moment
  // to appreciate it
  gamby.setPos(0, 3);
  gamby.drawIcon(loss_display_top);
  gamby.setPos(0, 4);
  gamby.drawIcon(loss_display_bottom);
  delay(2000);
  
  // Make the board and banner dissolve away before we set up the new game
  dissolve_away();
}

// [------] [------] [------] [------] [------] [------] [------] [------]
// ........ ........ ........ ........ ........ ........ ........ ........
// ........ ........ ........ ........ ........ ........ ........ ........
// ........ ........ ........ ........ ........ ........ ........ ........
// ........ ........ ........ ........ ........ ........ ........ ........
// ........ ........ ........ ........ ........ ........ ........ ........
// ........ ........ ........ ........ ........ ........ ........ ........
// ........ ........ X.X.X.X. X..X.X.X .X.X.X.X .X.XX.X. XX...... ........
// ........ .......X .X.X.XX. XXXXX.XX XXXXX.X. XXX.XXXX .X...... ........

// ........ ......X. X.XX.X.X ..X..XX. X....XXX X.XXX.X. XXXX.... ........
// ........ ......X. XX.X...X ...X..X. XXXX.... ....X.XX .X.X.... ........
// ........ .....XX. .XX.X... ....X.XX ..XXX.X. ......XX X.XXX... ........
// ........ .....X.X ..XXX... ....XX.. ...XXX.. ....XXX. ....X... ........
// ........ ....XX.. ....XXX. ..XXX.X. ....X.XX X....X.X ...XXX.. ........
// ........ ...X.X.. ....X.X. X.X.X... .....XX. X.XX.XX. ....XXX. ........
// ........ ..XXX... ....XXX. X.X..... ......XX X.XXX... ......XX ........
// ........ .X...... .....X.X X.X..... ........ X.X.X... .....X.X ........

// ........ .XXXXX.X .X.X.XX. XX..X.X. X..X...X .X..XX.. X.....XX X.......
// ........ .XX.XX.X .X.XX.XX .XX.X.X. XXX.XXX. XXX..XXX XXXXXX.. XX......
// .......X XXXXXXXX X.X.XX.X X.XXX.X. X.X.X.X. .X.X...X XX.XXXXX XX......
// ........ XX...X.. X.XX.XXX .X..XX.X .XX..X.X X.XX.X.. ..X..X.X .X......
// ........ .XXX.... ......X. X....... ........ .X.X.... ......XX X.......
// ........ ..XX.... ......XX XX...... ........ XXX..... ......XX X.......
// ........ ...XX... .......X .X...... ........ .XX..... ....X.X. ........
// ........ ...XXX.. .......X XX...... .......X X.X..... ....XX.. ........

// ........ ........ ........ ........ ......X. X....... ........ ........
// .XXXX..X XX..XXXX ...XXXXX .XXX.... XXXX.XXX XXXXXXXX XXXXXXXX XXX.XX..
// ..XXX... X..XXXXX X...XXX. ..X..... .XX..XXX XX.X..XX X..XXX.. .X..XX..
// ..XXX... X..XX..X XX..XXX. ..X..... .XX...XX ...X..XX X..XXX.. .X..XX..
// ..XXX..X X..XX..X XX..XXX. ..X..... .XX..XXX X..X..XX X..XXXX. .X..XX..
// ...XXX.X ..XXX..X XX..XXX. ..X.X... .XX..XXX X..X..XX X..XXXX. .X..XX..
// ...XXX.X ..XXX..X XX..XXX. ..X.X... .XXX.XXX ..X..XXX X..XXXX. .X..XX..
// ...XXX.X ..XXX..X XX..XXX. ..X.XX.. ..XXXXXX X.X.XXXX X..X.XXX .X..XX..

// ...XXXXX ..XXX..X XX..XXX. ..X.X... ..XX.XXX X.X.XXXX X..X.XXX .X..XX..
// ....XXX. ..XXX..X XX..XXX. ..X.XX.. ..XX.XXX X.X.X.XX X..X.XXX .X..XX..
// ....XXX. ..XXX..X XX..XXX. ..X.XX.. ..XXXXXX X.X.X.XX X..X..XX XX..X...
// ....XXX. ..XXX..X XX..XXX. ..X.XXX. ..XXX..X XXX...XX X..X..XX XX..X...
// ....XXX. ..XXX..X XX..XXX. ..X..XX. ..XXXX.. XXX...XX X..X...X XX..X...
// ....XXX. ..XXX..X XX..XXX. ..X.X.XX ..XXXX.X XXX...XX X..X...X XX......
// ....XXX. ...XX..X X...XXX. ..X.X.X. ..XXX..X XX....XX X..X...X XX......
// ....XXX. ...XX..X X...XXX. .XX..X.X XX.XXXX. XX....XX X..X.... XX......

// ....XXX. ...XX.XX X....XXX XX.XX.XX .X.XX.XX XX....XX X..X.... XX..XX..
// ...XXXXX ....XXXX ......XX X..XXX.X .X..XXX. X....XXX XXXXX... .X..XX..
// ........ ........ ........ ...X.XX. .XX..... ........ ........ ........
// ........ ........ ........ ....X.XX X.XX.X.. ........ ........ ........
// ........ ........ ........ .....X.X .X.X.... ........ ........ ........
// ........ ........ ........ ......XX X.X..... ........ ........ ........
// ........ ........ ........ ........ XXX..... ........ ........ ........
// ........ ........ ........ .......X X....... ........ ........ ........



PROGMEM const unsigned char win_display[8][65] = {
  {64, 
    B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B10000000,
    B01000000, B10000000, B10000000, B01000000, B10000000, B01000000, B10000000, B11000000,
    B11000000, B10000000, B10000000, B11000000, B10000000, B01000000, B10000000, B11000000,
    B10000000, B11000000, B10000000, B01000000, B11000000, B10000000, B11000000, B10000000,
    B10000000, B11000000, B10000000, B01000000, B11000000, B10000000, B11000000, B10000000,
    B01000000, B11000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000
  },
  
  {64,
    B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
    B00000000, B10000000, B01000000, B01100000, B01010000, B00111100, B00000111, B00001000,
    B00000011, B00000010, B00001101, B00001011, B01111100, B11010001, B01110000, B10000011,
    B11100000, B00000000, B11110001, B00010010, B00111100, B00001001, B00010111, B00000100,
    B00000011, B00000010, B00000110, B00001110, B00011100, B00101001, B01110101, B01010001,
    B11110001, B00000000, B11100001, B01100001, B11001011, B00111000, B00101111, B00010110,
    B00000101, B00000011, B00000101, B00010111, B00111100, B10110000, B01100000, B11000000,
    B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000
  },

  {64,
    B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000100,
    B00001100, B00011111, B00110111, B11110101, B11000111, B10001111, B00000100, B00000111, 
    B00001100, B00000011, B00001100, B00001011, B00000110, B00001101, B00111011, B11101110, 
    B10110101, B11101011, B00000110, B00000100, B00001111, B00001000, B00000111, B00001000,
    B00000111, B00001010, B00001110, B00000001, B00000110, B00001010, B00000110, B10001001, 
    B10101010, B01110111, B11101010, B00011100, B00000001, B00001011, B00000010, B00000110,
    B00000111, B00000110, B00001010, B00000110, B11000110, B10111110, B01110101, B00111101,
    B00110111, B00001110, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000
  },

  {64,
    B00000000, B00000010, B00011110, B11111110, B11111110, B11100000, B00000000, B11110010, 
    B00011110, B00000010, B11100000, B11111100, B11111110, B00000110, B00000010, B11111110,
    B11111100, B11111000, B00000000, B00000010, B11111110, B11111110, B11111110, B00000010,
    B00000000, B00000010, B11111110, B00000010, B11100000, B10000000, B00000000, B00000000,
    B00000010, B01111110, B11111110, B11000010, B10000000, B11110110, B11111111, B11111110,
    B10110111, B00000110, B11000010, B00111110, B10000010, B11000010, B11111110, B11111110,
    B11111110, B00000010, B00000010, B11111110, B01111110, B11111110, B11110010, B10000010,
    B00000010, B11111110, B00000010, B00000000, B11111110, B11111110, B00000000, B00000000
  },

  {64,
    B00000000, B00000000, B00000000, B00000001, B11111111, B11111111, B11111111, B00000001,
    B00000000, B00000000, B00111111, B11111111, B11111111, B00000000, B00000000, B11111111,
    B11111111, B00111111, B00000000, B00000000, B11111111, B11111111, B11111111, B00000000,
    B00000000, B10000000, B11111111, B00000000, B01101111, B10011110, B01111000, B10100000,
    B10000000, B10000000, B01111111, B11111111, B11111100, B10110111, B10000111, B01101111,
    B11111111, B11111000, B00111111, B00000000, B00000111, B00000001, B11111111, B11111111,
    B11111111, B00000000, B00000000, B11111111, B00000000, B00000011, B00001111, B01111111,
    B11111100, B11111111, B00000000, B00000000, B00011111, B00000011, B00000000, B00000000
  },

  {64,
    B00000000, B00000000, B00000000, B00000010, B00000011, B00000011, B00000011, B00000010,
    B00000000, B00000000, B00000000, B00000001, B00000011, B00000010, B00000011, B00000011,
    B00000001, B00000000, B00000000, B00000000, B00000000, B00000001, B00000011, B00000011, 
    B00000011, B00000001, B00000000, B00000111, B00001011, B00010110, B00101101, B10111011,
    B11101000, B01010111, B01101100, B00011001, B00000011, B00001010, B00000011, B00000001,
    B00000011, B00000001, B00000000, B00000000, B00000000, B00000010, B00000011, B00000011,
    B00000011, B00000010, B00000010, B00000011, B00000010, B00000000, B00000000, B00000000,
    B00000001, B00000011, B00000000, B00000000, B00000011, B00000011, B00000000, B00000000
  },
  
  {64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};  


// Win image is 64x64 pixels, filling the game board
void do_win_animation()
{
  draw_board();
  for (unsigned v = 0; v <= 7; v++) {
    gamby.setPos(0, v);
    gamby.drawIcon(win_display[v]);
  }
  delay(4000);
  dissolve_away();
}

bool has_horz_move(unsigned x, unsigned y)
{
  if (board.match(x, y, x-1, y)) return false;
  board.swap(x, y, x-1, y);
  bool has_run = mark_runs();
  board.clear_all_marks();
  board.swap(x, y, x-1, y);
  return has_run;
}

bool has_vert_move(unsigned x, unsigned y)
{
  if (board.match(x, y, x, y+1)) return false;
  board.swap(x, y, x, y+1);
  bool has_run = mark_runs();
  board.clear_all_marks();
  board.swap(x, y, x, y+1);
  return has_run;
}

bool check_loss()
{
  // The game is lost when there are no more legal moves.
  // We will determine this in a simple, straightforward way:
  // iterate through the board, check each possible move, and see if it
  // results in a group.
  // We only do this when there is nothing else going on, so the
  // performance hit is not noticeable.
  for (unsigned y = 0; y < 7; y++) {
    for (unsigned x = 0; x < 7; x++) {
      if (has_horz_move(x, y) || has_vert_move(x, y)) {
        return false;
      }
    }
  } 
  do_loss_animation();
  return true;
}

bool check_win()
{
  // The game is won when the score reaches or exceeds 10,000.
  // The number is not arbitrary: the largest score we can display
  // is 9999. Winning the game seems a reasonable thing to do when
  // we can no longer let you keep increasing your score.
  if (score < 1000) return false;  
  do_win_animation();
  return true;
}

bool check_end()
{
  bool redraw = check_win() || check_loss();
  if (redraw) init_board();
  return redraw;
}

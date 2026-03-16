// TODO support coloured text

#include "../utils/portio.h"
#include "printing.h"

typedef struct Cursor {
  int x;
  int y;
} Cursor;

Cursor CURSOR;

short SCREEN[25][80];

void write_short(short s, int x, int y) {
  volatile short* where;
  where = (volatile short*)0xb8000 + (y * 80 + x);
  *where = s;  // load the value into the pointer
}

short set_colour(char c, char bcolour, char fcolour) {
  short colours = (bcolour << 4) | (fcolour & 0x0f);
  return c | (colours << 8);
}

void printscreen() {
  for (int y = 0; y < 25; y++) {
    for (int x = 0; x < 80; x++) {
      write_short(SCREEN[y][x], x, y);
    }
  }
}

void scroll_without_render() {
  for (int i = 0; i < 24; i++) {
    for (int j = 0; j < 80; j++) {
      SCREEN[i][j] = SCREEN[i + 1][j];
    }
  }
  for (int i = 0; i < 80; i++) {
    SCREEN[24][i] = set_colour(0, BACKGROUND, FOREGROUND);
  }
  CURSOR.y--;
}

void vga_init() {
  CURSOR.x = 0;
  CURSOR.y = 0;

  // init screen with nulls
  for (int i = 0; i < 25; i++) {
    for (int j = 0; j < 80; j++) {
      SCREEN[i][j] = set_colour(0, BACKGROUND, FOREGROUND);
    }
  }

  printscreen();
}

void cursor_set(int x, int y) {  // Does some I/O black magic
  short pos = y * 80 + x;
  if (pos >= 0 && pos < 2000) {
    outb(0x3d4, 0x0f);
    outb(0x3d5, (char)(pos & 0xff));
    outb(0x3d4, 0x0e);
    outb(0x3d5, (char)((pos >> 8) & 0xff));
  }
}

void newline() {
  CURSOR.y++;
  CURSOR.x = 0;
  if (CURSOR.y > 24) {
    scroll_without_render();
  }
  cursor_set(CURSOR.x, CURSOR.y);
  printscreen();  // slow: calling here renders twice in some instances
}

void cursorlr(int d) {
  CURSOR.x += d;
  if (CURSOR.x > 79) {
    newline();
  }
  cursor_set(CURSOR.x, CURSOR.y);
}

void cursordu(int d) {
  if (CURSOR.y == 24 && d > 0) {
    return;
  }
  if (CURSOR.y == 0 && d < 0) {
    return;
  }
  CURSOR.y += d;
  cursor_set(CURSOR.x, CURSOR.y);
}

void _sprintln(const char* string, char bcolour, char fcolour) {
  while (*string != '\0') {
    if (*string == '\n') {
      newline();
      string++;
      continue;
    }
    SCREEN[CURSOR.y][CURSOR.x] = set_colour(*string, bcolour, fcolour);
    string++;
    cursorlr(1);
  }
  newline();
  printscreen();
};

void _sprint(const char* string, char bcolour, char fcolour) {
  while (*string != '\0') {
    if (*string == '\n') {
      newline();
      string++;
      continue;
    }
    SCREEN[CURSOR.y][CURSOR.x] = set_colour(*string, bcolour, fcolour);
    string++;
    cursorlr(1);
  }
  printscreen();
};

void _iprintln(long integer, int base, char bcolour, char fcolour) {
  if (integer == 0) {
    SCREEN[CURSOR.y][CURSOR.x] = set_colour('0', bcolour, fcolour);
    newline();
    printscreen();
    return;
  }
  char c;
  char* outstring;
  int counter = 0;
  while (integer != 0) {
    c = integer % base + 0x30;
    outstring[counter] = c > '9' && c < 'A' ? c + 7 : c;
    integer /= base;
    counter++;
  }
  for (int i = 1; i <= counter; i++) {
    SCREEN[CURSOR.y][CURSOR.x] =
        set_colour(outstring[counter - i], bcolour, fcolour);
    cursorlr(1);
  }
  newline();
  printscreen();
}

void _cprint(char c, char bcolour, char fcolour) {
  if (c == '\n') {
    newline();
    return;
  }
  SCREEN[CURSOR.y][CURSOR.x] = set_colour(c, bcolour, fcolour);
  cursorlr(1);
  printscreen();
}

void sprintln(const char* string) {
  _sprintln(string, BACKGROUND, FOREGROUND);
}

void sprintlnc(const char* string, char bg, char fg) {
  _sprintln(string, bg, fg);
}

void sprint(const char* string) {
  _sprint(string, BACKGROUND, FOREGROUND);
}

void sprintc(const char* string, char bg, char fg) {
  _sprint(string, bg, fg);
}

void iprintln(long integer, int base) {
  _iprintln(integer, base, BACKGROUND, FOREGROUND);
}

void iprintlnc(long integer, int base, char bg, char fg) {
  _iprintln(integer, base, bg, fg);
}

void cprint(char c) {
  _cprint(c, BACKGROUND, FOREGROUND);
}

void cprintc(char c, char bg, char fg) {
  _cprint(c, bg, fg);
}

// have a buffer and buffer delete in a tty echo server fingee
// maybe need malloc for that
// can maybe depreciate!
void cdelete() {
  cursorlr(-1);
  for (int i = CURSOR.x; i < 79; i++) {
    SCREEN[CURSOR.y][i] = SCREEN[CURSOR.y][i + 1];
  }
  SCREEN[CURSOR.y][79] = 0;
  printscreen();
}

void clear_line_from_cursor() {
  for (int i = CURSOR.x; i < 80; i++) {
    SCREEN[CURSOR.y][i] = set_colour(0, BACKGROUND, FOREGROUND);
  }
  printscreen();
}

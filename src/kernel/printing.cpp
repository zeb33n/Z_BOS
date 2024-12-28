
typedef struct Cursor {
  int x;
  int y;
} Cursor;

const char BACKGROUND = 0x0;
const char FOREGROUND = 0x7;

Cursor CURSOR;

void write_char(char c, char fcolour, char bcolour, int x, int y) {
  short colours = (bcolour << 4) | (fcolour & 0x0f);
  volatile short* where;
  where = (volatile short*)0xb8000 + (y * 80 + x);
  *where = c | (colours << 8);  // load the value into the pointer
}

// this my cursor we should move the vga cursor aswell
void cursor_init(int x, int y) {
  CURSOR = {x, y};
}

void newline() {
  CURSOR.y++;
  CURSOR.x = 0;
}

void sprintln(const char* string) {
  while (*string != '\0') {
    if (*string == '\n') {
      newline();
      string++;
      continue;
    }
    write_char(*string, FOREGROUND, BACKGROUND, CURSOR.x, CURSOR.y);
    CURSOR.x++;
    string++;
    if (CURSOR.x % 80 == 0) {
      newline();
    }
  }
  newline();
};

void iprintln(long integer, int base) {
  if (integer == 0) {
    write_char('0', FOREGROUND, BACKGROUND, CURSOR.x, CURSOR.y);
    newline();
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
    write_char(outstring[counter - i], FOREGROUND, BACKGROUND, CURSOR.x,
               CURSOR.y);
    CURSOR.x++;
    if (CURSOR.x % 80 == 0) {
      newline();
    }
  }
  newline();
}

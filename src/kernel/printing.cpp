
typedef struct Cursor {
  int x;
  int y;
} Cursor;

const char BACKGROUND = 0x0;
const char FOREGROUND = 0x7;

Cursor CURSOR;

char SCREEN[25][80];

void write_char(char c, char fcolour, char bcolour, int x, int y) {
  short colours = (bcolour << 4) | (fcolour & 0x0f);
  volatile short* where;
  where = (volatile short*)0xb8000 + (y * 80 + x);
  *where = c | (colours << 8);  // load the value into the pointer
}

void printscreen() {
  for (int y = 0; y < 25; y++) {
    for (int x = 0; x < 80; x++) {
      write_char(SCREEN[y][x], FOREGROUND, BACKGROUND, x, y);
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
    SCREEN[24][i] = 0;
  }
  CURSOR.y--;
}

// this my cursor we should move the vga cursor aswell
void vga_init() {
  CURSOR = {0, 0};

  // init screen with nulls
  for (int i = 0; i < 25; i++) {
    for (int j = 0; j < 80; j++) {
      SCREEN[i][j] = 0;
    }
  }

  printscreen();
}

void newline() {
  CURSOR.y++;
  CURSOR.x = 0;
  if (CURSOR.y > 24) {
    scroll_without_render();
  }
}

void sprintln(const char* string) {
  while (*string != '\0') {
    if (*string == '\n') {
      newline();
      string++;
      continue;
    }
    SCREEN[CURSOR.y][CURSOR.x] = *string;
    CURSOR.x++;
    string++;
    if (CURSOR.x % 80 == 0) {
      newline();
    }
  }
  newline();
  printscreen();
};

void iprintln(long integer, int base) {
  if (integer == 0) {
    SCREEN[CURSOR.y][CURSOR.x] = '0';
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
    SCREEN[CURSOR.y][CURSOR.x] = outstring[counter - i];
    CURSOR.x++;
    if (CURSOR.x % 80 == 0) {
      newline();
    }
  }
  newline();
  printscreen();
}

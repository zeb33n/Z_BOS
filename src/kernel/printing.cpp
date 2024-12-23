void write_char(char c, char fcolour, char bcolour, int x, int y) {
  short colours = (bcolour << 4) | (fcolour & 0x0f);
  volatile short* where;
  where = (volatile short*)0xb8000 + (y * 80 + x);
  *where = c | (colours << 8);  // load the value into the pointer
}

void write_string(const char* string,
                  char fcolour,
                  char bcolour,
                  int x,
                  int y) {
  while (*string != '\0') {
    write_char(*string, fcolour, bcolour, x, y);
    string++;
    x++;
    if (x % 80 == 0) {
      y++;
      x = 0;
    }
  }
}

// maybe  indexing the characters from a constant would be easier -> quicker
void write_base_int(long integer,
                    int base,
                    char fcolour,
                    char bcolour,
                    int x,
                    int y) {
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
    write_char(outstring[counter - i], fcolour, bcolour, x, y);
    x++;
    if (x % 80 == 0) {
      y++;
      x = 0;
    }
  }
}

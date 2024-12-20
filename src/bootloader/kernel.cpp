#define HEAPLOC 0xF0000

// print the value of ds to find where the data segment starts

void write_char(char c, char fcolour, char bcolour, int x, int y) {
  short colours = (bcolour << 4) | (fcolour & 0x0f);
  volatile short* where;
  where = (volatile short*)0xb8000 + (y * 80 + x);
  *where = c | (colours << 8);  // load the value into the pointer
}

void write_string(char* string, char fcolour, char bcolour, int x, int y) {
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

// need to skip ascii values 58 to 64
void write_base_int(int dec,
                    int base,
                    char fcolour,
                    char bcolour,
                    int x,
                    int y) {
  char* outstring;
  int counter = 0;
  while (dec != 0) {
    outstring[counter] = dec % base + 0x30;
    dec /= 10;
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

void heap_init() {}
// best fit or first fit
void* malloc(int size) {}

extern "C" int main() {
  // register int* foo asm("ax");

  write_char('B', 0xA, 0x1, 30, 2);
  // write_base_int(*foo, 16, 0xA, 0x1, 32, 4);
  write_base_int(56789, 16, 0xA, 0x1, 32, 3);
  char string[] = "welcome to zebOS!";
  write_string(string, 0x0, 0xC, 79, 10);
  return 0;
}

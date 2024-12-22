#define HEAPLOC 0xF0000

// how can i map out how im using my memory

// TODO:
// 1. interrupts
// 2. malloc
// 3. paging https://wiki.osdev.org/Paging,
// https://wiki.osdev.org/Setting_Up_Paging
//   - identity paging
//   - uses the ps register
// 4. processes

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

// maybe  indexing the characters from a constant would be easier -> quicker
void write_base_int(int integer,
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

void set_bitmap() {}

void heap_init() {}
// best fit or first fit
void* malloc(int size) {}

extern "C" int main() {
  char* foo = (char*)0x10000;
  // register int* foo asm("ax");

  write_char('B', 0xA, 0x1, 30, 2);
  write_char(*foo, 0xA, 0x1, 32, 4);
  write_base_int(0xb1A2F, 16, 0xA, 0x1, 32, 3);
  char string[] = "welcome to zebOS!";
  write_string(string, 0x0, 0xC, 79, 10);
  return 0;
}

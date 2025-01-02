#include "keyboard.h"
#include "portio.h"
#include "printing.h"

// TODO: add arrowkeys and backspace

char get_ascii(unsigned char scancode, int shift) {
  switch (scancode) {
    case 0x02:
      return shift ? '!' : '1';
    case 0x03:
      return shift ? '"' : '2';
    case 0x04:
      return shift ? '$' : '3';
    case 0x05:
      return shift ? '$' : '4';
    case 0x06:
      return shift ? '%' : '5';
    case 0x07:
      return shift ? '^' : '6';
    case 0x08:
      return shift ? '&' : '7';
    case 0x09:
      return shift ? '*' : '8';
    case 0x0A:
      return shift ? '(' : '9';
    case 0x0B:
      return shift ? ')' : '0';
    case 0x10:
      return shift ? 'Q' : 'q';
    case 0x11:
      return shift ? 'W' : 'w';
    case 0x12:
      return shift ? 'E' : 'e';
    case 0x13:
      return shift ? 'R' : 'r';
    case 0x14:
      return shift ? 'T' : 't';
    case 0x15:
      return shift ? 'Y' : 'y';
    case 0x16:
      return shift ? 'U' : 'u';
    case 0x17:
      return shift ? 'I' : 'i';
    case 0x18:
      return shift ? 'O' : 'o';
    case 0x19:
      return shift ? 'P' : 'p';
    case 0x1e:
      return shift ? 'A' : 'a';
    case 0x1f:
      return shift ? 'S' : 's';
    case 0x20:
      return shift ? 'D' : 'd';
    case 0x21:
      return shift ? 'F' : 'f';
    case 0x22:
      return shift ? 'G' : 'g';
    case 0x23:
      return shift ? 'H' : 'h';
    case 0x24:
      return shift ? 'J' : 'j';
    case 0x25:
      return shift ? 'K' : 'k';
    case 0x26:
      return shift ? 'L' : 'l';
    case 0x2C:
      return shift ? 'Z' : 'z';
    case 0x2D:
      return shift ? 'X' : 'x';
    case 0x2E:
      return shift ? 'C' : 'c';
    case 0x2F:
      return shift ? 'V' : 'v';
    case 0x30:
      return shift ? 'B' : 'b';
    case 0x31:
      return shift ? 'N' : 'n';
    case 0x32:
      return shift ? 'M' : 'm';
    case 0x1c:
      return '\n';
    case 0x39:
      return ' ';
    default:
      return 0;
  }
}

void keyboard_handle() {
  // preserve between function calls
  static int shift = 0;

  unsigned char scancode = inb(0x60);

  // iprintln((short)scancode, 16);

  // if top bit set key released
  if (scancode & 0b10000000) {
    if (scancode == LSHIFTR || scancode == RSHIFTR) {
      shift = 0;
    }
    return;
  }

  char cout = get_ascii(scancode, shift);
  if (cout) {
    cprint(cout);
    return;
  }

  switch (scancode) {
    case LSHIFT:
    case RSHIFT:
      shift = 1;
      return;
    case BSPACE:
      cdelete();
      return;
    case AUP:
      cursordu(-1);
      return;
    case ADOWN:
      cursordu(1);
      return;
    case ALEFT:
      cursorlr(-1);
      return;
    case ARIGHT:
      cursorlr(1);
      return;
  }
}

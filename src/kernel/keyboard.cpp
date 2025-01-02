#include "keyboard.h"
#include "portio.h"
#include "printing.h"

// TODO figure out modes -> rewrite?
// have an array of all the characters -> index is the scancode
// this is the mapping
// function pointer to handle what to do for each thing

char MAPPING[128];
char MAPPING_SHIFT[128];

const char MAP_DEFAULT[128] = {
    0,   ESC, '1',    '2',   '3',  '4',    '5',   '6',  '7',   '8', '9', '0',
    '-', '=', BSPACE, TAB,   'q',  'w',    'e',   'r',  't',   'y', 'u', 'i',
    'o', 'p', '[',    ']',   '\n', CTRL,   'a',   's',  'd',   'f', 'g', 'h',
    'j', 'k', 'l',    ';',   '\'', '#',    SHIFT, '\\', 'z',   'x', 'c', 'v',
    'b', 'n', 'm',    ',',   '.',  '/',    SHIFT, 0,    0,     ' ', 0,   0,
    0,   0,   0,      0,     0,    0,      0,     0,    0,     0,   0,   0,
    AUP, 0,   '-',    ALEFT, 0,    ARIGHT, '+',   0,    ADOWN, 0,   0,   0,
    0,   0,   0,      0,     0,    0,  // 90 here
};

const char SHIFT_DEFAULT[128] =
    {
        0,   ESC, '!',    '"',   '$',  '$',    '%',   '^', '&',   '*', '(', ')',
        '_', '+', BSPACE, TAB,   'Q',  'W',    'E',   'R', 'T',   'Y', 'U', 'I',
        'O', 'P', '{',    '}',   '\n', CTRL,   'A',   'S', 'D',   'F', 'G', 'H',
        'J', 'K', 'L',    ':',   '@',  '~',    SHIFT, '|', 'Z',   'X', 'C', 'V',
        'B', 'B', 'M',    '<',   '>',  '?',    SHIFT, 0,   0,     ' ', 0,   0,
        0,   0,   0,      0,     0,    0,      0,     0,   0,     0,   0,   0,
        AUP, 0,   '0',    ALEFT, 0,    ARIGHT, '0',   0,   ADOWN, 0,   0,   0,
        0,   0,   0,      0,     0,    0,  // 90 here
};

// void keyboard_load_mode(Mode mode) {}

void keyboard_load_mapping(const char* normal_layer, const char* shift_layer) {
  *MAPPING = *normal_layer;
  *MAPPING_SHIFT = *shift_layer;
}

// TODO this is borken
void keyboard_default() {
  keyboard_load_mapping(MAP_DEFAULT, SHIFT_DEFAULT);
}

void keyboard_handle() {
  // preserve between function calls
  static int shift = 0;

  unsigned char scancode = inb(0x60);

  // iprintln((short)scancode, 16);

  // if top bit set key released
  if (scancode & 0b10000000) {
    scancode = scancode ^ 0b10000000;
    if (MAP_DEFAULT[scancode] == SHIFT) {
      shift = 0;
    }
    return;
  }

  char cout = shift ? SHIFT_DEFAULT[scancode] : MAP_DEFAULT[scancode];

  switch (cout) {
    case SHIFT:
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
    default:
      cprint(cout);
      return;
  }
}

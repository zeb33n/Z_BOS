#include "../drivers/printing.h"
#include "../utils/portio.h"
#include "../utils/streams.h"
#include "keyboard.h"

// TODO figure out modes -> rewrite?
// have an array of all the characters -> index is the scancode
// this is the mapping
// function pointer to handle what to do for each thing

char MAP_DEFAULT[128] = {
    0,   ESC, '1',    '2',   '3',  '4',    '5',   '6',  '7',   '8', '9', '0',
    '-', '=', BSPACE, TAB,   'q',  'w',    'e',   'r',  't',   'y', 'u', 'i',
    'o', 'p', '[',    ']',   '\n', CTRL,   'a',   's',  'd',   'f', 'g', 'h',
    'j', 'k', 'l',    ';',   '\'', '#',    SHIFT, '\\', 'z',   'x', 'c', 'v',
    'b', 'n', 'm',    ',',   '.',  '/',    SHIFT, 0,    0,     ' ', 0,   0,
    0,   0,   0,      0,     0,    0,      0,     0,    0,     0,   0,   0,
    AUP, 0,   '-',    ALEFT, 0,    ARIGHT, '+',   0,    ADOWN, 0,   0,   0,
    0,   0,   0,      0,     0,    0,  // 90 here
};

char SHIFT_DEFAULT[128] =
    {
        0,   ESC, '!',    '"',   '$',  '$',    '%',   '^', '&',   '*', '(', ')',
        '_', '+', BSPACE, TAB,   'Q',  'W',    'E',   'R', 'T',   'Y', 'U', 'I',
        'O', 'P', '{',    '}',   '\n', CTRL,   'A',   'S', 'D',   'F', 'G', 'H',
        'J', 'K', 'L',    ':',   '@',  '~',    SHIFT, '|', 'Z',   'X', 'C', 'V',
        'B', 'N', 'M',    '<',   '>',  '?',    SHIFT, 0,   0,     ' ', 0,   0,
        0,   0,   0,      0,     0,    0,      0,     0,   0,     0,   0,   0,
        AUP, 0,   '0',    ALEFT, 0,    ARIGHT, '0',   0,   ADOWN, 0,   0,   0,
        0,   0,   0,      0,     0,    0,  // 90 here
};

KeyMap KEYMAP;

// void keyboard_load_mode(Mode mode) {}

// need to think about how to expose to user land safely
void keyboard_load_mapping(char* normal_layer, char* shift_layer) {
  KEYMAP.normal = normal_layer;
  KEYMAP.shift = shift_layer;
}

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

  char cout = shift ? KEYMAP.shift[scancode] : KEYMAP.normal[scancode];

  if (cout == SHIFT) {
    shift = 1;
    return;
  }

  stdin_put(cout);
}

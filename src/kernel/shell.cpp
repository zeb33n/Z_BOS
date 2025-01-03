#include "keyboard.h"
#include "memory.h"
#include "printing.h"
#include "streams.h"

// causes issues when stack allocated / doesn't get allocated on stack when
// called in a function? some compiler funny buisness?
// should have plenty of room on the stack
// Use these to print esp
// register int* esp asm("esp");
// iprintln(*esp, 16);

// TODO get rid of the flicker. only have to reprint from the cursor not the
// whole buffer

typedef struct CmdBuffer {
  int cursor;
  int size;
  char buffer[256];
} CmdBuffer;

CmdBuffer* CB;

void buffer_reset() {
  CB->cursor = -1;
  CB->size = 0;

  memset(CB->buffer, '\0', sizeof(char) * 255);
}

void buffer_remove() {
  if (CB->cursor < 0) {
    return;
  }
  for (int i = CB->cursor; i < CB->size; i++) {
    CB->buffer[i] = CB->buffer[i + 1];
  }
  CB->size--;
  CB->buffer[CB->size] = '\0';

  cursorlr(-1);
  clear_line_from_cursor();
  sprint(CB->buffer + CB->cursor);
  cursorlr(-(CB->size - CB->cursor));
  CB->cursor--;
}

void buffer_insert(char c) {
  if (CB->cursor >= 255) {
    return;
  }
  CB->cursor++;
  for (int i = CB->size; i > CB->cursor; i--) {
    CB->buffer[i] = CB->buffer[i - 1];
  }
  CB->buffer[CB->cursor] = c;

  clear_line_from_cursor();
  sprint(CB->buffer + CB->cursor);
  cursorlr(-(CB->size - CB->cursor));

  CB->size++;
}

void buffer_left() {
  if (CB->cursor < 0) {
    return;
  }
  CB->cursor--;
  cursorlr(-1);
}

void buffer_right() {
  if (CB->cursor >= CB->size - 1) {
    return;
  }
  CB->cursor++;
  cursorlr(1);
}

void shell_init() {
  buffer_reset();
  sprint("> ");
  for (;;) {
    char c = stdin_get();
    switch (c) {
      default:
        buffer_insert(c);
        break;
      case BSPACE:
        buffer_remove();
        break;
      case ALEFT:
        buffer_left();
        break;
      case ARIGHT:
        buffer_right();
        break;
      case '\n':
        sprintln("");
        sprintln(CB->buffer);
        buffer_reset();
        sprint("> ");
        break;
    }
  }
}

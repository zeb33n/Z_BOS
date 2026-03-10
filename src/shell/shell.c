#include "../drivers/keyboard.h"
#include "../drivers/printing.h"
#include "../kernel/filesystem.h"
#include "../utils/data_structures.h"
#include "../utils/memory.h"
#include "../utils/streams.h"
#include "../utils/strings.h"
#include "../utils/timer.h"

// causes issues when stack allocated / doesn't get allocated on stack when
// called in a function? some compiler funny buisness?
// should have plenty of room on the stack
// Use these to print esp
// register int* esp asm("esp");
// iprintln(*esp, 16);

typedef struct CmdBuffer {
  int cursor;
  int size;
  char buffer[256];
} CmdBuffer;

typedef struct {
  int count;
  int capacity;
  DynStr* values;
} DynTokens;

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

void tokens_alloc(char* cmd, DynTokens* tokens) {
  dyn_init((*tokens));
  for (int i = 0;; i++) {
    DynStr token;
    dyn_init(token);
    for (; (cmd[i] != ' ') && (cmd[i] != '\0'); i++) {
      dyn_append(token, cmd[i]);
    }
    dyn_append(token, '\0');
    dyn_append((*tokens), token);
    if (cmd[i] == '\0') {
      return;
    }
  }
}

void tokens_free(DynTokens tokens) {
  for (int i = 0; i < tokens.count; i++) {
    kfree(tokens.values[i].values);
  }
  kfree(tokens.values);
}

void parse_cmd(char* cmd) {
  DynTokens tokens;
  tokens_alloc(cmd, &tokens);
  if (strcmp(tokens.values[0].values, "sleep")) {
    int millis;
    if (str2uint(&millis, tokens.values[1].values, 10) != STR_SUC) {
      sprintln("ERROR");
      tokens_free(tokens);
      return;
    }
    sleep(millis);

  } else if (strcmp(tokens.values[0].values, "newfile")) {
    fs_report_status(fs_create_file(tokens.values[1].values));

  } else if (strcmp(tokens.values[0].values, "writefile")) {
    fs_report_status(fs_file_write_content(tokens.values[1].values,
                                           tokens.values[1].count,
                                           tokens.values[2].values));

  } else if (strcmp(cmd, "list")) {
    fs_list();

  } else {
    sprintln(cmd);
  }
  tokens_free(tokens);
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
        parse_cmd(CB->buffer);
        buffer_reset();
        sprint("> ");
        break;
    }
  }
}

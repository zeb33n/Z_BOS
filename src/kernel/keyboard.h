
enum special_keys {
  BSPACE = 1,
  SHIFT,
  AUP,
  ADOWN,
  ALEFT,
  ARIGHT,
  ESC,
  TAB,
  CTRL,
};

typedef struct KeyMap {
  char* normal;
  char* shift;
} KeyMap;

void keyboard_handle();

void keyboard_default();

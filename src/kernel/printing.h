#define WELCOMEMSG                                         \
  "\n"                                                     \
  "__________    __________ ________    _________\n"       \
  "\\____    /    \\______   \\\\_____  \\  /   _____/\n"  \
  "  /     /      |    |  _/ /   |   \\ \\_____  \\ \n"    \
  " /     /_      |    |   \\/    |    \\/        \\\n"    \
  "/_______ \\_____|______  /\\_______  /_______  /\n"     \
  "        \\/_____/      \\/         \\/        \\/ \n\n" \
  "[      Welcome To Zebs Operating System!     ]"         \
  "\n"

void sprintln(const char* string);

void sprint(const char* string);

void vga_init();

void iprintln(long integer, int base);

void cprint(char c);

void cdelete();

void cursorlr(int d);

void cursordu(int d);

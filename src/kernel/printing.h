#define WELCOMEMSG                                        \
  "\n"                                                    \
  "__________    __________ ________    _________\n"      \
  "\\____    /    \\______   \\\\_____  \\  /   _____/\n" \
  "  /     /      |    |  _/ /   |   \\ \\_____  \\ \n"   \
  " /     /_      |    |   \\/    |    \\/        \\\n"   \
  "/_______ \\_____|______  /\\_______  /_______  /\n"    \
  "        \\/_____/      \\/         \\/        \\/ \n"  \
  " welcome to zebs operating system!              "      \
  "\n"

void sprintln(const char* string);

void vga_init();

void iprintln(long integer, int base);

void cprint(char c);

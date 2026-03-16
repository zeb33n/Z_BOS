#ifndef PRINTING
#define PRINTING

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

#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define BROWN 6
#define LIGHT_GREY 7
#define DARK_GREY 8
#define LIGHT_BLUE 9
#define LIGHT_GREEN 0xA
#define LIGHT_CYAN 0xB
#define LIGHT_RED 0xC
#define LIGHT_MAGENTA 0xD
#define YELLOW 0xE
#define WHITE 0xF

#define BACKGROUND DARK_GREY
#define FOREGROUND WHITE
#define ERROR_COLOUR MAGENTA

void vga_init();

void sprintln(const char* string);

void sprint(const char* string);

void iprintln(long integer, int base);

void cprint(char c);

void sprintlnc(const char* string, char bg, char fg);

void sprintc(const char* string, char bg, char fg);

void iprintlnc(long integer, int base, char bg, char fg);

void cprintc(char c, char bg, char fg);

void cdelete();

void cursorlr(int d);

void cursordu(int d);

void clear_line_from_cursor();

#endif

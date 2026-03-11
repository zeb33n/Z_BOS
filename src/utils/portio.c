
void outb(short port, char byte) {
  asm volatile("outb %0, %1" : : "a"(byte), "Nd"(port));
}

char inb(short port) {
  char res;
  asm volatile("inb %1, %0" : "=a"(res) : "Nd"(port));
  return res;
}

void outw(short port, short value) {
  asm volatile("outw %w0, %1" : : "a"(value), "id"(port));
}

short inw(short port) {
  short ret;
  asm volatile("inw %1, %0" : "=a"(ret) : "dN"(port));
  return ret;
}


void outb(short port, char byte) {
  asm volatile("outb %0, %1" : : "a"(byte), "Nd"(port));
}

char inb(short port) {
  char res;
  asm volatile("inb %1, %0" : "=a"(res) : "Nd"(port));
  return res;
}

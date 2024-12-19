
void write_char (unsigned char c, unsigned char fcolour, unsigned char bcolour, int x, int y) {
    short colours = (bcolour << 4) | (fcolour & 0x0f); //background in first 4 bits foregrounf in last 4 bits
    volatile short *where; 
    where = (volatile short*)0xb8000 + (y * 80 + x); 
    *where = c | (colours << 8); // load the value into the pointer

}

extern "C" int main() {
    write_char('A', 0xA, 0x1, 30, 2);
    return 0;
}


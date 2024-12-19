extern "C" void main(){
    *(char*)0xb8000 + 2 = 'Q';
    return;
}

void write_char (unsigned char c, unsigned char fcolour, unsigned char bcolour)

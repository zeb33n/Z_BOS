
// todo move to memory file
void* memset(void* dest, unsigned char val, int count) {
  unsigned char* destC = (unsigned char*)dest;
  for (int i = 0; i < count; i++) {
    destC[i] = val;
  }
  return dest;
}

void memcopy(void* dest, void* data, int count) {
  unsigned char* destC = (unsigned char*)dest;
  unsigned char* dataC = (unsigned char*)data;
  for (int i = 0; i < count; i++) {
    destC[i] = dataC[i];
  }
}

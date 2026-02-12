// use the second sector to store a fat (file allocation table)
// 1st sector reserved for booting

typedef struct {
  int start_sector;
  int start_byte;
  int size;
} FileInfo;

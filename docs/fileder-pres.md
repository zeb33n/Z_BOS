
# Fileders

---


```
__________    __________ ________    _________
\____    /    \______   \\_____  \  /   _____/
  /     /      |    |  _/ /   |   \ \_____  \ 
 /     /_      |    |   \/    |    \/        \
/_______ \_____|______  /\_______  /_______  /
        \/_____/      \/         \/        \/ 
```

- Personal OS dev project written to learn.
- Not very many features....
---

# Demo!

---
# Add A Filesystem
---

## What is a Fileder?

### Files: 
A container for bytes (text, binary data).
`cat my_file.txt`

### Directories: 
A container for files
`ls zebs_stuff`

---

## What is a Fileder?
- **file + directory = filerectory** 

---

## What is a Fileder?
- **file + ~~directory~~ folder = fileder**

---

# Demo!

---

## Implementation
I had to write 3 components: 
1. Memory Allocator
2. ATA PIO mode Disk Driver
3. The Fileder System Itself.

I Learn best by doing. Minimal wider reading to force myself to do the thinking. 

---

## Memory Allocator
- Slab allocator
- `#define SLAB_SIZE 512`
- Contiguous slabs

---

## Memory Allocator
```
kheap_init();                                               
                                                            
┌─────┐┌───────────┐┌─────┐┌───────────┐┌─────┐┌───────────┐
│0x200││           ││0x400││           ││0x600││           │
└─────┘└───────────┘└─────┘└───────────┘└─────┘└───────────┘
──4B───                                                     
───────512──────────     
```

---

## Memory Allocator

```
void* my_ptr = kmalloc(513);                                
                                                            
┌─────┐┌────────┐   ┌─────┐┌───────────┐┌─────┐┌───────────┐
│0x400││        │...│0x600││           ││0x800││           │
└▲────┘└────────┘   └▲────┘└───────────┘└▲────┘└───────────┘
 └─0x0               └─0x400             └─0x600            
                                                            
┌─────┐┌────────┐   ┌─────┐┌───────────────────────────────┐
│0x800││        │...│  2  ││                               │
└──▲──┘└────────┘   └─────┘└┬──────────────────────────────┘
   │                        └─►return (void*)0x404;         
int* ptr = 0x0;     ───4B──                                 
ptr = (int*)0x800;         ─────────1020B───────────────────
```

---

## Memory Allocator
```
kfree(my_ptr);                                              
                                                            
┌─────┐┌────────┐   ┌─────┐┌───────────────────────────────┐
│0x800││        │...│  2  ││                               │
└─────┘└────────┘   └─────┘└───────────────────────────────┘
                                                            
┌─────┐┌────────┐   ┌─────┐┌───────────┐┌─────┐┌───────────┐
│0x800││        │...│0x600││           ││0x0  ││           │
└▲────┘└────────┘   └▲────┘└───────────┘└▲────┘└───────────┘
 └─0x0               └─0x400             └─0x600            
```

---

## Memory Allocator

Downsides:
- No defragging

---

## ATA PIO Disk Driver
- A controller Advanced Technology Attachment specification Programmed Input/Output mode.
- Kinda boring (IMO).
- reads and writes sectors (512 Bytes) at a time.
- 28bit and 48bit addressable logical block addresses.

---

## ATA PIO Disk Driver
- IDENTIFY command:
  - check drive is ATA compliant.
  - get the number of addressable sectors.
  - `outb 0xEC, 0x1F7`
  - read 512 Bytes of information about the disk (2 at a time).

---

## ATA PIO Disk Driver
- 28bit READ/WRITE command:
  - Write number of sectors to port `0x1F2`.
  - Write lba bytes to ports `0x1F3..0X1F6` (Top half of `0x1F6` reserved for drive).
  - Send READ or WRITE command to `0x1F7`.
  - read or write N sectors 2 bytes at a time.

---

## ATA PIO Disk Driver

Code!
```
void read_28bit(int drive, int lba, char sectorcount, short* ptr) {
  send_28bit_command(drive, lba, sectorcount, ptr, READ);
}

void write_28bit(int drive, int lba, char sectorcount, short* ptr) {
  send_28bit_command(drive, lba, sectorcount, ptr, WRITE);
}
```
---

## A Fileder System

- Inode like (you be the judge)
- 2 main data structures:
  1. `FreeDiskReigon` (FDR)
  2. `Fileder`

---

## A Fileder System

```
typedef struct {
  int lba;
  int n_sectors;
  int lba_next;
} FreeDiskReigon;
```
- Similar to the slabs in the memory allocator.
- Describe a reigon of dynamic size.

---

## A Fileder System
```
typedef struct {
  int parent_lba;
  DynStr name;
  DynStr content;
  DynIntArr fileders;
} Fileder;
```
- Dynamic arrays are aware of their size in bytes!
---

## A Fileder System
Writing to disk.
```
┌─────────────────┐     ┌───────────────┐     ┌──────────────┐    ┌──┬──────────────┬──┐    
│struct {         ├────►│     struct    ├────►│    struct    ├───►│  │              │  │    
│  int parent_lba;│     ├───────────────┤     ├──────────────┤    │  │              │  │    
│  struct {       │     │               │ ┌──►│    values    ├───►│  │              │  │    
│    int count;   │     │               │ │   ├──────────────┤    │  │              │  │    
│    int capacity;│     ├───────────────┤ │┌─►│    values    ├───►│  │              │  │    
│    char* values;┼────►│     values    ├─┘│  ├──────────────┤    │  └──────────────┘  │    
│  } name;        │     ├───────────────┤  │┌►│    values    ├───►│                    │    
│  struct {       │     │               │  ││ ├──────────────┤    │    ┌┬────────┬──┐  │    
│    int count;   │     │               │  ││ │              │    │    ├┼──┐     │  │  │    
│    int capacity;│     ├───────────────┤  ││ │              │    │    ││  │     │  │  │    
│    char* values;┼────►│     values    ├──┘│ │              │    │    ├┼──┘     │  │  │    
│  } content;     │     ├───────────────┤   │ │              │    └────┴┴────────┴──┴──┘    
│  struct {       │     │               │   │ │              │                              
│    int count;   │     │               │   │ │              │                              
│    int capacity;│     ├───────────────┤   │ │              │                              
│    int* values;─┼────►│     values    ├───┘ │              │                              
│  } fileders;    │     ├───────────────┤     │              │                              
│} Fileder;       │     │               │     │              │                              
└─────────────────┘     └───────────────┘     └──────────────┘                              
```
---
## A Fileder system
In code.
```
void fileder_to_disk(int lba, Fileder f) {
  FilederDiskMap map = fileder_map(f);

  char buff[map.n_sectors * 512];
  memcopy(buff, &f, sizeof(Fileder));
  memcopy(buff + map.name_index, f.name.values, f.name.count);
  memcopy(buff + map.fileders_index, f.fileders.values,
          f.fileders.count * sizeof(int));
  memcopy(buff + map.content_index, f.content.values, f.content.count);

  write_28bit(MASTER, lba, map.n_sectors, (short*)buff);
}
```
---
## A Fileder System

- Manage the lba like memory external to this function.
  - we dont always need to claim a new reigon.

```
fileder* f = fileder_new_alloc("name");
int lba = claim_disk_reigon(fileder_n_sectors(*f));
fileder_to_disk(lba, *f);
fileder_free(f);
```
---
## A Fileder System
Reading from disk
```
Fileder* fileder_from_disk_alloc(int lba) {
  union f_union {
    short arr[256];
    Fileder fileder;
  } f_header;

  read_28bit(MASTER, lba, 1, f_header.arr);

  FilederDiskMap map = fileder_map(f_header.fileder);

  char buff[map.n_sectors * 512];
  read_28bit(MASTER, lba, map.n_sectors, (short*)buff);

  Fileder* f = fileder_new_alloc(buff + map.name_index, f_header.fileder.parent_lba);

  int* fileders = (int*)(buff + map.fileders_index);
  dyn_copy_from(f->fileders, f_header.fileder.fileders.count, fileders);

  char* content = buff + map.content_index;
  dyn_copy_from(f->content, f_header.fileder.content.count, content);

  return f;
}
```
---
# Thanks for listening!

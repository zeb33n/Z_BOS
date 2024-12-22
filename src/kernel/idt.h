
extern void idt_set_gate(unsigned char num,
                         unsigned int base,
                         unsigned short sel,
                         unsigned char flags);

extern void idt_install();
extern void _idt_load();

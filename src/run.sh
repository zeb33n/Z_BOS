# TODO make into make file
 
export PATH=$PATH:/usr/local/i386elfgcc/bin

bootloader="bootloader/"
kernel="kernel/"
binaries="Binaries/"

nasm "${bootloader}boot.asm" -f bin -o "${binaries}/boot.bin"
nasm "${bootloader}kernel_entry.asm" -f elf -o "${binaries}/kernel_entry.o"
nasm "${kernel}idt.asm" -f elf -o "${binaries}/idt_entry.o"
i386-elf-gcc -ffreestanding -m32 -g -c "${kernel}kernel.cpp"  -o "${binaries}kernel.o"
i386-elf-gcc -ffreestanding -m32 -g -c "${kernel}printing.cpp"  -o "${binaries}printing.o"
i386-elf-gcc -ffreestanding -m32 -g -c "${kernel}idt.cpp"  -o "${binaries}idt.o"
nasm "${bootloader}zeroes.asm" -f bin -o "${binaries}/zeroes.bin"

i386-elf-ld -r -o "${binaries}full_idt.o" "${binaries}idt.o" "${binaries}idt_entry.o"
i386-elf-ld -o "${binaries}full_kernel.bin" -Ttext 0x1000 "${binaries}full_idt.o" "${binaries}kernel_entry.o" "${binaries}kernel.o" "${binaries}printing.o" --oformat binary

cat "${binaries}boot.bin" "${binaries}full_kernel.bin" "${binaries}zeroes.bin"  > "${binaries}OS.bin"

qemu-system-x86_64 -drive format=raw,file="Binaries/OS.bin",index=0,if=floppy,  -m 128M
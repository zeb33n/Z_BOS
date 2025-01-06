# TODO make into make file
 
export PATH=$PATH:/usr/local/i386elfgcc/bin

bootloader="bootloader/"
kernel="kernel/"
utils="utils/"
shell="shell/"
drivers="drivers/"
binaries="../Binaries/"

nasm "${bootloader}boot.asm" -f bin -o "${binaries}boot.bin"
nasm "${bootloader}kernel_entry.asm" -f elf -o "${binaries}kernel_entry.o"
nasm "${kernel}idt.asm" -f elf -o "${binaries}idt_entry.o"
i386-elf-gcc -ffreestanding -m32 -g -c "${kernel}kernel.cpp"  -o "${binaries}kernel.o"
i386-elf-gcc -ffreestanding -m32 -g -c "${kernel}idt.cpp" -o "${binaries}idt.o"
i386-elf-gcc -ffreestanding -m32 -g -c "${drivers}printing.cpp"  -o "${binaries}printing.o"
i386-elf-gcc -ffreestanding -m32 -g -c "${drivers}keyboard.cpp" -o "${binaries}keyboard.o"
i386-elf-gcc -ffreestanding -m32 -g -c "${utils}memory.cpp" -o "${binaries}memory.o"
i386-elf-gcc -ffreestanding -m32 -g -c "${utils}portio.cpp" -o "${binaries}portio.o"
i386-elf-gcc -ffreestanding -m32 -g -c "${utils}streams.cpp" -o "${binaries}streams.o"
i386-elf-gcc -ffreestanding -m32 -g -c "${shell}shell.cpp" -o "${binaries}shell.o"
nasm "${bootloader}zeroes.asm" -f bin -o "${binaries}zeroes.bin"

i386-elf-ld -o "${binaries}full_kernel.bin" -Ttext 0x1000 "${binaries}kernel_entry.o" "${binaries}kernel.o" "${binaries}printing.o"  "${binaries}idt.o" "${binaries}idt_entry.o" "${binaries}memory.o" "${binaries}portio.o"  "${binaries}keyboard.o" "${binaries}shell.o" "${binaries}streams.o" --oformat binary

cat "${binaries}boot.bin" "${binaries}full_kernel.bin" "${binaries}zeroes.bin"  > "${binaries}OS.bin"

qemu-system-x86_64 -drive format=raw,file="${binaries}OS.bin",index=0,if=floppy,  -m 128M

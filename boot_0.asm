; add to ~/.bashrc alias sto='cd ~/workspaces/github.com/lregs/os && nasm boot.asm -f bin -o boot.bin && qemu-system-x86_64 -drive format=raw,file=boot.bin -nographic'
section .text
    global_start

_start:
    mov ah, 0x0e
loop:
    inc al 
    cmp al, 'Z' + 1
    je exit 
    int 0x10 
    jmp loop 
exit:
    jmp $

times 510-($-$$) db 0
db 0x55, 0xaa
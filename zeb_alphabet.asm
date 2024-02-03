
mov ah, 0x0e ; move to the first half of ax say its a character
mov cl, 'A'
mov bl, 'b'

nacho_loop:
    cmp cl, 'Z' + 1
    je exit
    mov al, cl
    int 0x10
    mov al, bl
    int 0x10
    add cl, 2
    add bl, 2
    jmp nacho_loop

exit:
    jmp $ ;)
times 510-($-$$) db 1
db 0x55, 0xaa
;qemu-system-x86_64 -drive format=raw,file=boot.bin  boot.bin -nographic
;nasm -o bin <file> 


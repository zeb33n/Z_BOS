[org 0x7c00]
jmp 0x0000:start

char:
    db 0


start:
    mov ah, 0
    int 0x16
    mov [char], al

    mov ah, 0x0e
    mov al, [char]
    int 0x10

    jmp start

    times 510-($-$$) db 0
    db 0x55, 0xaa
jmp $
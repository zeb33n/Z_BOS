[org 0x7c00]
jmp 0x0000:start

char:
    db 0


start:
    mov ah, 0
    int 0x16 ; type interupt
    mov [char], al ; moves al into char 

    mov ah, 0x0e ;moves ax into teletype
    mov al, [char] ; char into al
    int 0x10 ; prints

    jmp start ; restarts

    times 510-($-$$) db 0
    db 0x55, 0xaa
jmp $
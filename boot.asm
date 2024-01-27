[org 0x7c00] ;move to here in memory. 
mov ah, 0x0e ;start teletype
mov al, 2
mov bx, string

string:
    db "Hello will", 0

loop:
    mov al, [bx]
    cmp al, 0
    je exit
    int 0x10 ;print
    inc bx
    jmp loop

exit:
    jmp $

times 510-($-$$) db 0
db 0x55, 0xaa
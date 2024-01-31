[org 0x7c00] ;move to here in memory. 
mov ah, 0 ;start teletype


loop:
    mov al
    cmp al, 0
    je exit
    int 0x16 ;print
    inc bx
    jmp loop

exit:

jmp $



times 510-($-$$) db 0
db 0x55, 0xaa
[org 0x7c00]

mov sp, 0
mov bp, sp

start:
    mov ah, 0 ; i dont understand this
    int 0x16 ; record keys
    cmp al, 0x0D ; representation of enter key
    je printStack
    mov ah, 0x0e
    int 0x10
    push ax
    jmp start

printStack:
    mov al, 0
    mov sp, bp
    push ax 
    loop:
        mov ax, [esp]
        sub sp, 2
        int 0x10
        cmp ax, 0 
        je exit
        jmp loop

exit: 
    jmp start


times 510 - ($-$$) db 0
db 0x55, 0xaa
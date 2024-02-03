[org 0x7c00]

; create the buffer 

; create 1 bit buffer

; record keys
; if key is enter
;   print buffer
; add char to buffer
; increment buffer pointer


mov bx, buffer ; initialise buffer

start:
    mov ah, 0 ; i dont understand this
    int 0x16 ; record keys
    cmp al, 0x0D ; representation of enter key
    je printBuffer
    mov ah, 0x0e
    int 0x10
    mov [bx], al 
    inc bx
    jmp start

printBuffer:
    mov bx, buffer
    mov ah, 0x0e
    loop: 
        mov al, [bx]
        cmp al, 0
        je exit
        int 0x10 ;print
        inc bx
        jmp loop


exit: 
    mov bx, buffer
    jmp start

buffer: 
    db 0

times 510 - ($-$$) db 0
db 0x55, 0xaa

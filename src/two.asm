; [org 0x7c00] ; set required offset for dereferencing characters from buffers

; lets refactor this to use the stack instead of a buffer

xor al, al ; set al to 0

main: 
        mov sp, bp; reset the stack

        mov ah, 0x0e ; teletype mode
        mov al, 10 ; newline character
        int 0x10 ; print interupt
        int 0x10 
        mov al, '>'
        int 0x10
        mov al, ' '
        int 0x10

        loop:
                mov ah, 0
                int 0x16 ; wait for keypress

                mov ah, 0x0e ; teletype mode
                int 0x10

                cmp al, 0x0D ; enter key
                je printBuffer

                push ax

                jmp loop

printBuffer: 
        mov ah, 0x0e

        mov al, 10 ; newline character
        int 0x10

        mov si, bp ;source index

        ploop: 
                mov al, [si]

                cmp si, sp ; if end of string
                je main
                
                int 0x10 ;print
                
                sub si, 1

                jmp ploop
                
        


jmp $ 


times 510 - ($ - $$) db 0 ;510 bytes minus previous code define as 0 
db 0x55, 0xaa ;special code that defines boot sector

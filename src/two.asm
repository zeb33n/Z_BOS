[org 0x7c00] ; set required offset for dereferencing characters from buffers

main: 
        xor ax, ax
        mov sp, bp; reset the stack

        mov ah, 0x0e ; teletype mode
        mov al, 0dh ; for some reason need carrage return here
        int 0x10    ; i.e move the cursor to the left

        mov al, 10 ; newline character
        int 0x10 ; print interupt
        int 0x10 
        mov al, '>'
        int 0x10
        mov al, ' '
        int 0x10

        xor ah, ah ;clear ah

        loop:
                int 0x16 ; wait for keypress

                mov ah, 0x0e ; teletype mode
                int 0x10

                cmp al, 0x0D ; enter key
                je printBuffer

                xor ah, ah ;clear ah
                push ax

                jmp loop

printBuffer: 
        mov ah, 0x0e
        mov al, 10 ; newline character
        int 0x10

        mov si, bp ;source index

        ploop: 
                cmp si, sp ; if end of string
                je main

                sub si, 2 ;move pointer

                mov al, [si]
                
                int 0x10 ;print

                jmp ploop
                
        


jmp $ 

times 510 - ($ - $$) db 0 ;510 bytes minus previous code define as 0 
db 0x55, 0xaa ;special code that defines boot sector

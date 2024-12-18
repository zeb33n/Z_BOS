[org 0x7c00] ; set required offset for dereferencing characters from buffers

xor al, al ; set al to 0

main: 
        mov bx, string ; pointer to bx 

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

                mov [bx], al
                inc bx
        
                cmp al, 0x0D ; enter key
                je printBuffer

                ;need to compare size of buffer with offset

                jmp loop

printBuffer: 

        mov bx, string 
        mov ah, 0x0e

        mov al, 10 ; newline character
        int 0x10

        xor cl, cl; initialise register for cleaning buffer
        ploop: 
                mov al, [bx]

                cmp al, 0 ; if end of string
                je main
                
                int 0x10 ;print
                
                mov [bx], cl ;reset buffer needs to use a register to know size
                inc bx

                jmp ploop
                
        


jmp $ 


string: ; buffer for input 
        times 50 db 0

times 510 - ($ - $$) db 0 ;510 bytes minus previous code define as 0 
db 0x55, 0xaa ;special code that defines boot sector

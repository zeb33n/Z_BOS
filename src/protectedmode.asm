[org 0x7c00]                        
      

mov [BOOT_DISK], dl                 



CODE_SEG equ GDT_code - GDT_start
DATA_SEG equ GDT_data - GDT_start

cli
lgdt [GDT_descriptor] ; load gdt descriptor
mov eax, cr0 ; load mode into register
or eax, 1
mov cr0, eax
jmp CODE_SEG:start_protected_mode

jmp $
                                    
                                     
GDT_start:                          ; must be at the end of real mode code
    GDT_null:
        dd 0x0
        dd 0x0

    GDT_code:                       ; ppp 1001 1if segment is used 00 ring priveldge 1 for data or code
        dw 0xffff                   ; type flag 1010 code1, conforming0, readable1, cpuflag0
        dw 0x0                      ; other flags 1100 granularity use more memory and 32bit mode
        db 0x0                      ; 0xffff define meory used set to max 
        db 0b10011010
        db 0b11001111
        db 0x0

    GDT_data:
        dw 0xffff
        dw 0x0
        db 0x0
        db 0b10010010               ;defining the data segment as above
        db 0b11001111
        db 0x0

GDT_end:

GDT_descriptor:
    dw GDT_end - GDT_start - 1     ;size
    dd GDT_start


[bits 32]
start_protected_mode:
    mov cl, 'Z'          ;the secret hiddne letter
    mov ch, 0x00         ;background and foreground to blue
    mov ebx, 0xb8000     ;video memory start

    bloop: 
        cmp ebx, 0xb8FA0  ; paint whole background
        je bend
        mov [ebx], cx
        add ebx, 2 
        jmp bloop

    bend: 

    
    mov ebx, 0xb8348
    mov ch, 0x90 ; start on the light blue background
    mov eax, welcomeMessage ; in 32 bit mode you have to use eax
    zloop: 
        mov cl, [eax] 
        cmp cl, 0 
        je zend
        mov [ebx], cx
        inc eax 
        add ebx, 2
        add ch, 0x10
        cmp ch, 0xF0
        je reset
        jmp zloop

    reset:
        mov ch, 0x90
        jmp zloop

    zend:
        
    jmp $

    welcomeMessage: 
        db "welcome to ZebOS!", 0 

BOOT_DISK: db 0                                     
 
times 510-($-$$) db 0              
dw 0xaa55
                                

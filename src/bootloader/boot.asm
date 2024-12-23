[org 0x7c00]                        ; start of data segment
KERNEL_LOCATION equ 0x1000
                                    

mov [BOOT_DISK], dl                 

                                    
xor ax, ax                           ;starting at 0x0
mov es, ax
mov ds, ax
mov bp, 0x8000
mov sp, bp

mov bx, KERNEL_LOCATION ; we want to load the kwrnwl to the kernel location
mov dh, 2 ;might have to change this number. number of sectors to read

mov ah, 0x02
mov al, dh  ; number of sectors
mov ch, 0x00
mov dh, 0x00
mov cl, 0x02
mov dl, [BOOT_DISK]
int 0x13                ;load from disk

                       ; error managment
jc error               ; jump if carry flag is set
cmp al, 2
jne error

jmp endload


error: 
    mov ah, 0x0e 
    mov bx, errorMsg

eloop:
    mov al, [bx]
    cmp al, 0
    je endload
    int 0x10
    inc bx 
    jmp eloop

endload:
                                    
mov ah, 0x0
mov al, 0x3
int 0x10                ; text mode


CODE_SEG equ GDT_code - GDT_start
DATA_SEG equ GDT_data - GDT_start

cli
lgdt [GDT_descriptor]
mov eax, cr0
or eax, 1
mov cr0, eax
jmp CODE_SEG:start_protected_mode

jmp $
                                
errorMsg: 
    db "error", 0 
                                    
BOOT_DISK: db 0

GDT_start:
    GDT_null:
        dd 0x0                         ; we get 4gb of memory in protected mode
        dd 0x0                         ; flat memory model

    GDT_code:
        dw 0xffff                      ; we only define the first 16bits of the limit the real value is 0xfffff 
        dw 0x0                         ; we multiply this value by 0x1000 as described in the flags
        db 0x0
        db 0b10011010                  
        db 0b11001111                  ; the last 4 bits of the limit are described here 0b1111
        db 0x0

    GDT_data:
        dw 0xffff
        dw 0x0
        db 0x0
        db 0b10010010
        db 0b11001111
        db 0x0

GDT_end:

GDT_descriptor:
    dw GDT_end - GDT_start - 1
    dd GDT_start

; need to refactor this file
; load the idtp

[bits 32]
start_protected_mode:
    mov ax, DATA_SEG ;segment registers ; the value is 16 
	mov ds, ax ;data segment
	mov ss, ax ;stack segment
	mov es, ax ;extra segments
	mov fs, ax
	mov gs, ax

	mov ebp, 0xA0000		; 32 bit stack base pointer
	mov esp, ebp

    ; ; print digit    
    ; mov ax, ax
    ; mov ecx, 0xb8000 ;print digit
    ; print_dec: 
    ;     xor dx, dx
    ;     mov bx, 10
    ;     div bx ; remainder goes to dx

    ;     xor ah, ah
        
    ;     mov bl, dl
    ;     add bl, 0x30
    ;     mov bh, 0x07
    ;     mov [ecx], bx    

    ;     add ecx, 2
    ;     cmp ax, 0
    ;     jne print_dec

    
    jmp KERNEL_LOCATION

                                     
 
times 510-($-$$) db 0              
dw 0xaa55

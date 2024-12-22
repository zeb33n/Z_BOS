
[org 0x7c00] ; origin in the data segment

; chs addressing

mov [diskNum], dl

mov ah, 2 ;not sure why
mov al, 1 ;number of sectors to read
mov ch, 0 ;cylinder 0
mov cl, 2 ;sector number
mov dh, 0 ;head number
mov dl, [diskNum]
mov bx, 0 ;cant move 0 directly to es
mov es, bx
mov bx, 0x7e00 ; es*16+bx = where we want to load the data
int 0x13 ;call the function (kinda lol)

;check for errors
jc error ;jump if carry flag is set
jmp print

cmp al, 1
jne error
jmp print


error: 
        mov ah, 0x0e 
        mov bx, errorMsg
        eloop:
                mov al, [bx]

                cmp al, 0
                je end

                int 0x10
                inc bx 

                jmp eloop
                


print:
        mov ah, 0x0e ;print the loaded memory
        mov al, [0x7e00]
        int 0x10

end:

jmp $ 

diskNum: 
        db 0

errorMsg: 
        db "error", 0 

times 510 - ($ - $$) db 0 ;510 bytes minus previous code define as 0 
db 0x55, 0xaa ;special code that defines boot sector

times 512 db 'A' ;fill sector with As a sector is 512 bytes long

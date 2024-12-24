
global _idt_load

_idt_load:
        [extern _idtp]
	lidt [_idtp]
	ret

global _isr_generic

[extern _fault_handler] 


_isr_generic:
	pusha
	push ds
	push es
	push fs
	push gs

	cli
	call _fault_handler
	hlt 
	
	pop gs
	pop fs
	pop es
	pop ds
	popa
	iret		

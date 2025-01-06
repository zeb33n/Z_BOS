
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
	call _fault_handler		   ; checks if interrupt number < 32 (if it represents an exception)
	hlt 
	
	pop gs
	pop fs
	pop es
	pop ds
	popa
	iret		


global _irq_keyboard

[extern _keyboard_handler]

_irq_keyboard:
	pusha
	push ds
	push es
	push fs
	push gs

	call _keyboard_handler
	
	pop gs
	pop fs
	pop es
	pop ds
	popa
	iret	

global _irq_under_40

[extern _less_than_40]

_irq_under_40:
	pusha
	push ds
	push es
	push fs
	push gs

	call _less_than_40
	
	pop gs
	pop fs
	pop es
	pop ds
	popa
	iret	

global _irq_over_39

[extern _more_than_39]

_irq_over_39:
	pusha
	push ds
	push es
	push fs
	push gs

	call _more_than_39
	
	pop gs
	pop fs
	pop es
	pop ds
	popa
	iret	

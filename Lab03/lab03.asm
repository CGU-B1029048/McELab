;define control registers (with address)
XBR2		equ		0e3h
P1MDIN		equ		0adh
P2MDOUT		equ		0a6h
WDTCN		equ		0ffh
SFRPAGE		equ		084h
P1			equ		090h
P2			equ		0a0h

;define control registers for timer control
TMOD		equ		089h
TCON		equ		088h
CKCON		equ		08eh
IE			equ		0a8h
TL0			equ		08ah
TH0			equ		08ch

;define control words
CONFIG_PAGE		equ		0fh
LEGACY_PAGE		equ		00h

		org		0h
		ljmp	main

		org		0bh
		ljmp	Timer0_ISR

		org		0100h
main:
		lcall	Port_Config
		lcall	Timer_Config
        mov     P1, #0
        mov     P2, #0
loop:	
		mov	    A, P1
        jnz     store
        mov     P2, R3
		sjmp	loop
store: ;Store status of button P1 when P1 not zero
        mov    R2, P1
        jnb    R2.7, Loop_RL_init
        jnb    R2.6, Loop_RL_init
        sjmp   loop
Loop_RL_init:
        mov    R3, #00000001b
        sjmp   loop

Loop_R:
    mov A, R3
    rr A
    mov R3, A
    ljmp reset_timer

Loop_L:
    mov A, R3
    rl A
    mov R3, A
    ljmp reset_timer

Port_Config:
		;turn-off the watch-dog timer
		mov		WDTCN, #0deh
		mov		WDTCN, #0adh

		;setup port configuration
		mov		SFRPAGE, #CONFIG_PAGE
		mov		XBR2, #0c0h
		mov		P1MDIN, #0ffh
		mov		P2MDOUT, #0ffh
		mov		SFRPAGE, #LEGACY_PAGE
		ret

Timer_Config:
		mov		TMOD, #01h
		mov		TCON, #010h
		mov		CKCON, #010h
		mov		IE, #082h
		mov		TL0, #0
		mov		TH0, #0
		ret

Timer0_ISR:
        jb R2.7, Loop_R

		jb R2.6, Loop_L

		;jb R2.5, Loop_odd

		;jb R2.4, Loop_even
reset_timer:
		mov		TL0, #0
		mov		TH0, #0
		reti
		end









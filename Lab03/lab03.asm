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
    mov     P1, #0              ;init P1,P2 = 0
    mov     P2, #0
loop:	
    mov	    A, P1
    jnz     store               ;if P1 pressed, store val in store flag
    mov     P2, R3              ;store LED into P2
    sjmp	loop
store: ;Store status of button P1 when P1 not zero
    mov     R2, A               ;Store status to A
    mov     B, R2
    jb      B.7, Loop_RL_init   ;init R or L loop
    jb      B.6, Loop_RL_init
    sjmp    loop
Loop_RL_init:
    mov    R3, #00000001b       ;init LED for R/L loop
    sjmp   loop






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





Loop_R: ;Right shift loop
    mov     A, R3
    rr      A
    mov     R3, A
    ljmp    reset_timer

Loop_L: ;Left Shift loop
    mov     A, R3
    rl      A
    mov     R3, A
    ljmp    reset_timer

Timer0_ISR:
        mov     R0, A           ;stash store val in Reg 
        mov     R1, B
        mov     B, R2

        jb      B.7, Loop_R     ;jump control
		jb      B.6, Loop_L
		jb      B.5, Loop_odd
		jb      B.4, Loop_even

reset_timer:
		mov		TL0, #0
		mov		TH0, #0
        mov     A, R0           ;restore stash
        mov     B, R1
		reti
		ends
;define control registers (w/ addr)
XBR2     equ  0e3h
P1MDIN   equ  0adh
P2MDOUT  equ  0a6h
WDTCN    equ  0ffh
SFRPAGE  equ  084h
P1       equ  090h
P2       equ  0a0h

;define control words
CONFIG_PAGE  equ  0fh
LEGACY_PAGE  equ  00h

    ;turn-off watch-dog timer
    mov  WDTCN, #0deh
    mov  WDTCN, #0adh

    ;setup port config
    mov SFRPAGE, #CONFIG_PAGE
    mov XBR2   , #0c0h
    mov P1MDIN , #0ffh
    mov P2MDOUT, #0ffh
    mov SFRPAGE, #LEGACY_PAGE

		;detect and set button, restart P1 and P2
		mov P1, #0
		mov P2, #0

;P1.7 pressed,
MAIN:
		MOV A, P1
		JZ MAIN

    JB P1.7, Loop_R

		JB P1.6, Loop_L

		JB P1.5, Loop_odd

		JB P1.4, Loop_even

Loop_R:
    MOV P2, A
    LCALL Delay
    RR A
    LJMP Loop_R

Loop_L:
    MOV P2, A
    LCALL Delay
    RL A
    LJMP Loop_L

Loop_odd:
		MOV P2, A
    LCALL Delay
    RL A
    LJMP Loop_L



Delay: MOV R0, #50
Delay0: MOV R1, #40
Delay1: MOV R2, #249
Delay2: DJNZ R2, Delay2
        DJNZ R1, Delay1
        DJNZ R0, Delay0
        RET
END

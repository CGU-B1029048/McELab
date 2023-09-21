# Microcontroller Experiment Lab03 Report
- [Microcontroller Experiment Lab03 Report](#microcontroller-experiment-lab03-report)
  - [Problem Description](#problem-description)
    - [Goal of Lab03](#goal-of-lab03)
    - [Detail Description](#detail-description)
  - [Code \& Explanation](#code--explanation)
  - [Difficulties Encountered and Solutions](#difficulties-encountered-and-solutions)
  - [Disscussion](#disscussion)
## Problem Description
### Goal of Lab03
+

### Detail Description

<div style="break-after: page; page-break-after: always;"></div>

## Code & Explanation
```asm
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
		mov A, P1
        mov R3, #00000001b
		jz MAIN

        jb P1.7, Loop_R

		jb P1.6, Loop_L

		jb P1.5, Loop_odd

		jb P1.4, Loop_even

Loop_R:
    mov P2, R3
    LCALL Delay
    mov A, R3
    rr A
    mov R3, A
    mov A, P1
    jnz MAIN
    LJMP Loop_R

Loop_L:
    mov P2, R3
    LCALL Delay
    mov A, R3
    RL A
    mov R3, A
    mov A, P1
    jnz MAIN
    LJMP Loop_L

Loop_odd:
    mov P2, #10101010b
    LCALL Delay
    mov P2, #00000000b
    LCALL Delay
    mov A, P1
    jnz MAIN
    LJMP Loop_odd

Loop_even:
    mov P2, #01010101b
    LCALL Delay
    mov P2, #00000000b
    LCALL Delay
    mov A, P1
    jnz MAIN
    LJMP Loop_even

Delay: MOV R0, #50
Delay0: MOV R1, #40
Delay1: MOV R2, #249
Delay2: DJNZ R2, Delay2
        DJNZ R1, Delay1
        DJNZ R0, Delay0
        RET
END

```
[](.png)
<div style="break-after: page; page-break-after: always;"></div>

## Difficulties Encountered and Solutions

## Disscussion

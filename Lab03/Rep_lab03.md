# Microcontroller Experiment Lab03 Report
- [Microcontroller Experiment Lab03 Report](#microcontroller-experiment-lab03-report)
  - [Problem Description](#problem-description)
  - [Code \& Explanation](#code--explanation)
  - [Difficulties Encountered and Solutions](#difficulties-encountered-and-solutions)
  - [Disscussion](#disscussion)
## Problem Description
+ Program 8051 to show some LED patterns like last week
+ But control using a timer and interrupt mechanism
+ Pattern
  * Right shift
  * Left shift
  * Blink odd LED
  * Blink even LED

<div style="break-after: page; page-break-after: always;"></div>

## Code & Explanation
```arm
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
```
Define the pin, control words and register, Timer and Port configuration.
```arm
main:
    lcall	Port_Config
    lcall	Timer_Config
    mov     P1, #0              ;init P1,P2 = 0
    mov     P2, #0
    mov     R4, #4              ;loop time delay for ISR
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
```
We first setup the config, init P1,P2 and setup the delay of interrupt, which will run the code in ISR every 4 time of interrupt. We use P1 as the button, so we use `loop` to detect if button is pressed, if so, we goto `store` to store the button status, i.e. which button is pressed.
[](.png)
<div style="break-after: page; page-break-after: always;"></div>

## Difficulties Encountered and Solutions
Before our code at Lab02 is written using the `delay` function loop, so when we use it to make our Lab03, we encountered some problem. Since we're using interrupt at Lab03, so we cannot use `delay` at each loop of `Loop_R`, `Loop_L`, etc. Instead, we have to change it to make it fit the mode of interrupt, thus doing one thing at an interrupt cycle only. After a long time trying, we finally finished by making blink odd/even by detect the state of LED first, then decide to blink or turn off light.
But Later we encountered another problem, our code still failed, and we suffer so long but still cannot find the problem. After asking the teacher, we found that is because our code in interrupt didn't store our data in registers first, thus when interrupt happened while a loop is still runing, the data used in the loop will be overwritten and lost. So we store the data in advance at the begining of the ISR, problem solved.
## Disscussion


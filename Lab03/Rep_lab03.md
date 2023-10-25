# Microcontroller Experiment Lab03 Report

- [Microcontroller Experiment Lab03 Report](#microcontroller-experiment-lab03-report)
  - [Problem Description](#problem-description)
  - [Code \& Explanation](#code--explanation)
  - [Difficulties Encountered and Solutions](#difficulties-encountered-and-solutions)
  - [Disscussion](#disscussion)
  - [Appendix](#appendix)
    - [Bonus](#a-bonus---led_shiftc-explain)
    - [full code](#b-full-code)

## Problem Description

- Program 8051 to show some LED patterns like last week
- But control using a timer and interrupt mechanism
- Pattern
  - Right shift
  - Left shift
  - Blink odd LED
  - Blink even LED

<div style="break-after: page; page-break-after: always;"></div>

## Code & Explanation

```nasm
;define control registers (with address)
XBR2        equ        0e3h
P1MDIN        equ        0adh
P2MDOUT        equ        0a6h
WDTCN        equ        0ffh
SFRPAGE        equ        084h
P1            equ        090h
P2            equ        0a0h

;define control registers for timer control
TMOD        equ        089h
TCON        equ        088h
CKCON        equ        08eh
IE            equ        0a8h
TL0            equ        08ah
TH0            equ        08ch

;define control words
CONFIG_PAGE        equ        0fh
LEGACY_PAGE        equ        00h

        org        0h
        ljmp    main

        org        0bh
        ljmp    Timer0_ISR

        org        0100h

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
```

<div style="break-after: page; page-break-after: always;"></div>

```nasm
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

```nasm
main:
    lcall    Port_Config
    lcall    Timer_Config
    mov     P1, #0              ;init P1,P2 = 0
    mov     P2, #0
    mov     R4, #4              ;loop time delay for ISR
loop:    
    mov        A, P1
    jnz     store               ;if P1 pressed, store val in store flag
    mov     P2, R3              ;store LED into P2
    sjmp    loop
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

We first setup the config, init P1,P2 and setup the delay of interrupt, which will run the code in ISR every 4 time of interrupt. We use `P1` as the button, so we use `loop` to detect if button is pressed, if so, we goto `store` to store the button status, i.e. which button was pressed. If button `P1.7`, `P1.6` is pressed, function "left shift" /" right shift", we jump to `Loop_RL_init` after `store` to set the shift start from LED `P2.0`.

<div style="break-after: page; page-break-after: always;"></div>

```nasm
Timer0_ISR:
        DJNZ    R4, reset_timer
        mov     R4, #4
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
		end
```

Then we goes to ISR, Since the code will jump to ISR interrupt every time the timer reach 0, but the delay is still to fast, so we use `DJNZ` so that it'll reset the timer through `reset_timer`. The process will repeat 4 time, making the delay 4 times longer.

Then we store the value of using registers `A` nd `B` to stash sorage `R0`, `R1`, then goes to jump control part, which will determine which process to jump to by the button value stored in `R2` using instruction`jb`.

<div style="break-after: page; page-break-after: always;"></div>

```nasm
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

Loop_odd:
    mov     A, R3
    jnz     Loop_odd_even_nz
    mov     R3, #10101010b
    sjmp    reset_timer

Loop_even:
    mov     A, R3
    jnz     Loop_odd_even_nz
    mov     R3, #01010101b
    sjmp    reset_timer

Loop_odd_even_nz:
    mov     R3, #0
    sjmp    reset_timer   
```

After ISR jump to each designated loop, we begin to implement the LED flashing functions.

`Loop_R`, `Loop_L` are shifting loop, since we have initialize the LED value at button pressed (see code above), we just need to shift the value of LED, which stores in `R3`, everytime ISR runs to this loop, so we simply use `rr`, `rl` to do it.

`Loop_odd`, `Loop_even` requires different way to implement. Since using ISR, delay function can't be used, so we detect the value of LED first. Since this is blink, so if LED is on, we've to turn it off light, otherwise chage lit the LED pattern, so the blink can be implemented.

<div style="break-after: page; page-break-after: always;"></div>

## Difficulties Encountered and Solutions

Before our code at Lab02 is written using the `delay` function loop, so when we use it to make our Lab03, we encountered some problem. Since we're using interrupt at Lab03, so we cannot use `delay` at each loop of `Loop_R`, `Loop_L`, etc. Instead, we have to change it to make it fit the mode of interrupt, thus doing one thing at an interrupt cycle only. After a long time trying, we finally finished by making blink odd/even by detect the state of LED first, then decide to blink or turn off light.
But Later we encountered another problem, our code still failed, and we suffer so long but still cannot find the problem. After asking the teacher, we found that is because our code in interrupt didn't store our data in registers first, thus when interrupt happened while a loop is still runing, the data used in the loop will be overwritten and lost. So we store the data in advance at the begining of the ISR, problem solved.

## Disscussion

After this Lab, I understand the basic of ISR, and the usage with it. By using assembly to code it, It's really quite a challenge. But after finished it, our knowledge can be improve so much. The example file also give us the version written in C, which will be the language we code in the later Lab. Hope we can have a smooth Lab later with C successfully.

<div style="break-after: page; page-break-after: always;"></div>

## Appendix

### A. Bonus - `LED_shift.c` explain

`Port_Configuration ()` and `Timer_Configuration ()`

```c
void
Port_Configuration ()
{
	//initialize SFR setup page
	SFRPAGE = CONFIG_PAGE;                 // Switch to configuration page

	//setup the cross-bar and configure the I/O ports
	XBR2 = 0xc0;
	P2MDOUT = 0xff;

	//set to normal mode
	SFRPAGE = LEGACY_PAGE;
}//end of function Port_Configuration

void
Timer_Configuration ()
{
	TMOD = 0x01;
	TCON = 0x10;
	CKCON = 0x10;

	IE = 0x82;
	TL0 = 0;
	TH0 = 0;
}//end of function Timer_Configuration

```

Set up the coniguration of Port and Timer, such as the `P2MDOUT` for LED output in port_config or the timer mode settings `TMOD`, `TCON` for timer mode.

<div style="break-after: page; page-break-after: always;"></div>

`Config()`

```c
void
Config ()
{
	//turn-off watch-dog timer
	WDTCN = 0xde;
	WDTCN = 0xad;

	OSCICN = 0x83;
	CLKSEL = 0x00;

	Port_Configuration ();

	Timer_Configuration ();
}//end of function Default_Config
```

The configuration function wihich will call for `Port_Configuration ()`and`Timer_Configuration ()` explained above, and some other coonfig such ass turning off watch-dog timer

**The `main` function***

```c
int main () 
{
	Config ();
	status = 0x80;
	count = 0;

	while (1) {
		P2 = status;

		/*
		if (TCON & 0x20) {
			Timer0_ISR ();
			TCON = 0x10;
		}
		*/
	}//end while (1)
}//end of function main

```

The main function actually only have 1 simple job. After setting up the basic `config()` and declare varibles, it repeats to send the status to `P2`, i.e change the LED light as the data show in `status`

<div style="break-after: page; page-break-after: always;"></div>

**The `ISR` function**

```c
void Timer0_ISR () interrupt 1
{
	count++;

	if (count==4) {
		count = 0;
		status = status>>1;
		if (status==0) status=0x80;
	}

	TH0 = 0;
	TL0 = 0;
}//end of function Timer0_ISR
```

The code above is in ISR, so it'll interrupt to do the code in ISR every time the timer reaches 0. It first extand the interrupt 間隔 by using a simple if-state. So count++ every time, and the code inside will only be run when every 4<sup>th</sup> time of interrupt, otherwise will go straight to the timer reset below.

For code inside the If-state, it right shift the value of status, and will round back to 128 (0x80) when it gets to 0. So the whole program will be running a Right shift LED at `P2`.

### B. Full code

**written version**

```nasm
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
    mov     R4, #4
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

Loop_odd:
    mov     A, R3
    jnz     Loop_odd_even_nz
    mov     R3, #10101010b
    sjmp    reset_timer

Loop_even:
    mov     A, R3
    jnz     Loop_odd_even_nz
    mov     R3, #01010101b
    sjmp    reset_timer

Loop_odd_even_nz:
    mov     R3, #0
    sjmp    reset_timer    

Timer0_ISR:
        DJNZ    R4, reset_timer
        mov     R4, #4
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
		end<div style="break-after: page; page-break-after: always;"></div>
```

**Picture version**

****

![](https://lh7-us.googleusercontent.com/jlY1nJMPnIHQ-VeM_HpHFb3QTQD3FMRexTx_K2pgblQoOw0bDJDWxPSrQzyMf1KZbiZhE70cdisEVwI906ALM3uiQZbdCor_a2uwXSJQIzdTr-wYl2w2Tfu4asUUGalWleB5ESMZoOZGh2_S51SEhNw)****

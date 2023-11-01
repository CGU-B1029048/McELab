# Microcontroller Experiment Lab04 Report

- [Microcontroller Experiment Lab04 Report](#microcontroller-experiment-lab04-report)
  - [Problem Description](#problem-description)
  - [Code \& Explanation](#code--explanation)
    - [Basic Part 1 - Pin Define](#basic-part-1---pin-define)
    - [Basic Part 2 - button Detect](#basic-part-2---button-detect)
    - [Basic Part 3 - Main function](#basic-part-3---main-function)
    - [Bonus Part 1 - Pin Define](#bonus-part-1---pin-define)
    - [Bonus Part 2 - Button Detect](#bonus-part-2---button-detect)
    - [Bonus Part 3 - Main](#bonus-part-3---main)
    - [Bonus Part 4 - Timer ISR](#bonus-part-4---timer-isr)
  - [Difficulties Encountered and Solutions](#difficulties-encountered-and-solutions)
  - [Disscussion](#disscussion)
  - [Appendix](#appendix)
    - [full code - basic](#full-code---basic)
    - [full code - Bonus](#full-code---bonus)
    - [picture of code - basic](#picture-of-code---basic)
    - [picture of code - bonus](#picture-of-code---bonus)
    
## Problem Description
Same as Lab03, but instead of using an `.asm` file to implement, we use C to write the same program, including Timer ISR.**Only C programming is allowed** for this lab!
+ Basic Part: (25%)
  + Two buttons, one for 1-bit left shift and one for 1-bit
    right shift
  + Initialize the LED with one bit set to 1, and others to 0
+ Advanced Part: (65%)
  + The LED runs 4 modes (Just like what we did before)
  + As a button is pressed, the mode changes. 
  + Mode-changing sequence: Mode 0 → Mode 1 → Mode 2 → Mode 3 → Mode 0 ... (Mode 0-3: change LED pattern every sec. Refer to Lab2-3)
+ Hint: use the timer interrupt to make two I/O devices work simultaneously

<div style="break-after: page; page-break-after: always;"></div>

## Code & Explanation
`Basic.c` for basic part
### Basic Part 1 - Pin Define
```c
#include "C8051F040.h"
sbit patten01 = P1^7;
sbit patten02 = P1^6;
int mode;

//set port address
void port_Configuration (){
	XBR2 = 0xc0;
	P1MDIN = 0xff;
	P2MDOUT = 0xff;
}//end of function Port_Configuration

void default_Config (){
	//turn-off watch-dog timer
	// disable watchdog timer
	WDTCN = 0xde;
	WDTCN = 0xad;

	//initialize SFR setup page
	SFRPAGE = CONFIG_PAGE;                 // Switch to configuration page

	port_Configuration ();

	//set to normal mode
	SFRPAGE = LEGACY_PAGE;
}//end of function Default_Config

```
We set the configuration for Pin like `P2`, `P1`, LED and button.

<div style="break-after: page; page-break-after: always;"></div>

### Basic Part 2 - button Detect
```c
void button_detect (){
	char key_hold;
	int key_release;
	int count;

	do {
		key_hold = patten01 || patten02;
	} while (!key_hold);

	//Stage 2: wait for key released
	if (patten01) {
		mode = 0;
	} else {
		mode = 1;
	}
	key_release = 0;
	count = 1000;
	while (!key_release) {
		key_hold = patten01 || patten02;
		if (key_hold) {
			count = 50;
		} else {
			count--;
			if (count==0) key_release = 1;
		}
	}//Stage 2: wait for key released
}//end of function button_detect ()

void mode_change(){
	default_Config ();

	button_detect ();
}//end of function main
```
We modify the `button_detect()` given by the example, and add to let it detect either button `P1.7` or `P1.6` was pressed or not.The function of `buttton__detect()` is when pressed, the `button_detect()` will stop running the while loop, letting the program to free to do things after `button_detect()`, if the pulse of button stays long enough for `key_release` to release the key.
We also made it to detect which button is pressed using the code below the comment stage 2, which will decide mode by the value of `P1.7`.
|mode|LED pattern|button|
|:--|:--|:--|
|0|left shift 1 bit|`P1.7`|
|1|right shift 1 bit|`P1.6`|

### Basic Part 3 - Main function

```c
int main(){
	int status;
	status = 1;
	P2 = 0;
	while(1){
		mode_change();
		P2 = status;
	
		if (mode == 0) {
			if (P2 == 128){
				status = 1;
			} else {
				status = status << 1;
			}
			P2 = status;
		}//end while (patten01)

		if (mode == 1) {
			if (P2 == 1){
				status = 128;
			} else {
				status = status >> 1;
			}
			P2 = status;
		}//end while (patten01)
	}
}
```
Aftee button pressed, we'll received the mode by which button is pressed, and thus got through the if statement below to do either left shift 1 bit or right shift 1 bit. And thus the program finished, going back to wait another button pressed signal.

<div style="break-after: page; page-break-after: always;"></div>

`lab04_bonus.c` for bonus part
### Bonus Part 1 - Pin Define
```c
#include "C8051F040.h"
//button1.7
sbit pattern = P1^7;
//global valuable
int status;
int mode;
int time_count;


//set port address
void Port_Configuration (){
	XBR2 = 0xc0;
	P1MDIN = 0xff;
	P2MDOUT = 0xff;
}//end of function Port_Configuration

void Timer_Configuration (){
	TMOD = 0x01;
	TCON = 0x10;
	CKCON = 0x10;

	IE = 0x82;
	TL0 = 0;
	TH0 = 0;
}//end of function Timer_Configuration

void default_Config (){
	//turn-off watch-dog timer
	// disable watchdog timer
	WDTCN = 0xde;
	WDTCN = 0xad;

	OSCICN = 0x83;
	CLKSEL = 0x00;
	//initialize SFR setup page
	SFRPAGE = CONFIG_PAGE;                 // Switch to configuration page

	Port_Configuration ();
	//set to normal mode
	SFRPAGE = LEGACY_PAGE;
	Timer_Configuration ();
}//end of function Default_Config
```
Basically the same as basic, but we added the `Timer_Configuration()` for controling ISR. Others are the same.

### Bonus Part 2 - Button Detect
```c
void button_detect (){
	char key_hold;
	int key_release;
	int count;

	do {
		key_hold = pattern;
	} while (!key_hold);

	//Stage 2: wait for key released
	key_release = 0;
	count = 50;
	while (!key_release) {
		key_hold = pattern;
		if (key_hold) {
			count = 50;
		} else {
			count--;
			if (count==0) key_release = 1;
		}
	}//Stage 2: wait for key released
}//end of function button_detect ()
```
This on is the same as the example file, which will pause the program in the while loop until button pressed long enough for `key_release` to release the key.

<div style="break-after: page; page-break-after: always;"></div>

### Bonus Part 3 - Main
```c
int main(){
	default_Config ();
	mode = 0;
	status = 1;
	P2 = 1;
	while(1){
		button_detect ();
		//P2 = status;
		if (mode == 3) {
			mode = 0;
			status = 1;
		}
		else mode++;
	}
}
```
Our main function only runs a simple program, change mode every time a button pressed. So everytime button pressed, mode will go through the cycle from 0,1 ,2 ,3 then back to 0.

<div style="break-after: page; page-break-after: always;"></div>

### Bonus Part 4 - Timer ISR
```c
void Timer0_ISR () interrupt 1{
	time_count++;

	if (time_count==4) {
		time_count = 0;

		if (mode == 0) {
			if (P2 == 128){
				status = 1;
			} else {
				status = status << 1;
			}
		//	P2 = status;
		}//end while (patten01)

		if (mode == 1) {
			if (P2 == 1){
				status = 128;
			} else {
				status = status >> 1;
			}
		//	P2 = status;
		}//end while (patten01)

		if (mode == 2) {
			if (status) status = 0;
			else status = 85;
		}

		if (mode == 3) {
			if (status) status = 0;
			else status = 170;
		}
	}
	P2 = status;
	TH0 = 0;
	TL0 = 0;
}//end of function Timer0_ISR
```
Since Timer ISR will appear every time the timer counts to 0, then interrupt thee whole program, with main control and only controls the mode, we'll implement LED pattern in ISR. So by using if statement to select which mode should do by current mode value, and do left/right shift every second in mode 0 and 1, do odd/even blink while mode is 2 and 3. Blink using the same method as Lab03, which is if LED is lit the turn off LED, else lit odd/even LEDs.
After deciding mode and LED current pattern, we throw the value in status to `P2` so it can lit the LED.
ISR will do the code inside every 4 times since the frequency of ISR is too fst, we'll let it delay 4 time to make LED change every 1 second.


## Difficulties Encountered and Solutions

At first we have trouble at making our code from assemnbly to C. We've stuck on the basic dfination and some grammar first, but sort it out after some time studying the `example.c`, then we start drawing our logic into C code from Lab03. Both of it isn't that hard as we figure out the basic part, and implement bonus with 4 button controlling it(not ISR controlled version of bonus). The first difficulties is even we can't find any problem at our code, the board won't work correctly. We've test so long but nohing works, then after asking teacher and TA, we've spolved the problem by simply change a board. This simple problem took us almost 2 hours. The bouns part without ISR only took 20 minutes later then our basic finished. But we struggles lot on our ISR implementation.
Even our logic is almost complete, bu we've no idea how to make ISR work, none of our config and code will work no matter how we write the code. After struggling for so long, my teammates finally finds out the problem, which is our `Timer_Configuration` should put after `LEGACY_PAGE`, we previously all put configs between `CONFIG_PAGE` and `LEGACT_PAGE`. So after fixing that, our code works.


## Disscussion
After this Lab, I've understand more about how to write in C with 8051 board, and also ISR setup using C to implement. I've also learn how to detect buttons in C. But the most of it is, most of our problem may not be our code, but our board. We've almost encountered failure board every time in lab recently, and struggld for so long to find our is the board have problem. Hope next Lab we can be free from getting failure boards.

<div style="break-after: page; page-break-after: always;"></div>

## Appendix
### full code - basic
```c
#include "C8051F040.h"
sbit patten01 = P1^7;
sbit patten02 = P1^6;
int mode;

//set port address
void port_Configuration (){
	XBR2 = 0xc0;
	P1MDIN = 0xff;
	P2MDOUT = 0xff;
}//end of function Port_Configuration

void default_Config (){
	//turn-off watch-dog timer
	// disable watchdog timer
	WDTCN = 0xde;
	WDTCN = 0xad;

	//initialize SFR setup page
	SFRPAGE = CONFIG_PAGE;                 // Switch to configuration page

	port_Configuration ();

	//set to normal mode
	SFRPAGE = LEGACY_PAGE;
}//end of function Default_Config


/****************
There are something wrong with the function below!
Please see the following hints:
1. See the error message and fix those errors
2. The initiailization of N
3. The initialization of P1
****************/
void button_detect (){
	char key_hold;
	int key_release;
	int count;

	do {
		key_hold = patten01 || patten02;
	} while (!key_hold);

	//Stage 2: wait for key released
	if (patten01) {
		mode = 0;
	} else {
		mode = 1;
	}
	key_release = 0;
	count = 1000;
	while (!key_release) {
		key_hold = patten01 || patten02;
		if (key_hold) {
			count = 50;
		} else {
			count--;
			if (count==0) key_release = 1;
		}
	}//Stage 2: wait for key released
}//end of function button_detect ()

void mode_change(){
	default_Config ();

	button_detect ();
}//end of function main

int main(){
	int status;
	status = 1;
	P2 = 0;
	while(1){
		mode_change();
		P2 = status;
	
		if (mode == 0) {
			if (P2 == 128){
				status = 1;
			} else {
				status = status << 1;
			}
			P2 = status;
		}//end while (patten01)

		if (mode == 1) {
			if (P2 == 1){
				status = 128;
			} else {
				status = status >> 1;
			}
			P2 = status;
		}//end while (patten01)
	}
}
```
### full code - Bonus
```c
#include "C8051F040.h"
//button1.7
sbit pattern = P1^7;
//global valuable
int status;
int mode;
int time_count;


//set port address
void Port_Configuration (){
	XBR2 = 0xc0;
	P1MDIN = 0xff;
	P2MDOUT = 0xff;
}//end of function Port_Configuration

void Timer_Configuration (){
	TMOD = 0x01;
	TCON = 0x10;
	CKCON = 0x10;

	IE = 0x82;
	TL0 = 0;
	TH0 = 0;
}//end of function Timer_Configuration

void default_Config (){
	//turn-off watch-dog timer
	// disable watchdog timer
	WDTCN = 0xde;
	WDTCN = 0xad;

	OSCICN = 0x83;
	CLKSEL = 0x00;
	//initialize SFR setup page
	SFRPAGE = CONFIG_PAGE;                 // Switch to configuration page

	Port_Configuration ();
	//set to normal mode
	SFRPAGE = LEGACY_PAGE;
	Timer_Configuration ();
}//end of function Default_Config


/****************
There are something wrong with the function below!
Please see the following hints:
1. See the error message and fix those errors
2. The initiailization of N
3. The initialization of P1
****************/
void button_detect (){
	char key_hold;
	int key_release;
	int count;

	do {
		key_hold = pattern;
	} while (!key_hold);

	//Stage 2: wait for key released
	key_release = 0;
	count = 50;
	while (!key_release) {
		key_hold = pattern;
		if (key_hold) {
			count = 50;
		} else {
			count--;
			if (count==0) key_release = 1;
		}
	}//Stage 2: wait for key released
}//end of function button_detect ()

int main(){
	default_Config ();
	mode = 0;
	status = 1;
	P2 = 1;
	while(1){
		button_detect ();
		//P2 = status;
		if (mode == 3) {
			mode = 0;
			status = 1;
		}
		else mode++;
	}
}

void Timer0_ISR () interrupt 1{
	time_count++;

	if (time_count==4) {
		time_count = 0;

		if (mode == 0) {
			if (P2 == 128){
				status = 1;
			} else {
				status = status << 1;
			}
		//	P2 = status;
		}//end while (patten01)

		if (mode == 1) {
			if (P2 == 1){
				status = 128;
			} else {
				status = status >> 1;
			}
		//	P2 = status;
		}//end while (patten01)

		if (mode == 2) {
			if (status) status = 0;
			else status = 85;
		}

		if (mode == 3) {
			if (status) status = 0;
			else status = 170;
		}
	}
	P2 = status;
	TH0 = 0;
	TL0 = 0;
}//end of function Timer0_ISR
```
<div style="break-after: page; page-break-after: always;"></div>

### picture of code - basic
![basic-1](https://lh7-us.googleusercontent.com/USp41bqk4nShjNMbekFnpN1z_TU6kI4U5JB0bPwO0TZZcl7Xhsfvrg4vct6JyB8Dt-gQw4xNYHbTW-rmfxYkVUCV-cvsLnXYUsLNyL6lWL1045z36QoHnMs7ypTThzpzKNiTcJObL1SlneNT0fUr64I)
![basic-2](https://lh7-us.googleusercontent.com/AKXi_iaE1E8P0-F0wUnaPc6GepA241l3qGNpsZ_W0-tCvZNUYwNBDp9Ca9b-NMRG2veddaQYYVRvdWYQeznJ5JiCQo0A4H_N3DDfnXu49IoTL94CC1_gGW-o7IqkgTpvf08O1B4mgpsueUtIRzrrY-Q)
![basic-3](https://lh7-us.googleusercontent.com/JGFS3d4JKsy5oMXTaFCYOfPqdBiPMVnc94yzqFbh5GXgrNUWq9HSEBzH6RhQz6djjLJoCZOr6LyKuxil1duxf3ABwFi3tcQZX0PzpgSW0M-e76a061wXng3NnUbfk6TUoXviweUXJxqBru3hN7YYnvs)
![basic-4](https://lh7-us.googleusercontent.com/xEnJD-voHkcsoxf9F4TIODNYzv89xRhz6R7lTj1EMwhapFBUfuB3wi3iVCkO3Yj2Yb6WmbRcPApWUl-xZoB5D2PHTlrVNu0iMIOYkAgBC-mxHcRint-qUZOEBLvj-i1Al2MGJfxRzoblmJzJxdXWpfU)
<div style="break-after: page; page-break-after: always;"></div>

### picture of code - bonus
![bonus-1](https://lh7-us.googleusercontent.com/zxiQAy1HX3iOKljBk2O6j61Urbp3sukNdChYZaii1vWoIzbQsrHP9pPIywtO1KZvmC0vu8VA78IO1wYvEAf9dJgbYZ1N4kTaMyRsgGydDEuLdFLxbwuGILFtNMX_LZ40HFAV1DcveVEGIrV5eyNB9BA)
![bonus-2](https://lh7-us.googleusercontent.com/T1o0lCbp93z4aMqUD2eSO71u-XzcNKfdWV1W0mrqu812BXzkKCrORVRz5rQh0lnwCLR2Qd8mnnskWp3_iOCZfIXezYM5cN2Boole8oGmJ-hVlHOyS6BGNGtYP5I1eYGmVvoCQ7imoyAHDC_IoADgvcM)
![bonus-3](https://lh7-us.googleusercontent.com/XFEJPhA2pg9y8RS3CN7v8770JsxyhaIQM218us85fYSuKtxqFKR-Ikpuzd4mE4QDs6DNWcfrQZs9NtB9ptuzY1uMA7pE3swlL-coVdtRjIPG0p4N0YX88C-h1QFF9fE1HVFbMStHWY4CLAAHaklmwRg)
![bonus-4](https://lh7-us.googleusercontent.com/ASx_DRstk_WOYu03LCnlVUO5wcMP673yjfW3xVsRmQM5AraM3OQMlYsXNknukM6jQdEinIts_DBZ1Dqek7GYxQzywU5zmfgLIzzA4JBlDO5iCMElZhgjyUsGQbP0TCfFJ_jA_Z4gqYJnBUUwm_KvIPs)

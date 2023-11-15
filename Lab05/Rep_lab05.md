# Microcontroller Experiment Lab04 Report

- [Microcontroller Experiment Lab04 Report](#microcontroller-experiment-lab04-report)
	- [Problem Description](#problem-description)
	- [Code \& Explanation](#code--explanation)
		- [Basic](#basic)
			- [Pin define](#pin-define)
			- [Button Detect](#button-detect)
			- [Main function](#main-function)
		- [Bonus 1](#bonus-1)
			- [Pin Define and Button Detect](#pin-define-and-button-detect)
		- [Main Function](#main-function-1)
		- [Bonus 1](#bonus-1-1)
			- [Main Function Initialize](#main-function-initialize)
			- [Main Function - Cursor Movement](#main-function---cursor-movement)
			- [Main Function - Insert Data](#main-function---insert-data)
	- [Difficulties Encountered and Solutions](#difficulties-encountered-and-solutions)
	- [Disscussion](#disscussion)
	- [Appendix](#appendix)
		- [full code - basic](#full-code---basic)
		- [full code - Bonus](#full-code---bonus)
		- [picture of code - basic](#picture-of-code---basic)
		- [picture of code - bonus](#picture-of-code---bonus)
    
## Problem Description
+ learn to display characters on the LCD
+ Basic: 
  + Display the character pressed at the cursor position
+ Bonus 1:
  + Implement the "new-line" key
  + Change to the next line if a new-line button is pressed at Line 1
  + Scroll the screen if a new-line button is pressed at Line 2
+ Bonus 2:
  + Implement the arrow keys (up, down, left, right)
  + Move the cursor by the arrow key
  + Insert character at the cursor position

<div style="break-after: page; page-break-after: always;"></div>

## Code & Explanation
We divide the Problem into 3 files to implement, `lab05_basic.c`, `Lab05_b1.c`, `Lab05.c`.
### Basic
We implement the basic part here. (`lab05_basic.c`)
#### Pin define
```c
#include "C8051F040.h"
#include "LCD.h"

char LCD_status;

void LCD_PortConfig (){
	//initialize SFR setup page
	SFRPAGE = CONFIG_PAGE;                 // Switch to configuration page
	P2MDIN = 0xff;	
	P2MDOUT = 0x00;
	P1MDOUT = 0xff;

	//setup the cross-bar and configure the I/O ports
	XBR2 = 0xc0;
	P3MDOUT = 0x3f;
	
	//set to normal mode
	SFRPAGE = LEGACY_PAGE;
}//end of function LCD_PortConfig ()
```
We setup the pin for this Lab. Use `P2` as button input, and `P1` for debugging. `P3` is used to communicate with LCD, which we moved from the example file given by the professor.

<div style="break-after: page; page-break-after: always;"></div>

#### Button Detect
```c
void button_detect (){
	char key_hold;
	int key_release;
	int count;
	int N = 50;

	do {
		key_hold = P2;
	} while (!key_hold);
	
	//Stage 2: wait for key released
	key_release = 0;
	count = N;
	while (!key_release) {
		key_hold = P2;
		if (key_hold) {
			count = N;
		} else {
			count--;
			if (count==0) key_release = 1;
		}
	}//Stage 2: wait for key released
}//end of function button_detect ()
```
We made the `button_detect()` to send the signal when `P2` button is pressed.

#### Main function
We'll skip the LCD function given by the professor, we just modify the `example.c` directly. We only use `LCD_SendData()` to implement basic, and `LCD_init()`, `LCD_ClearScreen()` for initialization and clearscreen. Full code will be provide in the Appendix.
```c
int main() {
	int i;
	P1 = 0x00;
	Shutup_WatchDog ();
	LCD_PortConfig ();
	LCD_Init ();
	LCD_ClearScreen ();
	P1 = 0xff;
	while (1){
		P2 = 0x00;
		//button_detect();
		if(P2 == 128) {
			LCD_SendData ('A');
		} else if(P2 == 64){
			LCD_SendData ('B');
		} else if(P2 == 32){
			LCD_SendData ('C');
		}
		//LCD_PrintString ("Hello World!!!");
	}

	while (1);
}
```
We'll first initialize the LCD before while loop, `P1` is for debug usage.
Then we'll enter the while loop. After `P2` button pressed, we'll go into different if statement and trigger `LCD_SendData()` to send different characters. Then `LCD_SendData()` will send the data to LCD and display it. The button and the data sent inside is shown as table below.
| button | Data Sent in LCD |
|--------|------------------|
| P2.7   | A                |
| P2.6   | B                |
| P2.5   | C                |

<div style="break-after: page; page-break-after: always;"></div>

### Bonus 1
We implement the bonus 1 part here. (`lab05_b1.c`)

#### Pin Define and Button Detect
```c
#include "C8051F040.h"
#include "LCD.h"

char LCD_status;
char array[16];
int i = 0;
int j = 0;
int k = 0;

void LCD_PortConfig () {
	//initialize SFR setup page
	SFRPAGE = CONFIG_PAGE;                 // Switch to configuration page
	P2MDIN = 0xff;	
	P2MDOUT = 0x00;
	P1MDOUT = 0xff;

	//setup the cross-bar and configure the I/O ports
	XBR2 = 0xc0;
	P3MDOUT = 0x3f;
	
	//set to normal mode
	SFRPAGE = LEGACY_PAGE;
}//end of function LCD_PortConfig ()
```
Basically the same as basic, so I only show the global varible declaration and pin setup.

### Main Function
As the same, I'll skip the display of LCD control function given in the example file. Full code will be provided in the Appendix.
```c
void main (){
	//int i;
	P1 = 0x00;
	Shutup_WatchDog ();
	LCD_PortConfig ();
	LCD_Init ();
	LCD_ClearScreen ();
	P1 = 0xff;
	for (j = 0; j < 16; j++){
		array[j] = ' ';
	}
	while (1){
		P2 = 0x00;
		if( i < 16){
			if(P2 == 128) {
				LCD_SendData ('A');
				array[i] = 'A';
				i++;
			} else if(P2 == 64){
				LCD_SendData ('B');
				array[i] = 'B';
				i++;
			} else if(P2 == 32){
				LCD_SendData ('C');
				array[i] = 'C';
				i++;
			} else if(P2 == 16){
				LCD_ClearScreen ();
				//LCD_PrintString (array);
				for (j = 0; j < i ; j++){
					LCD_SendData (array[j]);
				}
				LCD_SendCommand(0x00C0);
				i = 0;
			}
		} else {
			if(P2 == 16){
				LCD_ClearScreen ();
				//LCD_PrintString (array);
				for (j = 0; j < i ; j++){
					LCD_SendData (array[j]);
				}
				LCD_SendCommand(0x00C0);
				i = 0;
			}
		}
		//LCD_PrintString ("Hello World!!!");
	}

	while (1);

}		
```
Since we have to implement the newline key, we must first store the data we type into an `char` array name `array`, so that when we change line, the data on the second line can be moved to the first, thus create a newline.
After initialize the LCD, we clear the `array` by input space character `' '` in it. using for loop. 
Then, we use `i` to represent the cursor position. We limited `i` < 16 so no data can be input after current line is full. If `i` $\geq$ 16, only newline key will work, A,B,C buttons pressed won't send data into LCD.
After button for A, B, c is pressed, we'll send data into the LCD and also store the data in `array`, and move the cursor with `i++`.
When newline key is pressed, we'll clear the screen using `LCD_ClearScreen()`, then the cursor will be set to the begining of first line. We then print the data stored in `array`. Finally, we set the cursor to begining of line 2 and reset cursor position to 0, creating the newline function.
If we create newline at line 2, the data on line 1 will be discard.
| button | Data/Command Sent to LCD |
|--------|--------------------------|
| P2.7   | A  (data)                |
| P2.6   | B  (data)                |
| P2.5   | C  (data)                |
| P2.4   | newline                  |
<div style="break-after: page; page-break-after: always;"></div>

### Bonus 1
We implement the bonus 1 part here. (`lab05.c`)
I'll skip the explaination of Pin define and button detect since it's the same as bonus 1.

#### Main Function Initialize
```c
void main (){
	P1 = 0x00;
	Shutup_WatchDog ();
	LCD_PortConfig ();
	LCD_Init ();
	LCD_ClearScreen ();
	P1 = 0xff;
	cur_x = 0;
	cur_y = 0;
	for (a = 0; a < 16; a++){
		array[0][a] = ' ';
		array[1][a] = ' ';
	}
	while (1){ ... } //The part is too long so will explain later
}
```
The initialization for main function. Since this the we must move the cursor, we use `cur_x` and `cur_y` to record cursor position. We also record the whole data on LCD with a 16 x 2 array called `array`. We also initializ the `array` here.
The function main has implement and their correspond button is list at the following table.
| Button | Function         |
|--------|------------------|
| P2.7   | Print A on LCD   |
| P2.6   | Print B on LCD   |
| P2.5   | Print C on LCD   |
| P2.3   | Move cursor left |
| P2.2   | Move cursor right|
| P2.1   | Move cursor up   |
| P2.0   | Move cursor down |
<div style="break-after: page; page-break-after: always;"></div>

#### Main Function - Cursor Movement
```c
	while (1) {
		P2 = 0x00;
		if    (P2 == 128) { ... } // Insert A
		else if(P2 == 64) { ... } // Insert B
		else if(P2 == 32) { ... } // Insert C
		// cursor left
		else if (P2 == 8) {
			if (cur_x > 0) {
				LCD_SendCommand(0x0010);
				cur_x--;
			}
		// cursor right
		} else if (P2 == 4) {
			if (cur_x < 15) {
				LCD_SendCommand(0x0014);
				cur_x++;
			}
		// cursor up
		} else if (P2 == 2) {
			if (cur_y == 1) {
				cur_y = 0;
				jump_cursor(cur_x, cur_y);
			}
		// cursor down
		} else if (P2 == 1) {
			if (cur_y == 0) {
				cur_y = 1;
				jump_cursor(cur_x, cur_y);
			}
		}
	}
```
To implement cursor Left/Right, we simply send the cursor go left/right command using `LCD_SendCommand()` to achieve it. After that, we change the `cur_x` to match the cursor position according to the cursor movement. We limited the cursor to be in the LCD screen, so `cur_x` is limited between 0 ~ 15, out of edge movement won't be executed. For example, `cur_x` cannot go right at `cur_x` = 15.
To implement cursor up/down, we limited the cursor cannot go out of edge, so we'll check if the cursor have space to go up or down. If their is no space, the command won't be executed. Otherwise we change the `cur_y` to the correspond up/left command (note: `cur_y` = 0 is line 1, `cur_y` = 1 is line 2), then jump to the new cursor position using `jump_cursor()`.
`jump_cursor()` explaination
```c
void jump_cursor(int x, int y) {
	if (y) {LCD_SendCommand(0x00C0);}
	else {LCD_SendCommand(0x0002);}
	for (a = 0; a < x; a++) {
		LCD_SendCommand(0x0014);
	}
}
```
We jump to begining of line 1 or 2 according to the input `cur_y`, then move cursor right until it's at `cur_x` position.

#### Main Function - Insert Data
```c
void insert_data(char c) {
	LCD_SendData(c);
	for (a = cur_x; a < 15; a++) {
		k = array[cur_y][a+1];
		array[cur_y][a+1] = array[cur_y][cur_x];
		LCD_SendData(array[cur_y][a+1]);
		array[cur_y][cur_x] = k;
	}
	array[cur_y][cur_x] = c;
}
```
We insert the input data `c` at the current cursor position, and move all data behind the current cursor position (including the current one overwritten by the inserted data) left for 1 space, and update the data in `array` in the for loop.
Then back to `main` function.
```c
	while (1){
		P2 = 0x00;
		// Insert A
		if(P2 == 128) {
			insert_data('A');
			if (cur_x < 15) {
				cur_x++;
			}
			jump_cursor(cur_x, cur_y);
		// Insert B
		} else if(P2 == 64){
			insert_data('B');
			if (cur_x < 15) {
				cur_x++;
			}
			jump_cursor(cur_x, cur_y);
		// Insert C
		} else if(P2 == 32){
			insert_data('C');
			if (cur_x < 15) {
				cur_x++;
			}
			jump_cursor(cur_x, cur_y);}
		...//cursor movement function, mentioned above
	}
```
We thus fininshed the insert data part. After pressed the insert data A,B,C buttons, data will be inserted and displayed using `insert_data()`, then update the cursor position and jump the cursor back to it's position using `jump_cursor()`.
The function will not be executed when the line is full, i.e. `cur_x` = 15.

## Difficulties Encountered and Solutions
We've encounter quite a lot of problem this time.
First is the Board Problem. Maybe our luck is too good, we have already meet a lot of time from lab3 until now. This lab we spend 2 weeks to complete, but we encountered it at both week. Always when we are debugging on our code, but it turns out that is board problem. Hope next time we can find a borad without problem at the first time.
1. Change line overflow
When we try to demo, the assistance find that our code didn’t restrict the range of the array, so when press over 16 time from 0, the character will disappear at LCD and after 24 times it will display in the second row, so we need to restrict the input range to solve this problem.
1. When insert data have something wrong.
We meet some problem bonus 2. When we want to insert data at the cursor position, but LCD always cannot display correct characters behind insert data. After we think about the running logical and ask assistant help and had solved this problem.


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

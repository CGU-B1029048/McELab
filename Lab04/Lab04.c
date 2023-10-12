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

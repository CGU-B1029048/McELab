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

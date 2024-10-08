#include "C8051F040.h"
int N =50;

void
Port_Configuration ()
{
	XBR2 = 0xc0;
	P1MDIN = 0xff;
	P2MDOUT = 0xff;
	P1 = 0x90;
}//end of function Port_Configuration

void
Default_Config ()
{
	//turn-off watch-dog timer
	// disable watchdog timer
	WDTCN = 0xde;
	WDTCN = 0xad;

	//initialize SFR setup page
	SFRPAGE = CONFIG_PAGE;                 // Switch to configuration page

	Port_Configuration ();

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
void
button_detect ()
{
	char key_hold;
	int key_release;
	int count;

	do {
		key_hold = P1;
	} while (!key_hold);

	//Stage 2: wait for key released
	key_release = 0;
	count = N;
	while (!key_release) {
		key_hold = P1;
		if (key_hold) {
			count = N;
		}
		else {
			count--;
			if (count==0) key_release = 1;
		}
	}//Stage 2: wait for key released
}//end of function button_detect ()

int
main ()
{
	int status;
	P2 = 1;
	Default_Config ();

	status = 1;
	P2 = status;

	while (1) {
		button_detect ();
		status++;
		P2 = status;
	}//end while (1)
}//end of function main

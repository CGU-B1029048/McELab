#include "C8051F040.h"

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
	TL0 = 0xec;
	TH0 = 0xff;
}//end of function Timer_Configuration

void
Config ()
{
	//turn-off watch-dog timer
	WDTCN = 0xde;
	WDTCN = 0xad;

	SFRPAGE = CONFIG_PAGE;

	OSCICN = 0x83;
	CLKSEL = 0x00;

	Port_Configuration ();
	Timer_Configuration ();
}//end of function Default_Config

unsigned char status;
int count, count_sec;
int half_period;

void Timer0_ISR ();

int main () {
	Config ();
	status = 0;
	count = 0, count_sec =0;
	//half_period = 12;

	while (1) {
		P2 = status;
	}//end while (1)
}//end of function main

void Timer0_ISR () interrupt 1 {
	count++;
	count_sec++;

	if (count_sec == 100000) { // 1sec
		count_sec = 0;
		n++;
	}

	if (count==song_table[n]) {
		count = 0;
		status = ~status;
	}

	TH0 = 0xff;
	TL0 = 0x00;
}//end of function Timer0_ISR

void Timer1_ISR () interrupt 3 {
	count_sec++;
	if (count_sec == 100000) {
		count_sec = 0;

	}
}



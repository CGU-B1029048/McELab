#include "C8051F040.h"

void Port_Configuration (){
	//initialize SFR setup page
	SFRPAGE = CONFIG_PAGE;                 // Switch to configuration page

	//setup the cross-bar and configure the I/O ports
	XBR2 = 0xc0;
	P2MDOUT = 0xff;
	P1MDOUT = 0xff;
	//set to normal mode
	SFRPAGE = LEGACY_PAGE;
}//end of function Port_Configuration

void Timer_Configuration (){
	TMOD = 0x01;
	TCON = 0x10;
	CKCON = 0x10;

	IE = 0x82;
	TL0 = 0xec;
	TH0 = 0xff;

	TL1 = 0xf0;
	TH1 = 0xD8; 
}//end of function Timer_Configuration#include "C8051F040.h"

void Config (){
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
long count, count_sec;
int n;

void Timer0_ISR ();

//noted haven't done
int song_table[8] = {190/*D0*/, 170/*RE*/, 150/*MI*/, 143/*FA*/, 127/*SO*/, 113/*LA*/, 100/*SI*/, 92/*DO*/};

int main (){
	Config ();
	status = 0x00;
	count = 0;
	count_sec = 0;
	n = 0;
	while (1) {
		P2 = status;
	}//end while (1)
}//end of function main

//to control signal frequency
void Timer0_ISR () interrupt 1 {
	count++;
	count_sec++;
	
	//duration
	if (count_sec == 50000) { // 1sec
		count_sec = 0;
		P1 = ~P1;
		n++;
	}
	
	//frequency
	if (count==song_table[n%8]) {
		count = 0;
		status = ~status;
	}

	TH0 = 0xff;
	TL0 = 0xec;
}//end of function Timer0_ISR



//to change the tone
void Timer1_ISR () interrupt 3{
	//counting for 1 second…
	//n = Song_Table [i];
	//i = (i+1)%7;
}



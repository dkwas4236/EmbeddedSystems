/* Name: Darion Kwasnitza
 * ID: 3122890
 * Course: CMPT 464 Lab 6
*/

#include "sysio.h"
#include "ser.h"
#include "serf.h"

int choice = 0;
int red;
int green;
int off1;
int off2;
// stop flag
int sFlag = 0;
// monitor flag
int mFlag = 0;
// display monitor once flag
int displayOnce = 0;
// char to capture input
char ch;
// char to capture monitor input
char mch;

fsm blinker {
    // loop through all states in order, if Sflag stop, if Mflag print
	state Red:
		if(sFlag == 0){
			leds(0,0);
			leds(1,0);
			finish;
		}
		if (mFlag == 1){
			ser_out(Red,"R ");
		}
		leds(0,1);
		delay(red, Off1);
		when(&red, Red);
		release;
	state Off1:
		if(sFlag == 0){
			leds(0,0);
			leds(1,0);
			finish;
		}
		if (mFlag == 1){
			ser_out(Off1, "F ");
		}
		leds(0,0);
		delay(off1, Green);
		when(&red, Red);
		release;
	state Green:
		if(sFlag == 0){
			leds(0,0);
			leds(1,0);
			finish;
		}
		if (mFlag == 1){
			ser_out(Green, "G ");
		}
		leds(1,1);
		delay(green, Off2);
		when(&red, Red);
		release;
	state Off2:
		if(sFlag == 0){
			leds(0,0);
			leds(1,0);
			finish;
		}
		if (mFlag == 1){
			ser_out(Off2, "F ");
		}
		leds(1,0);
		delay(off2, Red);
		when(&red, Red);
		release;
}

void change(char ch)
{
    // depending on inputted character, set flags
	switch(ch){
	case 'A':
		choice = 1;
		sFlag = 1;
		break;
	case 'S':
		choice = 2;
		sFlag = 0;
		mFlag = 0;
		break;
	case 'V':
		choice = 3;
		break;
	case 'M':
		choice = 4;
		mFlag = 1;
		break;
	}
}

fsm root {

char username [32];
// get username
state GetUsername:
	ser_out(GetUsername, "Enter your name: ");
// store username
state Welcome:	
	ser_inf(Welcome,"%s", username);
// display welcome message
state DisplayUsername:
	ser_outf(DisplayUsername, "Welcome %s", username);
// display menu
state Menu:
	ser_out(Menu, "\n\rSelect one of the following operations:\n\r(A)djust Intervals and start\n\r(S)top operation\n\r(V)iew current setting\n\r(M)onitor\n\rChoice: ");
// get menu choice and proceed accordingly
state MenuChoice:
	ser_inf(MenuChoice, "%s", &ch);
	change(ch);
	if (choice == 1){
		proceed EnterIntervals;
	}
	else if (choice == 2){
		proceed TerminateBlinker;
	}
	else if (choice == 3){
		proceed ViewSettings;
	}
	else if (choice == 4){
		displayOnce = 0;
		proceed Monitor;
	}
// display enter intervals
state EnterIntervals:
	ser_out(EnterIntervals, "\rEnter the intervals (Red On, OFF, Green On, OFF): ");
// store intervals and run blinker with intervals
state GetIntervals:
	ser_inf(GetIntervals, "%d %d %d %d", &red, &off1, &green, &off2);
	runfsm blinker;
	trigger(&red);			
	proceed DisplayUsername;
// terminate the blinker
state TerminateBlinker:
	sFlag = 0;
	proceed DisplayUsername;
// display current settings
state ViewSettings:
	ser_outf(ViewSettings, "(Red, OFF, Green, OFF) intervals: (%d, %d, %d, %d)\n\r", red, off1, green, off2);
	proceed DisplayUsername;
// display monitor 
state Monitor:
    // had to add this because ser_out line was displaying one more time after 'S'
	if (displayOnce == 0){
		ser_out(Monitor, "\rMonitor (press S to stop): ");
		displayOnce = 1;
	}
    mFlag = 1;
    proceed Monitor2;
// loop with monitor3 until mch is S then stop
state Monitor2:
    	ser_inf(Monitor, "%c", &mch);
    	if (mch == 'S'){
        	sFlag = 0;
        	mFlag = 0;
        	proceed DisplayUsername;    
    	}
    	else{
    		proceed Monitor3;
    	}
// repeat monitor2
state Monitor3:
	proceed Monitor2;

}	

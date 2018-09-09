/* May 13, 2016
 * Project Stepper Motor Experiment:
 * The Magic Stepper Motor's Wheel of Fortune
 * By: Tarun K. Kevin Ta
 * 
 * \\\\\\\\\\
 * I/O Lines: Port A (RA2), Button input
 * PORT C (RC0-3), Each line controls a 
 * electromagnet in the stepper motor
 */

#include <stdlib.h>
#include <xc.h>    //Use this to include the device header for your PIC.

//set the configuration bits: internal OSC, everything off except MCLR
#pragma config FOSC=INTRCIO, WDTE=OFF, PWRTE=OFF, MCLRE=ON, CP=OFF, \
                CPD=OFF, BOREN=OFF, IESO=OFF, FCMEN=OFF

//For compiler to calculate how many
//instructions to delay in the calls to __delay_ms()
#define _XTAL_FREQ 1000000  

/* from part 1 of stepper motor experiment */
int CW[] = {0xE, 0xD, 0xB, 0x7};	// Sequence to rotate motor clockwise (CW)
int step = 0;	// Clockwise sequence # 
int foursteps = 0; // # of sets of 4 steps to rotate motor CW

int random = 0;	// # of quarter revolutions motor will make (see lines 40-42 for details)
int seed = 25;	// seed variable for srand();

// Operates in wavedrive mode, completing 4 steps to smoothly rotate clockwise
void rotateCW() {

    for (step = 0; step <= 3; step++) {
        PORTC = CW[step];
        __delay_ms(50);
    }

}

void interrupt isr() {

    srand(seed);

    /* Generate the number of times (between 1-8) the motor 
	rotates a quarter of a complete revolution */
    random = (rand() % 8)+1;	// '+1' added to remove chance where motor doesn't spin
    random = random * 12;

	// Create suspense for user since he or she must wait (half a revolution) longer
    random = random + 25;	

    for (foursteps = 0; foursteps <= random; foursteps++) {
        rotateCW();
    }

    INTCONbits.INTF = 0;

}

void initialize() {

    TRISA = 0xFF;   //set all digital I/O to inputs
    TRISC = 0;	//set all digital I/O to outputs	

	// Turn off analog and use digital I/O only
	ANSEL = 0;
    ANSELH = 0;     

    /* Startup cycle to know circuit power on correctly, 
	  motor spin exactly one whole revolution */
    for (foursteps = 0; foursteps <= 50; foursteps++) {
        rotateCW();
    }
	
	INTCONbits.INTF = 0;        // reset the external interrupt flag
    OPTION_REGbits.INTEDG = 0;  // interrupt on the falling edge
    INTCONbits.INTE = 1;        // enable the external interrupt
    INTCONbits.GIE = 1;         // set the Global Interrupt Enable
	OPTION_REGbits.nRABPU = 0;  // enables global pull-ups
	WPUAbits.WPUA2 = 1;         // enables pull-up on RA2 so switch is able to go to
								// the falling edge when button is pressed again
}

void main() {

    initialize();

		/* Since the seed increments/ constantly change where program 
		spent the majority of its time (idling) the number generated  
		for "random" var would be quite random */
		while(1){
			seed++;	
		}
		
}
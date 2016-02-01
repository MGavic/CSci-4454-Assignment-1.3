//*****************************************************************************
//
// MSP432 main.c
//
// CSci 4454 Assignment #1.3
//
// Blink light on and off every second
//
// Myles Gavic
//
// MSP432 LaunchPad
// S1  P1.1
// S2  P1.4
//
// Use the timer to change the color of the RGB LED in two different ways. In the first case, step through
// the 7 basic colors in sequence, so that each color is displayed for 1 second at a time, and then the LED is
// off for 1 second.

//****************************************************************************


#include <msp.h>


// mode state - default is self cycle.
unsigned int stop = 0;
// color - default is red
unsigned int color = 0;


void selectionRegister1 (uint8_t bit)
{
	if (P1SEL0 & bit){
		if (P1SEL1 & bit)
			P1SELC|=bit; // Changing both at the same time
		else
			P1SEL0&=~bit; // Changing only the one in register 0
	}
	else if (P1SEL1 & bit)
		P1SEL1&=~bit; // Changing only the one in register 1
}


void selectionRegister2 (uint8_t bit)
{
	if (P2SEL0 & bit){
		if (P2SEL1 & bit )
			P2SELC|=bit ;
		else
			P2SEL0&=~bit;
	}
	else if (P2SEL1 & bit )
		P2SEL1&=~bit ;
}


void initalizeLED(void){

	//RED
	P2DIR|=BIT0 ;
	selectionRegister2(BIT0) ;

	//GREEN
	P2DIR|=BIT1 ;
	selectionRegister2(BIT1) ;

	//BLUE
	P2DIR|=BIT2 ;
	selectionRegister2(BIT2) ;


}


void setColorRGB(unsigned int n){

	switch(n){

	//001 RED
	case 0 :
		P2OUT&=~BIT0 ;
		P2OUT&=~BIT1 ;
		P2OUT|=BIT2 ;
		break;
		//010 GREEN
	case 1 :
		P2OUT&=~BIT0 ;
		P2OUT|=BIT1 ;
		P2OUT&=~BIT2 ;
		break;
		//011 YELLOW
	case 2 :
		P2OUT&=~BIT0 ;
		P2OUT|=BIT1 ;
		P2OUT|=BIT2 ;
		break;
		//100 BLUE
	case 3 :
		P2OUT|=BIT0 ;
		P2OUT&=~BIT1 ;
		P2OUT&=~BIT2 ;
		break;
		//101 VIOLET
	case 4 :
		P2OUT|=BIT0 ;
		P2OUT&=~BIT1 ;
		P2OUT|=BIT2 ;
		break;
		//110 TEAL
	case 5 :
		P2OUT|=BIT0 ;
		P2OUT|=BIT1 ;
		P2OUT&=~BIT2 ;
		break;
		//111 WHITE
	case 6 :
		P2OUT|=BIT0 ;
		P2OUT|=BIT1 ;
		P2OUT|=BIT2 ;
		break;
		//000 OFF
	case 7 :
		P2OUT&=~BIT0 ;
		P2OUT&=~BIT1 ;
		P2OUT&=~BIT2 ;
		break;
	}
}


void cycleColors(void){
	if(color < 6){
		color = color + 1;
		setColorRGB(color);
	}else{
		color = 0;
		setColorRGB(color);
	}
}


void initalize(void){
	// Setting the direction of port 1 line 1 (setting to 0)
	P1DIR&=~BIT1;
	// Setting the direction of port 1 line 4 (setting to 0)
	P1DIR&=~BIT4;

	// Pullup resistors
	P1REN|=BIT1 ;
	P1OUT|=BIT1 ;

	P1REN|=BIT4 ;
	P1OUT|=BIT4 ;

	P1DIR|=BIT0 ;
}

void PortOneInterrupt ( void ){
	unsigned short iflag=P1IV ;
	P1OUT^=BIT0 ;

}


void TimerA0Interrupt(void){
	unsigned short time = TA0IV;
	if(time==0x0E){ //checking if we reached the overflow condition
		P1OUT^=BIT0 ;
		cycleColors();
	}
}


void SetClockFrequency (void){
	CSKEY=0x695A ;
	CSCTL1=0x00000233 ;
	CSCLKEN=0x0000800F ;
	CSKEY=0xA596 ;
}


void main (void)
{
	WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

	initalizeLED();
	initalize();
	cycleColors();
	SetClockFrequency();


	// Setting up timers
	TA0CTL=0x0100; // cofigures what clock is used and sets individual divider / shuts timer off

	TA0CCTL0=0x2000; // turns interupt off / sets configuration for the register
	TA0CCR0=0xFA00;  // sets the value of the register

	TA0CCTL1=0x2010; // enable equal interupt
	TA0CCR1=0x0040;

	TA0CTL=0x0116; // sets the counter to 0 / turns counter on in count up mode / enables timeout interupts



	// Setting up interrupts
	P1IE=(BIT1 | BIT4 ) ;
	P1IES|=(BIT1 | BIT4 ) ;
	NVIC_EnableIRQ(PORT1_IRQn);
	NVIC_EnableIRQ(TA0_N_IRQn); //enables timer interupts


	while(1){}
}

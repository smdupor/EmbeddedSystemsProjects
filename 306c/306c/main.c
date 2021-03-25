#define INPUT (0)
#define OUTPUT (1)
#define ONEMILSEC (3000)
#define THREEMILSEC (5000)
#define THRESHOLD (50)

#include "qsk_bsp.h"
#include "qsk_lcd.h"
#include "stdio.h"
#include "string.h"

#define  TIME_CONFIG 0x40  
				/*  01000000 value to load into timer mode register
                    ||||||||_  TMOD0,TMOD1: TIMER MODE SELECTED
                    ||||||____ MR0:         NO PULSE OUTPUT   
                    |||||_____ MR1,MR2:     GATE FUNCTION NOT SELECTED
                    |||_______ MR3:         SET TO 0 IN TIMER MODE     
                    ||________ TCK0,TCK1:   F DIVIDED BY 8 SELECTED */

#define  TIMER_IPL 0x03 		// timer interrupt priority level

unsigned int TempStore = 0x0000;		// Location where ADC result is stored
volatile unsigned int reflection_strength=0x0000;
volatile unsigned int ambient_IR=0x0000;
unsigned int off_brightness=0;
unsigned int on_brightness=0;
int state=0;


/* Prototype declarations */

void adc_init(void);
void mcu_init(void);			// MCU initialization
void ta0_init(void);
#pragma INTERRUPT timer_ISR 
void timer_ISR (void);

/**************************************************************************
 ** main
 *	AUTHOR: Stevan Dupor
 *	MODIFIED: 4/14/2008
 *  PARAMETERS: None
 *
 *  DESCRIPTION: Main function. Where program execution starts. Initialises MCU
 *	then begins display of ADC values as updated by interrupts
 *
 *  RETURNS: Nothing
 *
***************************************************************************/

void main (void)
{
	char buf[9];
	mcu_init();					// initialize MCU

    while (1)
	{
	  sprintf(buf, "Amb: %d", off_brightness);
	  DisplayString(LCD_LINE1, buf);
	  sprintf(buf, "Ref: %d", reflection_strength);
	  DisplayString(LCD_LINE2, buf);
	if(reflection_strength>100)
	{
			RED_LED=LED_ON;
			YLW_LED=GRN_LED=LED_OFF;
	}
	else if(reflection_strength>50)
	{
			YLW_LED=LED_ON;
			RED_LED=GRN_LED=LED_OFF;
	}
	else if(reflection_strength>20)
	{
			GRN_LED=LED_ON;
			RED_LED=YLW_LED=LED_OFF;
	}
	else
	{
			RED_LED=YLW_LED=GRN_LED=LED_OFF;
	}
		
}
}
/**************************************************************************
 ** adc_init
 *	AUTHOR: Stevan Dupor
 *	MODIFIED: 4/14/2008
 *  PARAMETERS: None
 *
 *  DESCRIPTION: Initialises the ADC to use channel 3 on P10 port group with divide-by-four,
 *	one-shot mode, software trigger.
 *
 *  RETURNS: Nothing
 *
***************************************************************************/
void adc_init(void) 
{  
pd10_3 = INPUT;	// To use channel 3 on P10 group as input
// Clock selection: 24 MHz system clock, 6 MHz phiAD -> divide
// by 4 cks bits are 0 0 0
// ADCON 0
  ch0 = 1;   // channel 3
  ch1 = 1;
  ch2 = 0;
  md0 = 0;   // one shot 
  md1 = 0;
  cks0 = 0;  // divide clock by 4
// ADCON 1
  trg = 0;  // SW trigger 
  md2 = 0;  // one shot  
  bits = 1; // 10-bit conversion
  cks1 = 0; // divide clock by 4
  vcut = 1; // connect reference voltage
// ADCON 2
  smp = 1;  // use sample and hold
  adgsel0 = 0; // select port P10 group
  adgsel1 = 0;	
  cks2 = 0;  // divide clock by 4
}


/**************************************************************************
Name       : mcu_init()  
Modified   : 4/14/2008 
Parameters : none                   
Returns    : nothing      
Description: The starter kit startup file initializes the clock circuit
             to the main crystal with a divide by 1.  This function
             enables the PLL 
             The function also sets the LED as an output port, displays 
			 smdupor's unity ID and runs an LED test, as well as calls 
			 ADC and timer initialisation functions.   
***************************************************************************/
void mcu_init(void)
{
  {
    prc1 = 1;                       /* enable access to processor mode registers */
    pm20 = 0;                       /* 2 wait states for sfr access...this is the reset value
                                       necessary for >16Mhz operation, can be set to 1 wait for <=16Mhz */
    prc1 = 0;                       /* disable access to processor mode registers */
  }

  {
    /* configure and switch main clock to PLL at 20MHz */
    unsigned int count = 20000;
    prc0 = 1;                       /* enable access to clock registers */
    cm1 = 0x20;                     /* set to hi-drive Xin, divide by 1 */
    cm0 = 0x08;                     /* set to main clock using divide by 1 */
    cm21 = 0;                       /* switch to Xin */
    plc0 = 0x11;                    /* configure PLL to x2 */
    plc0 = 0x91;                    /* enable PLL */
    while(count > 0) count--;       /* wait for PLL to stabilize (20mS maximum, 200,000 cycles @10Mhz)
                                       this decrement with no optimization is 12 cycles each (20,000*12 cycles=240,000=24mS) */
    cm11 = 1;                       /* switch to PLL */
    prc0 = 0;                       /* disable access to clock registers */
  	ENABLE_LEDS;
	InitDisplay();
	RED_LED=GRN_LED=YLW_LED=LED_ON;

	DisplayString(LCD_LINE1,"ECE306");
	DisplayString(LCD_LINE2,"smdupor");

	DisplayDelay(7500);
	GRN_LED=RED_LED=YLW_LED=LED_OFF;
	DisplayString(LCD_LINE1,"        ");
	DisplayString(LCD_LINE2,"        ");
	ta0_init();
	adc_init();
	pd10_4=OUTPUT;
  }
}

/**************************************************************************
 ** ta0_init
 *	AUTHOR: Stevan Dupor
 *	MODIFIED: 4/14/2008
 *  PARAMETERS: None
 *
 *  DESCRIPTION: function to initialise timer 0 using divide-by-eight clock, 
 *	IPL 3, timer mode, no pulse output, gate function deselected, and to start the timer
 *	with an interval of 3ms.
 *
 *  RETURNS: Nothing
 *
***************************************************************************/
void ta0_init(void){

	ta0mr = TIME_CONFIG;
	ta0 =  (int)(((f1_CLK_SPEED/8)*3e-3) - 1);   	// (20MHz/8)*150us - 1)

/* The recommended procedure for writing an Interrupt Priority Level is shown
   below (see M16C datasheets under 'Interrupts' for details). */

   	DISABLE_IRQ;			// disable irqs before setting irq registers - macro defined in skp_bsp.h
   	ta0ic |= TIMER_IPL;		// use read-modify-write instruction to write IPL
    ENABLE_IRQ;				// enable interrupts macro defined in skp_bsp.h

	ta0s = 1;				// start timer
}

/**************************************************************************
 ** timer_ISR
 *	AUTHOR: Stevan Dupor
 *	MODIFIED: 4/14/2008
 *  PARAMETERS: None
 *
 *  DESCRIPTION: timer 0 interrupt service routine. implements object detector state machine.
 *	This function is called every one or three milliseconds, and depending on the state, 
 *	reads the IR level from the phototransistor with the IRLED on or off, toggles the IRLED, 
 *	and computes the difference between the IRLED on and IRLED off values. It also determines
 *	if a reflection from an object has been detected and sets the "reflection value" variable 
 *	appropriately for the main() function to read and display.
 *
 *  RETURNS: Nothing
 *
***************************************************************************/
void timer_ISR(void)
{
	switch(state)
	{
		case 0:

			p10_4=1;
			ta0s=0;
			ta0=THREEMILSEC;
			ta0s=1;
		state=1;
		break;
		case 1:
			adst = 1;
	  		while (adst);
	  		off_brightness = 1023-(ad3 & 0x03ff);
			//turn on IRLED
			p10_4=0;
			ta0s=0;
			ta0=ONEMILSEC;
			ta0s=1;
		state=2;
		break;
		case 2:
			adst = 1;
	  		while (adst);
	  		on_brightness = 1023-(ad3 & 0x03ff);
			//turn off IRLED
			p10_4=1;
			ta0s=0;
			ta0=ONEMILSEC;
			ta0s=1;
			if((on_brightness-off_brightness)<0)
			{
				if((off_brightness-on_brightness)>THRESHOLD)
				{
					reflection_strength=off_brightness-on_brightness;
				}
			}
			else
			{
				if((on_brightness-off_brightness)>THRESHOLD)
				{
					reflection_strength=on_brightness-off_brightness;
				}
			}
		state=1;
		break;
		default:
		state=0;
	}
}
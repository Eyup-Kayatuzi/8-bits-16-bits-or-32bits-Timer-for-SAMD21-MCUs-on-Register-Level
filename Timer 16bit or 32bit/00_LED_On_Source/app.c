#include "app.h"
#include "clock.h"

void AppInit(void)
{

	ClocksInit();
	
	// Assign LED0 as OUTPUT
	REG_PORT_DIR0 = LED0_PIN_MASK;
	REG_PORT_DIR0 |= PORT_PA20;
	// Set LED0 OFF
	REG_PORT_OUTCLR0 = LED0_PIN_MASK;
	REG_PORT_OUTCLR0 |= PORT_PA20;
	
	PORT->Group[0].PINCFG[28].bit.PMUXEN = 1; // for gclk0 output
	PORT->Group[0].PMUX[14].bit.PMUXE = 0X7;
	
	PORT->Group[0].PINCFG[16].bit.PMUXEN = 1;// for gclk2 output
	PORT->Group[0].PMUX[8].bit.PMUXE = 0X7;
	
	PORT->Group[0].PINCFG[15].bit.PMUXEN = 1;// for gclk1 output
	PORT->Group[0].PMUX[7].bit.PMUXO = 0X7;
} // AppInit()

void AppRun(void)
{
	while(1)
	{
		// Set the drive strength to strong
		PORT->Group[LED0_PORT].PINCFG[LED0_PIN_NUMBER].bit.DRVSTR = 1;
		
		// Turn the LED on PA17 ON
		REG_PORT_OUTSET0 = LED0_PIN_MASK;
	}

} // Apprun()


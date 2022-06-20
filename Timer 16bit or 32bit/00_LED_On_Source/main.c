//////////////////////////////////////////////////////////////////////////
// Include and defines
//////////////////////////////////////////////////////////////////////////
#include "sam.h"
#include "definitions.h"
#include "app.h"

//////////////////////////////////////////////////////////////////////////
// Function Prototypes
//////////////////////////////////////////////////////////////////////////
void ClocksInit(void);	// Configure Clock, Wait States and synch, bus clocks for 48MHz operation

//////////////////////////////////////////////////////////////////////////
// GLOBAL REGISTERS
//////////////////////////////////////////////////////////////////////////
uint16_t cnt = 0;

int main(void)
{	
	// Application hardware and software initialization 
	AppInit();
	NVIC_EnableIRQ(TC3_IRQn); // FOR timer
	NVIC_EnableIRQ(TC4_IRQn);
	
	TC4->COUNT32.CTRLA.bit.ENABLE = 0;
	TC4->COUNT32.CTRLA.bit.MODE = 0x2; // FOR 32 BIT COUNTER
	TC4->COUNT32.CTRLBSET.bit.ONESHOT = 0;
	TC4->COUNT32.CTRLBSET.bit.DIR = 1;
	TC4->COUNT32.COUNT.reg = 32000 - 1; // FOR ONE SECOND
	TC4->COUNT32.INTENSET.bit.OVF = 1; // overflow is enabled
	TC4->COUNT32.CTRLA.bit.ENABLE = 1;
	
	
	
	TC3->COUNT16.CTRLA.bit.ENABLE = 0;
	TC3->COUNT16.CTRLBSET.bit.ONESHOT = 0; // we did not select one shot with 0
	TC3->COUNT16.CTRLBSET.bit.DIR = 1; // now is counting down
	TC3->COUNT16.COUNT.reg = 60000 - 1;
	TC3->COUNT16.INTENSET.bit.OVF = 1; // overflow is enabled
	TC3->COUNT16.CTRLA.bit.ENABLE = 1; // tc is enabled
	
	// Super loop
	while(1)
	{

	}
} // main()


void TC3_Handler(){
	cnt += 1;
	if(cnt == 800){
				
		PORT->Group[LED0_PORT].OUTTGL.reg = PORT_PA20;		
		cnt = 0;
	}
	TC3->COUNT16.COUNT.reg = 60000 - 1;
	TC3->COUNT16.INTFLAG.bit.OVF = 1; // we cleared flag of interrupt
}


void TC4_Handler(){
		
	PORT->Group[LED0_PORT].OUTTGL.reg = LED0_PIN_MASK;
	TC4->COUNT32.COUNT.reg = 32000 - 1;	
	TC4->COUNT32.INTFLAG.bit.OVF = 1; // we cleared flag of interrupt
}




//////////////////////////////////////////////////////////////////////////
// Include and defines
//////////////////////////////////////////////////////////////////////////
#include "clock.h"

void ClocksInit(void)
{
	PM->APBCMASK.reg |= (PM_APBCMASK_TC3 | PM_APBCMASK_TC4 | PM_APBCMASK_TC5); 
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// GCLK2 is SET TO 8MHZ
	
	SYSCTRL->OSC8M.bit.PRESC = 0x00;  // divide by 1
	SYSCTRL->OSC8M.bit.ENABLE = 1;
	GCLK->GENDIV.reg = GCLK_GENDIV_ID(2) | GCLK_GENDIV_DIV(1);
	GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(2) | GCLK_GENCTRL_SRC_OSC8M | GCLK_GENCTRL_OE | GCLK_GENCTRL_GENEN;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// GCLK1 is SET TO 32KHZ

	SYSCTRL->XOSC32K.reg =
    /* Crystal oscillators can take a long time to startup. This
       waits the maximum amount of time (4 seconds). This can be
       reduced depending on your crystal oscillator. */
    SYSCTRL_XOSC32K_STARTUP(0x7) |
    SYSCTRL_XOSC32K_EN32K |
    SYSCTRL_XOSC32K_XTALEN;

	/* This has to be a separate write as per datasheet section 17.6.3 */
	SYSCTRL->XOSC32K.bit.ENABLE = 1;

	/* Wait for the external crystal to be ready */
	while(!SYSCTRL->PCLKSR.bit.XOSC32KRDY);
	
	GCLK->GENDIV.reg =
	GCLK_GENDIV_ID(1) |
	GCLK_GENDIV_DIV(1);

	/* Setup GCLK1 using the external 32.768 kHz oscillator */
	GCLK->GENCTRL.reg =
	GCLK_GENCTRL_ID(1) |
	GCLK_GENCTRL_SRC_XOSC32K | GCLK_GENCTRL_OE |
	/* Improve the duty cycle. */
	GCLK_GENCTRL_IDC |
	GCLK_GENCTRL_GENEN;
	/* Wait for the write to complete */
	while(GCLK->STATUS.bit.SYNCBUSY);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// GCLK0 is SET TO 48MHZ
	
	NVMCTRL->CTRLB.bit.RWS = 1; /* Set the correct number of wait states for 48 MHz @ 3.3v */
	GCLK->CLKCTRL.reg =
	GCLK_CLKCTRL_ID_DFLL48 |
	GCLK_CLKCTRL_GEN_GCLK1 |
	GCLK_CLKCTRL_CLKEN;
	
	/* This works around a quirk in the hardware (errata 1.2.1) -
    the DFLLCTRL register must be manually reset to this value before
    configuration. */
	while(!SYSCTRL->PCLKSR.bit.DFLLRDY);
	SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_ENABLE;
	while(!SYSCTRL->PCLKSR.bit.DFLLRDY);

	/* Set up the multiplier. This tells the DFLL to multiply the 32.768 kHz
    reference clock to 48 MHz */
	SYSCTRL->DFLLMUL.reg =
    /* This value is output frequency / reference clock frequency,
       so 48 MHz / 32.768 kHz */
    SYSCTRL_DFLLMUL_MUL(1465) |
    /* The coarse and fine step are used by the DFLL to lock
       on to the target frequency. These are set to half
       of the maximum value. Lower values mean less overshoot,
       whereas higher values typically result in some overshoot but
       faster locking. */
    SYSCTRL_DFLLMUL_FSTEP(511) | // max value: 1023
    SYSCTRL_DFLLMUL_CSTEP(31);  // max value: 63

	/* Wait for the write to finish */
	while(!SYSCTRL->PCLKSR.bit.DFLLRDY);
	
	uint32_t coarse = (*((uint32_t *)FUSES_DFLL48M_COARSE_CAL_ADDR) & FUSES_DFLL48M_COARSE_CAL_Msk) >> FUSES_DFLL48M_COARSE_CAL_Pos;
	SYSCTRL->DFLLVAL.bit.COARSE = coarse;

	/* Wait for the write to finish */
	while(!SYSCTRL->PCLKSR.bit.DFLLRDY);
	SYSCTRL->DFLLCTRL.reg |=
	/* Closed loop mode */
	SYSCTRL_DFLLCTRL_MODE |
	/* Wait for the frequency to be locked before outputting the clock */
	SYSCTRL_DFLLCTRL_WAITLOCK |
	/* Enable it */
	SYSCTRL_DFLLCTRL_ENABLE;
	/* Wait for the frequency to lock */
	while (!SYSCTRL->PCLKSR.bit.DFLLLCKC || !SYSCTRL->PCLKSR.bit.DFLLLCKF) {}
		
	/* Setup GCLK0 using the DFLL @ 48 MHz */
	GCLK->GENCTRL.reg =
	GCLK_GENCTRL_ID(0) |
	GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_OE |
	/* Improve the duty cycle. */
	GCLK_GENCTRL_IDC |
	GCLK_GENCTRL_GENEN;

	/* Wait for the write to complete */
	while(GCLK->STATUS.bit.SYNCBUSY);	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_TCC2_TC3;
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK1 | GCLK_CLKCTRL_ID_TC4_TC5;
} 

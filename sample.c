/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The GLCD application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman, Paolo Bernardi
** Created date:            03/01/2022
** Version:                 v2.0
** Descriptions:            basic program for LCD and Touch Panel teaching
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             Francesco Fiorella
** Modified date:           03/01/2022
** Version:                 v3.0
** Descriptions:            pong game
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"
#include "GLCD/GLCD.h" 
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "adc/adc.h"
#include "pong/pong.h"
#include "button_EXINT/button.h"

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif

int main(void)
{
  SystemInit();  												/* System Initialization (i.e., PLL)  */
	
  LCD_Initialization();									/* LCD Initialization                 */
	
	init_pong();													/* Pong Game Initialization						*/
	
	// timer0 gestisce il movimento della palla
	init_timer(0,0x00098968);							/* TIMER0 Initialization              */
																				/* K = T*Fr = [s]*[Hz] = [s]*[1/s]	  */
																				/* T = K / Fr = 0x00098968 / 25MHz    */
																				/* T = K / Fr = 625000 / 25MHz       */
																				/* T = 0.025s					   							*/
	
	ADC_init();														/* ADC Initialization									*/
	
  BUTTON_init();												/* BUTTON Initialization              */
	
	init_RIT(0x004C4B40);									/* RIT Initialization                 */
																				/* T = K / Fr = 0x004C4B40 / 100MHz   */
																				/* T = K / Fr = 5000000 / 100MHz      */
																				/* T = 0.05s                          */
	
	enable_RIT();													/* RIT enabled												*/
	
	// init loudspeaker
	LPC_PINCON->PINSEL1 |= (1<<21);
	LPC_PINCON->PINSEL1 &= ~(1<<20);
	LPC_GPIO0->FIODIR |= (1<<26);
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);				
	
  while (1)	{
		__ASM("wfi");
  }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

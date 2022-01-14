/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  03/01/2022
** Last Version:        V1.00
** Descriptions:        functions to manage RIT interrupts
** Correlated files:    RIT.h, timer.h, pong.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "RIT.h"
#include "../pong/pong.h"

// game variables
extern Status game_status;
extern int corner;

// button management variables
extern int key1;
extern int key2;
extern int int0;

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

void RIT_IRQHandler (void)
{
	/* ADC management */
	ADC_start_conversion();
	
	/* button management */
	if (key1 > 1) {
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){ // key1 pressed
			switch (key1) {
				case 2:
					if (game_status == INITIAL){
						// start the game
						setStart();
					}
					break;
				default:
					break;
			}
			key1++;
		} else { // button released
			key1 = 0;
			NVIC_EnableIRQ(EINT1_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
		}
	} else if (key1 == 1){
		key1++;
	}
	
	if (key2 > 1) {
		if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){ // key2 pressed
			switch (key2) {
				case 2:
					if (game_status == RUNNING || game_status == WAITING) {
						// pause the game
						setPause();
					} else if (game_status == PAUSED || game_status == PAUSED_WAITING) {
						// resume the game
						setResume();
					}
					break;
				default:
					break;
			}
			key2++;
		} else { // button released
			key2 = 0;
			NVIC_EnableIRQ(EINT2_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
		}
	} else if (key2 == 1){
		key2++;
	}
	
	if (int0 > 1) {
		if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){ // int0 pressed
			switch (int0) {
				case 2:
					// restart the game
					newPong();
					break;
				default:
					break;
			}
			int0++;
		} else { // button released
			int0 = 0;
			NVIC_EnableIRQ(EINT0_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
		}
	} else if (int0 == 1){
		int0++;
	}
	
	LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
}

/******************************************************************************
**                            End Of File
******************************************************************************/

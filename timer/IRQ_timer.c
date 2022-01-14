/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  03/01/2022
** Last Version:        V2.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h, pong.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <string.h>
#include "lpc17xx.h"
#include "timer.h"
#include "../pong/pong.h"

// cpu_paddle (needed to retrieve current position)
extern Paddle cpu_paddle;
extern int cpu_paddle_target_position;

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

void TIMER0_IRQHandler (void)
{
	moveBall();
	
  LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

uint16_t SinTable[45] =  
{
    410, 467, 523, 576, 627, 673, 714, 749, 778,
    799, 813, 819, 817, 807, 789, 764, 732, 694, 
    650, 602, 550, 495, 438, 381, 324, 270, 217,
    169, 125, 87 , 55 , 30 , 12 , 2  , 0  , 6  ,   
    20 , 41 , 70 , 105, 146, 193, 243, 297, 353
};

void TIMER1_IRQHandler (void)
{
	static int ticks=0;
	/* DAC management */	
	LPC_DAC->DACR = SinTable[ticks]<<6;
	ticks++;
	if(ticks==45) ticks=0;

  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}

/******************************************************************************
** Function name:		Timer2_IRQHandler
**
** Descriptions:		Timer/Counter 2 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

void TIMER2_IRQHandler (void)
{
	// move the cpu_paddle
	if (cpu_paddle_target_position < 5)
		cpu_paddle_target_position = 5;
	if (cpu_paddle_target_position > 204)
		cpu_paddle_target_position = 204;
	
	if (cpu_paddle.x_start < cpu_paddle_target_position) {
		// go right
		drawPaddle(cpu_paddle.x_start + 1, CPU);
	} else if (cpu_paddle.x_start > cpu_paddle_target_position) {
		// go left
		drawPaddle(cpu_paddle.x_start - 1, CPU);
	} else {
		// stop moving
		disable_timer(2);
		reset_timer(2);
	}
	
  LPC_TIM2->IR = 1;			/* clear interrupt flag */
  return;
}

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 3 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

void TIMER3_IRQHandler (void)
{
	continueTheGame();
	
  LPC_TIM3->IR = 1;			/* clear interrupt flag */
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
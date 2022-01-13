/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_adc.c
** Last modified Date:  03/01/2022
** Last Version:        V1.00
** Descriptions:        functions to manage A/D interrupts
** Correlated files:    adc.h, pong.h
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

#include "lpc17xx.h"
#include "adc.h"
#include "../pong/pong.h"

/*----------------------------------------------------------------------------
  A/D IRQ: Executed when A/D Conversion is ready (signal from ADC peripheral)
 *----------------------------------------------------------------------------*/

unsigned short AD_current;   
unsigned short AD_last = 0xFF;     /* Last converted value               */

#define INTERVALS 15
// valori calcolati per bilanciare la non idealità (per grandi spostamenti):
// divide lo schermo in 15 intervalli,
// se il paddle è su intervalli diversi (a distanza 3) per tre volte di fila allora muove.
unsigned short current_interval;
unsigned short last_interval = 0xFF*INTERVALS/0xFFF;
unsigned int counter = 0;

short position_current;
short position_last = 0xFF*199/0xFFF;
//int counter1 = 0;
//int counter2 = 0;

void ADC_IRQHandler(void) {
	short sub_interval, sub_pixel;
	
  AD_current = ((LPC_ADC->ADGDR>>4) & 0xFFF);/* Read Conversion Result             */
	if (AD_current != AD_last) {
		current_interval = AD_current*INTERVALS/0xFFF; // calculate the new interval
		
		// non muovere se cambia 3 intervalli (ma non ci rimane per 3 esecuzioni dell'handler)
		// in questo modo il normale movimento (che cambia un solo intervallo) non ferma l'esecuzione
		sub_interval = current_interval - last_interval;
		if (counter < 2 && (sub_interval < -3 || sub_interval > 3)) {
			counter++;
			return;
		}
		counter = 0;
		
		position_current = AD_current*199/0xFFF + 5;
		
		sub_pixel = position_current - position_last;
		
		// impedisci gli spostamenti minori di 4 px, ok per il simulatore, da testare sulla scheda hardware
		if(position_current != 0 && (sub_pixel < -4 || sub_pixel > 4)){
			// 239 (tot px) - 10 (bordi sx e dx) - 30 (paddle width) = 199 -> poi shift a dx di 5 (bordo sx)
			position_current = AD_current*199/0xFFF + 5;
			drawPaddle(position_current);
			last_interval = current_interval;
			AD_last = AD_current;
			position_last = position_current;
		} else if (position_current == 0) {
			position_current = AD_current*199/0xFFF + 5;
		}
	}
}

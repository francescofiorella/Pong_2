/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           pong.c
** Last modified Date:  20/01/2022
** Last Version:        V2.00
** Descriptions:        functions to play pong game
** Correlated files:    pong.h
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"
#include "pong.h"
#include <stdio.h>

// game's variables
Status game_status;
unsigned int user_score;
unsigned int cpu_score;
unsigned int is_sound_on;

extern Paddle user_paddle;
extern Paddle cpu_paddle;

extern Ball ball;

void init_pong(void) {	
	LCD_Clear(Black);
	
	drawBorders();
	
	game_status = INITIAL;
	updateScore(0, USER);
	updateScore(0, CPU);
	
	resetBall();
	
	// draw the info
	GUI_Text(65, 119, (uint8_t *) "Press KEY1 to", White, Black);
	GUI_Text(60, 139, (uint8_t *) "start the game  ", White, Black);
	GUI_Text(40, 179, (uint8_t *) "KEY2 to Pause/Resume", White, Black);
	GUI_Text(60, 199, (uint8_t *) "INT0 to Reset", White, Black);
	
	// init the CPU paddle
	init_paddles();
	
	// user_paddle drawn by adc
}

void drawBorders() {
	unsigned int i; // counter
	
	for (i = 0; i <= 4; i++){
		LCD_DrawLine(i, 0, i, 319, Red);	// left border
		LCD_DrawLine(239-i, 0, 239-i, 319, Red);	// right border
	}
}

void updateScore(unsigned int new_score, Player player) { // update the score
	unsigned int x;
	// cast int to string
	char score_string[30];
	sprintf(score_string, "%d", new_score);
	
	// the score it's drawn in [156-164(px)], so 156 px from the top and 156 from the bottom
	
	switch(player) {
		case USER:
			x = 15;
			user_score = new_score;
			GUI_Text(x, 153, (uint8_t *) score_string, White, Black);
			break;
		case CPU:
			x = 215;
			cpu_score = new_score;
			GUI_TextInverted(x, 167, (uint8_t *) score_string, White, Black);
			break;
		default: // should not be possible
			break;
	}
}

// freqs:
/* 
131Hz		k=4240		C3
147Hz		k=3779	D3
165Hz		k=3367	E3
175Hz		k=3175	F3
196Hz		k=2834	G3
220Hz		k=2525	A3
247Hz		k=2249	B3
262Hz		k=2120		C4
294Hz		k=1890	D4
330Hz		k=1684	E4
349Hz		k=1592	F4
392Hz		k=1417	G4
440Hz		k=1263	A4
494Hz		k=1125	B4
523Hz		k=1062		C5
*/

void disableSound() {
	disable_timer(1);
	reset_timer(1);
	is_sound_on = 0;
}

void enableSound(int timer_num) {
	init_timer(1, timer_num);
	enable_timer(1);
	is_sound_on = 1;
}

void continueTheGame() {
	if (game_status != PAUSED_WAITING) {
		game_status = RUNNING;
		// go to the middle
		setCpuPaddleTarget(104);
		enableCpuPaddle();
		resetBall();
		// move the ball
		enable_timer(0);
		GUI_Text(50, 159, (uint8_t *) "You can do better!", Black, Black); // delete
	}
}

void newPong() { // if you lost, set again the parameters to the default values
	if (game_status == ENDED) {
		game_status = INITIAL;
		GUI_TextInverted(90, 111, (uint8_t *) "You Lose", Black, Black); // delete
		GUI_Text(85, 208, (uint8_t *) "You Lose", Black, Black); // delete
		updateScore(0, USER);
		updateScore(0, CPU);
		GUI_Text(65, 119, (uint8_t *) "Press KEY1 to", White, Black);
		GUI_Text(60, 139, (uint8_t *) "start the game  ", White, Black);
		GUI_Text(40, 179, (uint8_t *) "KEY2 to Pause/Resume", White, Black);
		GUI_Text(60, 199, (uint8_t *) "INT0 to Reset", White, Black);
		resetBall();
		// move the cpu paddle to the middle
		setCpuPaddleTarget(104);
		enableCpuPaddle();
	}
}

void setStart() { // start the game
	game_status = RUNNING;
	enable_timer(0);
	// delete the info
	GUI_Text(65, 119, (uint8_t *) "Press KEY1 to", Black, Black);
	GUI_Text(60, 139, (uint8_t *) "start the game  ", Black, Black);
	GUI_Text(40, 179, (uint8_t *) "KEY2 to Pause/Resume", Black, Black);
	GUI_Text(60, 199, (uint8_t *) "INT0 to Reset", Black, Black);
}

void setWin(Player player) { // someone won
	switch(player) {
		case USER:
			GUI_TextInverted(90, 111, (uint8_t *) "You Lose", Red, Black);
			GUI_Text(85, 208, (uint8_t *) "You Win", Green, Black);
			break;
		case CPU:
			GUI_TextInverted(95, 111, (uint8_t *) "You Win", Green, Black);
			GUI_Text(85, 208, (uint8_t *) "You Lose", Red, Black);
			break;
		default: // should not be possible
			break;
	}
	
	game_status = ENDED;
	disable_timer(0);
	reset_timer(0);
}

void setPause() { // pause the game
	disable_timer(0);
	reset_timer(0);
	
	// disable the sound (just to be sure)
	disableSound();
	
	// stop the cpu_paddle
	stopCpuPaddle();
	if (game_status == RUNNING) {
		GUI_Text(88, 153, (uint8_t *) "Paused", White, Black);
		game_status = PAUSED;
	} else if (game_status == WAITING) {
		GUI_Text(88, 145, (uint8_t *) "Paused", White, Black);
		game_status = PAUSED_WAITING;
	}
}

void setResume() { // resume the game
	if (game_status == PAUSED) {
		game_status = RUNNING;
		GUI_Text(88, 153, (uint8_t *) "Paused", Black, Black);
		enable_timer(0);
		// resume the cpu_paddle
		enableCpuPaddle();
	} else if (game_status == PAUSED_WAITING) {
		game_status = RUNNING;
		GUI_Text(88, 145, (uint8_t *) "Paused", Black, Black);
		continueTheGame();
	}
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           pong.c
** Last modified Date:  03/01/2022
** Last Version:        V1.00
** Descriptions:        functions to play pong game
** Correlated files:    pong.h, GLCD.h, timer.h
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"
#include "pong.h"
#include "../GLCD/GLCD.h"
#include "../timer/timer.h"
#include <stdio.h>
#include <stdlib.h>

// paddles y position
// cpu paddle
#define PADDLE_CPU_Y_START 32
#define PADDLE_CPU_Y_END 41
// player paddle
#define PADDLE_USER_Y_START 278
#define PADDLE_USER_Y_END 287

// initial ball position
#define BALL_X_START_DEFAULT 230
#define BALL_Y_START_DEFAULT 157
// ball default movements
#define BALL_X_MOVEMENT_DEFAULT -2
#define BALL_Y_MOVEMENT_DEFAULT 2

// game's variables
Status game_status;
unsigned int user_score;
unsigned int cpu_score;
Ball ball;
Paddle user_paddle;
Paddle cpu_paddle;
unsigned int is_sound_on;
int cpu_paddle_target_position;

void init_pong(void) {	
	LCD_Clear(Black);
	
	drawBorders();
	
	// user_paddle drawn by adc
	game_status = STOPPED;
	updateScore(0, USER);
	updateScore(0, CPU);
	
	ball.x_movement = BALL_X_MOVEMENT_DEFAULT;
	ball.y_movement = BALL_Y_MOVEMENT_DEFAULT;
	
	user_paddle.y_start = PADDLE_USER_Y_START;
	user_paddle.y_end = PADDLE_USER_Y_END;
	cpu_paddle.y_start = PADDLE_CPU_Y_START;
	cpu_paddle.y_end = PADDLE_CPU_Y_END;
	
	initBall(BALL_X_START_DEFAULT, BALL_Y_START_DEFAULT); // ball in the middle
	
	// draw the info
	GUI_Text(65, 119, (uint8_t *) "Press KEY1 to", White, Black);
	GUI_Text(60, 139, (uint8_t *) "start the game  ", White, Black);
	GUI_Text(40, 179, (uint8_t *) "KEY2 to Pause/Resume", White, Black);
	GUI_Text(60, 199, (uint8_t *) "INT0 to Reset", White, Black);
	
	// init the CPU paddle
	drawPaddle(104, CPU);
}

void drawBorders() {
	unsigned int i; // counter
	
	for (i = 0; i <= 4; i++){
		LCD_DrawLine(i, 0, i, 319, Red);	// left border
		LCD_DrawLine(239-i, 0, 239-i, 319, Red);	// right border
	}
}

void drawPaddle(unsigned int start, Player player) { // draw a 31x10px paddle starting from "start" and 32px high from the bottom
	unsigned int i, i_max; // counter
	int i1; // counter (adjusted)
	unsigned int end = start + 30;
	int offset; // negative if the paddle is moving to left, positive otherwise
	unsigned int x_delete, x_draw; // LCD_DrawLine() arguments
	Paddle *paddle;
	
	switch(player) {
		case USER:
			paddle = &user_paddle;
			break;
		case CPU:
			paddle = &cpu_paddle;
			break;
		default: // should not be possible
			break;
	}
	
	if (start >= 5 && start <= 204) { // if does not overlap the borders
		offset = start - paddle->x_start;		
		if (offset > 0) { // right movement
			x_delete = paddle->x_start;
			if (paddle->x_start == 0) { // draw the paddle for the first time
				x_draw = start;
				i_max = 30;
			} else {
				x_draw = paddle->x_end;
				i_max = offset;
			}
		} else { // left movement
			x_delete = paddle->x_end;
			x_draw = paddle->x_start;
			i_max = -offset;
		}
		
		for (i = 0; i <= i_max; i++) {
			if (offset > 0) {
				i1 = i;
			} else {
				i1 = -i;
			}
			// delete the previous paddle
			if (paddle->x_start >= 5 && paddle->x_start <= 204) // only if the paddle is already drawn on screen
				LCD_DrawLine(x_delete+i1, paddle->y_start, x_delete+i1, paddle->y_end, Black);
			
			// draw the new paddle
			LCD_DrawLine(x_draw+i1, paddle->y_start, x_draw+i1, paddle->y_end, Green);
		}
		
		paddle->x_start = start;
		paddle->x_end = end;
	}
}

void initBall(unsigned int x, unsigned int y) { // draw a 5x5 ball starting from "x" and "y"
	unsigned int i_old, i_new; // counter
	unsigned int x1 = x + 4;
	unsigned int y1 = y + 4;
	
	// delete the previous ball
	for (i_old = ball.y_start; i_old <= ball.y_end; i_old++){
		LCD_DrawLine(ball.x_start, i_old, ball.x_end, i_old, Black);
	}
	
	// draw the new ball
	for (i_new = y; i_new <= y1; i_new++){
		LCD_DrawLine(x, i_new, x1, i_new, Green);
	}
	
	// update the variables
	ball.x_start = x;
	ball.y_start = y;
	ball.x_end = x1;
	ball.y_end = y1;
}

void drawBall(unsigned int x, unsigned int y) { // draw a 5x5 ball starting from the previous one
	int i, i_max, i_x_max, i_y_max; // counter
	unsigned int i_x = 0;
	unsigned int i_y = 0;
	// "a" and "b" allow to delete 5px (instead of 4) when the ball is moving in one direction only
	unsigned int a = 0;
	unsigned int b = 0;
	unsigned int x1 = x + 4;
	unsigned int y1 = y + 4;
	// LCD_DrawLine() arguments
	unsigned int x1_delete1, x2_delete1, x1_delete2, x2_delete2, x1_draw1, x2_draw1, x1_draw2, x2_draw2;
	unsigned int y1_delete1, y2_delete1, y1_delete2, y2_delete2, y1_draw1, y2_draw1, y1_draw2, y2_draw2;
	
	if (x >= 5 && x <= 230 && y <= 315) {
		// set the maximum number of iterations (x, y e total)		
		i_x_max = abs(ball.x_movement)-1;
		i_y_max = abs(ball.y_movement)-1;
		if (i_x_max < i_y_max) {
			i_max = i_y_max;
		} else {
			i_max = i_x_max;
		}
		
		for (i = 0; i <= i_max; i++) {
			// set LCD_DrawLine() arguments
			if (ball.x_movement > 0) { // to right
				x1_delete1 = ball.x_start + i_x;
				x2_delete1 = ball.x_start + i_x;
				
				x1_delete2 = ball.x_start + i_x + 1;
				x2_delete2 = ball.x_end + i_x + a;
				
				x1_draw1 = ball.x_end + 1 + i_x;
				x2_draw1 = ball.x_end + 1 + i_x;
				
				x1_draw2 = ball.x_start + 1 + i_x;
				x2_draw2 = ball.x_end + 1 + i_x;
			} else if (ball.x_movement < 0) { // to left
				x1_delete1 = ball.x_end - i_x;
				x2_delete1 = ball.x_end - i_x;
				
				x1_delete2 = ball.x_start - i_x - a;
				x2_delete2 = ball.x_end - i_x - 1;
				
				x1_draw1 = ball.x_start - 1 - i_x;
				x2_draw1 = ball.x_start - 1 - i_x;
				
				x1_draw2 = ball.x_start - 1 - i_x;
				x2_draw2 = ball.x_end - 1 - i_x;
			} else { // not moving on x
				x1_delete1 = ball.x_end;
				x2_delete1 = ball.x_end;
				
				x1_delete2 = ball.x_start;
				x2_delete2 = ball.x_end;
				
				x1_draw1 = ball.x_start;
				x2_draw1 = ball.x_start;
				
				x1_draw2 = ball.x_start;
				x2_draw2 = ball.x_end;
			}
			
			if (ball.y_movement > 0) { // to bottom
				y1_delete1 = ball.y_start + i_y;
				y2_delete1 = ball.y_end + i_y + b;
				
				y1_delete2 = ball.y_start + i_y;
				y2_delete2 = ball.y_start + i_y;
				
				y1_draw1 = ball.y_start + 1 + i_y;
				y2_draw1 = ball.y_end + i_y + b;
				
				y1_draw2 = ball.y_end + 1 + i_y;
				y2_draw2 = ball.y_end + 1 + i_y;
			} else if (ball.y_movement < 0) { // to top
				y1_delete1 = ball.y_start - i_y - b;
				y2_delete1 = ball.y_end - i_y;
				
				y1_delete2 = ball.y_end - i_y;
				y2_delete2 = ball.y_end - i_y;
				
				y1_draw1 = ball.y_start - i_y - b;
				y2_draw1 = ball.y_end -1 - i_y;
				
				y1_draw2 = ball.y_start - 1 - i_y;
				y2_draw2 = ball.y_start - 1 - i_y;
			} else { // not moving on y (should not be possible)
				y1_delete1 = ball.y_start;
				y2_delete1 = ball.y_end;
				
				y1_delete2 = ball.y_end;
				y2_delete2 = ball.y_end;
				
				y1_draw1 = ball.y_start;
				y2_draw1 = ball.y_end;
				
				y1_draw2 = ball.y_start;
				y2_draw2 = ball.y_start;
			}
			
			// delete the previous ball
			if (i <= i_x_max)
				LCD_DrawLine(x1_delete1, y1_delete1, x2_delete1, y2_delete1, Black); // vertical
			if (i <= i_y_max)
				LCD_DrawLine(x1_delete2, y1_delete2, x2_delete2, y2_delete2, Black); // horizontal
			
			// draw the new ball
			if (i <= i_x_max)
				LCD_DrawLine(x1_draw1, y1_draw1, x2_draw1, y2_draw1, Green); // vertical
			if (i <= i_y_max)
				LCD_DrawLine(x1_draw2, y1_draw2, x2_draw2, y2_draw2, Green); // horizontal
			
			// increment the indexes and set the offset, if necessary
			if (i+1 <= i_x_max) {
				i_x++;
				a = 0;
			} else {
				a = 1;
			}
			if (i+1 <= i_y_max) {
				i_y++;
				b = 0;
			} else {
				b = 1;
			}
		}
		
		// update the variables
		ball.x_start = x;
		ball.y_start = y;
		ball.x_end = x1;
		ball.y_end = y1;
	}
}

void updateScore(unsigned int new_score, Player player) { // update the score
	unsigned int x;
	// cast int to string
	char score_string[30];
	
	switch(player) {
		case USER:
			x = 15;
			user_score = new_score;
			break;
		case CPU:
			x = 215;
			cpu_score = new_score;
			break;
		default: // should not be possible
			break;
	}

	sprintf(score_string, "%d", new_score);
	GUI_Text(x, 153, (uint8_t *) score_string, White, Black); // it's drawn in [156-164(px)], so 156 px from the top and 156 from the bottom
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

void moveBall() {	
	int pixel_hit;
	unsigned int paddle_x_start;
	
	// if the sound is on, disable it
	if (is_sound_on > 1) {
		is_sound_on = 0;
		disable_timer(1);
		reset_timer(1);
	} else if (is_sound_on == 1) {
		is_sound_on++;
	}
	
	if (game_status == STARTED) {
		// check the horizontal movement (x)
		if ((ball.x_movement > 0 && (ball.x_end+ball.x_movement >= 235)) // right border reached
			|| (ball.x_movement < 0 && (ball.x_start+ball.x_movement <= 4))) { // left border reached
			ball.x_movement = - ball.x_movement;
			
			// emit a lower pitched note
			is_sound_on = 1;
			init_timer(1, 4240); // C3
			enable_timer(1);
		}
		
		// check the vertical movement (y)
		// paddle reached or not
		if ((ball.y_movement > 0 // verso il basso [user_paddle]
			&& ball.y_end+ball.y_movement >= user_paddle.y_start // paddle height reached
			&& ball.y_end < user_paddle.y_start // paddle height not passed
			&& ball.x_end >= user_paddle.x_start // end_ball compared to start_paddle
			&& ball.x_start <= user_paddle.x_end) // start_ball compared to end_paddle
			|| (ball.y_movement < 0 // verso l'alto [cpu_paddle]
			&& ball.y_start+ball.y_movement <= cpu_paddle.y_end // paddle height reached
			&& ball.y_start > cpu_paddle.y_end // paddle height not passed
			&& ball.x_end >= cpu_paddle.x_start // end_ball compared to start_paddle
			&& ball.x_start <= cpu_paddle.x_end) // start_ball compared to end_paddle
		) {
			// disable and reset timer2 (just to be sure)
			disable_timer(2);
			reset_timer(2);
			
			if (ball.y_movement > 0) { // to the bottom
				paddle_x_start = user_paddle.x_start;
			} else {
				paddle_x_start = cpu_paddle.x_start;
			}
			
			// set ball y-movement checking the central pixel of the ball
			/*
				Paddle bits:
				[....-00-01-02-03-04-05-06-07-08-09-10-11-12-13-14-15-16-17-18-19-20-21-22-23-24-25-26-27-28-29-30-....]
				x-movement:
				[ -6 |   -5   |   -4   |   -3   |   -2   |   -1   |00|   01   |   02   |   03   |   04   |   05   | 06 ]
				Considering y-movement = 2;
				Corner (degrees) :
				[18.4|  21.8  |  26.6  |  33.7  |   45   |  63.4  |90|  63.4  |   45   |  33.7  |  26.6  |  21.8  |18.4]
			*/
			
			pixel_hit = ball.x_start + 2 - paddle_x_start;
			// pixel_hit can be < 0 or > 30 because the central pixel can fall outside the paddle
			if (pixel_hit < 0) {
				ball.x_movement = -6;
			} else if (pixel_hit <= 2) {
				ball.x_movement = -5;
			} else if (pixel_hit <= 5) {
				ball.x_movement = -4;
			} else if (pixel_hit <= 8) {
				ball.x_movement = -3;
			} else if (pixel_hit <= 11) {
				ball.x_movement = -2;
			} else if (pixel_hit <= 14) {
				ball.x_movement = -1;
			} else if (pixel_hit == 15) {
				ball.x_movement = 0;
			} else if (pixel_hit <= 18) {
				ball.x_movement = 1;
			} else if (pixel_hit <= 21) {
				ball.x_movement = 2;
			} else if (pixel_hit <= 24) {
				ball.x_movement = 3;
			} else if (pixel_hit <= 27) {
				ball.x_movement = 4;
			} else if (pixel_hit <= 30) {
				ball.x_movement = 5;
			} else {
				ball.x_movement = 6;
			}
			
			if (ball.y_movement > 0) { // bounced on the user_paddle
				// calculate the hit point
				cpu_paddle_target_position = calculateTrajectory(ball.x_start + 2);
				// go to the hit point
				enable_timer(2);
			} else { // bounced on the cpu_paddle
				// go to the middle
				cpu_paddle_target_position = 104;
				enable_timer(2);
			}
			
			ball.y_movement = - ball.y_movement;
			
			// re-write scores
			updateScore(user_score, USER);
			updateScore(cpu_score, CPU);
			
			// emit a higher pitched note
			is_sound_on = 1;
			init_timer(1, 2120); // C4
			enable_timer(1);
		} else if (ball.y_movement > 0 && ball.y_end+ball.y_movement >= 319) { // cpu won
			updateScore(cpu_score+1, CPU); // update score
			updateScore(user_score, USER); // re-draw score
			if (cpu_score == 5) {
				setWin(CPU);
			} else {
				GUI_Text(50, 159, (uint8_t *) "You can do better!", Yellow, Black);
				ball.x_movement = BALL_X_MOVEMENT_DEFAULT;
				ball.y_movement = BALL_Y_MOVEMENT_DEFAULT;
				initBall(BALL_X_START_DEFAULT, BALL_Y_START_DEFAULT); // ball in the middle
				GUI_Text(50, 159, (uint8_t *) "You can do better!", Black, Black); // delete
			}
		} else if (ball.y_movement < 0 && ball.y_start+ball.y_movement <= 0) { // player won
			updateScore(user_score+1, USER); // update score
			updateScore(cpu_score, CPU); // re-draw score
			if (user_score == 5) {
				setWin(USER);
			} else {
				GUI_Text(80, 159, (uint8_t *) "Well done!", Yellow, Black);
				// go to the middle
				cpu_paddle_target_position = 104;
				enable_timer(2);
				ball.x_movement = BALL_X_MOVEMENT_DEFAULT;
				ball.y_movement = BALL_Y_MOVEMENT_DEFAULT;
				initBall(BALL_X_START_DEFAULT, BALL_Y_START_DEFAULT); // ball in the middle
				GUI_Text(80, 159, (uint8_t *) "Well done!", Black, Black); // delete
			}
		}
		drawBall(ball.x_start + ball.x_movement, ball.y_start + ball.y_movement);
	}
}

int calculateTrajectory(unsigned int start_point) {
	int end_point; // target
	// y: from 42 to 277 (235px)
	// x: from 5 to 234 (229 px)
	// the ball in going to top
	// to calculate the end_point, use the line passing through two points formula
	/*
		    (y-y1)(x2-x1)
		x = ------------- + x1;
		       y2-y1
	
		x = end_point (shifted to left/right);		x1 = start_point - 5;		x2 = start_point - 5 + x_movement;
		y = 273;		y1 = 42;		y2 = 42 + y_movement;
	*/
	end_point = (231*ball.x_movement/ball.y_movement) + start_point - 5;
	return recursiveCheck(end_point) - 10; // shift (+5 due to the border and -15 to place the middle of the paddle))
}

int recursiveCheck(int value) { // calculate the x point
	if (value > 229) { // 234 - 5
		value = 229 - (value - 229); // subtract a "screen" and invert
		return recursiveCheck(value);
	} else if (value < 0) { // 5 - 5
		value = 229 - (value + 229); // add a "screen" and invert
		return recursiveCheck(value);
	} else {
		return value; // between [0, 229]
	}
}

void newPong() { // if you lost, set again the parameters to the default values
	if (game_status == OVER) {
		game_status = STOPPED;
		GUI_Text(85, 100, (uint8_t *) "You Lose", Black, Black); // delete
		GUI_Text(85, 208, (uint8_t *) "You Lose", Black, Black); // delete
		GUI_Text(65, 119, (uint8_t *) "Press KEY1 to", White, Black);
		GUI_Text(60, 139, (uint8_t *) "start the game  ", White, Black);
		GUI_Text(40, 179, (uint8_t *) "KEY2 to Pause/Resume", White, Black);
		GUI_Text(60, 199, (uint8_t *) "INT0 to Reset", White, Black);
		updateScore(0, USER);
		updateScore(0, CPU);
		ball.x_movement = BALL_X_MOVEMENT_DEFAULT;
		ball.y_movement = BALL_Y_MOVEMENT_DEFAULT;
		initBall(230, 157); // ball in the middle
	}
}

void setStart() { // start the game
	game_status = STARTED;
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
			GUI_Text(85, 100, (uint8_t *) "You Lose", Red, Black);
			GUI_Text(85, 208, (uint8_t *) "You Win", Green, Black);
			break;
		case CPU:
			GUI_Text(85, 100, (uint8_t *) "You Win", Green, Black);
			GUI_Text(85, 208, (uint8_t *) "You Lose", Red, Black);
			break;
		default: // should not be possible
			break;
	}
	
	game_status = OVER;
	disable_timer(0);
	reset_timer(0);
}

void setPause() { // pause the game
	game_status = PAUSED;
	disable_timer(0);
	reset_timer(0);
	GUI_Text(88, 153, (uint8_t *) "Paused", White, Black);
}

void setResume() { // resume the game
	game_status = STARTED;
	GUI_Text(88, 153, (uint8_t *) "Paused", Black, Black);
	enable_timer(0);
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

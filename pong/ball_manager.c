/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           ball_manager.c
** Last modified Date:  20/01/2022
** Last Version:        V2.00
** Descriptions:        functions to manage the ball in the pong game
** Correlated files:    pong.h
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"
#include "pong.h"
#include <stdio.h>
#include <stdlib.h>

// initial ball position
#define BALL_X_START_DEFAULT 230
#define BALL_Y_START_DEFAULT 157
// ball default movements
#define BALL_X_MOVEMENT_DEFAULT -2
#define BALL_Y_MOVEMENT_DEFAULT 2

Ball ball;

extern Status game_status;
extern unsigned int is_sound_on;
extern unsigned int user_score;
extern unsigned int cpu_score;

void resetBall() {
	ball.x_movement = BALL_X_MOVEMENT_DEFAULT;
	ball.y_movement = BALL_Y_MOVEMENT_DEFAULT;
	
	initBall(BALL_X_START_DEFAULT, BALL_Y_START_DEFAULT); // ball in the middle
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

void moveBall() {	
	// if the sound is on, disable it
	if (is_sound_on > 1) {
		disableSound();
	} else if (is_sound_on == 1) {
		is_sound_on++;
	}
	
	if (game_status == RUNNING) {
		// check the horizontal movement (x)
		if ((ball.x_movement > 0 && (ball.x_end+ball.x_movement >= 235)) // right border reached
			|| (ball.x_movement < 0 && (ball.x_start+ball.x_movement <= 4))) { // left border reached
			ball.x_movement = - ball.x_movement;
			
			// emit a lower pitched note
			enableSound(4240); //C3
		}
		
		// check the vertical movement (y)
		// paddle reached or not
		if (hasHitAPaddle(&ball)) {
			// disable and reset timer2 (just to be sure)
			stopCpuPaddle();
			
			ball.x_movement = calculateNewBallMovement(&ball);
			
			if (ball.y_movement > 0) { // bounced on the user_paddle
				// calculate the hit point
				setCpuPaddleTarget(calculateTrajectory(ball.x_start + 2));
				// go to the hit point
				enableCpuPaddle();
			}
			
			ball.y_movement = - ball.y_movement;
			
			// re-write scores
			updateScore(user_score, USER);
			updateScore(cpu_score, CPU);
			
			// emit a higher pitched note
			enableSound(2120); //C4
		} else if (ball.y_movement > 0 && ball.y_end+ball.y_movement >= 319) { // cpu won
			updateScore(cpu_score+1, CPU); // update score
			updateScore(user_score, USER); // re-draw score
			if (cpu_score == 5) {
				setWin(CPU);
			} else {
				GUI_Text(50, 159, (uint8_t *) "You can do better!", Yellow, Black);
				disable_timer(0); // stop the ball
				reset_timer(0);
				game_status = WAITING;
				enable_timer(3); // pause for 2 sec
			}
		} else if (ball.y_movement < 0 && ball.y_start+ball.y_movement <= 0) { // player won
			updateScore(user_score+1, USER); // update score
			updateScore(cpu_score, CPU); // re-draw score
			if (user_score == 5) {
				setWin(USER);
			} else {
				GUI_Text(80, 159, (uint8_t *) "Well done!", Yellow, Black);
				disable_timer(0); // stop the ball
				reset_timer(0);
				game_status = WAITING;
				enable_timer(3); // pause for 1 sec
			}
		}
		drawBall(ball.x_start + ball.x_movement, ball.y_start + ball.y_movement);
	}
}

// calculate the target point on the cpu_paddle
// the calculation is not accurate so that the cpu can lose
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

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

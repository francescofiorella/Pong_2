/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           pong.h
** Last modified Date:  03/01/2022
** Last Version:        V1.00
** Descriptions:        Prototypes of functions included in the pong.c file
** Correlated files:    
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/

#include "../GLCD/GLCD.h"
#include "../timer/timer.h"

// game_status options
typedef enum {INITIAL, RUNNING, PAUSED, ENDED, WAITING, PAUSED_WAITING} Status;

// player types
typedef enum {USER, CPU} Player;

// ball and paddle structs
typedef struct {
	int x_start, y_start, x_end, y_end;
	int x_movement, y_movement;
} Ball;

typedef struct {
	unsigned int x_start, x_end, y_start, y_end;
} Paddle;

// pong.c
extern void init_pong( void );
extern void drawBorders( void );
extern void updateScore( unsigned int new_score, Player player );
extern void disableSound( void );
extern void enableSound( int timer_num );
extern void continueTheGame( void );
extern void newPong( void );
extern void setStart( void );
extern void setWin( Player player );
extern void setPause( void );
extern void setResume( void );

// paddle_manager.c
extern void init_paddles( void );
extern void drawPaddle( unsigned int start, Player player );
extern void moveCpuPaddle( void );
extern void enableCpuPaddle( void );
extern void stopCpuPaddle( void );
extern int hasHitAPaddle( Ball *ball );
extern int calculateNewBallMovement( Ball *ball );

// ball_manager.c
extern void resetBall( void );
extern void initBall( unsigned int x_start, unsigned int y_start );
extern void drawBall( unsigned int x_start, unsigned int y_start );
extern void moveBall( void );
extern int calculateTrajectory( unsigned int start_point );
extern int recursiveCheck( int value );
extern void setCpuPaddleTarget( int x_target );

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           pong.h
** Last modified Date:  03/01/2022
** Last Version:        V1.00
** Descriptions:        Prototypes of functions included in the pong.c file
** Correlated files:    
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/

// game_status options
typedef enum {STOPPED, STARTED, PAUSED, OVER} Status;

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

// functions
extern void init_pong( void );
extern void drawBorders( void );
extern void drawPaddle( unsigned int start, Player player );
extern void initBall( unsigned int x_start, unsigned int y_start );
extern void drawBall( unsigned int x_start, unsigned int y_start );
extern void updateScore( unsigned int new_score, Player player );
extern void moveBall( void );
extern int calculateTrajectory( unsigned int start_point );
extern void newPong( void );
extern int recursiveCheck(int value);
extern void setStart( void );
extern void setWin( Player player );
extern void setPause( void );
extern void setResume( void );

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

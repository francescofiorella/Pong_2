# Pong_2
The software implements the game called “Pong”, the classic table tennis-themed arcade game, originally released in 1972 by Atari. The game is developed for the "LandTiger", a development board based on a second-generation ARM Cortex-M3 microcontroller.

This project implements a multi-player version of the game where the player must not let the ball fall below the paddle (the single-player version was implemented in the project "Pong_1").

The bottom paddle is driven by the first player, and it can only move horizontally. The first player operates it through the potentiometer available in the LandTiger board.
On the contrary, the top paddle is driven by the second player. It is fully automatic and, when the play starts, it only moves horizontally at a constant speed.
The game field is implemented vertically, i.e., the bottom paddle is in the bottom portion of the LCD display with red walls (5px thick) on the left and right portion of the display for the ball to bounce on. The top paddle is in the top portion of the LCD display and has the same size of the bottom paddle.

As for the previous project, when the ball hits a wall, it bounces with a reflection angle equal to the incident one. For instance, if a ball going downwards hits the left wall with an incident angle of 30°, then it will bounce downwards with the same 30° angle.
When the ball hits the paddles, the reflection angle is not always equal to the incident one, and it changes depending on what part of the paddle is hit by the ball.
The bottom paddle is 32px high from the bottom part of the screen and 10px thick, and the ball is a 5x5px square. The top paddle is 32px lower from the top part of the screen and 10px thick.

Whenever the ball hits a wall or a paddle, the buzzer (loudspeaker) emits a lower pitched note (when bouncing on the wall) and a higher pitched note (when bouncing on the paddle).

Every time the ball falls below one’s paddle, the other player’s score is incremented by 1. The game ends as soon as one player scores 5 points, with the message “You win” or “You lose” displayed on the screen for both players. The score is printed on the left side of the screen for the first player and on the right side for the second player, at mid high (160px from both the bottom and top part of the LCD).

The user can start the game by pressing the button KEY1. When the game starts, the ball touches the right wall at mid high (160px from both the bottom and top part of the screen) and goes downwards with an angle of 45°. To pause the game and later resume it, the user can press the button KEY2. In case of game over, to prepare a new game the player can press the INT0 button, followed by KEY1 to start it.

_Homepage preview on the simulator:_

<img width="591" alt="pong_2_preview" src="https://user-images.githubusercontent.com/22116863/149555396-6699769f-a0cd-45e0-bdad-483924adc4cd.png">

_Gameplay on the Landtiger board:_


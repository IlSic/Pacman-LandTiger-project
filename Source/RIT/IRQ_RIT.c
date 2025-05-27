/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <LPC17xx.h>
#include <RIT/RIT.h>
#include <Pacman/pacman.h>
#include <GLCD.h>
#include <timer.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
volatile int down_0 = 0;
volatile int down_1 = 0;
volatile int down_2 = 0;



volatile int x_pacman_pos = x_pacman_spawn;
volatile int y_pacman_pos = y_pacman_spawn;
extern uint16_t x_blinky_pos;
extern uint16_t y_blinky_pos;
volatile int punteggio = 0;
//int punteggio_old = 0;
int pausa = 0;
int J = 0;
volatile int vite = 1;
extern uint16_t color_blinky;
extern volatile uint16_t mode;
static int pills = 240;
int blinky_respawn = 0;
extern int total_turns;
extern int routine_blinky;
int soglia = 1000;
int gameover = 0;

void RIT_IRQHandler (void)
{			


if(punteggio>=soglia){
	vite++;
	//DrawLives(vite);
	soglia+=1000;
}

if(blinky_respawn>0){
	if(blinky_respawn<60){ // spawn dopo 3s=60*50ms
		blinky_respawn++;
	} else{
		blinky_respawn=0;
		DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
		enable_timer(0);
	}
}

if(pills==0){
	disable_timer(0);
	disable_timer(1);
	LCD_Clear(Black);
	GUI_Text(96, 152, (uint8_t *) "VICTORY", White, Black);
	GUI_Text(0, 304, (uint8_t *) "Press RESET to play again", White, Black);
} else{

	switch(pausa){
		
		if(punteggio>=soglia){
			vite++;
			DrawLives(vite);
			soglia+=1000;
		}

		if(blinky_respawn>0){
			if(blinky_respawn<60){ // spawn dopo 3s=60*50ms
				blinky_respawn++;
			} else{
				blinky_respawn=0;
				DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
				enable_timer(0);
			}
		}		

		case 0: //prima dello start
			
			GUI_Text(96, 152, (uint8_t *) "READY", White, Black);
			DrawPacman(x_pacman_pos, y_pacman_pos);
			DrawBlinky(x_blinky_spawn, y_blinky_spawn, Red);
			break;
			
		case 1:	//in game
			GUI_Text(96, 152, (uint8_t *) "PAUSE", Black, Black);
			enable_timer(1);
			enable_timer(2);
			if(color_blinky==Blue2){
				enable_timer(3);
			}
			pausa=3; //dalla seconda volta in poi entro nel case 3 in modo da non riabilitare il timer ogni volta
			
			if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){	/* Joytick UP pressed */
				J=1;
			}

			
			if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){	/* Joytick DOWN pressed */
				J=2;
			}

			
			if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){	/* Joytick LEFT pressed */
				J=3;
				
			}

			
			if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){	/* Joytick RIGHT pressed */
				J=4;
			}

			switch(J){
				
				case 1: //up
					switch (map[((y_pacman_pos-3*CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_pacman_pos/CELL_SIZE]){
						
						case 1:
							if(check_positions(x_pacman_pos, y_pacman_pos, x_blinky_pos, y_blinky_pos)==1){
								if(mode==1){
									punteggio+=100;
									DrawPacman(x_pacman_pos, y_pacman_pos);
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									mode=0;
									x_blinky_pos=x_blinky_spawn;
									y_blinky_pos=y_blinky_spawn;
									color_blinky=Red;
									disable_timer(0);
									reset_timer(0);
									blinky_respawn=1;
									routine_blinky=0;
									total_turns=15;
								} else {
									vite--;
									
									if(vite==-1){
										disable_timer(0);
										//disable_timer(1);
										LCD_Clear(Black);
										GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
										GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
										GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);
										gameover=1;
											
									} else{
										DrawLives(vite);
										DeletePacman(x_pacman_pos, y_pacman_pos);
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										J=0;
										x_pacman_pos=x_pacman_spawn;
										y_pacman_pos=y_pacman_spawn;
										DrawPacman(x_pacman_pos, y_pacman_pos);
									}
								}

							}							
							break;
						
						case 2:
							map[((y_pacman_pos-3*CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_pacman_pos/CELL_SIZE] = 0;
							DeletePacman(x_pacman_pos, y_pacman_pos);
							y_pacman_pos -= CELL_SIZE;
							if(check_positions(x_pacman_pos, y_pacman_pos, x_blinky_pos, y_blinky_pos)==1){
								if(mode==1){
									punteggio+=100;
									DrawPacman(x_pacman_pos, y_pacman_pos);
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									mode=0;
									x_blinky_pos=x_blinky_spawn;
									y_blinky_pos=y_blinky_spawn;
									color_blinky=Red;
									disable_timer(0);
									reset_timer(0);
									blinky_respawn=1;
									routine_blinky=0;
									total_turns=15;
								} else {
									vite--;
									
									if(vite==-1){
										disable_timer(0);
										//disable_timer(1);
										LCD_Clear(Black);
										GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
										GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
										GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);
										gameover=1;
											
									} else{
										DrawLives(vite);
										DeletePacman(x_pacman_pos, y_pacman_pos);
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										J=0;
										x_pacman_pos=x_pacman_spawn;
										y_pacman_pos=y_pacman_spawn;
										DrawPacman(x_pacman_pos, y_pacman_pos);
									}
								}

							}								
							DrawPacman(x_pacman_pos, y_pacman_pos);					
							punteggio +=10;
							pills--;
							
							//DisplayInteger(56, 0, punteggio, White, Black);
							break;
						
						case 3:
							map[((y_pacman_pos-3*CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_pacman_pos/CELL_SIZE] = 0;
							DeletePacman(x_pacman_pos, y_pacman_pos);
							y_pacman_pos -= CELL_SIZE;
							if(check_positions(x_pacman_pos, y_pacman_pos, x_blinky_pos, y_blinky_pos)==1){
								if(mode==1){
									punteggio+=100;
									DrawPacman(x_pacman_pos, y_pacman_pos);
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									mode=0;
									x_blinky_pos=x_blinky_spawn;
									y_blinky_pos=y_blinky_spawn;
									color_blinky=Red;									
									disable_timer(0);
									reset_timer(0);
									blinky_respawn=1;
									routine_blinky=0;
									total_turns=15;
								} else {
									vite--;
									
									if(vite==-1){
										disable_timer(0);
										//disable_timer(1);
										LCD_Clear(Black);
										GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
										GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
										GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);
										gameover=1;										
											
									} else{
										DrawLives(vite);
										DeletePacman(x_pacman_pos, y_pacman_pos);
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										J=0;
										x_pacman_pos=x_pacman_spawn;
										y_pacman_pos=y_pacman_spawn;
										DrawPacman(x_pacman_pos, y_pacman_pos);
									}
								}

							}						
							DrawPacman(x_pacman_pos, y_pacman_pos);					
							punteggio +=50;
							pills--;
							//DisplayInteger(56, 0, punteggio, White, Black);
							if(blinky_respawn==0){
								color_blinky=Blue2;
								mode=1;
							}
							reset_timer(3);
							enable_timer(3);
						
							break;
						
						default:
							DeletePacman(x_pacman_pos, y_pacman_pos);
							y_pacman_pos -= CELL_SIZE;
							if(check_positions(x_pacman_pos, y_pacman_pos, x_blinky_pos, y_blinky_pos)==1){
								if(mode==1){
									punteggio+=100;
									DrawPacman(x_pacman_pos, y_pacman_pos);
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									mode=0;
									x_blinky_pos=x_blinky_spawn;
									y_blinky_pos=y_blinky_spawn;
									color_blinky=Red;		
									disable_timer(0);
									reset_timer(0);
									blinky_respawn=1;
									routine_blinky=0;
									total_turns=15;
								} else {
									vite--;
									
									if(vite==-1){
										disable_timer(0);
										//disable_timer(1);
										LCD_Clear(Black);
										GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
										GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
										GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);
										gameover=1;											
											
									} else{
										DrawLives(vite);
										DeletePacman(x_pacman_pos, y_pacman_pos);
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										J=0;
										x_pacman_pos=x_pacman_spawn;
										y_pacman_pos=y_pacman_spawn;
										DrawPacman(x_pacman_pos, y_pacman_pos);
									}
								}

							}						
							DrawPacman(x_pacman_pos, y_pacman_pos);
							break;
					
					
					}
					break;
				
					
				case 2:	//down
					switch(map[((y_pacman_pos+CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_pacman_pos/CELL_SIZE]){
						
						case 0:
							DeletePacman(x_pacman_pos, y_pacman_pos);
							y_pacman_pos += CELL_SIZE;
							if(check_positions(x_pacman_pos, y_pacman_pos, x_blinky_pos, y_blinky_pos)==1){
								if(mode==1){
									punteggio+=100;
									DrawPacman(x_pacman_pos, y_pacman_pos);
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									mode=0;
									x_blinky_pos=x_blinky_spawn;
									y_blinky_pos=y_blinky_spawn;
									color_blinky=Red;		
									disable_timer(0);
									reset_timer(0);
									blinky_respawn=1;
									routine_blinky=0;
									total_turns=15;
								} else {
									vite--;
									
									if(vite==-1){
										disable_timer(0);
										//disable_timer(1);
										LCD_Clear(Black);
										GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
										GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
										GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);
										gameover=1;											
											
									} else{
										DrawLives(vite);
										DeletePacman(x_pacman_pos, y_pacman_pos);
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										J=0;
										x_pacman_pos=x_pacman_spawn;
										y_pacman_pos=y_pacman_spawn;
										DrawPacman(x_pacman_pos, y_pacman_pos);
									}
								}

							}						
							DrawPacman(x_pacman_pos, y_pacman_pos);
							break;
						
						case 2:
							map[((y_pacman_pos+CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_pacman_pos/CELL_SIZE] = 0;
							DeletePacman(x_pacman_pos, y_pacman_pos);
							y_pacman_pos += CELL_SIZE;
							if(check_positions(x_pacman_pos, y_pacman_pos, x_blinky_pos, y_blinky_pos)==1){
								if(mode==1){
									punteggio+=100;
									DrawPacman(x_pacman_pos, y_pacman_pos);
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									mode=0;
									x_blinky_pos=x_blinky_spawn;
									y_blinky_pos=y_blinky_spawn;
									color_blinky=Red;										
									disable_timer(0);
									reset_timer(0);
									blinky_respawn=1;
									routine_blinky=0;
									total_turns=15;
								} else {
									vite--;
									
									if(vite==-1){
										disable_timer(0);
										//disable_timer(1);
										LCD_Clear(Black);
										GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
										GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
										GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);
										gameover=1;											
											
									} else{
										DrawLives(vite);
										DeletePacman(x_pacman_pos, y_pacman_pos);
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										J=0;
										x_pacman_pos=x_pacman_spawn;
										y_pacman_pos=y_pacman_spawn;
										DrawPacman(x_pacman_pos, y_pacman_pos);
									}
								}

							}						
							DrawPacman(x_pacman_pos, y_pacman_pos);
							punteggio+=10;
							pills--;
							//DisplayInteger(56, 0, punteggio, White, Black);
							break;
						
						case 3:
							map[((y_pacman_pos+CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_pacman_pos/CELL_SIZE] = 0;
							DeletePacman(x_pacman_pos, y_pacman_pos);
							y_pacman_pos += CELL_SIZE;
							if(check_positions(x_pacman_pos, y_pacman_pos, x_blinky_pos, y_blinky_pos)==1){
								if(mode==1){
									punteggio+=100;
									DrawPacman(x_pacman_pos, y_pacman_pos);
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									mode=0;
									x_blinky_pos=x_blinky_spawn;
									y_blinky_pos=y_blinky_spawn;
									color_blinky=Red;										
									disable_timer(0);
									reset_timer(0);
									blinky_respawn=1;
									routine_blinky=0;
									total_turns=15;
									
								} else {
									vite--;
									
									if(vite==-1){
										disable_timer(0);
										//disable_timer(1);
										LCD_Clear(Black);
										GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
										GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
										GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);	
										gameover=1;											
											
									} else{
										DrawLives(vite);
										DeletePacman(x_pacman_pos, y_pacman_pos);
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										J=0;
										x_pacman_pos=x_pacman_spawn;
										y_pacman_pos=y_pacman_spawn;
										DrawPacman(x_pacman_pos, y_pacman_pos);
									}
								}

							}						
							DrawPacman(x_pacman_pos, y_pacman_pos);
							punteggio+=50;
							pills--;
							//DisplayInteger(56, 0, punteggio, White, Black);
							if(blinky_respawn==0){
								color_blinky=Blue2;
								mode=1;
							}
							reset_timer(3);
							enable_timer(3);						
							break;							
						
						default:
							if(check_positions(x_pacman_pos, y_pacman_pos, x_blinky_pos, y_blinky_pos)==1){
								if(mode==1){
									punteggio+=100;
									DrawPacman(x_pacman_pos, y_pacman_pos);
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									mode=0;
									x_blinky_pos=x_blinky_spawn;
									y_blinky_pos=y_blinky_spawn;
									color_blinky=Red;
									disable_timer(0);
									reset_timer(0);
									blinky_respawn=1;
									routine_blinky=0;
									total_turns=15;
								} else {
									vite--;
									
									if(vite==-1){
										disable_timer(0);
										//disable_timer(1);
										LCD_Clear(Black);
										GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
										GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
										GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);
										gameover=1;
											
									} else{
										DrawLives(vite);
										DeletePacman(x_pacman_pos, y_pacman_pos);
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										J=0;
										x_pacman_pos=x_pacman_spawn;
										y_pacman_pos=y_pacman_spawn;
										DrawPacman(x_pacman_pos, y_pacman_pos);
									}
								}

							}							
							break;					
					
					}
					break;
				
				case 3:	//left
					switch(map[(y_pacman_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_pacman_pos-3*CELL_SIZE/2)/CELL_SIZE]){		
						case 1:
							if(check_positions(x_pacman_pos, y_pacman_pos, x_blinky_pos, y_blinky_pos)==1){
								if(mode==1){
									punteggio+=100;
									DrawPacman(x_pacman_pos, y_pacman_pos);
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									mode=0;
									x_blinky_pos=x_blinky_spawn;
									y_blinky_pos=y_blinky_spawn;
									color_blinky=Red;
									disable_timer(0);
									reset_timer(0);
									blinky_respawn=1;
									routine_blinky=0;
									total_turns=15;
								} else {
									vite--;
									
									if(vite==-1){
										disable_timer(0);
										//disable_timer(1);
										LCD_Clear(Black);
										GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
										GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
										GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);
										gameover=1;
											
									} else{
										DrawLives(vite);
										DeletePacman(x_pacman_pos, y_pacman_pos);
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										J=0;
										x_pacman_pos=x_pacman_spawn;
										y_pacman_pos=y_pacman_spawn;
										DrawPacman(x_pacman_pos, y_pacman_pos);
									}
								}

							}							
							break;
						
						case 2:
							map[(y_pacman_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_pacman_pos-3*CELL_SIZE/2)/CELL_SIZE] = 0;
							DeletePacman(x_pacman_pos, y_pacman_pos);
							x_pacman_pos -= CELL_SIZE;
							if(check_positions(x_pacman_pos, y_pacman_pos, x_blinky_pos, y_blinky_pos)==1){
								if(mode==1){
									punteggio+=100;
									DrawPacman(x_pacman_pos, y_pacman_pos);
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									mode=0;
									x_blinky_pos=x_blinky_spawn;
									y_blinky_pos=y_blinky_spawn;
									color_blinky=Red;										
									disable_timer(0);
									reset_timer(0);
									blinky_respawn=1;
									routine_blinky=0;
									total_turns=15;
								} else {
									vite--;
									
									if(vite==-1){
										disable_timer(0);
										//disable_timer(1);
										LCD_Clear(Black);
										GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
										GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
										GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);
										gameover=1;											
											
									} else{
										DrawLives(vite);
										DeletePacman(x_pacman_pos, y_pacman_pos);
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										J=0;
										x_pacman_pos=x_pacman_spawn;
										y_pacman_pos=y_pacman_spawn;
										DrawPacman(x_pacman_pos, y_pacman_pos);
									}
								}

							}						
							DrawPacman(x_pacman_pos, y_pacman_pos);
							punteggio+=10;
							pills--;
							//DisplayInteger(56, 0, punteggio, White, Black);
							break;

						case 3:
							map[(y_pacman_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_pacman_pos-3*CELL_SIZE/2)/CELL_SIZE] = 0;
							DeletePacman(x_pacman_pos, y_pacman_pos);
							x_pacman_pos -= CELL_SIZE;
							if(check_positions(x_pacman_pos, y_pacman_pos, x_blinky_pos, y_blinky_pos)==1){
								if(mode==1){
									punteggio+=100;
									DrawPacman(x_pacman_pos, y_pacman_pos);
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									mode=0;
									x_blinky_pos=x_blinky_spawn;
									y_blinky_pos=y_blinky_spawn;
									color_blinky=Red;										
									disable_timer(0);
									reset_timer(0);
									blinky_respawn=1;
									routine_blinky=0;
									total_turns=15;
								} else {
									vite--;
									
									if(vite==-1){
										disable_timer(0);
										//disable_timer(1);
										LCD_Clear(Black);
										GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
										GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
										GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);	
										gameover=1;											
											
									} else{
										DrawLives(vite);
										DeletePacman(x_pacman_pos, y_pacman_pos);
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										J=0;
										x_pacman_pos=x_pacman_spawn;
										y_pacman_pos=y_pacman_spawn;
										DrawPacman(x_pacman_pos, y_pacman_pos);
									}
								}

							}						
							DrawPacman(x_pacman_pos, y_pacman_pos);
							punteggio+=50;
							//DisplayInteger(56, 0, punteggio, White, Black);
							pills--;
							if(blinky_respawn==0){
								color_blinky=Blue2;
								mode=1;
							}
							reset_timer(3);
							enable_timer(3);						
							break;						
				
						case 5:
							DeletePacman(x_pacman_pos, y_pacman_pos);
							x_pacman_pos = 236;
							DrawPacman(x_pacman_pos, y_pacman_pos);
							break;

						default:
							DeletePacman(x_pacman_pos, y_pacman_pos);
							x_pacman_pos -= CELL_SIZE;
							if(check_positions(x_pacman_pos, y_pacman_pos, x_blinky_pos, y_blinky_pos)==1){
								if(mode==1){
									punteggio+=100;
									DrawPacman(x_pacman_pos, y_pacman_pos);
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									mode=0;
									x_blinky_pos=x_blinky_spawn;
									y_blinky_pos=y_blinky_spawn;
									color_blinky=Red;										
									disable_timer(0);
									reset_timer(0);
									blinky_respawn=1;
									routine_blinky=0;
									total_turns=15;
								} else {
									vite--;
									
									if(vite==-1){
										disable_timer(0);
										//disable_timer(1);
										LCD_Clear(Black);
										GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
										GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
										GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);		
										gameover=1;											
											
									} else{
										DrawLives(vite);
										DeletePacman(x_pacman_pos, y_pacman_pos);
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										J=0;
										x_pacman_pos=x_pacman_spawn;
										y_pacman_pos=y_pacman_spawn;
										DrawPacman(x_pacman_pos, y_pacman_pos);
									}
								}

							}						
							DrawPacman(x_pacman_pos, y_pacman_pos);
							break;
						
						
					}
					break;
				
				case 4:	//right
					switch(map[(y_pacman_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_pacman_pos+CELL_SIZE/2)/CELL_SIZE]){
								
						case 1:
							if(check_positions(x_pacman_pos, y_pacman_pos, x_blinky_pos, y_blinky_pos)==1){
								if(mode==1){
									punteggio+=100;
									DrawPacman(x_pacman_pos, y_pacman_pos);
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									mode=0;
									x_blinky_pos=x_blinky_spawn;
									y_blinky_pos=y_blinky_spawn;
									color_blinky=Red;
									disable_timer(0);
									reset_timer(0);
									blinky_respawn=1;
									routine_blinky=0;
									total_turns=15;
								} else {
									vite--;
									
									if(vite==-1){
										disable_timer(0);
										//disable_timer(1);
										LCD_Clear(Black);
										GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
										GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
										GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);
										gameover=1;
											
									} else{
										DrawLives(vite);
										DeletePacman(x_pacman_pos, y_pacman_pos);
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										J=0;
										x_pacman_pos=x_pacman_spawn;
										y_pacman_pos=y_pacman_spawn;
										DrawPacman(x_pacman_pos, y_pacman_pos);
									}
								}

							}							
							break;
						
						case 2:
							map[(y_pacman_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_pacman_pos+CELL_SIZE/2)/CELL_SIZE] = 0;
							DeletePacman(x_pacman_pos, y_pacman_pos);
							x_pacman_pos += CELL_SIZE;
							if(check_positions(x_pacman_pos, y_pacman_pos, x_blinky_pos, y_blinky_pos)==1){
								if(mode==1){
									punteggio+=100;
									DrawPacman(x_pacman_pos, y_pacman_pos);
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									mode=0;
									x_blinky_pos=x_blinky_spawn;
									y_blinky_pos=y_blinky_spawn;
									color_blinky=Red;										
									disable_timer(0);
									reset_timer(0);
									blinky_respawn=1;
									routine_blinky=0;
									total_turns=15;
								} else {
									vite--;
									
									if(vite==-1){
										disable_timer(0);
										//disable_timer(1);
										LCD_Clear(Black);
										GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
										GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
										GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);				
										gameover=1;	
											
									} else{
										DrawLives(vite);
										DeletePacman(x_pacman_pos, y_pacman_pos);
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										J=0;
										x_pacman_pos=x_pacman_spawn;
										y_pacman_pos=y_pacman_spawn;
										DrawPacman(x_pacman_pos, y_pacman_pos);
									}
								}

							}						
							DrawPacman(x_pacman_pos, y_pacman_pos);
							punteggio+=10;
							pills--;
							//DisplayInteger(56, 0, punteggio, White, Black);
							break;

						case 3:
							map[(y_pacman_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_pacman_pos+CELL_SIZE/2)/CELL_SIZE] = 0;
							DeletePacman(x_pacman_pos, y_pacman_pos);
							x_pacman_pos += CELL_SIZE;
							if(check_positions(x_pacman_pos, y_pacman_pos, x_blinky_pos, y_blinky_pos)==1){
								if(mode==1){
									punteggio+=100;
									DrawPacman(x_pacman_pos, y_pacman_pos);
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									mode=0;
									x_blinky_pos=x_blinky_spawn;
									y_blinky_pos=y_blinky_spawn;
									color_blinky=Red;										
									disable_timer(0);
									reset_timer(0);
									blinky_respawn=1;
									routine_blinky=0;
									total_turns=15;
								} else {
									vite--;
									
									if(vite==-1){
										disable_timer(0);
										//disable_timer(1);
										LCD_Clear(Black);
										GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
										GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
										GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);	
										gameover=1;											
											
									} else{
										DrawLives(vite);
										DeletePacman(x_pacman_pos, y_pacman_pos);
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										J=0;
										x_pacman_pos=x_pacman_spawn;
										y_pacman_pos=y_pacman_spawn;
										DrawPacman(x_pacman_pos, y_pacman_pos);
									}
								}

							}						
							DrawPacman(x_pacman_pos, y_pacman_pos);
							punteggio+=50;
							pills--;
							//DisplayInteger(56, 0, punteggio, White, Black);
							if(blinky_respawn==0){
								color_blinky=Blue2;
								mode=1;
							}
							reset_timer(3);
							enable_timer(3);						
							break;						
				
						case 5:
							DeletePacman(x_pacman_pos, y_pacman_pos);
							x_pacman_pos = 4;
							DrawPacman(x_pacman_pos, y_pacman_pos);
							break;

						default:
							DeletePacman(x_pacman_pos, y_pacman_pos);
							x_pacman_pos += CELL_SIZE;
							if(check_positions(x_pacman_pos, y_pacman_pos, x_blinky_pos, y_blinky_pos)==1){
								if(mode==1){
									punteggio+=100;
									DrawPacman(x_pacman_pos, y_pacman_pos);
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									mode=0;
									x_blinky_pos=x_blinky_spawn;
									y_blinky_pos=y_blinky_spawn;
									color_blinky=Red;										
									disable_timer(0);
									reset_timer(0);
									blinky_respawn=1;
									routine_blinky=0;
									total_turns=15;
								} else {
									vite--;
									
									if(vite==-1){
										disable_timer(0);
										//disable_timer(1);
										LCD_Clear(Black);
										GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
										GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
										GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);	
										gameover=1;											
											
									} else{
										DrawLives(vite);
										DeletePacman(x_pacman_pos, y_pacman_pos);
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										J=0;
										x_pacman_pos=x_pacman_spawn;
										y_pacman_pos=y_pacman_spawn;
										DrawPacman(x_pacman_pos, y_pacman_pos);
									}
								}

							}						
							DrawPacman(x_pacman_pos, y_pacman_pos);
							break;
						
						
					}
					break;
				
				default:
					if(check_positions(x_pacman_pos, y_pacman_pos, x_blinky_pos, y_blinky_pos)==1){
						if(mode==1){
							punteggio+=100;
							DrawPacman(x_pacman_pos, y_pacman_pos);
							DeleteBlinky(x_blinky_pos, y_blinky_pos);
							mode=0;
							x_blinky_pos=x_blinky_spawn;
							y_blinky_pos=y_blinky_spawn;
							color_blinky=Red;
							disable_timer(0);
							reset_timer(0);
							blinky_respawn=1;
							routine_blinky=0;
							total_turns=15;
						} else {
							vite--;
							
							if(vite==-1){
								disable_timer(0);
								//disable_timer(1);
								LCD_Clear(Black);
								GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
								GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
								GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);
								gameover=1;
									
							} else{
								DrawLives(vite);
								DeletePacman(x_pacman_pos, y_pacman_pos);
								DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
								J=0;
								x_pacman_pos=x_pacman_spawn;
								y_pacman_pos=y_pacman_spawn;
								DrawPacman(x_pacman_pos, y_pacman_pos);
							}
						}

					}					
					break;
			}
			break;
		
		case 2:	//pausa in game
			
			GUI_Text(96, 152, (uint8_t *) "PAUSE", White, Black);
			disable_timer(1);
			disable_timer(2);
			disable_timer(3);
			break;
		
		
		case 3:	//in game ma qui entra solo dalla seconda volta in poi (fatto per non avviare i timer ogni volta)
			
			if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){	/* Joytick UP pressed */
				J=1;
			}

			
			if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){	/* Joytick DOWN pressed */
				J=2;
			}

			
			if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){	/* Joytick LEFT pressed */
				J=3;
				
			}

			
			if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){	/* Joytick RIGHT pressed */
				J=4;
			}

			switch(J){
				
				case 1: //up
					switch (map[((y_pacman_pos-3*CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_pacman_pos/CELL_SIZE]){
						
						case 1:
							if(check_positions(x_pacman_pos, y_pacman_pos, x_blinky_pos, y_blinky_pos)==1){
								if(mode==1){
									punteggio+=100;
									DrawPacman(x_pacman_pos, y_pacman_pos);
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									mode=0;
									x_blinky_pos=x_blinky_spawn;
									y_blinky_pos=y_blinky_spawn;
									color_blinky=Red;
									disable_timer(0);
									reset_timer(0);
									blinky_respawn=1;
									routine_blinky=0;
									total_turns=15;
								} else {
									vite--;
									
									if(vite==-1){
										disable_timer(0);
										//disable_timer(1);
										LCD_Clear(Black);
										GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
										GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
										GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);
										gameover=1;
											
									} else{
										DrawLives(vite);
										DeletePacman(x_pacman_pos, y_pacman_pos);
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										J=0;
										x_pacman_pos=x_pacman_spawn;
										y_pacman_pos=y_pacman_spawn;
										DrawPacman(x_pacman_pos, y_pacman_pos);
									}
								}

							}							
							break;
						
						case 2:
							map[((y_pacman_pos-3*CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_pacman_pos/CELL_SIZE] = 0;
							DeletePacman(x_pacman_pos, y_pacman_pos);
							y_pacman_pos -= CELL_SIZE;
							if(check_positions(x_pacman_pos, y_pacman_pos, x_blinky_pos, y_blinky_pos)==1){
								if(mode==1){
									punteggio+=100;
									DrawPacman(x_pacman_pos, y_pacman_pos);
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									mode=0;
									x_blinky_pos=x_blinky_spawn;
									y_blinky_pos=y_blinky_spawn;
									color_blinky=Red;										
									disable_timer(0);
									reset_timer(0);
									blinky_respawn=1;
									routine_blinky=0;
									total_turns=15;
								} else {
									vite--;
									
									if(vite==-1){
										disable_timer(0);
										//disable_timer(1);
										LCD_Clear(Black);
										GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
										GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
										GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);
										gameover=1;											
											
									} else{
										DrawLives(vite);
										DeletePacman(x_pacman_pos, y_pacman_pos);
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										J=0;
										x_pacman_pos=x_pacman_spawn;
										y_pacman_pos=y_pacman_spawn;
										DrawPacman(x_pacman_pos, y_pacman_pos);
									}
								}

							}						
							DrawPacman(x_pacman_pos, y_pacman_pos);					
							punteggio +=10;
							pills--;
							//DisplayInteger(56, 0, punteggio, White, Black);
							break;

						case 3:
							map[((y_pacman_pos-3*CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_pacman_pos/CELL_SIZE] = 0;
							DeletePacman(x_pacman_pos, y_pacman_pos);
							y_pacman_pos -= CELL_SIZE;
							if(check_positions(x_pacman_pos, y_pacman_pos, x_blinky_pos, y_blinky_pos)==1){
								if(mode==1){
									punteggio+=100;
									DrawPacman(x_pacman_pos, y_pacman_pos);
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									mode=0;
									x_blinky_pos=x_blinky_spawn;
									y_blinky_pos=y_blinky_spawn;
									color_blinky=Red;										
									disable_timer(0);
									reset_timer(0);
									blinky_respawn=1;
									routine_blinky=0;
									total_turns=15;
								} else {
									vite--;
									
									if(vite==-1){
										disable_timer(0);
										//disable_timer(1);
										LCD_Clear(Black);
										GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
										GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
										GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);	
										gameover=1;											
											
									} else{
										DrawLives(vite);
										DeletePacman(x_pacman_pos, y_pacman_pos);
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										J=0;
										x_pacman_pos=x_pacman_spawn;
										y_pacman_pos=y_pacman_spawn;
										DrawPacman(x_pacman_pos, y_pacman_pos);
									}
								}

							}						
							DrawPacman(x_pacman_pos, y_pacman_pos);					
							punteggio +=50;
							pills--;
							//DisplayInteger(56, 0, punteggio, White, Black);
							if(blinky_respawn==0){
								color_blinky=Blue2;
								mode=1;
							}
							reset_timer(3);
							enable_timer(3);						
							break;						
						
						default:
							DeletePacman(x_pacman_pos, y_pacman_pos);
							y_pacman_pos -= CELL_SIZE;
							if(check_positions(x_pacman_pos, y_pacman_pos, x_blinky_pos, y_blinky_pos)==1){
								if(mode==1){
									punteggio+=100;
									DrawPacman(x_pacman_pos, y_pacman_pos);
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									mode=0;
									x_blinky_pos=x_blinky_spawn;
									y_blinky_pos=y_blinky_spawn;
									color_blinky=Red;										
									disable_timer(0);
									reset_timer(0);
									blinky_respawn=1;
									routine_blinky=0;
									total_turns=15;
								} else {
									vite--;
									
									if(vite==-1){
										disable_timer(0);
										//disable_timer(1);
										LCD_Clear(Black);
										GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
										GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
										GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);		
										gameover=1;											
											
									} else{
										DrawLives(vite);
										DeletePacman(x_pacman_pos, y_pacman_pos);
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										J=0;
										x_pacman_pos=x_pacman_spawn;
										y_pacman_pos=y_pacman_spawn;
										DrawPacman(x_pacman_pos, y_pacman_pos);
									}
								}

							}						
							DrawPacman(x_pacman_pos, y_pacman_pos);
							break;
					
					
					}
					break;
				
					
				case 2:	//down
					switch(map[((y_pacman_pos+CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_pacman_pos/CELL_SIZE]){
						
						case 0:
							DeletePacman(x_pacman_pos, y_pacman_pos);
							y_pacman_pos += CELL_SIZE;
							if(check_positions(x_pacman_pos, y_pacman_pos, x_blinky_pos, y_blinky_pos)==1){
								if(mode==1){
									punteggio+=100;
									DrawPacman(x_pacman_pos, y_pacman_pos);
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									mode=0;
									x_blinky_pos=x_blinky_spawn;
									y_blinky_pos=y_blinky_spawn;
									color_blinky=Red;										
									disable_timer(0);
									reset_timer(0);
									blinky_respawn=1;
									routine_blinky=0;
									total_turns=15;
								} else {
									vite--;
									
									if(vite==-1){
										disable_timer(0);
										//disable_timer(1);
										LCD_Clear(Black);
										GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
										GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
										GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);
										gameover=1;											
											
									} else{
										DrawLives(vite);
										DeletePacman(x_pacman_pos, y_pacman_pos);
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										J=0;
										x_pacman_pos=x_pacman_spawn;
										y_pacman_pos=y_pacman_spawn;
										DrawPacman(x_pacman_pos, y_pacman_pos);
									}
								}

							}						
							DrawPacman(x_pacman_pos, y_pacman_pos);							
							break;
						
						case 2:
							map[((y_pacman_pos+CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_pacman_pos/CELL_SIZE] = 0;
							DeletePacman(x_pacman_pos, y_pacman_pos);
							y_pacman_pos += CELL_SIZE;
							if(check_positions(x_pacman_pos, y_pacman_pos, x_blinky_pos, y_blinky_pos)==1){
								if(mode==1){
									punteggio+=100;
									DrawPacman(x_pacman_pos, y_pacman_pos);
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									mode=0;
									x_blinky_pos=x_blinky_spawn;
									y_blinky_pos=y_blinky_spawn;
									color_blinky=Red;										
									disable_timer(0);
									reset_timer(0);
									blinky_respawn=1;
									routine_blinky=0;
									total_turns=15;
								} else {
									vite--;
									
									if(vite==-1){
										disable_timer(0);
										//disable_timer(1);
										LCD_Clear(Black);
										GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
										GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
										GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);		
										gameover=1;											
											
									} else{
										DrawLives(vite);
										DeletePacman(x_pacman_pos, y_pacman_pos);
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										J=0;
										x_pacman_pos=x_pacman_spawn;
										y_pacman_pos=y_pacman_spawn;
										DrawPacman(x_pacman_pos, y_pacman_pos);
									}
								}

							}						
							DrawPacman(x_pacman_pos, y_pacman_pos);
							punteggio+=10;
							pills--;
							//DisplayInteger(56, 0, punteggio, White, Black);
							break;

						case 3:
							map[((y_pacman_pos+CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_pacman_pos/CELL_SIZE] = 0;
							DeletePacman(x_pacman_pos, y_pacman_pos);
							y_pacman_pos += CELL_SIZE;
							if(check_positions(x_pacman_pos, y_pacman_pos, x_blinky_pos, y_blinky_pos)==1){
								if(mode==1){
									punteggio+=100;
									DrawPacman(x_pacman_pos, y_pacman_pos);
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									mode=0;
									x_blinky_pos=x_blinky_spawn;
									y_blinky_pos=y_blinky_spawn;
									color_blinky=Red;										
									disable_timer(0);
									reset_timer(0);
									blinky_respawn=1;
									routine_blinky=0;
									total_turns=15;
								} else {
									vite--;
									
									if(vite==-1){
										disable_timer(0);
										//disable_timer(1);
										LCD_Clear(Black);
										GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
										GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
										GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);
										gameover=1;											
											
									} else{
										DrawLives(vite);
										DeletePacman(x_pacman_pos, y_pacman_pos);
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										J=0;
										x_pacman_pos=x_pacman_spawn;
										y_pacman_pos=y_pacman_spawn;
										DrawPacman(x_pacman_pos, y_pacman_pos);
									}
								}

							}						
							DrawPacman(x_pacman_pos, y_pacman_pos);
							punteggio+=50;
							pills--;
							//DisplayInteger(56, 0, punteggio, White, Black);
							if(blinky_respawn==0){
								color_blinky=Blue2;
								mode=1;
							}
							reset_timer(3);
							enable_timer(3);						
							break;						
						
						default:
							if(check_positions(x_pacman_pos, y_pacman_pos, x_blinky_pos, y_blinky_pos)==1){
								if(mode==1){
									punteggio+=100;
									DrawPacman(x_pacman_pos, y_pacman_pos);
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									mode=0;
									x_blinky_pos=x_blinky_spawn;
									y_blinky_pos=y_blinky_spawn;
									color_blinky=Red;
									disable_timer(0);
									reset_timer(0);
									blinky_respawn=1;
									routine_blinky=0;
									total_turns=15;
								} else {
									vite--;
									
									if(vite==-1){
										disable_timer(0);
										//disable_timer(1);
										LCD_Clear(Black);
										GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
										GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
										GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);
										gameover=1;
											
									} else{
										DrawLives(vite);
										DeletePacman(x_pacman_pos, y_pacman_pos);
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										J=0;
										x_pacman_pos=x_pacman_spawn;
										y_pacman_pos=y_pacman_spawn;
										DrawPacman(x_pacman_pos, y_pacman_pos);
									}
								}

							}							
							break;					
					
					}
					break;
				
				case 3:	//left
					switch(map[(y_pacman_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_pacman_pos-3*CELL_SIZE/2)/CELL_SIZE]){		
						case 1:
							if(check_positions(x_pacman_pos, y_pacman_pos, x_blinky_pos, y_blinky_pos)==1){
								if(mode==1){
									punteggio+=100;
									DrawPacman(x_pacman_pos, y_pacman_pos);
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									mode=0;
									x_blinky_pos=x_blinky_spawn;
									y_blinky_pos=y_blinky_spawn;
									color_blinky=Red;
									disable_timer(0);
									reset_timer(0);
									blinky_respawn=1;
									routine_blinky=0;
									total_turns=15;
								} else {
									vite--;
									
									if(vite==-1){
										disable_timer(0);
										//disable_timer(1);
										LCD_Clear(Black);
										GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
										GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
										GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);
										gameover=1;
											
									} else{
										DrawLives(vite);
										DeletePacman(x_pacman_pos, y_pacman_pos);
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										J=0;
										x_pacman_pos=x_pacman_spawn;
										y_pacman_pos=y_pacman_spawn;
										DrawPacman(x_pacman_pos, y_pacman_pos);
									}
								}

							}							
							break;
						
						case 2:
							map[(y_pacman_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_pacman_pos-3*CELL_SIZE/2)/CELL_SIZE] = 0;
							DeletePacman(x_pacman_pos, y_pacman_pos);
							x_pacman_pos -= CELL_SIZE;
							if(check_positions(x_pacman_pos, y_pacman_pos, x_blinky_pos, y_blinky_pos)==1){
								if(mode==1){
									punteggio+=100;
									DrawPacman(x_pacman_pos, y_pacman_pos);
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									mode=0;
									x_blinky_pos=x_blinky_spawn;
									y_blinky_pos=y_blinky_spawn;
									color_blinky=Red;										
									disable_timer(0);
									reset_timer(0);
									blinky_respawn=1;
									routine_blinky=0;
									total_turns=15;
								} else {
									vite--;
									
									if(vite==-1){
										disable_timer(0);
										//disable_timer(1);
										LCD_Clear(Black);
										GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
										GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
										GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);	
										gameover=1;											
											
									} else{
										DrawLives(vite);
										DeletePacman(x_pacman_pos, y_pacman_pos);
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										J=0;
										x_pacman_pos=x_pacman_spawn;
										y_pacman_pos=y_pacman_spawn;
										DrawPacman(x_pacman_pos, y_pacman_pos);
									}
								}

							}						
							DrawPacman(x_pacman_pos, y_pacman_pos);
							punteggio+=10;
							pills--;
							//DisplayInteger(56, 0, punteggio, White, Black);
							break;

						case 3:
							map[(y_pacman_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_pacman_pos-3*CELL_SIZE/2)/CELL_SIZE] = 0;
							DeletePacman(x_pacman_pos, y_pacman_pos);
							x_pacman_pos -= CELL_SIZE;
							if(check_positions(x_pacman_pos, y_pacman_pos, x_blinky_pos, y_blinky_pos)==1){
								if(mode==1){
									punteggio+=100;
									DrawPacman(x_pacman_pos, y_pacman_pos);
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									mode=0;
									x_blinky_pos=x_blinky_spawn;
									y_blinky_pos=y_blinky_spawn;
									color_blinky=Red;										
									disable_timer(0);
									reset_timer(0);
									blinky_respawn=1;
									routine_blinky=0;
									total_turns=15;
								} else {
									vite--;
									
									if(vite==-1){
										disable_timer(0);
										//disable_timer(1);
										LCD_Clear(Black);
										GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
										GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
										GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);	
										gameover=1;											
											
									} else{
										DrawLives(vite);
										DeletePacman(x_pacman_pos, y_pacman_pos);
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										J=0;
										x_pacman_pos=x_pacman_spawn;
										y_pacman_pos=y_pacman_spawn;
										DrawPacman(x_pacman_pos, y_pacman_pos);
									}
								}

							}						
							DrawPacman(x_pacman_pos, y_pacman_pos);
							punteggio+=50;
							pills--;
							//DisplayInteger(56, 0, punteggio, White, Black);
							if(blinky_respawn==0){
								color_blinky=Blue2;
								mode=1;
							}
							reset_timer(3);
							enable_timer(3);						
							break;						
				
						case 5:
							DeletePacman(x_pacman_pos, y_pacman_pos);
							x_pacman_pos = 236;
							DrawPacman(x_pacman_pos, y_pacman_pos);
							break;

						default:
							DeletePacman(x_pacman_pos, y_pacman_pos);
							x_pacman_pos -= CELL_SIZE;
							if(check_positions(x_pacman_pos, y_pacman_pos, x_blinky_pos, y_blinky_pos)==1){
								if(mode==1){
									punteggio+=100;
									DrawPacman(x_pacman_pos, y_pacman_pos);
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									mode=0;
									x_blinky_pos=x_blinky_spawn;
									y_blinky_pos=y_blinky_spawn;
									color_blinky=Red;										
									disable_timer(0);
									reset_timer(0);
									blinky_respawn=1;
									routine_blinky=0;
									total_turns=15;
								} else {
									vite--;
									
									if(vite==-1){
										disable_timer(0);
										//disable_timer(1);
										LCD_Clear(Black);
										GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
										GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
										GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);	
										gameover=1;											
											
									} else{
										DrawLives(vite);
										DeletePacman(x_pacman_pos, y_pacman_pos);
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										J=0;
										x_pacman_pos=x_pacman_spawn;
										y_pacman_pos=y_pacman_spawn;
										DrawPacman(x_pacman_pos, y_pacman_pos);
									}
								}

							}						
							DrawPacman(x_pacman_pos, y_pacman_pos);
							break;
						
						
					}
					break;
				
				case 4:	//right
					switch(map[(y_pacman_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_pacman_pos+CELL_SIZE/2)/CELL_SIZE]){
								
						case 1:
							if(check_positions(x_pacman_pos, y_pacman_pos, x_blinky_pos, y_blinky_pos)==1){
								if(mode==1){
									punteggio+=100;
									DrawPacman(x_pacman_pos, y_pacman_pos);
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									mode=0;
									x_blinky_pos=x_blinky_spawn;
									y_blinky_pos=y_blinky_spawn;
									color_blinky=Red;
									disable_timer(0);
									reset_timer(0);
									blinky_respawn=1;
									routine_blinky=0;
									total_turns=15;
								} else {
									vite--;
									
									if(vite==-1){
										disable_timer(0);
										//disable_timer(1);
										LCD_Clear(Black);
										GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
										GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
										GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);
										gameover=1;
											
									} else{
										DrawLives(vite);
										DeletePacman(x_pacman_pos, y_pacman_pos);
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										J=0;
										x_pacman_pos=x_pacman_spawn;
										y_pacman_pos=y_pacman_spawn;
										DrawPacman(x_pacman_pos, y_pacman_pos);
									}
								}

							}							
							break;
						
						case 2:
							map[(y_pacman_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_pacman_pos+CELL_SIZE/2)/CELL_SIZE] = 0;
							DeletePacman(x_pacman_pos, y_pacman_pos);
							x_pacman_pos += CELL_SIZE;
							if(check_positions(x_pacman_pos, y_pacman_pos, x_blinky_pos, y_blinky_pos)==1){
								if(mode==1){
									punteggio+=100;
									DrawPacman(x_pacman_pos, y_pacman_pos);
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									mode=0;
									x_blinky_pos=x_blinky_spawn;
									y_blinky_pos=y_blinky_spawn;
									color_blinky=Red;										
									disable_timer(0);
									reset_timer(0);
									blinky_respawn=1;
									routine_blinky=0;
									total_turns=15;
								} else {
									vite--;
									
									if(vite==-1){
										disable_timer(0);
										//disable_timer(1);
										LCD_Clear(Black);
										GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
										GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
										GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);		
										gameover=1;											
											
									} else{
										DrawLives(vite);
										DeletePacman(x_pacman_pos, y_pacman_pos);
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										J=0;
										x_pacman_pos=x_pacman_spawn;
										y_pacman_pos=y_pacman_spawn;
										DrawPacman(x_pacman_pos, y_pacman_pos);
									}
								}

							}						
							DrawPacman(x_pacman_pos, y_pacman_pos);
							punteggio+=10;
							pills--;
							//DisplayInteger(56, 0, punteggio, White, Black);
							break;

						case 3:
							map[(y_pacman_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_pacman_pos+CELL_SIZE/2)/CELL_SIZE] = 0;
							DeletePacman(x_pacman_pos, y_pacman_pos);
							x_pacman_pos += CELL_SIZE;
							if(check_positions(x_pacman_pos, y_pacman_pos, x_blinky_pos, y_blinky_pos)==1){
								if(mode==1){
									punteggio+=100;
									DrawPacman(x_pacman_pos, y_pacman_pos);
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									mode=0;
									x_blinky_pos=x_blinky_spawn;
									y_blinky_pos=y_blinky_spawn;
									color_blinky=Red;										
									disable_timer(0);
									reset_timer(0);
									blinky_respawn=1;
									routine_blinky=0;
									total_turns=15;
								} else {
									vite--;
									
									if(vite==-1){
										disable_timer(0);
										//disable_timer(1);
										LCD_Clear(Black);
										GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
										GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
										GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);
										gameover=1;											
											
									} else{
										DrawLives(vite);
										DeletePacman(x_pacman_pos, y_pacman_pos);
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										J=0;
										x_pacman_pos=x_pacman_spawn;
										y_pacman_pos=y_pacman_spawn;
										DrawPacman(x_pacman_pos, y_pacman_pos);
									}
								}

							}						
							DrawPacman(x_pacman_pos, y_pacman_pos);
							punteggio+=50;
							pills--;
							//DisplayInteger(56, 0, punteggio, White, Black);
							if(blinky_respawn==0){
								color_blinky=Blue2;
								mode=1;
							}
							reset_timer(3);
							enable_timer(3);						
							break;						
				
						case 5:
							DeletePacman(x_pacman_pos, y_pacman_pos);
							x_pacman_pos = 4;
							DrawPacman(x_pacman_pos, y_pacman_pos);
							break;

						default:
							DeletePacman(x_pacman_pos, y_pacman_pos);
							x_pacman_pos += CELL_SIZE;
							if(check_positions(x_pacman_pos, y_pacman_pos, x_blinky_pos, y_blinky_pos)==1){
								if(mode==1){
									punteggio+=100;
									DrawPacman(x_pacman_pos, y_pacman_pos);
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									mode=0;
									x_blinky_pos=x_blinky_spawn;
									y_blinky_pos=y_blinky_spawn;
									color_blinky=Red;										
									disable_timer(0);
									reset_timer(0);
									blinky_respawn=1;
									routine_blinky=0;
									total_turns=15;
								} else {
									vite--;
									
									if(vite==-1){
										disable_timer(0);
										//disable_timer(1);
										LCD_Clear(Black);
										GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
										GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
										GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);	
										gameover=1;											
											
									} else{
										DrawLives(vite);
										DeletePacman(x_pacman_pos, y_pacman_pos);
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										J=0;
										x_pacman_pos=x_pacman_spawn;
										y_pacman_pos=y_pacman_spawn;
										DrawPacman(x_pacman_pos, y_pacman_pos);
									}
								}

							}						
							DrawPacman(x_pacman_pos, y_pacman_pos);
							break;
						
						
					}
					break;
				
				default:
					if(check_positions(x_pacman_pos, y_pacman_pos, x_blinky_pos, y_blinky_pos)==1){
						if(mode==1){
							punteggio+=100;
							DrawPacman(x_pacman_pos, y_pacman_pos);
							DeleteBlinky(x_blinky_pos, y_blinky_pos);
							mode=0;
							x_blinky_pos=x_blinky_spawn;
							y_blinky_pos=y_blinky_spawn;
							color_blinky=Red;
							disable_timer(0);
							reset_timer(0);
							blinky_respawn=1;
							routine_blinky=0;
							total_turns=15;
						} else {
							vite--;
							
							if(vite==-1){
								disable_timer(0);
								//disable_timer(1);
								LCD_Clear(Black);
								GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
								GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
								GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);
								gameover=1;
											
							} else{
								DrawLives(vite);
								DeletePacman(x_pacman_pos, y_pacman_pos);
								DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
								J=0;
								x_pacman_pos=x_pacman_spawn;
								y_pacman_pos=y_pacman_spawn;
								DrawPacman(x_pacman_pos, y_pacman_pos);
							}
						}

					}					
					break;
			}			
			break;
		
		default:
			break;
	}
	

				
		



/*************************INT0***************************/
if(down_0 !=0){
	if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){
		switch(down_0){
			case 2:
				switch(pausa){
					case 0:	//0->1 start
						pausa=1;
						break;
					case 1:	//1->2 pause
						pausa=2;
						break;
					case 2:	//2->1 resume
						pausa=1;
						break;
					case 3:	//3->1 pause
						pausa=2;
						break;
					default:
						break;
					break;
				}
			default:
				break;
		}
		
		down_0++;
	}
	else {	/* button released */
		down_0=0;			
		NVIC_EnableIRQ(EINT0_IRQn);							 /* enable Button interrupts			*/
		LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
	}
} // end INT0


	
	reset_RIT();
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}
}


//refresh



/******************************************************************************
**                            End Of File
******************************************************************************/

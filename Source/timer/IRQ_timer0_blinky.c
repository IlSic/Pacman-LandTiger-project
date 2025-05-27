/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <string.h>
#include <LPC17xx.h>
#include <timer.h>
#include <RIT/RIT.h>
#include <../GLCD/GLCD.h> 
#include <../TouchPanel/TouchPanel.h>
#include <Pacman/pacman.h>

uint16_t x_blinky_pos = x_blinky_spawn;
uint16_t y_blinky_pos = y_blinky_spawn;
extern int pausa;
extern volatile int x_pacman_pos;
extern volatile int y_pacman_pos;
extern volatile int punteggio;

extern volatile int vite;
int quadrante_pacman = 1;	//indica il quadrante in cui si trova pacman (up-left allo spawn)
int routine_blinky = 0;	//indica la routine di quale quadrante sta eseguendo blinky
int total_turns = 15;

int prosegui = 1;
volatile uint16_t mode = 0;	//0=chase, 1=frightened
uint16_t color_blinky = Red;

extern int J;
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
	if(LPC_TIM0->IR & 1){ // MR0 questo timer gestisce blinky come il rit gestisce pacman
		
		
		if(y_pacman_pos<=160){
			//metà superiore
			if(x_pacman_pos<=120){
				quadrante_pacman=1; //up-left
			} else{
				quadrante_pacman=2; //up-right
			}
		} else{
			//metà inferiore
			if(x_pacman_pos<=120){
				quadrante_pacman=3; //down-left
			} else{
				quadrante_pacman=4; //down-right
			}			
		}
		
 

		//punteggio_old = punteggio;		
	
		
		switch(pausa){
		
			case 0: //pausa pre-game
				break;
			
			case 3: //in game
				
				switch(routine_blinky){
					case 0: //no-routine
						DeleteBlinky(x_blinky_pos, y_blinky_pos);
						switch(map[((y_blinky_pos-3*CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]){ //controllo sulla cella sopra perché allo spawn deve salire fino al muro
							case 1: //muro, ora deve scegliere quale routine eseguire a seconda di dove si trova pacman
								DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
								switch(quadrante_pacman){
									case 1:
										if(mode==0){ //chase mode
											routine_blinky=1;
										} else { //frightened mode
											routine_blinky=4;
										}
										
										break;
									case 2:
										if(mode==0){ //chase mode
											routine_blinky=2;
										} else { //frightened mode
											routine_blinky=3;
										}
										break;
									case 3:
										if(mode==0){ //chase mode
											routine_blinky=3;
										} else { //frightened mode
											routine_blinky=2;
										}
										break;
									case 4:
										if(mode==0){ //chase mode
											routine_blinky=4;
										} else { //frightened mode
											routine_blinky=1;
										}
										break;
								}
								break;
							case 4: //deve uscire dal recinto
								DeleteBlinky(x_blinky_pos, y_blinky_pos);
								y_blinky_pos-=CELL_SIZE;
								DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
								break;
							default: //percorso
								if(map[((y_blinky_pos)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]==4){ //devo ridisegnare il recinto
									DrawBlinky(x_blinky_pos, y_blinky_pos, Red);
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
								} else{ //non devo ridisegnare nulla
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
								}
								break;
						}
						break;
					case 1: //routine up-left
						switch(total_turns){
							case 1: //right poi stop
								if(map[((y_blinky_pos-3*CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]==1 & map[((y_blinky_pos+CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]==4){
									total_turns=15;
									routine_blinky=0;
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}
							case 2: //up poi right
								if(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos+CELL_SIZE/2)/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}								
							case 3: //right poi up
								if(map[((y_blinky_pos-3*CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}								
							case 4: //down poi right
								if(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos+CELL_SIZE/2)/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}
							case 5: //right poi down
								if(map[((y_blinky_pos+CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}								
							case 6: //down poi right
								if(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos+CELL_SIZE/2)/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}
							case 7: //left poi down
								if(map[((y_blinky_pos+CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}									
							case 8: //down poi left
								if(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos-3*CELL_SIZE/2)/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}									
							case 9: //left poi down
								if(map[((y_blinky_pos+CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}								
							case 10: //up poi left
								if(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos-3*CELL_SIZE/2)/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}								
							case 11: //right poi up
								if(map[((y_blinky_pos-3*CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}								
							case 12: //up poi right
								if(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos+CELL_SIZE/2)/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}									
							case 13: //left poi up
								if(map[((y_blinky_pos-3*CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}																
							case 14: //up poi left
								if(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos-3*CELL_SIZE/2)/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}
									x_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}								
							case 15: //left poi up
								if(map[((y_blinky_pos-3*CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}								
							
						}
 
						break;
					case 2: //routine up-right
						switch(total_turns){
							case 15: //right poi up
								if(map[((y_blinky_pos-3*CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}
										
							case 14: //up poi right
								if(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos+CELL_SIZE/2)/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}
							case 13: //right poi up
								if(map[((y_blinky_pos-3*CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}								
								break;
							case 12: //up poi left
								if(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos-3*CELL_SIZE/2)/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}
									x_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}								
							case 11: //left poi up
								if(map[((y_blinky_pos-3*CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}
							case 10: //up poi right
								if(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos+CELL_SIZE/2)/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}
							case 9:  //right poi down
								if(map[((y_blinky_pos+CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}
							case 8:  //down poi right
								if(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos+CELL_SIZE/2)/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}
							case 7:  //right poi down
								if(map[((y_blinky_pos+CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}
							case 6:  //down poi left
								if(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos-3*CELL_SIZE/2)/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}			
							case 5:  //left poi down
								if(map[((y_blinky_pos+CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}
							case 4:  //down poi left
								if(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos-3*CELL_SIZE/2)/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}
							case 3:  //left poi up
								if(map[((y_blinky_pos-3*CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}
							case 2:  //up poi left
								if(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos-3*CELL_SIZE/2)/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}
									x_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}	
							case 1:  //left poi stop
								if(map[((y_blinky_pos-3*CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]==1 & map[((y_blinky_pos+CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]==4){
									total_turns=15;
									routine_blinky=0;
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}							
							
								
								
						}
						break;
					case 3: //routine down-left
						switch(total_turns){
							case 15:	//left poi down
								if(map[((y_blinky_pos+CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]!=1 & map[((y_blinky_pos+CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]!=4){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}
							case 14:	//down poi left
								if(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos-3*CELL_SIZE/2)/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}
							case 13:	//left poi down
								if(map[((y_blinky_pos+CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}
							case 12:	//down, dritto poi left
								if(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos-3*CELL_SIZE/2)/CELL_SIZE]!=1){
									if(prosegui==1){
										prosegui=0;
										DeleteBlinky(x_blinky_pos, y_blinky_pos);
										switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
											default:
												break;
											case 2:
												DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
												break;
											case 3:
												DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
												break;
										}									
										y_blinky_pos+=CELL_SIZE;
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										break;										
										
									} else{
										prosegui=1;
										total_turns--;
										DeleteBlinky(x_blinky_pos, y_blinky_pos);
										switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
											default:
												break;
											case 2:
												DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
												break;
											case 3:
												DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
												break;
										}									
										x_blinky_pos-=CELL_SIZE;
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
										break;
									}
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}
							case 11:	//left poi down
								if(map[((y_blinky_pos+CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}
							case 10:	//down poi right
								if(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos+CELL_SIZE/2)/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}
							case 9:		//right poi up
								if(map[((y_blinky_pos-3*CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}								
								break;
							case 8:		//up poi left
								if(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos-3*CELL_SIZE/2)/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}
									x_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}
							case 7:		//left poi up
								if(map[((y_blinky_pos-3*CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}
							case 6:		//up poi right
								if(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos+CELL_SIZE/2)/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}
							case 5:		//right poi up
								if(map[((y_blinky_pos-3*CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}								
								break;
							case 4:		//up poi left
								if(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos-3*CELL_SIZE/2)/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}
									x_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}
							case 3:		//left poi up
								if(map[((y_blinky_pos-3*CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}
							case 2:		//up, dritto poi right
								if(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos+CELL_SIZE/2)/CELL_SIZE]!=1){
									if(prosegui==1){
										prosegui=0;
										DeleteBlinky(x_blinky_pos, y_blinky_pos);
										switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
											default:
												break;
											case 2:
												DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
												break;
											case 3:
												DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
												break;
										}									
										y_blinky_pos-=CELL_SIZE;
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										break;										
									} else{
										prosegui=1;
										total_turns--;
										DeleteBlinky(x_blinky_pos, y_blinky_pos);
										switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
											default:
												break;
											case 2:
												DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
												break;
											case 3:
												DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
												break;
										}									
										x_blinky_pos+=CELL_SIZE;
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
										break;										
									}

								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}
							case 1:		//right poi stop
								if(map[((y_blinky_pos-3*CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]==1 & map[((y_blinky_pos+CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]==4){
									total_turns=15;
									routine_blinky=0;
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}						
						}
						break;
					case 4: //routine down-right
						switch(total_turns){
							case 15:	//right poi down
								if(map[((y_blinky_pos+CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]!=1 & map[((y_blinky_pos+CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]!=4){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}	
							case 14:	//down poi right
								if(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos+CELL_SIZE/2)/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}
							case 13:	//right poi down
								if(map[((y_blinky_pos+CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}	
							case 12: 	//down, dritto poi right
								if(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos+CELL_SIZE/2)/CELL_SIZE]!=1){
									if(prosegui==1){
										prosegui=0;
										DeleteBlinky(x_blinky_pos, y_blinky_pos);
										switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
											default:
												break;
											case 2:
												DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
												break;
											case 3:
												DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
												break;
										}									
										y_blinky_pos+=CELL_SIZE;
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										break;										
										
									} else{
										prosegui=1;
										total_turns--;
										DeleteBlinky(x_blinky_pos, y_blinky_pos);
										switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
											default:
												break;
											case 2:
												DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
												break;
											case 3:
												DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
												break;
										}									
										x_blinky_pos+=CELL_SIZE;
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
										break;
									}
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}
							case 11:	//right poi down
								if(map[((y_blinky_pos+CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}	
							case 10:	//down poi left
								if(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos-3*CELL_SIZE/2)/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}
							case 9:		//left poi up
								if(map[((y_blinky_pos-3*CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}
							case 8:		//up poi right
								if(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos+CELL_SIZE/2)/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}
							case 7:		//right poi up
								if(map[((y_blinky_pos-3*CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}								
								break;
							case 6:		//up poi left
								if(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos-3*CELL_SIZE/2)/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}
									x_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}
							case 5:		//left poi up
								if(map[((y_blinky_pos-3*CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}
							case 4: 	//up poi right
								if(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos+CELL_SIZE/2)/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}
							case 3:		//right poi up
								if(map[((y_blinky_pos-3*CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]!=1){
									total_turns--;
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos+=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}								
								break;
							case 2:		//up, dritto poi left
								if(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos-3*CELL_SIZE/2)/CELL_SIZE]!=1){
									if(prosegui==1){
										prosegui=0;
										DeleteBlinky(x_blinky_pos, y_blinky_pos);
										switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
											default:
												break;
											case 2:
												DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
												break;
											case 3:
												DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
												break;
										}									
										y_blinky_pos-=CELL_SIZE;
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
										break;										
									} else{
										prosegui=1;
										total_turns--;
										DeleteBlinky(x_blinky_pos, y_blinky_pos);
										switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
											default:
												break;
											case 2:
												DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
												break;
											case 3:
												DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
												break;
										}									
										x_blinky_pos-=CELL_SIZE;
										DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);									
										break;										
									}

								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}
							case 1:		//left poi stop
								if(map[((y_blinky_pos-3*CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]==1 & map[((y_blinky_pos+CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]==4){
									total_turns=15;
									routine_blinky=0;
									break;
								} else {
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									switch(map[(y_blinky_pos-MAP_Y_SHIFT)/CELL_SIZE][(x_blinky_pos)/CELL_SIZE]){
										default:
											break;
										case 2:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/4, Magenta);
											break;
										case 3:
											DrawCircle(x_blinky_pos, y_blinky_pos, CELL_SIZE/2, Magenta);
											break;
									}									
									x_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, color_blinky);
									break;
								}								
						}
						break;
				
				}
				break;
			
			case 2: //re-pause
				break;
			
			case 1: //in game ma qui entra solo dalla seconda volta in poi (fatto per non avviare i timer ogni volta)
				switch(routine_blinky){
					case 0: //no-routine
						DrawBlinky(x_blinky_pos, y_blinky_pos, Black);
						switch(map[((y_blinky_pos-3*CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]){ //controllo sulla cella sopra perché allo spawn deve salire fino al muro
							case 1: //muro, ora deve scegliere quale routine eseguire a seconda di dove si trova pacman
								DrawBlinky(x_blinky_pos, y_blinky_pos, Red);
								switch(quadrante_pacman){
									case 1:
										routine_blinky=1;
										break;
									case 2:
										routine_blinky=2;
										break;
									case 3:
										routine_blinky=3;
										break;
									case 4:
										routine_blinky=4;
										break;
								}
								break;
							case 4: //deve uscire dal recintio
								DeleteBlinky(x_blinky_pos, y_blinky_pos);
								y_blinky_pos-=CELL_SIZE;
								DrawBlinky(x_blinky_pos, y_blinky_pos, Red);
								break;
							default: //percorso
								if(map[((y_blinky_pos+CELL_SIZE/2)-MAP_Y_SHIFT)/CELL_SIZE][x_blinky_pos/CELL_SIZE]==4){ //devo ridisegnare il recinto
									DrawBlinky(x_blinky_pos, y_blinky_pos, Red);
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, Red);									
								} else{ //non devo ridisegnare nulla
									DeleteBlinky(x_blinky_pos, y_blinky_pos);
									y_blinky_pos-=CELL_SIZE;
									DrawBlinky(x_blinky_pos, y_blinky_pos, Red);									
								}
								break;
						}
						break;
					case 1: //routine up-left
						break;
					case 2: //routine up-right
						break;
					case 3: //routine down-left
						break;
					case 4: //routine down-right
						break;
				
				}
				break;				
				
		
		}
	

		

	

		LPC_TIM0->IR = 1;			//clear interrupt flag
	}
	else if(LPC_TIM0->IR & 2){ // MR1
		
		LPC_TIM0->IR = 2;			// clear interrupt flag 
	}
	else if(LPC_TIM0->IR & 4){ // MR2
		// your code	
		LPC_TIM0->IR = 4;			// clear interrupt flag 
	}
	else if(LPC_TIM0->IR & 8){ // MR3
		// your code	
		LPC_TIM0->IR = 8;			// clear interrupt flag 
	} 

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


/******************************************************************************
**                            End Of File
******************************************************************************/

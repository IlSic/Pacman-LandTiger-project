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
#include <stdio.h>
#include <CAN/CAN.h>
#include <string.h>


extern volatile int punteggio;
static int countdown = 60;
extern int pausa;
extern int J;
extern volatile int punteggio_old;
extern volatile int vite;
extern uint16_t color_blinky;
extern volatile uint16_t mode;
extern int gameover;

void TIMER1_IRQHandler (void)
{
	if(LPC_TIM1->IR & 1) // MR0
	{ 
		DisplayInteger(224, 0, countdown+1, Black, Black);
		if(countdown==0 | gameover==1) {
			disable_RIT();
			LCD_Clear(Black);
			GUI_Text(112, 144, (uint8_t *) "GAME", Red, Black);
			GUI_Text(112, 160, (uint8_t *) "OVER", Red, Black);
			GUI_Text(0, 304, (uint8_t *) "Press RESET to try again", White, Black);
			pausa = 6;
			J = 0;
		} else 	{
			//DisplayInteger(224, 0, countdown, White, Black);
			countdown--;
		}
	

		LPC_TIM1->IR = 1;			//clear interrupt flag
	}
	else if(LPC_TIM1->IR & 2){ // MR1
		
		LPC_TIM1->IR = 2;			// clear interrupt flag 
	}
	else if(LPC_TIM1->IR & 4){ // MR2
		// your code	
		LPC_TIM1->IR = 4;			// clear interrupt flag 
	}
	else if(LPC_TIM1->IR & 8){ // MR3
		// your code	
		LPC_TIM1->IR = 8;			// clear interrupt flag 
	} 

	return;
}



void TIMER2_IRQHandler (void)
{
	if(LPC_TIM2->IR & 1){ // MR0
				
			
			CAN_TxMsg.data[0] = punteggio >> 24; //(punteggio & 0xFF00) >> 8;
			CAN_TxMsg.data[1] =  punteggio >> 16;
			CAN_TxMsg.data[2] =  punteggio >> 8;
			CAN_TxMsg.data[3] =  punteggio & 0xFF;
			CAN_TxMsg.data[4] = vite;
			CAN_TxMsg.data[5] = countdown;
			CAN_TxMsg.len = 6;
			CAN_TxMsg.id = 2;
			CAN_TxMsg.format = STANDARD_FORMAT;
			CAN_TxMsg.type = DATA_FRAME;
			CAN_wrMsg (1, &CAN_TxMsg);               /* transmit message */		
		
		LPC_TIM2->IR = 1;			//clear interrupt flag
	}
	else if(LPC_TIM2->IR & 2){ // MR1
		
		LPC_TIM2->IR = 2;			// clear interrupt flag 
	}
	else if(LPC_TIM2->IR & 4){ // MR2
		// your code	
		LPC_TIM2->IR = 4;			// clear interrupt flag 
	}
	else if(LPC_TIM2->IR & 8){ // MR3
		// your code	
		LPC_TIM2->IR = 8;			// clear interrupt flag 
	} 

	return;
}

void TIMER3_IRQHandler (void)
{
	if(LPC_TIM3->IR & 1){ // MR0
				
		color_blinky=Red;
		mode=0;
		
		LPC_TIM3->IR = 1;			//clear interrupt flag
	}
	else if(LPC_TIM3->IR & 2){ // MR1
		
		LPC_TIM3->IR = 2;			// clear interrupt flag 
	}
	else if(LPC_TIM3->IR & 4){ // MR2
		// your code	
		LPC_TIM3->IR = 4;			// clear interrupt flag 
	}
	else if(LPC_TIM3->IR & 8){ // MR3
		// your code	
		LPC_TIM3->IR = 8;			// clear interrupt flag 
	} 

	return;
}


/******************************************************************************
**                            End Of File
******************************************************************************/

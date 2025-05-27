/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The GLCD application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-7
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             Paolo Bernardi
** Modified date:           03/01/2020
** Version:                 v2.0
** Descriptions:            basic program for LCD and Touch Panel teaching
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <LPC17xx.h>
#include <GLCD/GLCD.h> 
#include <TouchPanel/TouchPanel.h>
#include <timer/timer.h>
#include <RIT/RIT.h>
#include <Pacman/pacman.h>
#include <adc/adc.h>




#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif



int main(void)
{
  SystemInit();  												/* System Initialization (i.e., PLL)  */
	BUTTON_init();
	joystick_init();
  LCD_Initialization();
	ADC_init();
	ADC_start_conversion();
	LPC_SC->PCONP |= (1 << 22); // Enable Timer 2
  LPC_SC->PCONP |= (1 << 23); // Enable Timer 3
	CAN_Init();

	init_timer(1, 0, 0, 3, 0x17D7840);	//match register countdown
	
	init_timer(0, 0, 0, 3, 0x004C4B40); //timer blinky che funziona come il rit

	init_timer(2, 0, 0, 3, 0x4E2);			//can timer
	
	init_timer(3, 0, 0, 7, 0x0EE6B280);	//timer per far diventare blinky blu per 10 secondi
	

	
	LCD_Clear(Black);
	GUI_Text(0, 0, (uint8_t *) "Score:", White, Black);

	GenerateRandomPills();
	DrawMap();
	//vite
	DrawCircle(3*CELL_SIZE/4, MAP_Y_SHIFT + MAP_ROWS*CELL_SIZE + CELL_SIZE, 3*CELL_SIZE/4, Yellow);
	
	
	init_RIT(0x004C4B40);
	enable_RIT();
	enable_timer(0);
	
	
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);						
	
  while (1)	
  {
		__ASM("wfi");
  }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

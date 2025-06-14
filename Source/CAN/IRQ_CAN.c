/*----------------------------------------------------------------------------
 * Name:    Can.c
 * Purpose: CAN interface for for LPC17xx with MCB1700
 * Note(s): see also http://www.port.de/engl/canprod/sv_req_form.html
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <LPC17xx.h>                  /* LPC17xx definitions */
#include <CAN/CAN.h>                      /* LPC17xx CAN adaption layer */
#include <GLCD/GLCD.h>
#include <Pacman/pacman.h>

extern uint8_t icr ; 										//icr and result must be global in order to work with both real and simulated landtiger.
extern uint32_t result;
extern CAN_msg       CAN_TxMsg;    /* CAN message for sending */
extern CAN_msg       CAN_RxMsg;    /* CAN message for receiving */  

static int vite_check = 1;
static int punteggio_check = 0;
static int countdown_check = 0;


uint16_t val_RxCoordX = 0;            /* Locals used for display */
uint16_t val_RxCoordY = 0;

/*----------------------------------------------------------------------------
  CAN interrupt handler
 *----------------------------------------------------------------------------*/
void CAN_IRQHandler (void)  {

  /* check CAN controller 1 */
	icr = 0;
  icr = (LPC_CAN1->ICR | icr) & 0xFF;               /* clear interrupts */
	
  if (icr & (1 << 0)) {                          		/* CAN Controller #1 meassage is received */
		CAN_rdMsg (1, &CAN_RxMsg);	                		/* Read the message */
    LPC_CAN1->CMR = (1 << 2);                    		/* Release receive buffer */
		//srivere qua lo score da disegnare
/*		val_RxCoordX = (CAN_RxMsg.data[0] << 8)  ;
		val_RxCoordX = val_RxCoordX | CAN_RxMsg.data[1];
		
		val_RxCoordY = (CAN_RxMsg.data[2] << 8);
		val_RxCoordY = val_RxCoordY | CAN_RxMsg.data[3];
		
		display.x = val_RxCoordX;
		display.y = val_RxCoordY-140;
		TP_DrawPoint_Magnifier(&display);*/
		
		
		
  }
	if (icr & (1 << 1)) {                         /* CAN Controller #1 meassage is transmitted */
		// do nothing in this example
		//puntiInviati1++;
	}
		
	/* check CAN controller 2 */
	icr = 0;
	icr = (LPC_CAN2->ICR | icr) & 0xFF;             /* clear interrupts */

	if (icr & (1 << 0)) {                          	/* CAN Controller #2 meassage is received */
		CAN_rdMsg (2, &CAN_RxMsg);	                		/* Read the message */
    LPC_CAN2->CMR = (1 << 2);                    		/* Release receive buffer */

		
		if(vite_check!=CAN_RxMsg.data[4]){
			DeleteLives(vite_check);
			vite_check=CAN_RxMsg.data[4];
			DrawLives(vite_check);
		}

		punteggio_check=(CAN_RxMsg.data[0]<<24) | (CAN_RxMsg.data[1]<<16) | (CAN_RxMsg.data[2]<<8) | CAN_RxMsg.data[3];
		DisplayInteger(56, 0, punteggio_check, White, Black);
		if(countdown_check!=CAN_RxMsg.data[5]){
			countdown_check=CAN_RxMsg.data[5];
			DisplayInteger(224, 0, countdown_check, White, Black);
		}
		
	}
	if (icr & (1 << 1)) {                         /* CAN Controller #2 meassage is transmitted */
		// do nothing in this example
		//puntiInviati2++;
	}
}

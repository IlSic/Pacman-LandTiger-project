#include <Pacman/pacman.h>
#include <GLCD.h>
#include <adc/adc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

uint16_t provax;
uint16_t provay;
int countpills = 0;
extern unsigned int AD_current;
extern int J;


void DrawPacman(uint16_t x, uint16_t y) {

	
		/*uint16_t i;
		uint16_t j;
		uint16_t radius = CELL_SIZE/2;
    for (i = -radius; i < radius; i++) {
        for (j = -radius; j < radius; j++) {
            // Condizione per disegnare un cerchio (equazione x^2 + y^2 <= r^2)
            if (i * i + j * j <= radius * radius) {
                LCD_SetPoint(x + j, y + i, Yellow);
            }
        }
    }*/
	DrawCircle(x,y,CELL_SIZE/2,Yellow);
	//DrawCircle(x,y,CELL_SIZE,Yellow);
	}


void DeletePacman(uint16_t x, uint16_t y) {

		/*uint16_t i;
		uint16_t j;
		uint16_t radius = CELL_SIZE/2;
    for (i = -radius; i < radius; i++) {
        for (j = -radius; j < radius; j++) {
            // Condizione per disegnare un cerchio (equazione x^2 + y^2 <= r^2)
            if (i * i + j * j <= radius * radius) {
                LCD_SetPoint(x + j, y + i, Black);
            }
        }
    }*/
	DrawCircle(x,y,CELL_SIZE/2,Black);
	//DrawCircle(x,y,CELL_SIZE,Black);
		
}



/*void DrawCircle(uint16_t x, uint16_t y, uint16_t radius, uint16_t color) {

		int16_t i;
		int16_t j;

    for (i = -radius; i <= radius; i++) {
        for (j = -radius; j <= radius; j++) {
            // Condizione per disegnare un cerchio (equazione x^2 + y^2 <= r^2)
            if (i * i + j * j <= radius * radius) {
                LCD_SetPoint(x + j, y + i, color);
            }
        }
    }
		
}*/

void DrawCircle(uint16_t x, uint16_t y, uint16_t radius, uint16_t color) {

		int16_t i;
		int16_t j;

    for (i = -radius; i <= radius; i++) {
        for (j = -radius; j <= radius; j++) {
            // Condizione per disegnare un cerchio (equazione x^2 + y^2 <= r^2)
            if (i * i + j * j < radius * radius) {
                LCD_SetPoint(x + j, y + i, color);
            }
        }
    }
		
}

void DrawLives(int vite){
	if(vite>0){
		uint16_t i;
		for(i=0; i<vite; i++){
			DrawCircle(3*CELL_SIZE/4 +2*i*CELL_SIZE, MAP_Y_SHIFT + MAP_ROWS*CELL_SIZE + CELL_SIZE, 3*CELL_SIZE/4, Yellow);
		}		
	}

}

void DeleteLives(int vite){
	if(vite>0){
		uint16_t i;
		for(i=0; i<vite; i++){
			DrawCircle(3*CELL_SIZE/4 +2*i*CELL_SIZE, MAP_Y_SHIFT + MAP_ROWS*CELL_SIZE + CELL_SIZE, 3*CELL_SIZE/4, Black);
		}		
	}

}

void DrawBlinky(uint16_t x, uint16_t y, uint16_t color){

	DrawCircle(x, y, CELL_SIZE/2, color);
	
}

void DeleteBlinky(uint16_t x, uint16_t y){

	DrawCircle(x, y, CELL_SIZE/2, Black);	
	
}

/*void MoveBlinky(uint16_t x_blinky_position, uint16_t y_blinky_position, uint16_t direction){


	switch(direction){
		
		case 1: //up
			
			DeleteBlinky(x_blinky_position, y_blinky_position);
			y_blinky_position -= CELL_SIZE;
			DrawBlinky(x_blinky_position, y_blinky_position);
			break;
		
		case 2: //down
			
			DeleteBlinky(x_blinky_position, y_blinky_position);
			y_blinky_position += CELL_SIZE;
			DrawBlinky(x_blinky_position, y_blinky_position);
			break;
		
		case 3: //left
			
			DeleteBlinky(x_blinky_position, y_blinky_position);
			x_blinky_position -= CELL_SIZE;
			DrawBlinky(x_blinky_position, y_blinky_position);			
			break;
		
		case 4: //right
			
			DeleteBlinky(x_blinky_position, y_blinky_position);
			x_blinky_position += CELL_SIZE;
			DrawBlinky(x_blinky_position, y_blinky_position);			
			break;
	}
	
}*/

/*
uint16_t map[MAP_ROWS][MAP_COLS] = {			   //m
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 1},
	{1, 0, 2, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 1, 0, 2, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 1},
	{1, 0, 2, 0, 1, 1, 1, 0, 2, 0, 1, 1, 0, 2, 0, 0, 0, 2, 0, 1, 1, 1, 0, 2, 0, 1, 0, 2, 0, 1},
	{1, 0, 2, 0, 1, 1, 1, 0, 2, 0, 1, 1, 0, 2, 2, 2, 2, 2, 0, 1, 1, 1, 0, 2, 0, 1, 0, 2, 0, 1},
	{1, 0, 2, 0, 1, 1, 1, 0, 2, 0, 1, 1, 0, 2, 0, 0, 0, 2, 0, 1, 1, 1, 0, 2, 0, 1, 0, 2, 0, 1},
	{1, 0, 2, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 1, 0, 2, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 1},
	{1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 1},
	{1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 1},
	{1, 0, 2, 0, 1, 1, 1, 1, 1, 1, 1, 0, 2, 0, 1, 1, 0, 2, 0, 1, 1, 1, 1, 1, 1, 1, 0, 2, 0, 1},
	{1, 0, 2, 0, 1, 1, 1, 1, 1, 1, 1, 0, 2, 0, 1, 1, 0, 2, 0, 1, 1, 1, 1, 1, 1, 1, 0, 2, 0, 1},
	{1, 0, 2, 2, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 1, 1, 0, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 1},
	{1, 0, 2, 2, 2, 2, 0, 1, 0, 2, 2, 2, 2, 0, 0, 0, 0, 2, 2, 2, 2, 0, 1, 0, 2, 2, 2, 2, 0, 1},
	{1, 0, 0, 0, 0, 2, 0, 1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 1, 0, 2, 0, 0, 0, 0, 1},
	{1, 1, 1, 1, 0, 2, 0, 1, 0, 2, 0, 1, 1, 4, 4, 4, 4, 1, 1, 0, 2, 0, 1, 0, 2, 0, 1, 1, 1, 1},
	{5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5},
	{5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}, //m
	{5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5},	
	{1, 1, 1, 1, 0, 2, 0, 1, 0, 2, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 2, 0, 1, 1, 1, 1},
	{1, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 1},
	{1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 1},
	{1, 0, 2, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 2, 0, 1},
	{1, 0, 2, 0, 1, 1, 1, 1, 0, 2, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 2, 0, 1, 1, 1, 1, 0, 2, 0, 1},
	{1, 0, 2, 0, 1, 1, 1, 0, 0, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 2, 0, 1, 1, 1, 1, 0, 2, 0, 1},
	{1, 0, 2, 0, 1, 1, 1, 0, 2, 2, 2, 2, 2, 0, 1, 0, 2, 2, 2, 2, 2, 0, 1, 1, 1, 1, 0, 2, 0, 1},
	{1, 0, 2, 0, 1, 1, 1, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 1, 1, 1, 1, 0, 2, 0, 1},
	{1, 0, 2, 0, 0, 0, 1, 0, 2, 0, 1, 0, 2, 2, 2, 2, 2, 0, 1, 0, 2, 0, 0, 0, 0, 0, 0, 2, 0, 1},
	{1, 0, 2, 2, 2, 0, 1, 0, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 0, 1},
	{1, 0, 0, 0, 2, 0, 1, 0, 2, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 1},
	{1, 1, 1, 0, 2, 0, 1, 0, 2, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 2, 0, 1, 0, 2, 0, 1, 1, 1, 1},
	{1, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 1},
	{1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
	
};*/
uint16_t map[MAP_ROWS][MAP_COLS] = {			   //m
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
	{1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1},
	{1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1},
	{1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1},
	{1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1},
	{1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1},
	{1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
	{1, 2, 1, 1, 1, 1, 0, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 0, 1, 1, 1, 1, 2, 1},
	{1, 2, 1, 1, 1, 1, 0, 1, 1, 1, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 1, 1, 1, 0, 1, 1, 1, 1, 2, 1},
	{1, 2, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 2, 1},
	{1, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 1},
	{1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 4, 4, 4, 4, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1},
	{5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}, //18
	{1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1},	//19
	{1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1},
	{1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 1, 1, 1, 1, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
	{1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1},
	{1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1},
	{1, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 1},
	{1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1},
	{1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1},
	{1, 2, 2, 2, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 2, 2, 2, 2, 2, 2, 1},
	{1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1},
	{1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1},
	{1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
	
};


int count_2 = 0;
int pow_pills[6];

void GenerateRandomPills(){
	
	
	uint16_t i;
	uint16_t row;
	uint16_t col;
	
	//GENERARE UN SOLO NUMERO RANDOM E POI MOLTIPLICARE PER METTERLI SPAZIATI DI 40 (240/6)
	AD_current = AD_current%240+1;

	for(i=0; i<6; i++){
		if(AD_current+40*i>240){
			AD_current = AD_current - 240;			
		} 
		pow_pills[i] = AD_current + 40*i;
		
	}
	for(row = 0; row < MAP_ROWS; row++){
		for (col = 0; col < MAP_COLS; col++){
			//finire la funzione
			//scorrere la matrice, ad ogni punto che equivale ad un punto nel vettore cambiare il valore da 2 a 3
			if(map[row][col]==2){
				count_2++;
				for(i=0; i<6; i++){
					if(count_2==pow_pills[i]){
						map[row][col]=3;
						break;
					}
				}
			}
		}
	}		
	

	

}

// Funzione per disegnare una cella
void LCD_DrawCell(uint16_t x, uint16_t y, uint16_t size, uint16_t color) {
		uint16_t i;
		uint16_t j;
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            LCD_SetPoint(x + j, y + i, color);
        }
    }
}

void DrawMap(void) {

	uint16_t row;
	uint16_t col;
	
    for (row = 0; row < MAP_ROWS; row++) {
        for (col = 0; col < MAP_COLS; col++) {
            uint16_t x = (col * CELL_SIZE);
            uint16_t y = (row * CELL_SIZE) + MAP_Y_SHIFT; 
						provax = x;
						provay = y;
            switch (map[row][col]) {
                case 0: // Percorso
                    LCD_DrawCell(x, y, CELL_SIZE, Black);
                    break;
                case 1: // Muro
                    LCD_DrawCell(x, y, CELL_SIZE, Blue);
                    break;
                case 2: // Punto (Pellet)
                    DrawCircle(x+CELL_SIZE/2, y+CELL_SIZE/2, CELL_SIZE/4, Magenta);
										countpills++;
									  break;
								case 3:	//Power Pill
										DrawCircle(x+CELL_SIZE/2, y+CELL_SIZE/2, CELL_SIZE/2, Magenta);
										break;
								case 4: //Recinto Blinky
										LCD_DrawCell(x, y, CELL_SIZE, Red);
								    break;
                default:
                    break;
            }
        }
    }
}

int check_positions(uint16_t x_pacman_pos, uint16_t y_pacman_pos, uint16_t x_blinky_pos, uint16_t y_blinky_pos){
	int res = 0;
	if(x_pacman_pos==x_blinky_pos){
		if(y_pacman_pos==y_blinky_pos){
			res=1;
		} 
	}
	return res;
}







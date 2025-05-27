#ifndef __pacman_h
#define __pacman_h

#include <stdint.h>

#define CELL_SIZE 8  // Dimensione di una cella (in pixel)
#define MAP_ROWS 36   // Numero di righe della mappa
#define MAP_COLS 30  // Numero di colonne della mappa
#define MAP_Y_SHIFT 16

#define x_pacman_spawn 108
#define y_pacman_spawn 124

#define x_blinky_spawn 124
#define y_blinky_spawn 148

extern uint16_t map[MAP_ROWS][MAP_COLS];
extern uint16_t ppill[6];


extern void DrawPacman(uint16_t x, uint16_t y);
extern void DeletePacman(uint16_t x, uint16_t y);
extern void DrawCircle(uint16_t x, uint16_t y, uint16_t radius, uint16_t color);
extern void DrawLives(int vite);
extern void DeleteLives(int vite);
extern void DrawBlinky(uint16_t x, uint16_t y, uint16_t color);
extern void DeleteBlinky(uint16_t x, uint16_t y);
extern void MoveBlinky(uint16_t x_blinky_position, uint16_t y_blinky_position, uint16_t direction);
extern void GenerateRandomPills();
extern void LCD_DrawCell(uint16_t x, uint16_t y, uint16_t size, uint16_t color);
extern void DrawMap(void);
extern int check_positions(uint16_t x_pacman_pos, uint16_t y_pacman_pos, uint16_t x_blinky_pos, uint16_t y_blinky_pos);
#endif


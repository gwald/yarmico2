/*
 *
 PUBLIC DOMAIN 2022
 THIS CODE IS A HACKY MESS - READ IT AT YOUR OWN RISK!
 https://github.com/gwald/yarmico
 *
 */


#ifndef GAME_HEADER
#define GAME_HEADER


#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>


#define MAX_MAP_ROW 100
#define MAX_MAP_COL 200
#include "yar_API.h"


#ifdef YARMICO_INSTANCE_GAME
#define INSTANCE
#else
#define INSTANCE extern
#endif


INSTANCE u32 g_game_controller_timer;
INSTANCE u32 g_game_frame_counter;
INSTANCE u32 g_current_level;

typedef struct {
	yarmico_item_t item;
	s8 alive;
	u8 hit_cnt;
	u8 speed;
	u8 shot_tick;
	u8 on_ground;

	s32 velX, velY;
	s32 posX_fixed;
	s32 posY_fixed;
	u8 direction;
	u8 hanging_upsidedown;
	u32 gems,bots;
	u8 UI,hitcnt;

}player_t;

	// tick if( (g_game_counter & 127 ) == (127- (g_gamestate.wave) ) )//% 250 ==0)
typedef struct {
	yarmico_item_t item;
	s8 alive;
	u8 shot_tick;
	u8 displayed;
}bot_t;


typedef struct {
	yarmico_rect_t pos_rect;
	u8 alive;
	s32 dx;
	s32 dy;
	u8 r,g,b;
	u8 owner;
}bullet_t;


typedef struct {
	yarmico_rect_t pos_rect;
	u8 alive;
	u8 speed;
}explosion_t;


typedef struct {
	yarmico_item_t item;
	s8 alive;
	u8 sprite;
}gem_t;


#define CAM_MOVE_AMOUNT 01


#define BOT_SPEED 10
#define BULLET_INIT_SPEED 4000
#define FIXED_MOVE_AMOUNT 4608
#define MAX_BULLETS 64



#define MAX_GEMS 1000
#define MAX_BOTS 700

#define MAX_EXPLOSIONS 16

#define DATA_START_CHECK "#sTaRtIng BIT 2001#"
#define DATA_END_CHECK   "#EnDiNg bIt 1999#"


#define GAME_MAX_LEVEL 10


typedef struct {
	char start[32];
	player_t player;
	u8 level;
	u8 level_disp;
	 u8 bot_width;
	 u8 bot_rows;
	 u32 score;
	 char end[32];
}save_data_t;



typedef union {
	save_data_t data;
	char block[128]; // playstation requires save to be 128bytes
}save_t;


INSTANCE player_t g_player;

INSTANCE gem_t  g_gems[MAX_GEMS];
INSTANCE bot_t  g_bots[MAX_BOTS];
INSTANCE bullet_t  g_bots_bullets[MAX_BULLETS];
INSTANCE bullet_t  g_player_bullets[MAX_BULLETS];
INSTANCE explosion_t  g_explosions[MAX_EXPLOSIONS];
INSTANCE u32 g_button_press_time;


//INSTANCE u8 g_wave;
// INSTANCE u32 g_move_down_fixed;

#define LOG_EXPL // printf
#define LOG_BULLET  //  printf
#define LOG_BOT2 // printf
#define LOG_BOTS  printf
#define LOG_LEVEL // printf




#endif // #ifndef GAME_HEADER

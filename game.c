/*
 *
 PUBLIC DOMAIN 2022
 THIS CODE IS A HACKY MESS - READ IT AT YOUR OWN RISK!
 https://github.com/gwald/yarmico
 *
 */

#define YARMICO_INSTANCE_GAME


#include "game.h"

// #define printf //printf


#include "ts_maps.inc"




#ifdef _RELEASE_
#define printf //printf
#endif

#define LOG_MAIN   //yarmico_debugf

#define START_BOT_Y 32
#define BOT_TEXTURES_PER_ROWS 16 // * 8pixels
#define BOT_TEXTURE_ROWS 16

// sin/cos from https://github.com/sehugg/8bitworkshop/blob/master/presets/vector-z80color/game.c#L286
void explosion_add(u32 x, u32 y , u32 side);

void gameloop_start_screen(void);
void gameloop_dead_screen(void);
void gameloop_won_screen(void);
void yarmico2_init(void);

const s8 sintbl[64] = {
		0, 3, 6, 9, 12, 16, 19, 22, 25, 28, 31, 34, 37, 40, 43, 46,
		49, 51, 54, 57, 60, 63, 65, 68, 71, 73, 76, 78, 81, 83, 85, 88,
		90, 92, 94, 96, 98, 100, 102, 104, 106, 107, 109, 111, 112, 113, 115, 116,
		117, 118, 120, 121, 122, 122, 123, 124, 125, 125, 126, 126, 126, 127, 127, 127,
};

s8 isin(u8 x0) {
	u8 x = x0;
	if (x0 & 0x40) x = 127-x;
	if (x0 & 0x80) {
		return -sintbl[x-128];
	} else {
		return sintbl[x];
	}
}

s8 icos(u8 x) {
	return isin(x+64);
}



void display_UI(void)
{
	// printf("g_player.UI %d\n", g_player.UI);

	if(g_player.UI)
	{
		u32 coordY;
		char line[25]; //14 chars + NUL

		coordY = g_player.UI>>6;

		g_player.UI--;


		sprintf(line, "%1d %03d %03d %05d",g_player.alive, g_player.gems,  g_player.bots,  g_yarmico_game_time );


#if HARDWARE_TARGET == HARDWARE_TARGET_NET_YAROZE
		// PSX hack, coloured rects are order behind textures, so need to use dark texture as background
		{
			u32 i;
			for(i=0;i<16;i++)
				yarmico_sprite(1,  8, 32,  8, 8,  i*8,  coordY, ONE , 0,0);

		}
#else
		yarmico_rectangle(0,0,0, 0, coordY, YARMICO_SCREEN_WIDTH, 8);
#endif




		yarmico_render_text(0,coordY, ONE, line);
		//printf("coordY %d\n", coordY);

	}
}




u32 bullet_add_player(s32 x, s32 y, s32 velX, s32 velY, u8 owner  )
{
	u32 i;
	u32 test;


	for(i=0; i<MAX_BULLETS; i++)
	{
		test = g_player_bullets[i].alive;
		if(test == 0)
		{


			LOG_BULLET("bullet_add i %d X %d Y %d \n",i , x, y);

			// bzero(g_bullets+i, sizeof(bullet_t) );
			// g_bullets[b].speed =  (BULLET_SPEED>>4) + g_level + g_wave;
			g_player_bullets[i].alive = 20; // Visible on screen then disappears
			g_player_bullets[i].owner = owner;


			g_player_bullets[i].pos_rect.x = x;
			g_player_bullets[i].pos_rect.y = y;
			g_player_bullets[i].pos_rect.w = 2;
			g_player_bullets[i].pos_rect.h = 2;

			g_player_bullets[i].dx = velX;
			g_player_bullets[i].dy = velY;


			// audio

			if(1) //owner)
			{
				// audio
				g_yarmico_pico_play_sound.SFX =  YARMICO_PICO_SOUND_SFX2_UP_DOWN;// | YARMICO_PICO_SOUND_SFX1_ON_OFF;
				g_yarmico_pico_play_sound.play_back = YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_ATTACK;
				g_yarmico_pico_play_sound.SFX_trigger_ON_OFF = 0;
				g_yarmico_pico_play_sound.SFX_hold_ON_OFF = 0; // 5 frame skips

				g_yarmico_pico_play_sound.SFX_trigger2 = 1;
				g_yarmico_pico_play_sound.SFX_hold2 = 5; // 5 frame skips
				g_yarmico_pico_play_sound.dir_speed = 80;


				g_yarmico_pico_play_sound.loop = YARMICO_PICO_SOUND_LOOP_NONE;
				g_yarmico_pico_play_sound.loop_times = 0;
				YARMICO_MACRO_SET_PICO_AUDIO(444, 100, 100, 0, 0 , 5);
			}
			else
			{
				g_yarmico_pico_play_sound.SFX =  YARMICO_PICO_SOUND_SFX2_UP_DOWN;// | YARMICO_PICO_SOUND_SFX1_ON_OFF;
				g_yarmico_pico_play_sound.play_back = YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_ATTACK;
				g_yarmico_pico_play_sound.SFX_trigger_ON_OFF = 0;
				g_yarmico_pico_play_sound.SFX_hold_ON_OFF = 0; // 5 frame skips

				g_yarmico_pico_play_sound.SFX_trigger2 = 1;
				g_yarmico_pico_play_sound.SFX_hold2 = 2; // 5 frame skips
				g_yarmico_pico_play_sound.dir_speed = 50;


				g_yarmico_pico_play_sound.loop = YARMICO_PICO_SOUND_LOOP_NONE;
				g_yarmico_pico_play_sound.loop_times = 0;
				YARMICO_MACRO_SET_PICO_AUDIO(666, 100, 100, 0, 0 , 5);
			}



			return 1;
		}
	}
	return 0;
}



void explosion_add(u32 x, u32 y, u32 good_bad_audio ) // side 0= player
{
	u32 i;

	LOG_MAIN("explosion_add   X:%d Y:%d \n", x, y );

	for(i=0; i<MAX_EXPLOSIONS; i++)
	{

		if(g_explosions[i].alive == 0)
		{

			g_explosions[i].alive = 222;
			g_explosions[i].pos_rect.x = x;
			g_explosions[i].pos_rect.y = y;
			g_explosions[i].pos_rect.w = g_explosions[i].pos_rect.h = 4;


			if(good_bad_audio)
			{
				// bad audio
				g_yarmico_pico_play_sound.SFX = YARMICO_PICO_SOUND_SFX1_ON_OFF | YARMICO_PICO_SOUND_SFX2_UP_DOWN;
				g_yarmico_pico_play_sound.play_back = YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_ATTACK;
				g_yarmico_pico_play_sound.SFX_trigger_ON_OFF = 5;
				g_yarmico_pico_play_sound.SFX_hold_ON_OFF = 5; // 5 frame skips

				g_yarmico_pico_play_sound.SFX_trigger2 = 10;
				g_yarmico_pico_play_sound.SFX_hold2 = 20; // 5 frame skips
				g_yarmico_pico_play_sound.dir_speed = -100;


				g_yarmico_pico_play_sound.loop = YARMICO_PICO_SOUND_LOOP_TIMES;
				g_yarmico_pico_play_sound.loop_times = 2;

				YARMICO_MACRO_SET_PICO_AUDIO(1665, 100, 100, 0, 0 , 15);
			}
			else
			{
				// good audio
				g_yarmico_pico_play_sound.SFX = YARMICO_PICO_SOUND_SFX1_ON_OFF | YARMICO_PICO_SOUND_SFX2_UP_DOWN;
				g_yarmico_pico_play_sound.play_back = YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_ATTACK;
				g_yarmico_pico_play_sound.SFX_trigger_ON_OFF = 5;
				g_yarmico_pico_play_sound.SFX_hold_ON_OFF = 5; // 5 frame skips

				g_yarmico_pico_play_sound.SFX_trigger2 = 10;
				g_yarmico_pico_play_sound.SFX_hold2 = 20; // 5 frame skips
				g_yarmico_pico_play_sound.dir_speed = 100;


				g_yarmico_pico_play_sound.loop = YARMICO_PICO_SOUND_LOOP_TIMES;
				g_yarmico_pico_play_sound.loop_times = 2;

				YARMICO_MACRO_SET_PICO_AUDIO(1665, 100, 100, 0, 0 , 15);
			}
			return;
		}
	}


}

void display_gems(void)
{


	u32 i, x, y, test;




	//setup player for collision check
	yarmico_setup_collision( (g_player.posX_fixed>>ONE_SHIFT)+1,  (g_player.posY_fixed>>ONE_SHIFT)+1, (6) , (6) );


	for(i=0; i<MAX_GEMS; i++)
	{


		if(g_gems[i].alive)
		{
			// x =   g_bots[i].item.pos_rect.x;
			x =  g_gems[i].item.pos_rect.x + (g_gems[i].item.screen_x_fixed >> ONE_SHIFT);
			y =   g_gems[i].item.pos_rect.y;

			//test = yarmico_tilemap_in_screen(x,y);
			test =  yarmico_tilemap_sprite(g_gems[i].item.tpage,  g_gems[i].item.u, g_gems[i].item.v, 8, 8, x,  y, ONE, g_bots[i].item.dpad, 0);

			// if it's on the screen, do it's AI
			if(test)
			{
				test = g_gems[i].item.u;
				g_gems[i].sprite+=3;
				test = (test + g_gems[i].sprite)>>5;
				g_gems[i].item.u = test*8;


#if HARDWARE_TARGET == HARDWARE_TARGET_NET_YAROZE
				// stupid libGS sprite bug!
				if(g_gems[i].item.u == 0)
					g_gems[i].item.u++;
#else

#endif
				// printf("%d u %d, v %d s %d \n",i, g_gems[i].item.u, g_gems[i].item.v ,g_gems[i].sprite);

				{//test if player collision

					yarmico_rect_t gem_rect;

					u32 x, y;



					x =  g_gems[i].item.pos_rect.x;
					y =   g_gems[i].item.pos_rect.y;


					gem_rect.x = x;
					gem_rect.y = y;
					gem_rect.w =  gem_rect.h= 8;  // W H doesn't change



					if(yarmico_rect_collision_test(&gem_rect) )
					{
						//collected

						explosion_add(x, y, 0 );

						g_gems[i].alive = 0;

						g_player.gems--;
						g_player.UI = 254;


						// audio
#if 1
						g_yarmico_pico_play_sound.SFX = YARMICO_PICO_SOUND_SFX1_ON_OFF | YARMICO_PICO_SOUND_SFX2_UP_DOWN;
						g_yarmico_pico_play_sound.play_back = YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_ATTACK;
						g_yarmico_pico_play_sound.SFX_trigger_ON_OFF = 10;
						g_yarmico_pico_play_sound.SFX_hold_ON_OFF = 0; // 5 frame skips

						g_yarmico_pico_play_sound.SFX_trigger2 = 10;
						g_yarmico_pico_play_sound.SFX_hold2 = 0; // 5 frame skips
						g_yarmico_pico_play_sound.dir_speed = 55;


						g_yarmico_pico_play_sound.loop = YARMICO_PICO_SOUND_LOOP_TIMES;
						g_yarmico_pico_play_sound.loop_times = 2;

						YARMICO_MACRO_SET_PICO_AUDIO(1665, 100, 100, 0, 0 , 15);
#endif


					} //if(yarmico_rect_collision_test(&bot_rect) )

				}//test if player collision


			} // displayed


		}	//if(g_gems[i].alive)


	}

}
void display_explosion(void)
{

	u32 i;
	u8 r, g, b;



	for(i=0; i<MAX_EXPLOSIONS; i++)
	{

		if(g_explosions[i].alive)
		{
			LOG_EXPL("g_explosions[i].alive %d \n ", g_explosions[i].alive );
			g_explosions[i].alive-=3;

			r= g_explosions[i].alive;
			g= g_explosions[i].alive;
			b= g_explosions[i].alive;

			if(  (g_explosions[i].alive   & 1) == 0)
			{
				g_explosions[i].pos_rect.w = g_explosions[i].pos_rect.h =  g_explosions[i].alive >> 4;
				r=255;
				g+=50;
				b-=50;

			}
			else
			{
				g_explosions[i].pos_rect.w = g_explosions[i].pos_rect.h =  g_explosions[i].alive >> 3;
				r=255;
				g-=50;
				b+=50;
				// g_explosions[i].pos_rect.x+=1;
				// g_explosions[i].pos_rect.y+=1;
			}


			yarmico_tilemap_rectangle(r,g,b, g_explosions[i].pos_rect.x-8, g_explosions[i].pos_rect.y-8, g_explosions[i].pos_rect.w, g_explosions[i].pos_rect.h );

		}


	}
}


void display_bot(void)
{
	u32 test, i;

	u8 wave, r, g, b;
	yarmico_rect_t bot_rect;


	// LOG_BOT("bot_display\n");

	r = 255;
	g = 126;
	b = 0;

#if HARDWARE_TARGET == HARDWARE_TARGET_PICOSYSTEM
	if( (g_game_frame_counter & 15 ) ==  0 ) //(31- (g_head_bot.shoot_cnt) ) )
#else
		if( (g_game_frame_counter & 127 ) == (0) )
#endif
		{


		}


	//setup player for collision check
	yarmico_setup_collision( (g_player.posX_fixed>>ONE_SHIFT)+1,  (g_player.posY_fixed>>ONE_SHIFT)+1, (6) , (6) );




	for(i=0; i<MAX_BOTS; i++)
	{
		test = g_bots[i].alive;
		if(test)
		{
			u32 x, y;


			// x =   g_bots[i].item.pos_rect.x;
			x =  g_bots[i].item.pos_rect.x + (g_bots[i].item.screen_x_fixed >> ONE_SHIFT);
			y =   g_bots[i].item.pos_rect.y;

			//test = yarmico_tilemap_in_screen(x,y);
			test =  yarmico_tilemap_sprite(0,  g_bots[i].item.u, g_bots[i].item.v, 8, 8, x,  y, ONE, g_bots[i].item.dpad, 0);

			// if it's on the screen, do it's AI
			if(test)
			{
				g_bots[i].displayed =1;

				//update new screen offset
				g_bots[i].item.screen_x_fixed += g_bots[i].item.speed;

				if(g_bots[i].item.screen_x_fixed > 10*ONE) // limit speed per level
					g_bots[i].item.screen_x_fixed = 10*ONE;

				// update new x pos
				if(	g_bots[i].item.dpad)
					x =  g_bots[i].item.pos_rect.x +  (g_bots[i].item.screen_x_fixed >> ONE_SHIFT);
				else
					x =  g_bots[i].item.pos_rect.x -  (g_bots[i].item.screen_x_fixed >> ONE_SHIFT);

				//				g_bots[i].item.screen_x_fixed -= g_bots[i].item.speed;
				//						else
				//							g_bots[i].item.screen_x_fixed += g_bots[i].item.speed;


				// check there's floor and no blocks in new pos
				{

					yarmico_tile_data_t *c1, *c2; // check 1 = block, 2 = floor
					u32 cellx, celly;

					// check center to avoid getting stuck

					cellx = (YARMICO_SCREEN_WIDTH_MID+x+4+ (g_bots[i].item.dpad*8)) >> 3; //move to center + pos + 4 mid cell, (g_bots[i].item.dpad*8) is to check right block if going right and div by 8(pixels per cell)
					celly = (YARMICO_SCREEN_HEIGHT_MID+y+4) >> 3;


					// printf("xcell %d ycell %d \n",xcell, ycell );
					c1 = yarmico_tilemap_get_cell(cellx ,  celly );



					celly = (YARMICO_SCREEN_HEIGHT_MID+y+12) >> 3; //check for floor 8 + 4 for middle
					c2 = yarmico_tilemap_get_cell(cellx ,  celly );

					if( (c1 && c1->tile_cell_u+c1->tile_cell_v == 0 )
							&&	(c2 && 	c2->tile_cell_u+c2->tile_cell_v > 0)
					)
					{
						// move correct
						g_bots[i].item.pos_rect.x =x;


					}
					else
					{
						g_bots[i].item.dpad = !g_bots[i].item.dpad;
						g_bots[i].item.screen_x_fixed  = 0; // reset translation
					}

					//	yarmico_tilemap_sprite(0,  g_bots[i].item.u, g_bots[i].item.v, 8, 8, g_bots[i].item.pos_rect.x,  y, ONE, g_bots[i].item.dpad, 0);
				}
				// check there's floor and no blocks in new pos


				if(g_player.hitcnt == 0)
				{//test if player collision


					bot_rect.x = x;
					bot_rect.y = y;
					bot_rect.w =  bot_rect.h= 8;  // W H doesn't change

					// test
					// yarmico_tilemap_rectangle( 55,122,55, bot_rect.x, bot_rect.y,bot_rect.w, bot_rect.h );
					// yarmico_tilemap_rectangle( 5,55,55, (g_player.posX_fixed>>ONE_SHIFT),  (g_player.posY_fixed>>ONE_SHIFT), 8 ,8 );


					//set bot rect and check collision
					if(yarmico_rect_collision_test(&bot_rect) )
					{
						// both bullet and bot are dead!


						explosion_add(x, y, 1 ); // bad explosion

						g_bots[i].alive = 0;
						g_bots[i].displayed =0;
						g_player.alive--;
						g_player.bots--;
						g_player.hitcnt = g_player.UI = 254;
						break; //hit once



					} //if(yarmico_rect_collision_test(&bot_rect) )

				}//test if player collision

			}
			else
				g_bots[i].displayed = 0;// not displayed


		}// bot alive


	}// for each bot




}


void display_bullet(void)
{
	u32 test, i;
	u8 r, g, b;
	yarmico_rect_t pos_rect;
	u32 hit;
	u32 x,y;


	r = 255;
	g = 255;
	b = 255;

	//g_player_bullets

	for(i=0; i<MAX_BULLETS; i++)
	{
		test = g_player_bullets[i].alive;
		if(test)
		{
			g_player_bullets[i].alive--; // bullets time out


			g_player_bullets[i].pos_rect.x += g_player_bullets[i].dx  >> ONE_SHIFT;
			g_player_bullets[i].pos_rect.y += g_player_bullets[i].dy  >> ONE_SHIFT;

			x =  g_player_bullets[i].pos_rect.x;
			y =  g_player_bullets[i].pos_rect.y;




			LOG_COLLISION("\n Bull %d x %d y %d  \n",i, x, y );

			{
				s32 x1cell, y1cell;
				yarmico_tile_data_t *c1, *c2;

				x1cell = (YARMICO_SCREEN_WIDTH_MID+x+4) >> 3;
				y1cell = (YARMICO_SCREEN_HEIGHT_MID+y+4) >> 3;

				c1 = yarmico_tilemap_get_cell(x1cell ,  y1cell );

				//	yarmico_tilemap_rectangle(0, 0, 0, g_yarmico_collision_test_x, g_yarmico_collision_test_y  , 8, 8);

				//	printf(" %d %d c->tile_cell_u %d  c->tile_cell_v %d \n", g_yarmico_collision_test_x >> 3, g_yarmico_collision_test_y>> 3, c->tile_cell_u , c->tile_cell_v);
				if( c1->tile_cell_u || c1->tile_cell_v )
				{
					g_player_bullets[i].alive = 0;
					explosion_add(x, y, g_player_bullets[i].owner );

				}
				else // bullet still active display and check bots
				{
					u32 bot;
					yarmico_rect_t bot_rect;

					// display bullet
					yarmico_tilemap_rectangle( r,g, b, x, y, 2 ,2 );

					//setup this bullet for collision check
					yarmico_setup_collision(x, y, 2 ,2 );

					bot_rect.w =  bot_rect.h= 8;  // W H doesn't change

					for(bot=0; bot<MAX_BOTS; bot++)
					{

						if(g_bots[bot].displayed)
						{
							bot_rect.x = g_bots[bot].item.pos_rect.x;
							bot_rect.y = g_bots[bot].item.pos_rect.y;

							// test
							// yarmico_tilemap_rectangle( 122,122,55, bot_rect.x, bot_rect.y, 8 , 8 );

							//set bot rect and check collision
							if(yarmico_rect_collision_test(&bot_rect) )
							{
								// both bullet and bot are dead!

								g_player_bullets[i].alive = 0;
								explosion_add(x, y, g_player_bullets[i].owner );

								g_bots[bot].alive = g_bots[bot].displayed =0;
								g_player.UI = 255;
								g_player.bots--;
								//g_player.score += 1;

							} //if(yarmico_rect_collision_test(&bot_rect) )

						}//if(g_bots[bot].displayed)
					}//for(bot=0; bot<MAX_BOTS; bot++)
				}// else bullet still active display and check bots


				//display bullet

				DO_NEW_BULLET: // break out of current loop and continue with the bullet loop

				LOG_BULLET("bullet_display  i %d X %d Y %d Y2 %d \n", i , g_player_bullets[i].pos_rect.x, g_player_bullets[i].pos_rect.y, pos_rect.y);


			} // collision check


		}//if(test)

	}// for(i=0; i<MAX_BULLETS; i++)

} //void bullet_display(void)



void game_screen_clear(void)
{
	u32 test, i;
	// player hit test on bots bullets


	yarmico_stop_all_SFX();
}





u32 game_save(void)
{
	save_t data={1};
	sprintf(data.data.start, DATA_START_CHECK);
	data.data.player = g_player;
	sprintf(data.data.end, DATA_END_CHECK);
	yarmico_save_data( (char *) &data, sizeof(save_t));
	return sizeof(data);

}

u32 game_load(void)
{
	save_t data={0};
	u32 ret;
	ret = yarmico_load_data( (char *) &data, sizeof(save_t));

	if(ret)
	{
		g_player = data.data.player;


		LOG_MAIN("%s;\n",data.data.start);
		LOG_MAIN("%s;\n",data.data.end);

		LOG_MAIN("%d;\n",g_player.alive);
		LOG_MAIN("%d;\n",g_player.posX_fixed);

#if 0
		if( strcmp(data.data.start, DATA_START_CHECK) )
			return 0;

		if(  strcmp(data.data.end, DATA_END_CHECK)  )
			return 0;
#endif

	}

	return ret;

}




void game_update(void)
{
	g_gameloop_function();
}






#define PAD_MIN 4
#define PAD_MAX 11



void gameloop_game(void)
{
	s32 scroll_offsetx, scroll_offsety;
	s32 old_x, old_y;
	s32 x1cell, y1cell;
	s32 x2cell, y2cell;



	if(g_player.gems == 0 && g_player.bots == 0)
	{
		yarmico_stop_all_SFX();
		g_gameloop_function = gameloop_won_screen;
		return;
	}

	if(g_player.alive == 0)
	{
		yarmico_stop_all_SFX();
		g_gameloop_function = gameloop_dead_screen;
		return;
	}


	//	lazy collision detection - two points per side
	x2cell = y2cell = 0;
	x1cell = y1cell = 0;

	old_x  = g_player.posX_fixed;
	old_y  = g_player.posY_fixed;


	g_player.velY += 300;


	if(g_yarmico_controller_dpad & YARMICO_CONTROLER_BUT_U)
	{
		if(g_player.on_ground)
		{
			//bullet_add_player( g_player.posX_fixed>>ONE_SHIFT, g_player.posY_fixed>>ONE_SHIFT, 0, 20000);
			g_player.velY = -15500;
			g_player.posY_fixed-=FIXED_MOVE_AMOUNT;
			// g_yarmico_tilemap_camera_y-=3;
		}


	}
	else if(g_yarmico_controller_dpad & YARMICO_CONTROLER_BUT_D)
	{

		g_yarmico_tilemap_camera_y+=CAM_MOVE_AMOUNT*2;

		LOG_MAIN("D\n");
	}


	// max velY and velX
	if(g_player.velY > 20000)
		g_player.velY = 20000;

	if(g_player.velY < -20000)
		g_player.velY = -20000;

	if(g_player.velX > 20000)
		g_player.velX = 20000;


	if(g_player.velX < -20000)
		g_player.velX = -20000;



	g_player.posY_fixed += g_player.velY;



	// check gravity and for ground block
	{

		yarmico_tile_data_t *c1, *c2;

		x1cell = (YARMICO_SCREEN_WIDTH_MID+PAD_MAX+(g_player.posX_fixed>>ONE_SHIFT)) >> 3;
		y1cell = (YARMICO_SCREEN_HEIGHT_MID+PAD_MAX+(g_player.posY_fixed>>ONE_SHIFT)) >> 3;

		x2cell = (YARMICO_SCREEN_WIDTH_MID+PAD_MIN+(g_player.posX_fixed>>ONE_SHIFT)) >> 3;
		y2cell = y1cell;


		c1 = yarmico_tilemap_get_cell(x1cell ,  y1cell );
		c2 = yarmico_tilemap_get_cell(x2cell ,  y2cell );

		//	yarmico_tilemap_rectangle(0, 0, 0, g_yarmico_collision_test_x, g_yarmico_collision_test_y  , 8, 8);

		//	printf(" %d %d c->tile_cell_u %d  c->tile_cell_v %d \n", g_yarmico_collision_test_x >> 3, g_yarmico_collision_test_y>> 3, c->tile_cell_u , c->tile_cell_v);
		if( c1->tile_cell_u || c1->tile_cell_v || c2->tile_cell_u || c2->tile_cell_v )
		{
			// reset
			g_player.posX_fixed = old_x;
			g_player.posY_fixed = old_y;

			g_player.velY =0;
			//g_player.hanging_upsidedown = 0;
			g_player.on_ground = TRUE;

		}
		else // check jumping up collision with blocks
		{

			x1cell = (YARMICO_SCREEN_WIDTH_MID+PAD_MIN+(g_player.posX_fixed>>ONE_SHIFT)) >> 3;
			y1cell = (YARMICO_SCREEN_HEIGHT_MID+PAD_MIN+(g_player.posY_fixed>>ONE_SHIFT)) >> 3;

			x2cell = (YARMICO_SCREEN_WIDTH_MID+PAD_MAX+(g_player.posX_fixed>>ONE_SHIFT)) >> 3;
			y2cell = y1cell;

			c1 = yarmico_tilemap_get_cell(x1cell ,  y1cell );
			c2 = yarmico_tilemap_get_cell(x2cell ,  y2cell );

			//	yarmico_tilemap_rectangle(0, 0, 0, g_yarmico_collision_test_x, g_yarmico_collision_test_y  , 8, 8);

			//	printf(" %d %d c->tile_cell_u %d  c->tile_cell_v %d \n", g_yarmico_collision_test_x >> 3, g_yarmico_collision_test_y>> 3, c->tile_cell_u , c->tile_cell_v);
			if( c1->tile_cell_u || c1->tile_cell_v || c2->tile_cell_u || c2->tile_cell_v )
			{
				// reset
				g_player.posX_fixed = old_x;
				g_player.posY_fixed = old_y; // lock to botton of block //old_y;


				if(g_player.hanging_upsidedown == FALSE)// not already upside down?
				{
					s32 newY=g_player.posY_fixed>>ONE_SHIFT; // get world Y

					//printf("Y1 %d %d\n", newY, g_player.posY_fixed);

					// For the top sliding through single cell spaces
					// div by 8, then mul by 8 to get correct cell boundary, then back to fixed
					old_y = g_player.posY_fixed = (((newY>>3)) * 8) << ONE_SHIFT;

					// printf("Y2 %d %d\n", (((newY>>3)) * 8), g_player.posY_fixed);
					g_player.hanging_upsidedown = g_player.on_ground = TRUE;
				}

				g_player.velY =0;


			}
			else
			{
				g_player.hanging_upsidedown = g_player.on_ground = FALSE;

				// clear
				x1cell = y1cell = 	x2cell 	= y2cell = 0;
			}

		}

	}



	if(g_yarmico_controller_dpad & YARMICO_CONTROLER_BUT_L)
	{
		g_player.posX_fixed-=FIXED_MOVE_AMOUNT;
		g_player.direction=1;
		g_yarmico_tilemap_camera_x-=CAM_MOVE_AMOUNT;

		x1cell = (YARMICO_SCREEN_WIDTH_MID+PAD_MIN+(g_player.posX_fixed>>ONE_SHIFT)) >> 3;
		y1cell = (YARMICO_SCREEN_HEIGHT_MID+PAD_MIN+(g_player.posY_fixed>>ONE_SHIFT)) >> 3;

		x2cell = x1cell;
		y2cell = (YARMICO_SCREEN_HEIGHT_MID+PAD_MAX+(g_player.posY_fixed>>ONE_SHIFT)) >> 3;

		//	LOG_MAIN("L\n");
	}
	else if(g_yarmico_controller_dpad & YARMICO_CONTROLER_BUT_R)
	{
		g_player.posX_fixed+=FIXED_MOVE_AMOUNT;
		g_player.direction=0;
		g_yarmico_tilemap_camera_x+=CAM_MOVE_AMOUNT;


		x1cell = (YARMICO_SCREEN_WIDTH_MID+PAD_MAX+(g_player.posX_fixed>>ONE_SHIFT)) >> 3;
		y1cell = (YARMICO_SCREEN_HEIGHT_MID+PAD_MAX+(g_player.posY_fixed>>ONE_SHIFT)) >> 3;

		x2cell = x1cell;
		y2cell = (YARMICO_SCREEN_HEIGHT_MID+PAD_MIN+(g_player.posY_fixed>>ONE_SHIFT)) >> 3;


		//	LOG_MAIN("R\n");
	}


	// else // more lazy,no diagonals
#if 0
	if(g_yarmico_controller_dpad & YARMICO_CONTROLER_BUT_U)
	{
		g_player.posY_fixed-=FIXED_MOVE_AMOUNT;
		g_yarmico_tilemap_camera_y-=CAM_MOVE_AMOUNT;


		x1cell = (YARMICO_SCREEN_WIDTH_MID+PAD_MIN+(g_player.posX_fixed>>ONE_SHIFT)) >> 3;
		y1cell = (YARMICO_SCREEN_HEIGHT_MID+PAD_MIN+(g_player.posY_fixed>>ONE_SHIFT)) >> 3;

		x2cell = (YARMICO_SCREEN_WIDTH_MID+PAD_MAX+(g_player.posX_fixed>>ONE_SHIFT)) >> 3;
		y2cell = y1cell;


		LOG_MAIN("U\n");
	}

	else if(g_yarmico_controller_dpad & YARMICO_CONTROLER_BUT_D)
	{
		g_player.posY_fixed+=FIXED_MOVE_AMOUNT;
		g_yarmico_tilemap_camera_y+=CAM_MOVE_AMOUNT;


		x1cell = (YARMICO_SCREEN_WIDTH_MID+PAD_MAX+(g_player.posX_fixed>>ONE_SHIFT)) >> 3;
		y1cell = (YARMICO_SCREEN_HEIGHT_MID+PAD_MAX+(g_player.posY_fixed>>ONE_SHIFT)) >> 3;

		x2cell = (YARMICO_SCREEN_WIDTH_MID+PAD_MIN+(g_player.posX_fixed>>ONE_SHIFT)) >> 3;
		y2cell = y1cell;


		LOG_MAIN("D\n");
	}

#endif



	// movement check
	if(x1cell + y1cell > 0)
	{
		yarmico_tile_data_t *c1, *c2;

		// printf("xcell %d ycell %d \n",xcell, ycell );
		c1 = yarmico_tilemap_get_cell(x1cell ,  y1cell );
		c2 = yarmico_tilemap_get_cell(x2cell ,  y2cell );

		//	yarmico_tilemap_rectangle(0, 0, 0, g_yarmico_collision_test_x, g_yarmico_collision_test_y  , 8, 8);

		//	printf(" %d %d c->tile_cell_u %d  c->tile_cell_v %d \n", g_yarmico_collision_test_x >> 3, g_yarmico_collision_test_y>> 3, c->tile_cell_u , c->tile_cell_v);
		if( c1->tile_cell_u || c1->tile_cell_v || c2->tile_cell_u || c2->tile_cell_v )
		{
			g_player.posX_fixed = old_x;
			g_player.posY_fixed = old_y;

			g_player.velY =0;
			g_player.on_ground = 4;


		}

	}



	g_yarmico_collision_test_y =   (g_player.posY_fixed>>ONE_SHIFT);
	g_yarmico_collision_test_x =   (g_player.posX_fixed>>ONE_SHIFT);

	if(g_yarmico_collision_test_y < 1)
	{
		g_player.posY_fixed +=FIXED_MOVE_AMOUNT;
		g_yarmico_collision_test_y =   (g_player.posY_fixed>>ONE_SHIFT);

	}

	if(g_yarmico_collision_test_y > g_yarmico_maps.map_height*g_yarmico_maps.tile_size)
	{
		g_player.posY_fixed -=FIXED_MOVE_AMOUNT;
		g_yarmico_collision_test_y =   (g_player.posY_fixed>>ONE_SHIFT);

	}



	if(g_yarmico_collision_test_x < 1)
	{
		g_player.posX_fixed +=FIXED_MOVE_AMOUNT;
		g_yarmico_collision_test_x =   (g_player.posX_fixed>>ONE_SHIFT);

	}

	if(g_yarmico_collision_test_x > g_yarmico_maps.map_width*g_yarmico_maps.tile_size)
	{
		g_player.posX_fixed -=FIXED_MOVE_AMOUNT;
		g_yarmico_collision_test_x =   (g_player.posX_fixed>>ONE_SHIFT);

	}



#if 1

	if(  g_yarmico_tilemap_camera_y  >  g_yarmico_collision_test_y)
		g_yarmico_tilemap_camera_y -= ( g_yarmico_tilemap_camera_y -  g_yarmico_collision_test_y ) >> 4;



	if(  g_yarmico_tilemap_camera_y <  g_yarmico_collision_test_y)
		g_yarmico_tilemap_camera_y+=  ( g_yarmico_collision_test_y - g_yarmico_tilemap_camera_y ) >> 4;



	if(  g_yarmico_tilemap_camera_x >  g_yarmico_collision_test_x)
		g_yarmico_tilemap_camera_x -=  ( g_yarmico_tilemap_camera_x -  g_yarmico_collision_test_x ) >> 4;


	if(  g_yarmico_tilemap_camera_x <  g_yarmico_collision_test_x)
		g_yarmico_tilemap_camera_x +=  ( g_yarmico_collision_test_x - g_yarmico_tilemap_camera_x ) >> 4;
#endif




	if(g_player.shot_tick)
	{
		g_player.shot_tick--;
	}
	else
	{
#define SHOT_TICKS 8
#define CENTER_X 3
#define CENTER_Y 1

		// yarmico_tilemap_get_next_level();
		// g_button_press_time = YARMICO_BUTTON_PRESS_TIME <<2;

		if(g_yarmico_controller_action_buttons)
		{
			if( (g_yarmico_controller_action_buttons & YARMICO_CONTROLER_BUT_U) && (g_yarmico_controller_action_buttons & YARMICO_CONTROLER_BUT_L) ) // Up left
			{
				g_player.shot_tick = SHOT_TICKS;
				bullet_add_player( g_yarmico_collision_test_x+CENTER_X, g_yarmico_collision_test_y+CENTER_Y, -20000, -20000, 0);
			}
			else if( (g_yarmico_controller_action_buttons & YARMICO_CONTROLER_BUT_D) && (g_yarmico_controller_action_buttons & YARMICO_CONTROLER_BUT_L) ) // Down left
			{
				g_player.shot_tick = SHOT_TICKS;
				bullet_add_player( g_yarmico_collision_test_x+CENTER_X, g_yarmico_collision_test_y+CENTER_Y, -20000, 20000, 0);
			}
			else if( (g_yarmico_controller_action_buttons & YARMICO_CONTROLER_BUT_U) && (g_yarmico_controller_action_buttons & YARMICO_CONTROLER_BUT_R) ) // Up Right
			{
				g_player.shot_tick = SHOT_TICKS;
				bullet_add_player( g_yarmico_collision_test_x+CENTER_X, g_yarmico_collision_test_y+CENTER_Y, 20000, -20000, 0);
			}
			else if( (g_yarmico_controller_action_buttons & YARMICO_CONTROLER_BUT_D) && (g_yarmico_controller_action_buttons & YARMICO_CONTROLER_BUT_R) ) // Down Right
			{
				g_player.shot_tick = SHOT_TICKS;
				bullet_add_player( g_yarmico_collision_test_x+CENTER_X, g_yarmico_collision_test_y+CENTER_Y, 20000, 20000, 0);
			}
			else
			{
				if(g_yarmico_controller_action_buttons & YARMICO_CONTROLER_BUT_U)
				{
					g_player.shot_tick = SHOT_TICKS;
					bullet_add_player( g_yarmico_collision_test_x+CENTER_X, g_yarmico_collision_test_y+CENTER_Y, 0, -20000, 0);

				}
				else if(g_yarmico_controller_action_buttons & YARMICO_CONTROLER_BUT_D)
				{
					g_player.shot_tick = SHOT_TICKS;
					bullet_add_player( g_yarmico_collision_test_x+CENTER_X, g_yarmico_collision_test_y+CENTER_Y, 0, 20000, 0);
				}

				if(g_yarmico_controller_action_buttons & YARMICO_CONTROLER_BUT_L)
				{
					g_player.shot_tick = SHOT_TICKS;
					bullet_add_player( g_yarmico_collision_test_x+CENTER_X, g_yarmico_collision_test_y+CENTER_Y, -20000,0, 0);

				}
				else if(g_yarmico_controller_action_buttons & YARMICO_CONTROLER_BUT_R)
				{
					g_player.shot_tick = SHOT_TICKS;
					bullet_add_player( g_yarmico_collision_test_x+CENTER_X, g_yarmico_collision_test_y+CENTER_Y,  20000,0, 0);
				}
			}
		}// if g_yarmico_controller_action_buttons

	}


	// do background rect first
	if(g_player.hitcnt) // is hit immunity still active
	{
		g_player.hitcnt--;

		// if(g_player.hitcnt &1) //flash
		yarmico_tilemap_rectangle( g_player.hitcnt, g_player.hitcnt, g_player.hitcnt, g_yarmico_collision_test_x-1, g_yarmico_collision_test_y-0, 10, 10);


	}



	// LOG_MAIN("g_yarmico_collision_test_x %d \n", g_yarmico_collision_test_x);
	yarmico_tilemap_sprite(0,   24,56,      8,8,  g_yarmico_collision_test_x, g_yarmico_collision_test_y ,ONE, g_player.direction, g_player.hanging_upsidedown );//g_yarmico_collision_test_x, g_yarmico_collision_test_y, ONE,g_player.direction , 0);
	//yarmico_tilemap_rectangle(0, 0, 0, g_yarmico_collision_test_x, g_yarmico_collision_test_y  , 8, 8);


	// update the rest

	yarmico_update_time();


	tilemap_draw();

	display_bullet();

	display_explosion();

	display_gems();

	display_bot();
	//g_player.posX_fixed += scroll_offsetx<<ONE;
	//g_player.posY_fixed += scroll_offsety<<ONE;


	display_UI();


}



void game_deinit(void)
{
	// do nothing
}


void gameloop_won_screen(void)
{


	// yarmico_do_C64_loading_screen();
	if( (g_yarmico_game_time & 1) == 0)
	{

		//yarmico_do_C64_loading_screen();

		yarmico_render_text(4,10, ONE, "  YARMICO II");
		yarmico_render_text(4,40, ONE, "   YOU WIN!");
		yarmico_render_text(0,50, ONE, "CONGRATULATIONS");

		yarmico_render_text(0,80, ONE, "LET GWALD KNOW!");
		yarmico_render_text(4,90, ONE, " VIA NETYAROZE");
		yarmico_render_text(4,100, ONE, " CHAT SERVER");



		// audio
		g_yarmico_pico_play_sound.SFX = YARMICO_PICO_SOUND_SFX1_ON_OFF | YARMICO_PICO_SOUND_SFX2_ALT_SIGN;
		g_yarmico_pico_play_sound.play_back = YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_ATTACK;
		g_yarmico_pico_play_sound.SFX_trigger_ON_OFF = 10;
		g_yarmico_pico_play_sound.SFX_hold_ON_OFF = 2; // 5 frame skips

		g_yarmico_pico_play_sound.SFX_trigger2 = 11;
		g_yarmico_pico_play_sound.SFX_hold2 = 15; // 5 frame skips
		g_yarmico_pico_play_sound.dir_speed = -10;


		g_yarmico_pico_play_sound.loop = YARMICO_PICO_SOUND_LOOP_TIMES;
		g_yarmico_pico_play_sound.loop_times = 2;

		YARMICO_MACRO_SET_PICO_AUDIO(965, 100, 100, 0, 0 , 100);


	}
	else
	{
		char line[32];

		sprintf(line, "%d", g_yarmico_game_time);
		// yarmico_do_C64_loading_screen();

		yarmico_render_text(4,10, ONE, "  YARMICO II");
		yarmico_render_text(4,40, ONE, "   YOU WIN!");
		yarmico_render_text(0,50, ONE, "CONGRATULATIONS");

		yarmico_render_text(4,80, ONE, "YOUR TIME:");
		yarmico_render_text(4,90, ONE, line);



		// audio
		g_yarmico_pico_play_sound.SFX = YARMICO_PICO_SOUND_SFX1_ON_OFF | YARMICO_PICO_SOUND_SFX2_ALT_SIGN;
		g_yarmico_pico_play_sound.play_back = YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_ATTACK;
		g_yarmico_pico_play_sound.SFX_trigger_ON_OFF = 5;
		g_yarmico_pico_play_sound.SFX_hold_ON_OFF = 10; // 5 frame skips

		g_yarmico_pico_play_sound.SFX_trigger2 = 10;
		g_yarmico_pico_play_sound.SFX_hold2 = 20; // 5 frame skips
		g_yarmico_pico_play_sound.dir_speed = -100;


		g_yarmico_pico_play_sound.loop = YARMICO_PICO_SOUND_LOOP_TIMES;
		g_yarmico_pico_play_sound.loop_times = 2;

		YARMICO_MACRO_SET_PICO_AUDIO(665, 100, 100, 0, 0 , 50);



	}

	if(g_button_press_time)
		g_button_press_time--;
	else if(g_yarmico_controller_action_buttons)
	{
		game_screen_clear();

		g_button_press_time = YARMICO_BUTTON_PRESS_TIME <<2;
		g_yarmico_game_time = g_game_controller_timer = 0;
		g_gameloop_function = gameloop_start_screen;
	}

	yarmico_update_time();


} // void gameloop_won_screen(void)




void gameloop_dead_screen(void)
{

	if( (g_yarmico_game_time & 1) )
		yarmico_do_C64_loading_screen();

	{

		char line[32];

		sprintf(line, "%03d  %03d %05d", g_player.gems ,  g_player.bots, g_yarmico_game_time);

		yarmico_render_text(4,10, ONE, "  YARMICO II");
		yarmico_render_text(4,40, ONE, "   YOU DEAD!");


		// yarmico_render_text(0,80, ONE, "LET GWALD KNOW!");
		yarmico_render_text(2,90, ONE, "GEMS BOTS TIME");
		yarmico_render_text(8,100, ONE, line);




	}


	if(g_button_press_time)
		g_button_press_time--;
	else if(g_yarmico_controller_action_buttons)
	{
		game_screen_clear();

		g_button_press_time = YARMICO_BUTTON_PRESS_TIME <<2;
		g_yarmico_game_time = g_game_controller_timer = 0;
		g_gameloop_function = gameloop_start_screen;

	}

	yarmico_update_time();


} // void gameloop_won_screen(void)

void gameloop_start_screen(void)
{

	g_yarmico_background_color_red = 0;
	g_yarmico_background_color_green  =  0;
	g_yarmico_background_color_blue  =  0;

	// yarmico_do_C64_loading_screen();
	if( (g_yarmico_game_time & 1) == 0)
	{


		//yarmico_do_C64_loading_screen();

		yarmico_render_text(4,10, ONE, "YARMICO II");
		yarmico_render_text(2,20, ONE, "SCROLLN/TROLLN");
		yarmico_render_text(4,30, ONE, "PIXELART: S4M"); //https://s4m-ur4i.itch.io/
		yarmico_render_text(4,40, ONE, "CODING: GWALD");

		yarmico_render_text(4,60, ONE, "NETYAROZE JAM");
		yarmico_render_text(4,70, ONE, "GAMEJAM NOV 22");
		yarmico_render_text(4,90, ONE, "# PICOSYSTEM");
		yarmico_render_text(4,100, ONE, "WITH 32BLIT");



		// audio
		g_yarmico_pico_play_sound.SFX = YARMICO_PICO_SOUND_SFX1_ON_OFF | YARMICO_PICO_SOUND_SFX2_ALT_SIGN;
		g_yarmico_pico_play_sound.play_back = YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_ATTACK;
		g_yarmico_pico_play_sound.SFX_trigger_ON_OFF = 10;
		g_yarmico_pico_play_sound.SFX_hold_ON_OFF = 2; // 5 frame skips

		g_yarmico_pico_play_sound.SFX_trigger2 = 11;
		g_yarmico_pico_play_sound.SFX_hold2 = 15; // 5 frame skips
		g_yarmico_pico_play_sound.dir_speed = -10;


		g_yarmico_pico_play_sound.loop = YARMICO_PICO_SOUND_LOOP_TIMES;
		g_yarmico_pico_play_sound.loop_times = 2;

		YARMICO_MACRO_SET_PICO_AUDIO(965, 100, 100, 0, 0 , 100);


	}
	else
	{

		// yarmico_do_C64_loading_screen();

		yarmico_render_text(4,10, ONE, "YARMICO II");
		yarmico_render_text(2,20, ONE, "SCROLLN/TROLLN");
		yarmico_render_text(4,40, ONE, " GET DA GEMS");
		yarmico_render_text(4,50, ONE, " SHOT DA BOTS");

		yarmico_render_text(4,70, ONE, "GITHUB.COM/");
		yarmico_render_text(4,80, ONE, "GWALD/YARMICO2");



		// audio
		g_yarmico_pico_play_sound.SFX = YARMICO_PICO_SOUND_SFX1_ON_OFF | YARMICO_PICO_SOUND_SFX2_ALT_SIGN;
		g_yarmico_pico_play_sound.play_back = YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_ATTACK;
		g_yarmico_pico_play_sound.SFX_trigger_ON_OFF = 5;
		g_yarmico_pico_play_sound.SFX_hold_ON_OFF = 10; // 5 frame skips

		g_yarmico_pico_play_sound.SFX_trigger2 = 10;
		g_yarmico_pico_play_sound.SFX_hold2 = 20; // 5 frame skips
		g_yarmico_pico_play_sound.dir_speed = -100;


		g_yarmico_pico_play_sound.loop = YARMICO_PICO_SOUND_LOOP_TIMES;
		g_yarmico_pico_play_sound.loop_times = 2;

		YARMICO_MACRO_SET_PICO_AUDIO(665, 100, 100, 0, 0 , 50);



	}

	if(g_button_press_time)
		g_button_press_time--;
	else if(g_yarmico_controller_action_buttons)
	{
		game_screen_clear();

		g_gameloop_function = gameloop_game;
		yarmico2_init();
	}

	yarmico_update_time();


#if 0
	if(g_yarmico_game_time > 10)
		{
		g_yarmico_game_time =0;
		game_screen_clear();

			g_gameloop_function = gameloop_game;
			yarmico2_init();
		}

#endif
}

void game_init(void)
{
	g_button_press_time = YARMICO_BUTTON_PRESS_TIME <<2;
	g_yarmico_game_time = g_game_controller_timer = 0;

	g_gameloop_function = gameloop_start_screen;
	// g_gameloop_function = gameloop_won_screen;
	// g_gameloop_function = gameloop_dead_screen;

}

void yarmico2_init(void) // game is responsible for deinit'ing and exiting
{
	u32 x,y,h,w,bots, gems;
	yarmico_tile_data_u* cell;
	// printf("game_init g_player.posX_fixed  %d  g_player.posY_fixed %d \n", g_yarmico_maps.start_cell_pos_X , g_yarmico_maps.start_cell_pos_Y);

	g_yarmico_maps.current_level = 0; // only 1 level
	yarmico_tilemap_get_next_level();// init level zero
#if 1
	h= g_yarmico_maps.map_height;
	w= g_yarmico_maps.map_width;

	// printf("map %d x %d\n", h, w);

	//populate bots and player start from map
	gems = bots=0;
	for(y=0; y<h; y++)
	{
		for(x=0; x<w; x++)
		{

			cell =  (yarmico_tile_data_u *) yarmico_tilemap_get_cell(x, y);

			// first cells 1-5 in the map are informational
			if(cell->cell.tile_cell_u > 7 && cell->cell.tile_cell_u  <= (6*8) && cell->cell.tile_cell_v == 0)
			{
				u32 type = cell->cell.tile_cell_u>>3; // div 8;

				// printf("found %d %d\n", cell->cell.tile_cell_u , cell->cell.tile_cell_v );

				if(type == 1) // player start
				{




					g_yarmico_maps.start_cell_pos_X = (x-8)*8; // 16 cells is screen width, -8 for cam center, i think?
					g_yarmico_maps.start_cell_pos_Y = (y-1)*8; // 16 cells is screen width, -8 for cam center, i think?





					// printf("player %d %d - %d %d\n", x, y , g_yarmico_maps.start_cell_pos_X , g_yarmico_maps.start_cell_pos_Y );
				}
				else if(type == 2) // level enemies
				{
					type--; // level 1 starts at tile 2

					if( type <= g_yarmico_maps.current_level)
						if(bots < MAX_BOTS)
						{
							g_bots[bots].alive = g_yarmico_maps.current_level;
							g_bots[bots].item.pos_rect.h = g_bots[bots].item.pos_rect.w = 8;
							g_bots[bots].item.pos_rect.x = (x-8)*8; // 16 cells is screen width, -8 for cam center, i think?
							g_bots[bots].item.pos_rect.y = (y-8)*8;


							g_bots[bots].item.dpad = !(bots&1); // easier if right is 1 and left 0, - just left and right is needed not dpad YARMICO_CONTROLER_BUT_L;


							g_bots[bots].item.screen_x_fixed = 0;

							g_bots[bots].item.speed = 128 * g_yarmico_maps.current_level;


							// get texture
							{
								u8 u, v;

								u = (bots>>3); // y
								v = bots - u*8; //x is left overs from y

								//limit to 16 x 16
								if(u > 16)
									u = u-(u>>4)*16;

								if(v > 16)
									v = v-(v>>4)*16;

								g_bots[bots].item.u = v*8;
								g_bots[bots].item.v = (3*8) + u*8; // starts on 3rd row (8pixels each) on TPAGE
							}



							// printf("bots #%d X%d Y%d %d %d\n",bots, x,y,  g_bots[bots].item.pos_rect.x , g_bots[bots].item.pos_rect.y );
							bots++;

						}
					// else			printf("ERROR, TOO MANY BOTS!\n");

				}else if(type == 5) // level gems
				{


					if(gems < MAX_GEMS)
					{
						g_gems[gems].sprite = 32*gems>>2;
						g_gems[gems].alive = 1;
						g_gems[gems].item.pos_rect.h = g_gems[gems].item.pos_rect.w = 8;
						g_gems[gems].item.pos_rect.x = (x-8)*8; // 16 cells is screen width, -8 for cam center, i think?
						g_gems[gems].item.pos_rect.y = (y-8)*8;


						// get texture
						{
							u8 u, v;

							u = (bots>>3); // y
							v = bots - u*8; //x is left overs from y

							g_gems[gems].item.tpage = 1;
							g_gems[gems].item.u = 0;
							g_gems[gems].item.v = 56; // starts on 3rd row (8pixels each) on TPAGE
						}



						// printf("gems #%d X%d Y%d %d %d\n",gems, x,y,  g_gems[gems].item.pos_rect.x , g_gems[gems].item.pos_rect.y );
						gems++;

					}
					// else	printf("ERROR, TOO MANY GEMS!\n");

				}
				else
				{
					// printf("type unknown %d \n" , type);
				}





				// remove info tiles from map
				g_yarmico_maps.cur_map_data[(y*w)+x].tile_cell_u = g_yarmico_maps.cur_map_data[(y*w)+x].tile_cell_v = 0;


			}


		} // for(x=0; x<w; x++)
	}// for(y=0; y<h; y++)


#endif

	// g_yarmico_collision_test_x = 16;
	// g_yarmico_collision_test_y = 16;
	g_button_press_time = YARMICO_BUTTON_PRESS_TIME <<2;


	g_player.alive = 9;
	g_player.posX_fixed = g_yarmico_maps.start_cell_pos_X<<ONE_SHIFT;
	g_player.posY_fixed = (g_yarmico_maps.start_cell_pos_Y)<<ONE_SHIFT;



	g_yarmico_tilemap_camera_x =  g_yarmico_maps.start_cell_pos_X; //center
	g_yarmico_tilemap_camera_y = g_yarmico_maps.start_cell_pos_Y; //center


	g_gameloop_function = gameloop_game;
	g_yarmico_game_time = g_game_controller_timer = 0;
	g_player.gems = gems;
	g_player.bots = bots;
	// printf("game_init g_player.posX_fixed  %d  g_player.posY_fixed %d \n", g_player.posX_fixed , g_player.posY_fixed );




#if 0


	g_yarmico_pico_play_sound.SFX = YARMICO_PICO_SOUND_SFX1_ON_OFF | YARMICO_PICO_SOUND_SFX2_UP_DOWN;
	g_yarmico_pico_play_sound.play_back = YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_ATTACK;
	g_yarmico_pico_play_sound.SFX_hold1 = 15; // 5 frame skips
	g_yarmico_pico_play_sound.dir_speed = 1000;
	g_yarmico_pico_play_sound.SFX_trigger1 = 10;


	g_yarmico_pico_play_sound.SFX_trigger2 = 1;
	g_yarmico_pico_play_sound.SFX_hold2 = 20; // 5 frame skips
	g_yarmico_pico_play_sound.dir_speed = 1000;

	g_yarmico_pico_play_sound.time_frames_repeat = g_yarmico_pico_play_sound.time_frames = 15;
	g_yarmico_pico_play_sound.loop = YARMICO_PICO_SOUND_LOOP_REPEAT;
	g_yarmico_pico_play_sound.loop_times = 500;

	YARMICO_MACRO_SET_PICO_AUDIO(1730, 100, 100, 10, 0 );



#endif

}

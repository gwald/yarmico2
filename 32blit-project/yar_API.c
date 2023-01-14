/*
 *
 PUBLIC DOMAIN 2022
 THIS CODE IS A HACKY MESS - READ IT AT YOUR OWN RISK!
 https://github.com/gwald/yarmico
 *
 */


#ifdef __ANDROID__
// ANDROID uses CPP not C
extern "C" {
#endif




#define INSTANCE_YARMICO_API
#include "yar_API.h"
// #define printf //
// 12 rows X 20 cells = 240
// 240 / 8bits = 30bytes
// 10 10
// 9 2 9
// 8 4 8




// #define printf //

#if YARMICO_USE_TILEMAP == TRUE


// tile/map code inspired by:
// https://github.com/martincohen/Punity/blob/def395699e24670ae92d438f5b6b5e60aeec961c/punity.h#L3716
// Super-Square-Bros https://github.com/ThePythonator/Super-Square-Bros/blob/main/SuperSquareBros.cpp
// Code-It-Yourself! Simple Tile Based Platform Game #1 https://www.youtube.com/watch?v=oJvJZNyW_rw

void tilemap_draw(void)
{
	u32 map_width;
	u32 map_height, total_size;
	u32 tile_size;
	u32 tpage;
	u32 scroll_offsetx, scroll_offsety;


	yarmico_tile_data_t* (tile_array);
	u32 y, x;
	u32 visible_tiles_x, visible_tiles_y;
	s32 offset_y, offset_x , camera_pos_x, camera_pos_y;

	u32 cell, tilex, tiley;



	// todo g_yarmico_screen_rect

	camera_pos_x = YARMICO_SCREEN_WIDTH_MID +  g_yarmico_tilemap_camera_x ;

	camera_pos_y = YARMICO_SCREEN_HEIGHT_MID +  g_yarmico_tilemap_camera_y;


	tile_array = g_yarmico_maps.cur_map_data;

	map_width = g_yarmico_maps.map_width;
	map_height = g_yarmico_maps.map_height;
	tile_size = g_yarmico_maps.tile_size;

	tpage = g_yarmico_maps.tpage;
	total_size = map_width * map_height;



	//printf("\n TILEMAP camera_pos_x:%d camera_pos_y:%d map_width:%d map_height:%d  tile_size:%d  \n", camera_pos_x, camera_pos_y, map_width,  map_height, tile_size);

	scroll_offsetx = camera_pos_x;
	scroll_offsety = camera_pos_y;

	//camera pos to tile pos??
	camera_pos_x /= tile_size; // map_width;
	camera_pos_y  /= tile_size; //  map_width;


	scroll_offsetx =   scroll_offsetx - (camera_pos_x*tile_size);
	scroll_offsety =   scroll_offsety - (camera_pos_y*tile_size);
//	printf("\nscroll_offsetx:%d scroll_offsety:%d   \n", scroll_offsetx,  scroll_offsety );



	//	scroll_offsetx = scroll_offsetx/2;
	//	scroll_offsety = scroll_offsety/2;
	//	scroll_offsetx*=tile_size;
	//	scroll_offsety*=tile_size;

	//tiles on screen
	visible_tiles_x = YARMICO_SCREEN_WIDTH / tile_size;
	visible_tiles_y = YARMICO_SCREEN_HEIGHT / tile_size;

	//visible_tiles_x -= 1;
	//visible_tiles_y  -= 1;



	// calculate top left visible tile
	offset_x = camera_pos_x - (visible_tiles_x >> 1);
	offset_y = camera_pos_y - (visible_tiles_y >> 1);


	// printf("\n TILEMAP map_width:%d map_height:%d  tile_size:%d \n camera_pos_x:%d camera_pos_y:%d \n", map_width,  map_height, tile_size,  camera_pos_x, camera_pos_y);


#if 0
	// clamp camera to game boundaries
	if(offset_x < 0)
		offset_x =0;

	if(offset_y < 0)
		offset_y =0;

	if(offset_x > map_width-visible_tiles_x)
		offset_x = map_width - visible_tiles_x;

	if(offset_y > map_height -visible_tiles_y)
		offset_y = map_height - visible_tiles_y;
#endif
	// check if tilemap is in view
	// i = tilemap->map_start_offset_x + (tilemap->map_width * tilemap->tile_size);

	// printf("\n TILEMAP x:%d y:%d  OSx:%d OSy:%d Vx:%d Vy:%d  \n", camera_pos_x,  camera_pos_y, offset_x, offset_y, visible_tiles_x, visible_tiles_y);



	for(y = 0; y < visible_tiles_y+1; y++)
	{

		//	printf("   y:%d    Ay:%d  \n",  y, ( map_width*(offset_y+y)));

		for(x = 0; x < visible_tiles_x+1; x++)
		{

			//u32 cell = tile_array[ ( map_width*(offset_y+y))  + offset_x +x].tile_cell_;
			// printf(" [%d %d %d %d,%d] ", t, ( map_width*(offset_y+y))  + offset_x +x,( map_width*(offset_y+y)), offset_x +x  );


			// pixel map 128 w / 8 cells = 16
			//tiley = cell / 16;
			//tilex = cell % 16;

			// printf(" [%d  %d,%d] ", cell, tilex, tiley );
			//	if(cell > 0)
			//	yarmico_sprite(tpage, tile_size * tilex, tile_size * tiley,  8, 8 , x*tile_size, y*tile_size, ONE, 0,0);

			cell = ( map_width*(offset_y+y)) + offset_x +x;

			if(cell > 0 && cell < total_size)
			{


				//tilex = (u32) g_yarmico_maps.tile_array[0][cell].tile_cell_u;
				//tiley = (u32) g_yarmico_maps.tile_array[0][cell].tile_cell_v;

				tilex = tile_array[cell ].tile_cell_u;
				tiley = tile_array[ cell ].tile_cell_v;



				// printf(" [%d,%d] ",  tilex, tiley );
				if(tilex+tiley > 0)
				{

					//	 printf(" [%d  %d,%d] ", cell, tilex, tiley );
					//	 fflush(stdout);
					yarmico_sprite(tpage, tilex, tiley,  8, 8 , (x*tile_size)-scroll_offsetx, (y*tile_size) - scroll_offsety , ONE, 0,0);
				}
			}
			/*
			if(t > 0)
				yarmico_rectangle(110, 110, 110,  x*tile_size, y*tile_size,tile_size, tile_size );
			else
				yarmico_rectangle(0, 0, 0,  x*tile_size, y*tile_size,tile_size, tile_size );
			 */

		}
		//	printf("\n");
	}
	// printf("\n");
}


#endif //YARMICO_USE_TILEMAP



#if YARMICO_PICO_SOUND == TRUE

void yarmico_play_pico_sound(void) // check time_frame before calling
{
	u32 turn_sound_off;
	u32 play_back = g_yarmico_pico_play_sound.play_back;
	//u16 freq = g_yarmico_pico_play_sound.frequency;

	turn_sound_off=0;

	g_yarmico_pico_play_sound.time_frames--;


	if(g_yarmico_pico_play_sound.time_frames == 0)
	{
		if(g_yarmico_pico_play_sound.loop != YARMICO_PICO_SOUND_LOOP_NONE)
		{

			g_yarmico_pico_play_sound.time_frames = g_yarmico_pico_play_sound.time_frames_repeat;

			if(g_yarmico_pico_play_sound.loop == YARMICO_PICO_SOUND_LOOP_REPEAT)
				; // do nothing

			else if(g_yarmico_pico_play_sound.loop == YARMICO_PICO_SOUND_LOOP_TIMES && g_yarmico_pico_play_sound.loop_times)
			{
				g_yarmico_pico_play_sound.loop_times--;

			}
			else
			{
				g_yarmico_pico_play_sound.loop =YARMICO_PICO_SOUND_LOOP_NONE;
				g_yarmico_pico_play_sound.time_frames = 0;
				turn_sound_off = 1;

			}



		}
	}
	else if(play_back == YARMICO_PICO_SOUND_PLAYBACK_OFF)
	{
		g_yarmico_pico_play_sound.time_frames = 0;
		turn_sound_off = 1;
		//	return yarmico_play_pico_sound_platform(turn_sound_off);
	}
	else if(g_yarmico_pico_play_sound.SFX)
	{


		if(g_yarmico_pico_play_sound.SFX & YARMICO_PICO_SOUND_SFX2_ALT_SIGN )
		{
			g_yarmico_pico_play_sound.SFX_counter2++;
			if(g_yarmico_pico_play_sound.SFX_counter2 > g_yarmico_pico_play_sound.SFX_trigger2)
			{
				s32 speed = g_yarmico_pico_play_sound.dir_speed;


				if(g_yarmico_pico_play_sound.time_frames&1)
					speed = -speed;

				g_yarmico_pico_play_sound.frequency+= speed;

				if(g_yarmico_pico_play_sound.SFX_counter2 > g_yarmico_pico_play_sound.SFX_hold2)
				{
					g_yarmico_pico_play_sound.SFX_counter2=0;
				}
			}
		}

		if(g_yarmico_pico_play_sound.SFX & YARMICO_PICO_SOUND_SFX2_UP_DOWN )
		{
			g_yarmico_pico_play_sound.SFX_counter2++;
			if(g_yarmico_pico_play_sound.SFX_counter2 > g_yarmico_pico_play_sound.SFX_trigger2)
			{
				g_yarmico_pico_play_sound.frequency+=g_yarmico_pico_play_sound.dir_speed;
				if(g_yarmico_pico_play_sound.SFX_counter2 > g_yarmico_pico_play_sound.SFX_hold2)
					g_yarmico_pico_play_sound.SFX_counter2=0;

			}

		}

		if(g_yarmico_pico_play_sound.SFX & YARMICO_PICO_SOUND_SFX1_ON_OFF )
		{
			g_yarmico_pico_play_sound.SFX_counter_ON_OFF++;

			if(g_yarmico_pico_play_sound.SFX_counter_ON_OFF > g_yarmico_pico_play_sound.SFX_trigger_ON_OFF)
			{
				if(g_yarmico_pico_play_sound.SFX_counter_ON_OFF > g_yarmico_pico_play_sound.SFX_hold_ON_OFF)
					g_yarmico_pico_play_sound.SFX_counter_ON_OFF=0;


				turn_sound_off = 1;

			}
		}


	}

	yarmico_play_pico_sound_platform(turn_sound_off);

}

#endif //#if YARMICO_PICO_SOUND==TRUE


// update time
void yarmico_update_time(void)
{
	u8 c  = g_yarmico_frame_counter>>YARMICO_UPDATE_TIME_SHIFT; // abit slower then normal minute, but better then too fast!
	u8 t = g_yarmico_game_time;

	c = c & 1;
	t = t & 1;

	//printf("c %d t %d test %d \n", c , t , c != t );

	if(c  != t ) // test for change
		g_yarmico_game_time++;

	g_yarmico_frame_counter++;

	//printf("C:%d T:%d\n",g_yarmico_frame_counter,g_yarmico_game_time);


}



void intern_black_and_white_BG(u32 side)
{

	if(side)
	{
		yarmico_rectangle( 255, 255, 255,  160, 0, 320,240 );
		yarmico_rectangle(0, 0, 0,    0, 0, 160,240 );
	}
	else
	{

		yarmico_rectangle( 255, 255, 255,   0, 0, 160,240 );
		yarmico_rectangle(0, 0, 0,   160, 0, 320,240 );
	}
}



void yarmico_do_C64_loading_screen(void)
{
	u32 i;
	u32 h;
	u32 t;
	t = 16;
	h = YARMICO_SCREEN_HEIGHT/t;

	// C64 loading bards
	//for(i=0; i<24; i++)
	for(i=0; i<YARMICO_SCREEN_HEIGHT/h; i++)
	{

		char R, G, B;


		switch(rand()%16)
		//switch(i%16)
		{


		// white    0xff    0xff    0xff    2
		case 1:
			R = 0xa0;
			G = 0xa0;
			B = 0xa0;
			break;

			// red    0x88    0x00    0x00    3
		case 2:
			R = 0x88;
			G = 0x0;
			B = 0x0;
			break;

			// cyan    0xaa    0xff    0xee    4
		case 3:
			R = 0xaa;
			G = 0xff;
			B = 0xee;
			break;

			// purple    0xcc    0x44    0xcc    5
		case 4:
			R = 0xcc;
			G = 0x44;
			B = 0xcc;
			break;

			// green    0x00    0xcc    0x55    6
		case 5:
			R = 0;
			G = 0xcc;
			B = 0x55;
			break;

			// blue    0x00    0x00    0xaa    7
		case 6:
			R = 0;
			G = 0;
			B = 0xaa;
			break;

			// yellow    0xee    0xee    0x77    8
		case 7:
			R = 0xee;
			G = 0xee;
			B = 0x77;
			break;

			// orange    0xdd    0x88    0x55    9
		case 8:
			R = 0xdd;
			G = 0x88;
			B = 0x55;
			break;

			// brown    0x66    0x44    0x00    10
		case 9:
			R = 0x66;
			G = 0x44;
			B = 0;
			break;

			// l,red    0xff    0x77    0x77    11
		case 10:
			R = 0xff;
			G = 0x77;
			B = 0x77;
			break;

			// d,gray    0x33    0x33    0x33    12
		case 11:
			R = 0x33;
			G = 0x33;
			B = 0x33;
			break;

			// gray    0x77    0x77    0x77    13
		case 12:
			R = 0x77;
			G = 0x77;
			B = 0x77;
			break;
			// lgreen    0xaa    0xff    0x66    14
		case 13:
			R = 0xaa;
			G = 0xff;
			B = 0x66;
			break;

			// l,blue    0x00    0x88    0xff    15
		case 14:
			R = 0;
			G = 0x88;
			B = 0x88;
			break;

			// l,gray    0xbb    0xbb    0xbb    16
		case 15:
			R = 0xbb;
			G = 0xbb;
			B = 0xbb;
			break;


			// black    0x00    0x00    0x00    1
		default:
			R = 0x0;
			G = 0xbb;
			B = 0x0;
			break;
		}

		yarmico_rectangle(R, G, B, 0, (i*h), YARMICO_SCREEN_WIDTH, h); //+(i&1)

	}

}



// poor man's percent - not in critical loop!
//intern_PSX_calc_percent(g_gamestate.level_bullets_hit,  g_gamestate.level_bullets_used );
//ie g_total_bullets_hit  / g_total_bullets_used
#define LOG_PERC //  printf
#if 1
u32 intern_PSX_calc_percent(s32 numerator , s32 denominator)
{

	u32 ret =0;
	LOG_PERC( "intern_PSX_calc_percent %d / %d \n", numerator ,  denominator );



	if(numerator == 0 )
		return 0;

	if(denominator == 0 )
		return 0;


	if(denominator == numerator )
		return 100;

	if(denominator == 1 )
		return numerator;


	numerator*=100;

	do
	{
		numerator -=denominator;
		ret++;
		LOG_PERC( "intern_PSX_DIV %d ret %d \n", numerator ,  ret );
	}while(numerator>0);

	if(numerator < -50)
		ret--;

	return ret;


}
#endif



//PSX qsort has issues - avoid this one works fine
//https://www.geeksforgeeks.org/generic-implementation-of-quicksort-algorithm-in-c/



/* The GNU glibc version of qsort allocates memory, which we must not
   do if we are invoked by a signal handler.  So provide our own
   sort. https://github.com/gcc-mirror/gcc/blob/master/libbacktrace/sort.c
 */

void swap (char *a, char *b, u32 size)
{
	u32 i;

	for (i = 0; i < size; i++, a++, b++)
	{
		char t;

		t = *a;
		*a = *b;
		*b = t;
	}
}

void _qsort (void *basearg, u32 count, u32 size,
		s32 (*compar) ( void *,  void *) )
{
	char *base = (char *) basearg;
	u32 i;
	u32 mid;

	tail_recurse:
	if (count < 2)
		return;

	/* The symbol table and DWARF tables, which is all we use this
     routine for, tend to be roughly sorted.  Pick the middle element
     in the array as our pivot point, so that we are more likely to
     cut the array in half for each recursion step.  */
	swap (base, base + (count>>1) * size, size);

	mid = 0;
	for (i = 1; i < count; i++)
	{
		if ((*compar) (base, base + i * size) > 0)
		{
			++mid;
			if (i != mid)
				swap (base + mid * size, base + i * size, size);
		}
	}

	if (mid > 0)
		swap (base, base + mid * size, size);

	/* Recurse with the smaller array, loop with the larger one.  That
     ensures that our maximum stack depth is log count.  */
	if (2 * mid < count)
	{
		_qsort (base, mid, size, compar);
		base += (mid + 1) * size;
		count -= mid + 1;
		goto tail_recurse;
	}
	else
	{
		_qsort (base + (mid + 1) * size, count - (mid + 1),
				size, compar);
		count = mid;
		goto tail_recurse;
	}
}




// 8x8
// 0: A - P
// 8: Q - Z - arrows, $ $
// 16: 0 - 9 - . ? ! music
// 24: heart1, 2 checkers5, 6 bull

#define CHAR_SPACE 8
void yarmico_render_text(u32 posX, u32 posY, s32 scale, char *string)
{
	u32 i,  U, V; //len,
	u8 c;

	LOG_TEXT("posX %d, posY %d, scale %d, %s \n", posX,  posY,  scale, string);


#if 0

	// process string ------------
	len=strlen(string);

	LOG_TEXT("len=strlen(string); %d  \n", len);


	if(len > YARMICO_SCREEN_WIDTH>>3) //limit string to screen width
		len = YARMICO_SCREEN_WIDTH>>3;
#endif

	for(i=0;i<=YARMICO_SCREEN_WIDTH>>3; i++)
	{
		c = string[i];

		LOG_TEXT("i: %d c: %c \n", i, c);



		if(c == 0) //NUL
		{
			return;
		}

		if(c == 32) //space
		{
			posX+=CHAR_SPACE;
			continue;
		}


		//first mostlikely cases
		if(c > 47 && c < 58) // 0-9 zero digit
		{
			U = 8 * (c - '0');
			V = 16;
		}
		else if(c > 64 && c < 81) // A - P
		{
			U = 8 * (c - 'A');
			V = 0;
		}
		else if(c > 80 && c < 91) // Q - Z
		{
			U = 8 * (c - 'Q');
			V = 8;
		}
		// special string chars -.!$?
		else if(c == '-')
		{
			U = 8 * 12;
			V = 16;
		}
		else if(c == '.')
		{
			U = 8 * 11;
			V = 16;
		}
		else if(c == '?')
		{
			U = 8 * 13;
			V = 16;
		}
		else if(c == '!')
		{
			U = 8 * 14;
			V = 16;
		}
		else if(c == '/')
		{
			U = 8 * 10;
			V = 8;
		}
		else if(c == '=')
		{
			U = 8 * 11;
			V = 8;
		}
		else if(c == ',')
		{
			U = 8 * 12;
			V = 8;
		}
		else if(c == '@') // Smile face
		{

			posX+=CHAR_SPACE;
			continue;
		}
		else if(c == '$')
		{
			posX+=CHAR_SPACE;
			continue;
		}

		//last row 8*3=24
		else if(c == '+')
		{
			posX+=CHAR_SPACE;
			continue;
		}


		else if(c == ':')
		{
			U = 8 * 15;
			V = 16;
		}


		else if(c == '\'')
		{
			posX+=CHAR_SPACE;
			continue;

		}



		else if(c == '(')
		{
			U = 8 * 15;
			V = 8;

		}
		else if(c == ')')
		{
			U = 8 * 16;
			V = 8;
		}
		else if(c == '#') // heart
		{ // 86x56
			U = 96; // 8 * ( (u32)string & 0x0F);
			V = 56; // 24;
		}
		else if(c == '_')
		{
			U =  8 *13;
			V = 16;
		}
		else if(c == '[')
		{
			posX+=CHAR_SPACE;
			continue;
		}
		else if(c == ']')
		{
			posX+=CHAR_SPACE;
			continue;
		}
		else
			//95 underscore
			//			if(c > 96) // small caps chars
		{
			printf(" len %d  (%d char %c) Char lower case! string %s\n",strlen(string),  c, c, string);
			printf(" len %d  (%d char %c) Char lower case! string %s\n",strlen(string),  c, c, string);
			printf(" len %d  (%d char %c) Char lower case! string %s\n",strlen(string),  c, c, string);

			exit(0);//c -= 32; // turn into big capitals
		}



		V += FONT_Y_OFFSET; // start Y 224
		yarmico_sprite(FONT_TPAGE,   U, V, 8, 8,    posX+0,     posY+0,    scale ,  0, 0);
		posX+=CHAR_SPACE;
	}
}










#ifdef __ANDROID__
// ANDROID uses CPP not C
}
#endif




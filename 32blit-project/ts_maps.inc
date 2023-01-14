
// 1/10/2023
//maps gwald PD TS:1.3.0 NY-VM 
// export to: C:\Documents and Settings\NetYaroze\Desktop\NY-demos\scrolla-yaroze\
// tiles 256 - must be less the u8 to use in map
// must rerun dataman on binary data when updated







yarmico_tilemap_t g_yarmico_maps  = 
{
	// [HEADER] from Project lists and exported 
1, // TILEMAP_TPAGE  
100, //<MapWidth>, 
100,   //	<MapHeight>, 
8,  //	<TileWidth>, 
0, // current_level set start to zero 
// start_cell_pos_X, start_cell_pos_Y; 
6,6, 
0, // yarmico_tile_data_t* map_data; 
0,  // yarmico_tile_data_t* cur_map_data; 
 {110,0,0}  // RGB backgrounds
 
};





#ifndef __MAP_H__
#define	__MAP_H__

#include <darnit/darnit.h>

struct Map {
	struct {
		int		lim_lx;
		int		lim_ly;
		int		lim_bx;
		int		lim_by;
		int		x;
		int		y;
		int		follow;
	} camera;

	DARNIT_MAP		*map;
};

extern struct Map map_s;
int map_get_solid(int x, int y, int dx, int dy, int layer);
int map_check_area(int x, int y, int dx, int dy, int layer, int w, int h);
unsigned int map_get_blocki(int i, int layer);
unsigned int map_get_block(int x, int y, int layer);
int map_kill_blocki(int i, int layer);
int map_load(const char *map);
void map_render();
void map_follow_me(int id);

#endif

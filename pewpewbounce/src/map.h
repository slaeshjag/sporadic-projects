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
int map_load(const char *map);
void map_render();

#endif
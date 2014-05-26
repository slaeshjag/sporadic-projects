#ifndef __MAP_H__
#define	__MAP_H__

#include <darnit/darnit.h>
#include "object.h"

struct map_s {
	const char			*current_map;
	DARNIT_MAP			*map;
	struct object_s			object;

	struct {
		int32_t			cam_x;
		int32_t			cam_y;
		int			follow;
	} cam;
};

void map_init();
void map_reset();
void map_load(const char *map, int spawn_objects);
void map_draw();

#endif

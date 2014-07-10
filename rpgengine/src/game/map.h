#ifndef __MAP_H__
#define	__MAP_H__

#include <darnit/darnit.h>
#include <stdint.h>
#include "object.h"

#define	MAP_CAMERA_DESPAWN_BORDER	128

struct map_s {
	const char			*current_map;
	
	struct {
		char			*map;
		int			id;
		int			x;
		int			y;
		int			layer;
	} teleport;

	DARNIT_MAP			*map;
	struct object_s			object;

	struct {
		int32_t			cam_x;
		int32_t			cam_y;
		int			follow;
		int			player;
	} cam;
};

void map_init();
void map_reset();
void map_load(const char *map, int spawn_objects);
void map_draw();
void map_logic();

#endif

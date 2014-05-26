#include <darnit/darnit.h>

#include "world.h"
#include "map.h"

#define	MAX(x, y)	((x) > (y) ? (x) : (y))

void map_init() {
	world.map.map = NULL;
	world.map.current_map = NULL;
	world.map.object.not_spawned = NULL;

	object_init();

	return;
}


void map_reset() {
	world.map.map = d_map_unload(world.map.map);
	world.map.current_map = NULL;

	object_reset();

	return;
}


void map_load(const char *map, int spawn_objects) {
	/* TODO: Look up real map file name */
	world.map.map = d_map_unload(world.map.map);
	world.map.current_map = map;
	world.map.map = d_map_load(map);
	if (!world.map.map) {
		fprintf(stderr, "Unable to open map %s\n", map);
		return;
	}

	if (spawn_objects)
		object_refresh();

	return;
}


void map_camera_move(int32_t center_x, int32_t center_y) {
	world.map.cam.cam_x = center_x - d_platform_get().screen_w / 2;
	world.map.cam.cam_y = center_y - d_platform_get().screen_h / 2;
	return;
}


static void map_camera_loop() {
	if (world.map.cam.follow > 0) {
		/* TODO: calculate coordinates from object position */
	}

	return;
}


void map_draw() {
	int i;

	map_camera_loop();
	d_map_camera_move(world.map.map, world.map.cam.cam_x, world.map.cam.cam_y);

	/* There shouldn't be any objects on a layer above the map anyway */
	for (i = 0; i < (int) world.map.map->layers; i++) {
		d_tilemap_draw(world.map.map->layer[i].tilemap);
	}

	return;
}

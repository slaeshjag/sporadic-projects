#include <darnit/darnit.h>

#include "world.h"
#include "map.h"

#define	MAX(x, y)	((x) > (y) ? (x) : (y))
#define	MAP_W_P		(world.map.map->layer->tile_w * world.map.map->layer->tilemap->w)
#define	MAP_H_P		(world.map.map->layer->tile_h * world.map.map->layer->tilemap->h)

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
	int cam_x, cam_y;

	/* TODO: Look up real map file name */
	world.map.map = d_map_unload(world.map.map);
	world.map.current_map = map;
	world.map.map = d_map_load(map);
	world.map.cam.cam_x = world.map.cam.cam_y = 0;
	world.map.cam.follow = world.map.cam.player = -1;
	
	if (!world.map.map) {
		fprintf(stderr, "Unable to open map %s\n", map);
		return;
	}

	if (spawn_objects)
		object_refresh();
	
	if (d_map_prop(world.map.map->prop, "cam_origin_x") && d_map_prop(world.map.map->prop, "cam_origin_y")) {
		cam_x = atoi(d_map_prop(world.map.map->prop, "cam_origin_x")) * world.map.map->layer->tile_w, cam_y = atoi(d_map_prop(world.map.map->prop, "cam_origin_y")) * world.map.map->layer->tile_h;
	} else
		cam_x = 0, cam_y = 0;
	
	fprintf(stderr, "Using map camera coordinates %i %i\n", cam_x, cam_y);
	world.map.cam.cam_x = cam_x, world.map.cam.cam_y = cam_y;
		

	return;
}


void map_camera_move(int32_t center_x, int32_t center_y) {
	world.map.cam.cam_x = center_x - d_platform_get().screen_w / 2;
	world.map.cam.cam_y = center_y - d_platform_get().screen_h / 2;
	return;
}


static void map_camera_loop() {
	int m;

	if (world.map.cam.follow >= 0) {
		world.map.cam.cam_x = (world.map.object.entry[world.map.cam.follow].x >> 8) - d_platform_get().screen_w / 2 + d_sprite_width(world.map.object.entry[world.map.cam.follow].sprite) / 2;
		world.map.cam.cam_y = (world.map.object.entry[world.map.cam.follow].y >> 8) - d_platform_get().screen_h / 2 + d_sprite_height(world.map.object.entry[world.map.cam.follow].sprite) / 2;
		if (world.map.cam.cam_x < 0)
			world.map.cam.cam_x = 0;
		if (world.map.cam.cam_y < 0)
			world.map.cam.cam_y = 0;
		if (world.map.cam.cam_x > (signed) MAP_W_P - (signed) d_platform_get().screen_w)
			world.map.cam.cam_x = MAP_W_P - d_platform_get().screen_w;
		if (world.map.cam.cam_y > (signed) MAP_H_P - (signed) d_platform_get().screen_h)
			world.map.cam.cam_y = MAP_H_P - d_platform_get().screen_h;
	}

	if (MAP_W_P < d_platform_get().screen_w)
		world.map.cam.cam_x = -((signed) (d_platform_get().screen_w - MAP_W_P)) / 2;
	if (MAP_H_P < d_platform_get().screen_h)
		world.map.cam.cam_y = -((signed) (d_platform_get().screen_h - MAP_H_P)) / 2;

	m = d_bbox_test(world.map.object.not_spawned, world.map.cam.cam_x, world.map.cam.cam_y, d_platform_get().screen_w, d_platform_get().screen_h, world.map.object.buff1, OBJECT_MAX);
	m--;
	for (; m >= 0; m--)
		object_spawn(world.map.object.buff1[m]);

	return;
}


void map_logic() {
	map_camera_loop();
	d_map_camera_move(world.map.map, world.map.cam.cam_x, world.map.cam.cam_y);
	object_loop();
}


void map_draw() {
	int i;

	/* There shouldn't be any objects on a layer above the map anyway */
	for (i = 0; i < (int) world.map.map->layers; i++) {
		d_tilemap_draw(world.map.map->layer[i].tilemap);
		d_render_blend_enable();
		object_render_layer(i);
		d_render_blend_disable();
	}

	return;
}

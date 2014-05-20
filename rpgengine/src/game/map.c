#include <darnit/darnit.h>

#include "world.h"
#include "map.h"


void map_init() {
	world.map.map = NULL;
	world.map.current_map = NULL;

	return;
}


void map_reset() {
	world.map.map = d_map_unload(world.map.map);
	world.map.current_map = NULL;

	return;
}


void map_load(const char *map) {
	/* TODO: Look up real map file name */
	world.map.map = d_map_unload(world.map.map);
	world.map.current_map = map;
	world.map.map = d_map_load(map);
	if (!world.map.map)
		fprintf(stderr, "Unable to open map %s\n", map);

	return;
}


void map_draw() {
	d_tilemap_draw(world.map.map->layer[0].tilemap);

	return;
}

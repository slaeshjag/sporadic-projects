#include <darnit/darnit.h>
#include "world.h"
#include "map.h"

int main(int argc, char **argv) {
	/* We're aiming for Pyra, not pandora */
	d_init_custom("RPGtest", 1280, 720, 0, "rpgtest", NULL);
	world_init();

	//map_load("res/testroom.ldmz", 1);
	map_load("res/testmap.ldmz", 1);

	for (;;) {
		d_render_begin();
		map_draw();
		d_render_end();
		d_loop();
	}

	return 0;
}

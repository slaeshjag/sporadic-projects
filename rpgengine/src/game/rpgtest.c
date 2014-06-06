#include <darnit/darnit.h>
#include "world.h"
#include "map.h"


void state_handle() {
	if (world.new_state != world.state) {
		if (d_render_fade_status() < 2)
			d_render_fade_in(RPGTEST_FADE_TIME, 0, 0, 0);
		else {
			/* Run destructors */
			switch (world.state) {
				case STATE_MENU_E:
					break;
				default:
					break;
			}

			/* Run construction */
			switch (world.new_state) {
				default:
					break;
			}

			world.state = world.new_state;
			d_render_fade_out(RPGTEST_FADE_TIME);
		}
	}

	switch (world.state) {
		case STATE_MENU_E:
			world.new_state = STATE_OVERWORLD_E;
			break;
		case STATE_OVERWORLD_E:
			/* NOTE TO SELF: Player control will be frozen by broadcasting MSG_SILE */
			map_logic();
			d_render_begin();
			map_draw();
			d_render_end();
			break;
		default:
			break;
	}

	d_render_offset(0, 0);
	
	return;
}


int main(int argc, char **argv) {
	/* We're aiming for Pyra, not pandora */
	d_init_custom("RPGtest", 1280, 720, 0, "rpgtest", NULL);
	world_init();
	world.state = STATE_DUMMY_E;
	world.new_state = STATE_MENU_E;

	//map_load("res/testroom.ldmz", 1);
	map_load("res/testmap.ldmz", 1);

	for (;;) {
		#if 0
		map_logic();
		d_render_begin();
		map_draw();
		d_render_end();
		#else
		state_handle();
		#endif
		d_loop();
	}

	return 0;
}

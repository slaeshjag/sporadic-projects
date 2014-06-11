#include <darnit/darnit.h>
#include "world.h"
#include "map.h"
#include "object.h"
#include "textbox.h"


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
				case STATE_TELEPORTING_E:
					map_load(world.map.teleport.map, 1);
					free(world.map.teleport.map);
					world.map.teleport.map = NULL;
					world.map.object.entry[world.map.teleport.id].x = (world.map.teleport.x * world.map.map->layer->tile_w) << 8;
					world.map.object.entry[world.map.teleport.id].y = (world.map.teleport.y * world.map.map->layer->tile_h) << 8;
					world.map.object.entry[world.map.teleport.id].l = world.map.teleport.layer;
					object_silence_all(0);
					world.new_state = STATE_OVERWORLD_E;
					break;
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
			textbox_loop();
			d_render_begin();
			map_draw();
			d_render_offset(0, 0);
			d_render_blend_enable();
			textbox_draw();
			d_render_blend_disable();
			d_render_end();
			break;
		default:
			break;
	}
	
	d_render_offset(0, 0);

	
	return;
}


int main(int argc, char **argv) {
	struct menu_s *m;

	/* We're aiming for Pyra, not pandora */
	d_init_custom("RPGtest", 1280, 720, 0, "rpgtest", NULL);
	//d_init_custom("RPGtest", 800, 480, 0, "rpgtest", NULL);
	world_init();
	world.state = STATE_DUMMY_E;
	world.new_state = STATE_MENU_E;

	m = menu_new_container(20, 5);
	menu_new_widget_list(m, 0, 0, 640, 5, "Arne\nGösta\nDoris\nBerit\nIngvar\nRagnar", NULL, world.config.font);

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
		d_render_begin();
		d_render_blend_enable();
		menu_draw(m);
		d_render_blend_disable();
		d_render_end();
		d_render_offset(0, 0);

		d_loop();
	}

	return 0;
}

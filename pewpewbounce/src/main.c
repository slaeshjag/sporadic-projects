#include <darnit/darnit.h>


int main(int argc, char **argv) {
	d_init_custom("PewPewBounce", 800, 600, 0, "pewpewbounce", NULL);
	object_init();
	map_load("res/test.ldmz");

	for (;;) {
		object_loop();
		d_render_begin();
		map_render();
		d_render_end();
		d_loop();
	}

	d_quit();
	return 0;
}

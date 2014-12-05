#include <darnit/darnit.h>
#include <copypasta/copypasta.h>
#include "object.h"

void ai_test_init(int id) {
	return;
}

void ai_test_kill(int id) {
	return;
}


void ai_test_loop(int id) {
	return;
}


void ai_test_collide(int mid, int fid, int xvelc, int yvelc, int xdir, int ydir) {
	return;
}


void ai_test_collide_map(int id, int xdir, int ydir) {
	return;
}


void ai_player_loop(int id) {
	struct ObjectEntry *oe;

	if (!(oe = c_dynalloc_get(obj.obj, id)))
		return;
	if (d_keys_get().up)
		oe->vel_y = -128;
	else if (d_keys_get().down)
		oe->vel_y = 128;
	return;
}

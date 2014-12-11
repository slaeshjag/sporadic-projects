#include <copypasta/copypasta.h>
#include "../object.h"
#include "../map.h"

static int get_next_xdir(int xdir, int ydir) {
	return (xdir) ? 0 : ydir * -1;
}

static int get_next_ydir(int xdir, int ydir) {
	return (ydir) ? 0 : xdir;
}

void ai_crawler_init(int id) {
	struct ObjectEntry *oe;
	
	oe = c_dynalloc_get(obj.obj, id);
	oe->vel_y = AI_PLAYER_VEL;
}


void ai_crawler_kill(int id) {
}


void ai_crawler_loop(int id) {

}


void ai_crawler_collide(int mid, int fid, int xvelc, int yvelc, int xdir, int ydir) {
	struct ObjectEntry *oe;
	int xd, yd;

	oe = c_dynalloc_get(obj.obj, mid);
	xd = get_next_xdir(oe->vel_x, oe->vel_y);
	yd = get_next_ydir(oe->vel_x, oe->vel_y);
	oe->vel_x = xd, oe->vel_y = yd;

	if (fid == map_s.camera.follow)
		object_damage(fid, AI_CRAWLER_DAMAGE);

	/* TODO: Inflict damage if appropriate */
}


void ai_crawler_collide_map(int id, int xdir, int ydir, int maptile) {
	struct ObjectEntry *oe;
	int xd, yd;

	oe = c_dynalloc_get(obj.obj, id);
	xd = get_next_xdir(oe->vel_x, oe->vel_y);
	yd = get_next_ydir(oe->vel_x, oe->vel_y);
	oe->vel_x = xd, oe->vel_y = yd;
	return;
}

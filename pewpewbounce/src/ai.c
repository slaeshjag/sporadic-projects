#include <darnit/darnit.h>
#include <copypasta/copypasta.h>
#include "object.h"
#include "map.h"

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


void ai_test_collide_map(int id, int xdir, int ydir, int maptile) {
	return;
}


void ai_player_init(int id) {
	struct AiPlayerState *ai;
	struct ObjectEntry *oe;

	ai = malloc(sizeof(*ai));
	ai->dir = 1;
	ai->bounce = 0;
	oe = c_dynalloc_get(obj.obj, id);
	oe->data = ai;
	map_follow_me(id);
	oe->solid = 1;

	return;
}


void ai_player_kill(int id) {
	struct ObjectEntry *oe;
	
	fprintf(stderr, "KILL\n");
	oe = c_dynalloc_get(obj.obj, id);
	free(oe->data);
	oe->data = NULL;
	return;
}


void ai_player_loop(int id) {
	struct ObjectEntry *oe;
	struct AiPlayerState *ai;
	int vel;

	if (!(oe = c_dynalloc_get(obj.obj, id)))
		return;
	ai = oe->data;

	vel = (d_keys_get().l?AI_PLAYER_VEL_BOOST:AI_PLAYER_VEL);
	oe->vel_y = ai->dir * vel;
	
	if (d_time_get() - ai->bounce >= AI_PLAYER_BOUNCE_TIME) {
		if (d_keys_get().left)
			oe->vel_x = -1 * vel;
		else if (d_keys_get().right)
			oe->vel_x = vel;
		else
			oe->vel_x = 0;
	}

	return;
}


void ai_player_collide(int mid, int fid, int xvelc, int yvelc, int xdir, int ydir) {
	struct ObjectEntry *oe;
	struct AiPlayerState *ai;
	
	if (!(oe = c_dynalloc_get(obj.obj, mid)))
		return;
	ai = oe->data;
	if (ydir)
		ai->dir *= -1;
	if (xdir) {
		ai->bounce = d_time_get();
		oe->vel_x *= -1;
	}
}


void ai_player_collide_map(int id, int xdir, int ydir, int maptile) {
	struct ObjectEntry *oe;
	struct AiPlayerState *ai;
	
	if (!(oe = c_dynalloc_get(obj.obj, id)))
		return;
	ai = oe->data;
	if (ydir)
		ai->dir *= -1;
	if (xdir) {
		ai->bounce = d_time_get();
		oe->vel_x *= -1;
	}

	if (map_get_blocki(maptile, oe->layer) & 0x100000)
		map_kill_blocki(maptile, oe->layer);
	return;
}

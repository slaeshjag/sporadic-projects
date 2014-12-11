#include <limits.h>
#include "object.h"
#include "map.h"
#include "ai.h"

struct Object obj;

struct ObjectFunction object_func_define[] = {
	{ "test", ai_test_init, ai_test_kill, ai_test_loop, ai_test_collide, ai_test_collide_map },
	{ "player", ai_player_init, ai_player_kill, ai_player_loop, ai_player_collide, ai_player_collide_map },
	{ "crawler", ai_crawler_init, ai_crawler_kill, ai_crawler_loop, ai_crawler_collide, ai_crawler_collide_map },
	{ NULL, ai_test_init, ai_test_kill, ai_test_loop, ai_test_collide, ai_test_collide_map },
};



struct ObjectFunction object_func_lookup(const char *str) {
	int i;

	for (i = 0; object_func_define[i].name; i++)
		if (!strcmp(str, object_func_define[i].name))
			return object_func_define[i];
	fprintf(stderr, "Warning: Unable to find AI %s\n", str);
	return object_func_define[i];
}


int object_init() {
	int i;

	obj.bbox = d_bbox_new(OBJECT_CAP);
	d_bbox_set_indexkey(obj.bbox);

	for (i = 0; i < OBJECT_CAP; i++)
		d_bbox_add(obj.bbox, INT_MAX - 1, INT_MAX - 1, 1, 1);
	obj.obj = c_dynalloc_init(OBJECT_CAP, sizeof(struct ObjectEntry));

	return obj.obj ? 1 : 0;
}


void object_update_pos(struct ObjectEntry *oe) {
	int x, y, w, h;

	d_sprite_hitbox(oe->sprite, &x, &y, &w, &h);
	d_bbox_resize(obj.bbox, oe->id, w, h);
	d_bbox_move(obj.bbox, oe->id, oe->pos_x / 1000 + x, oe->pos_y / 1000 + y);
	d_sprite_move(oe->sprite, oe->pos_x / 1000 + x, oe->pos_y / 1000 + y);
	d_sprite_rotate(oe->sprite, oe->rotation);

	return;
}


static void object_center_coord(DARNIT_SPRITE *sprite, int *x, int *y) {
	int sx, sy, sw, sh;

	d_sprite_hitbox(sprite, &sx, &sy, &sw, &sh);
	*x += sx;
	*y += sy;
	*x -= sw / 2;
	*y += sy / 2;
	
	return;
}


int object_spawn(const char *sprite, const char *ai, int x, int y, int l, DARNIT_MAP_OBJECT *mobj) {
	int id;
	struct ObjectEntry *oe;

	id = c_dynalloc_alloc(obj.obj);
	if (id >= OBJECT_CAP) {
		/* Overflowing is bad, mmkay? */
		c_dynalloc_release(obj.obj, id);
		return -1;
	}
	oe = c_dynalloc_get(obj.obj, id);
	oe->sprite = d_sprite_load(sprite, 0, DARNIT_PFORMAT_RGB5A1);
	object_center_coord(oe->sprite, &x, &y);
	oe->mobj = mobj;
	oe->pos_x = x * 1000, oe->pos_y = y * 1000, oe->layer = l;
	oe->vel_x = oe->vel_y = 0, oe->rotation = 0;
	oe->solid = 0;
	oe->invincible = 0;
	oe->id = id;
	oe->health = oe->health_max = 4;

	oe->func = object_func_lookup(ai);
	oe->func.init(id);

	object_update_pos(oe);
	
	return id;
}


void object_kill(int id) {
	struct ObjectEntry *oe;

	if (!(oe = c_dynalloc_get(obj.obj, id)))
		return;
	d_bbox_move(obj.bbox, id, INT_MAX - 1, INT_MAX - 1);
	oe->func.kill(id);
	d_sprite_free(oe->sprite);
	c_dynalloc_release(obj.obj, id);
}


void object_nuke() {
	int i;

	for (i = 0; i < 0; i++) {
		object_kill(i);
	}

	return;
}


static int get_movement_delta(int d) {
	int a;

	a = d < 0 ? -1 : 1;
	if (a * d >= 1000)
		d = 999;
	else
		d = d * a;
	return d * a;
}


static int object_test_collision(struct ObjectEntry *oe, int dx, int dy) {
	int x, y, w, h, i, coll = 0, xvelc, yvelc, id1, id2, tile;
	unsigned int list[32], ll;
	struct ObjectEntry *coe;

	if ((oe->pos_x + dx) / 1000 == oe->pos_x / 1000 && (oe->pos_y + dy) / 1000 == oe->pos_y / 1000)
		return 0;

	d_sprite_hitbox(oe->sprite, &x, &y, &w, &h);
	x += (oe->pos_x + dx) / 1000;
	y += (oe->pos_y + dy) / 1000;
	
	ll = d_bbox_test(obj.bbox, x, y, w, h, list, 32);
	for (i = 0; i < ll; i++) {
		if (list[i] == oe->id)
			continue;
		if (!(coe = c_dynalloc_get(obj.obj, list[i])))
			continue;

		id1 = oe->id, id2 = coe->id;
		xvelc = oe->vel_x - coe->vel_x, yvelc = oe->vel_y - coe->vel_y;
		coe->func.collide(id2, id1, xvelc * -1, yvelc * -1, dx * -1, dy * -1);
		if (!(oe = c_dynalloc_get(obj.obj, id1))) {
			fprintf(stderr, "Object despawned\n");
			return 1;
		}

		oe->func.collide(id1, id2, xvelc, yvelc, dx, dy);
		if (!(oe = c_dynalloc_get(obj.obj, id1))) {
			fprintf(stderr, "Object despawned\n");
			return 1;
		}

		if (!coe->solid)
			continue;
		coll = 1;
	}

	if ((tile = map_check_area(oe->pos_x / 1000 + (dx>0?w-1:0), oe->pos_y / 1000 + (dy>0?h-1:0), dx?(dx<0?-1:1):0, dy?(dy<0?-1:1):0, oe->layer, dy?w:0, dx?h:0))) {
		return (oe->func.collide_map(oe->id, dx, dy, tile - 1), 1);
	}

	return coll;
}


void object_loop() {
	int i, j, k, dts, cnt, dt, dx, dy;
	unsigned int list[OBJECT_CAP], hits;
	struct ObjectEntry *oe;

	dt = d_last_frame_time();
	hits = d_bbox_test(obj.bbox, map_s.camera.lim_lx, map_s.camera.lim_ly, map_s.camera.lim_bx, map_s.camera.lim_by, list, OBJECT_CAP);
	for (i = 0; i < hits; i++) {
		if (!(oe = c_dynalloc_get(obj.obj, list[i])))
			continue;
		
		/* Lets just start with a stupid kind of collision detection */
		/* It's easier to make a game if *something* works */
		dx = oe->vel_x * dt;
		dy = oe->vel_y * dt;
		for (k = dx; k; k -= get_movement_delta(k)) {
			dts = get_movement_delta(k);
			if (object_test_collision(oe, dts, 0))
				break;
			oe->pos_x += dts;
		}
		
		/* If collision, the object may not exist anymore */
		if (!(oe = c_dynalloc_get(obj.obj, list[i])))
			continue;
		
		for (k = dy; k; k -= get_movement_delta(k)) {
			dts = get_movement_delta(k);
			if (object_test_collision(oe, 0, dts)) {
				break;
			}
			oe->pos_y += dts;
		}
		
		object_update_pos(oe);

		oe->func.loop(list[i]);
		if (oe->health <= 0)
			object_kill(list[i]);
	}
}


void object_render(int layer) {
	unsigned int i, len, list[OBJECT_CAP];
	struct ObjectEntry *oe;

	len = d_bbox_test(obj.bbox, map_s.camera.lim_lx, map_s.camera.lim_ly, map_s.camera.lim_bx, map_s.camera.lim_by, list, OBJECT_CAP);

	for (i = 0; i < len; i++) {
		if (!(oe = c_dynalloc_get(obj.obj, list[i])))
			continue;
		if (oe->layer != layer)
			continue;
		d_sprite_draw(oe->sprite);
	}
	
	return;
}


void object_damage(int object, int amount) {
	struct ObjectEntry *oe;
	
	if (!(oe = c_dynalloc_get(obj.obj, object)))
		return;
	if (d_time_get() - oe->invincible <= OBJECT_INVINCIBLE_TIMER)
		return;
	oe->health -= amount;
	oe->invincible = d_time_get();
}

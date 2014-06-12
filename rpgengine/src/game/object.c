#include <darnit/darnit.h>
#include <string.h>
#include <limits.h>

#include "common/aicomm.h"
#include "common/common.h"
#include "aicomm_f.h"
#include "aicomm_handlers.h"
#include "world.h"
#include "object.h"

#define	MAP_LAYER(l)			(world.map.map->layer[(l)])
#define	TILE_W(l)			(MAP_LAYER(l).tile_w)
#define	TILE_H(l)			(MAP_LAYER(l).tile_h)
#define	OBJ_TO_TILE_COORD(x, y, l)	((x)/TILE_W(l) + (y)/TILE_H(l) * MAP_LAYER(l).tilemap->w)
#define	TILE(i, l)			(MAP_LAYER(l).tilemap->data[i])

void object_despawn(int entry);
int object_spawn(int map_id);

void object_init() {
	char ai_name[64];
	world.map.object.not_spawned = NULL;
	world.map.object.spawned = d_bbox_new(OBJECT_MAX);
	d_bbox_sortmode(world.map.object.spawned, DARNIT_BBOX_SORT_Y);
	d_bbox_set_indexkey(world.map.object.spawned);
	
	/* Load AI code library */
	sprintf(ai_name, "bin/%s.ai", d_platform_string());
	fprintf(stderr, "Loading %s\n", ai_name);
	world.map.object.ai_lib = d_dynlib_open(ai_name);

	return;
}


/* really_all=1 -> despawn even those marked as no despawn */
void object_despawn_all(int really_all) {
	int i;

	for (i = 0; i < OBJECT_MAX; i++) {
		if (really_all || (!world.map.object.entry[i].special_action.nodespawn && world.map.teleport.id != i))
			object_despawn(i);
		else
			world.map.object.entry[i].map_id = -1;
	}
	/* TODO: Implement */
	return;
}


void object_reset() {
	object_despawn_all(1);
	world.map.object.not_spawned = d_bbox_free(world.map.object.not_spawned);

	return;
}


void object_refresh() {
	int i;
	DARNIT_MAP *m = world.map.map;

	object_despawn_all(0);
	world.map.object.not_spawned = d_bbox_free(world.map.object.not_spawned);
	world.map.object.not_spawned = d_bbox_new(m->objects);
	d_bbox_set_indexkey(world.map.object.not_spawned);
	
	for (i = 0; i < (int) m->objects; i++) {
		d_bbox_add(world.map.object.not_spawned, m->object[i].x * m->layer->tile_w, m->object[i].y * m->layer->tile_h, 1, 1);
	}

	return;
}


int object_get_character_looked_at(int src) {
	int x, y, w, h, xt, yt, n, i;
	unsigned int t[2];

	if (src < 0 || src >= OBJECT_MAX)
		return -1;
	if (!world.map.object.entry[src].loop)
		return -1;

	d_sprite_hitbox(world.map.object.entry[src].sprite, &x, &y, &w, &h);
	x += (world.map.object.entry[src].x >> 8);
	y += (world.map.object.entry[src].y >> 8);
	x += (w >> 1);
	y += (h >> 1);

	switch (world.map.object.entry[src].dir) {
		case 0:	/* West */
			xt = x - (w >> 1) * 3;
			yt = y - (h >> 1);
			break;
		case 1:	/* North */
			xt = x - (w >> 1);
			yt = y - (h >> 1) * 3;
			break;
		case 2:	/* East */
			xt = x + (w >> 1);
			yt = y - (h >> 1);
			break;
		case 3:	/* South */
			xt = x - (w >> 1);
			yt = y + (h >> 1);
			break;
		case 4: /* North-west */
			xt = x - (w >> 1) * 3;
			yt = y - (h >> 1) * 3;
			break;
		case 5:	/* North-east */
			xt = x + (w >> 1);
			yt = y - (h >> 1) * 3;
			break;
		case 6:	/* South-east */
			xt = x + (w >> 1);
			yt = y + (h >> 1);
			break;
		case 7:	/* South-west */
			xt = x - (w >> 1) * 3;
			yt = y + (h >> 1);
			break;
		default:
			fprintf(stderr, "Invalid direction %i\n", world.map.object.entry[src].dir);
			return -1;
			break;
	}

	n = d_bbox_test(world.map.object.spawned, xt, yt, w, h, t, 9);
	if (!n)
		return -1;
	for (i = 0; i < 2; i++) {
		if (t[i] == (unsigned) src)
			continue;
		return t[i];
	}
	
	return -1;
}


int object_slot_get() {
	int i;
	
	for (i = 0; i < OBJECT_MAX; i++)
		if (!world.map.object.entry[i].loop)
			return i;
	return -1;
}


void object_update_sprite(int entry) {
	int x, y, w, h;
	struct character_entry *ce;

	ce = &world.map.object.entry[entry];

	d_sprite_hitbox(ce->sprite, &x, &y, &w, &h);
//	x *= -1;
//	y *= -1;
	x += (ce->x >> 8);
	y += (ce->y >> 8);

	d_sprite_direction_set(ce->sprite, ce->dir);
	(ce->special_action.animate ? d_sprite_animate_start : d_sprite_animate_stop)(ce->sprite);
	d_sprite_move(ce->sprite, (ce->x >> 8), (ce->y >> 8));
	d_bbox_move(world.map.object.spawned, ce->self, x, y);
	d_bbox_resize(world.map.object.spawned, ce->self, w, h);

	return;
}


void object_set_hitbox(int entry) {
	int x, y, w, h;
	struct character_entry *ce;

	if (!world.map.object.entry[entry].loop)
		return;

	ce = &world.map.object.entry[entry];
	d_sprite_hitbox(ce->sprite, &x, &y, &w, &h);
	x += (ce->x >> 8);
	y += (ce->y >> 8);
	d_bbox_move(world.map.object.spawned, entry, x, y);
	d_bbox_resize(world.map.object.spawned, entry, (unsigned) w, (unsigned) h);

	return;
}


static void object_signal_map_event(int entry, int t, int l) {
	struct aicomm_struct ac;

	ac.from = -1;
	ac.msg = AICOMM_MSG_MAPE;
	ac.self = entry;
	if (TILE(t, l) & MAP_FLAG_EVENT) {
		ac.arg[0] = t;
		ac.arg[1] = TILE(t, l);
		object_message_loop(ac);
	}

	return;
}


static int object_test_map_x(int x, int x2, int y, int l, int h, int dir, int entry) {
	int tile_h, dy, t1, t2, f = 0;
	int collide = 0;

	if ((x2) / TILE_W(l) >= (unsigned) MAP_LAYER(l).tilemap->w)
		return 1;

	tile_h = world.map.map->layer[l].tile_h;
	for (dy = 0; dy < h; dy += tile_h) {
		test_tile:

		
		t1 = OBJ_TO_TILE_COORD(x, y + dy, l);
		t2 = OBJ_TO_TILE_COORD(x2, y + dy, l);
		if (t1 < 0 || t2 < 0) {
			collide = 1;
			continue;
		}
		if (t1 == t2)
			continue;
		
		if (TILE(t2, l) & dir)
			collide = 1;
		object_signal_map_event(entry, t2, l);
	}

	if (dy >= h && !f) {
		f = 1;
		dy = h - 1;
		goto test_tile;
	}

	return collide;
}


static int object_test_map_y(int x, int y, int y2, int l, int w, int dir, int entry) {
	int tile_w, dx, t1, t2, f = 0;
	int collide = 0;

	if ((y2) / TILE_H(l) >= (unsigned) MAP_LAYER(l).tilemap->h)
		return 1;
	tile_w = world.map.map->layer[l].tile_w;
	for (dx = 0; dx < w; dx += tile_w) {
		test_tile:
		

		t1 = OBJ_TO_TILE_COORD(x + dx, y, l);
		t2 = OBJ_TO_TILE_COORD(x + dx, y2, l);
		if (t1 < 0 || t2 < 0) {
			collide = 1;
			continue;
		}
		if (t1 == t2)
			continue;
		if (TILE(t2, l) & dir)
			collide = 1;
		object_signal_map_event(entry, t2, l);
	}
	
	if (dx >= w && !f) {
		f = 1;
		dx = w - 1;
		goto test_tile;
	}

	return collide;
}


int object_test_map(int entry, int dx, int dy) {
	int x, y, w, h, x2, y2, dir;
	struct character_entry *ce;
	
	if (d_keys_get().l)
		return 0;

	ce = &world.map.object.entry[entry];
	d_sprite_hitbox(ce->sprite, &x, &y, &w, &h);
	x2 = x;
	y2 = y;
	x2 += ((ce->x + dx) >> 8);
	y2 += ((ce->y + dy) >> 8);
	x += (ce->x >> 8);
	y += (ce->y >> 8);
	if (x < 0 || x2 < 0 || y < 0 || y2 < 0)
		return 1;

	/* TODO: FIXA */
	if (!dx) {
		y += (dy > 0) ? h - 1 : 0;
		y2 += (dy > 0) ? h - 1 : 0;
		dir = (((dy < 0) ? 0x1 : 0x4) << 16);
		return object_test_map_y(x, y, y2, ce->l, w, dir, entry);
	} else if (!dy) {
		x += (dx > 0) ? w - 1 : 0;
		x2 += (dx > 0) ? w - 1 : 0;
		dir = (((dx < 0) ? 0x8 : 0x2) << 16);
		return object_test_map_x(x, x2, y, ce->l, h, dir, entry);
	}
	return 0;
}


int object_test_collision(int entry, int dx, int dy) {
	struct aicomm_struct ac;
	struct character_entry *ce;
	int x, y, w, h, e, i, s, n;

	if (d_keys_get().l)
		return 0;

	ce = &world.map.object.entry[entry];
	d_sprite_hitbox(ce->sprite, &x, &y, &w, &h);
	x += ((ce->x + dx) >> 8);
	y += ((ce->y + dy) >> 8);
	n = d_bbox_test(world.map.object.spawned, x, y, w, h, (unsigned *) world.map.object.buff1, 
		OBJECT_MAX);
	
	ac.msg = AICOMM_MSG_COLL;
	ac.from = -1;
	
	for (i = s = 0; i < n; i++) {
		e = world.map.object.buff1[i];
		if (world.map.object.entry[e].l != world.map.object.entry[ce->self].l) {
			continue;
		}
		if (e == ce->self)
			continue;
		ac.arg[0] = ce->self;
		ac.self = e;
		object_message_loop(ac);
		ac.arg[0] = e;
		ac.self = ce->self;
		object_message_loop(ac);
		if (world.map.object.entry[entry].special_action.solid
		    && world.map.object.entry[e].special_action.solid)
			s = 1;
		
	}

	return s;
}


void object_handle_movement(int entry) {
	int dx, dy;
	struct character_entry *e;

	e = &world.map.object.entry[entry];
	dx = e->dx;
	dy = e->dy;

	if (!dx && !dy)
		return;

	dx *= d_last_frame_time();
	dy *= d_last_frame_time();
	dx /= 1000;
	dy /= 1000;

	while (dx) {
		if (!object_test_collision(entry, dx, 0) && !object_test_map(entry, dx, 0)) {
			world.map.object.entry[entry].x += dx;
			break;
		}
		if (!(dx / 256))
			break;
		dx += (dx < 0) ? 256 : -256;
	}

	while (dy) {
		if (!object_test_collision(entry, 0, dy) && !object_test_map(entry, 0, dy)) {
			world.map.object.entry[entry].y += dy;
			break;
		}
		if (!(dy / 256))
			break;
		dy += (dy < 0) ? 256 : -256;
	}

	object_update_sprite(entry);

	e->dx = 0, e->dy = 0;
	
	return;
}


void object_message_loop(struct aicomm_struct ac) {
	for (;;) {
		ac.ce = world.map.object.entry;
		if (ac.self < 0)
			return;
		else if (ac.self >= OBJECT_MAX || !world.map.object.entry[ac.self].loop) {
			ac.msg = AICOMM_MSG_NOAI;
			if (ac.from < 0 || ac.from >= OBJECT_MAX)
				return;
			if (!world.map.object.entry[ac.from].loop)
				return;
			ac.self = ac.from;
			ac.from = -1;
			ac = world.map.object.entry[ac.self].loop(ac);
		} else
			ac = world.map.object.entry[ac.self].loop(ac);

		switch (ac.msg) {
			case AICOMM_MSG_DONE:
				return;
			case AICOMM_MSG_INIT:
			case AICOMM_MSG_NOAI:
			case AICOMM_MSG_DESTROY:
			case AICOMM_MSG_SILE:
			case AICOMM_MSG_BOXR:
			case AICOMM_MSG_LOOP:
				/* Invalid return messages */
				fprintf(stderr, "WARNING: char %i returned invalid message %i\n",
					ac.from, ac.msg);
				return;
			default:
				if (ac.msg >= AICOMM_MSG_END)
					ac = aicomm_f_invm(ac);
				else if (!aihandle[ac.msg])
					ac = aicomm_f_invm(ac);
				else
					ac = aihandle[ac.msg](ac);
				break;
		}

	}

	return;
}


struct aicomm_struct object_message_next(struct aicomm_struct ac) {
	ac.self = ac.from;
	ac.from = -1;
	ac.msg = AICOMM_MSG_NEXT;

	return ac;
}


void object_loop() {
	int i;
	struct aicomm_struct ac;

	for (i = 0; i < OBJECT_MAX; i++) {
		if (!world.map.object.entry[i].loop)
			continue;
		ac.msg = AICOMM_MSG_LOOP;
		ac.self = i;
		ac.from = -1;
		object_message_loop(ac);
		object_handle_movement(i);
	}

	return;
}


void object_render_layer(int l) {
	int i, m;

	m = d_bbox_test(world.map.object.spawned, world.map.cam.cam_x, world.map.cam.cam_y, d_platform_get().screen_w, d_platform_get().screen_h, world.map.object.buff1, OBJECT_MAX);
	d_render_offset(world.map.cam.cam_x, world.map.cam.cam_y);

	for (i = 0; i < m; i++) {
		if (!world.map.object.entry[world.map.object.buff1[i]].loop)
			continue;
		if (world.map.object.entry[world.map.object.buff1[i]].l != l)
			continue;
		d_sprite_draw(world.map.object.entry[world.map.object.buff1[i]].sprite);
	}

	return;
}


void object_load_sprite(int obj, const char *path) {
	world.map.object.entry[obj].sprite = d_sprite_load(path, 0, DARNIT_PFORMAT_RGB5A1);
	d_sprite_activate(world.map.object.entry[obj].sprite, 0);
	object_update_sprite(obj);
	object_set_hitbox(obj);

	return;
}

void object_despawn(int entry) {
	struct character_entry *ce;
	struct aicomm_struct ac;

	if (entry < 0 || entry >= OBJECT_MAX)
		return;
	if (!world.map.object.entry[entry].loop)
		return;

	ce = &world.map.object.entry[entry];
	
	ac.msg = AICOMM_MSG_DESTROY;
	ac.from = -1;
	ac.self = entry;
	object_message_loop(ac);

	if (ce->map_id >= 0) {
		d_bbox_move(world.map.object.not_spawned, ce->map_id, ce->x >> 8, ce->y >> 8);
		d_bbox_resize(world.map.object.not_spawned, ce->map_id, d_sprite_width(ce->sprite), d_sprite_height(ce->sprite));
	}
	
	d_bbox_delete(world.map.object.spawned, entry);
	d_sprite_free(ce->sprite);
	memset(&world.map.object.entry[entry], 0, sizeof(world.map.object.entry[entry]));

	if (world.map.cam.player == entry)
		world.map.cam.player = -1;

	return;
}


int object_spawn_direct(int x, int y, int l, const char *ai, const char *sprite, int map_id) {
	int slot, w, h;
	struct character_entry *ce;
	struct aicomm_struct ac;

	if ((slot = d_bbox_add(world.map.object.spawned, 0, 0, 1, 1)) < 0)
		return -1;
	
	ce = &world.map.object.entry[slot];
	ce->sprite = d_sprite_load(sprite, 0, DARNIT_PFORMAT_RGB5A1);
	ce->map_id = map_id;
	ce->x = x << 8;
	ce->y = y << 8;
	ce->l = l;
	
	d_sprite_hitbox(ce->sprite, &x, &y, &w, &h);
	d_bbox_move(world.map.object.spawned, slot, (ce->x >> 8) + x, (ce->y >> 8) + y);
	d_bbox_resize(world.map.object.spawned, slot, w, h);
	
	ce->dx = ce->dy = 0;
	ce->self = slot;
	ce->dir = 0;
	ce->stat = NULL;
	ce->stats = 0;
	ce->state = NULL;
	*((unsigned int *) (&ce->special_action)) = 0;

	d_sprite_activate(ce->sprite, 0);
	object_update_sprite(slot);
	
	strcpy(ce->ai, ai);
	
	ce->loop = d_dynlib_get(world.map.object.ai_lib, ce->ai);
	ac.msg = AICOMM_MSG_INIT;
	ac.from = -1;
	ac.self = ce->self;
	object_message_loop(ac);
	
	object_set_hitbox(slot);

	return slot;
}

int object_spawn(int map_id) {
	int slot, x, y, l;
	const char *ai, *sprite;

	x = (world.map.map->object[map_id].x * world.map.map->layer->tile_w);
	y = (world.map.map->object[map_id].y * world.map.map->layer->tile_h);
	l = world.map.map->object[map_id].l;
	sprite = d_map_prop(world.map.map->object[map_id].ref, "sprite");
	ai = d_map_prop(world.map.map->object[map_id].ref, "ai");

	if ((slot = object_spawn_direct(x, y, l, ai, sprite, map_id)) < 0)
		return -1;
		
	d_bbox_move(world.map.object.not_spawned, map_id, INT_MAX/2, INT_MAX/2);
	d_bbox_resize(world.map.object.not_spawned, map_id, 1, 1);

	return slot;
}


void object_tell_all(struct aicomm_struct ac) {
	int i;

	for (i = 0; i < OBJECT_MAX; i++)
		if (world.map.object.entry[i].loop)
			ac.self = i, object_message_loop(ac);
	return;
}


void object_silence_all(int silence) {
	struct aicomm_struct ac;

	ac.msg = AICOMM_MSG_SILE;
	ac.arg[0] = silence;
	ac.from = -1;
	object_tell_all(ac);
	
	return;
}

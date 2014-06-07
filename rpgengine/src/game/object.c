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
#define	OBJ_TO_TILE_COORD(x, y, l)	((x)/TILE_W(l) + y/TILE_H(l) * MAP_LAYER(l).tilemap->w)
#define	TILE(i, l)			(MAP_LAYER(l).tilemap->data[i])

void object_despawn(int entry);
int object_spawn(int map_id);
void object_message_loop(struct aicomm_struct ac);

void object_init() {
	char ai_name[64];
	world.map.object.not_spawned = NULL;
	world.map.object.spawned = d_bbox_new(OBJECT_MAX);
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
	if (world.map.object.entry[src].loop)
		return -1;
	d_sprite_hitbox(world.map.object.entry[src].sprite, &x, &y, &w, &h);
	x += (world.map.object.entry[src].x >> 8);
	y += (world.map.object.entry[src].y >> 8);

	switch (world.map.object.entry[src].dir) {
		case 0:	/* West */
			xt = x - (w >> 1);
			yt = y + (h >> 1);
			break;
		case 1:	/* North */
			xt = x + (w >> 1);
			yt = y - (h >> 1);
			break;
		case 2:	/* East */
			xt = x + w;
			yt = y + (h >> 1);
			break;
		case 3:	/* South */
			xt = x + (w >> 1);
			yt = y + h;
			break;
		case 4: /* North-west */
			xt = x - (w >> 1);
			yt = y - (h >> 1);
			break;
		case 5:	/* North-east */
			xt = x + w;
			yt = y - (h >> 1);
			break;
		case 6:	/* South-east */
			xt = x + w;
			yt = y + h;
			break;
		case 7:	/* South-west */
			xt = x - (w >> 1);
			yt = y + h;
			break;
		default:
			return -1;
			break;
	}

	n = d_bbox_test(world.map.object.spawned, xt, yt, (w >> 1), (h >> 1), t, 9);
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
	x *= -1;
	y *= -1;
	x += (ce->x >> 8);
	y += (ce->y >> 8);

	d_sprite_direction_set(ce->sprite, ce->dir);
	(ce->special_action.animate ? d_sprite_animate_start : d_sprite_animate_stop)(ce->sprite);
	d_sprite_move(ce->sprite, x, y);
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


int object_test_map(int entry, int dx, int dy) {
	int x, y, w, h, x2, y2, t1, t2, t3, t4, dir, d1, d2;
	struct character_entry *ce;
	struct aicomm_struct ac;
	
	if (d_keys_get().l)
		return 0;

	ce = &world.map.object.entry[entry];
	d_sprite_hitbox(ce->sprite, NULL, NULL, &w, &h);
	x2 = ((ce->x + dx) >> 8);
	y2 = ((ce->y + dy) >> 8);
	x = (ce->x >> 8);
	y = (ce->y >> 8);
	if (x < 0 || x2 < 0 || y < 0 || y2 < 0)
		return 1;
	
	if (!dx) {
		x2 += (w - 1);
		y += (dy > 0) ? h - 1 : 0;
		y2 += (dy > 0) ? h - 1 : 0;
		t1 = OBJ_TO_TILE_COORD(x, y, ce->l);
		t2 = OBJ_TO_TILE_COORD(x, y2, ce->l);
		t3 = OBJ_TO_TILE_COORD(x2, y, ce->l);
		t4 = OBJ_TO_TILE_COORD(x2, y2, ce->l);
		dir = (((dy < 0) ? 0x1 : 0x4) << 16);
	} else if (!dy) {
		y2 += (h - 1);
		x += (dx > 0) ? w - 1 : 0;
		x2 += (dx > 0) ? w - 1 : 0;
		t1 = OBJ_TO_TILE_COORD(x, y, ce->l);
		t2 = OBJ_TO_TILE_COORD(x2, y, ce->l);
		t3 = OBJ_TO_TILE_COORD(x, y2, ce->l);
		t4 = OBJ_TO_TILE_COORD(x2, y2, ce->l);
		dir = (((dx < 0) ? 0x8 : 0x2) << 16);
	}
		
	if (t1 < 0 || t2 < 0 || t3 < 0 || t4 < 0)
		return 1;
	if (t1 == t2 && t3 == t4)
		return 0;

	d1 = TILE(t2, ce->l);
	d2 = TILE(t4, ce->l);

	ac.from = -1;
	ac.msg = AICOMM_MSG_MAPE;
	ac.self = entry;
	if (d1 & MAP_FLAG_EVENT) {
		ac.arg[0] = t2;
		ac.arg[1] = d1;
		object_message_loop(ac);
	}

	if (d2 & MAP_FLAG_EVENT) {
		ac.arg[0] = t4;
		ac.arg[1] = d2;
		object_message_loop(ac);
	}
	
	return ((d1 & dir) || (d2 & dir));
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
		if (world.map.object.entry[e].l != world.map.object.entry[ce->self].l)
			continue;
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
			ac = world.map.object.entry[ac.from].loop(ac);
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
	int i;

	d_render_offset(world.map.cam.cam_x, world.map.cam.cam_y);

	for (i = 0; i < OBJECT_MAX; i++) {
		if (!world.map.object.entry[i].loop)
			continue;
		if (world.map.object.entry[i].l != l)
			continue;
		d_sprite_draw(world.map.object.entry[i].sprite);
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
	if (!world.map.object.entry[entry].loop)
		return;
	
	ac.msg = AICOMM_MSG_DESTROY;
	ac.from = -1;
	ac.self = entry;
	object_message_loop(ac);

	if (ce->map_id >= 0) {
		d_bbox_move(world.map.object.not_spawned, ce->map_id, ce->x << 8, ce->y << 8);
		d_bbox_resize(world.map.object.not_spawned, ce->map_id, d_sprite_width(ce->sprite), d_sprite_height(ce->sprite));
	}
	
	d_bbox_delete(world.map.object.spawned, entry);
	d_sprite_free(ce->sprite);
	world.map.object.entry[entry].loop = NULL;

	if (world.map.cam.player == entry)
		world.map.cam.player = -1;

	return;
}


int object_spawn(int map_id) {
	int slot, x, y, w, h;
	struct character_entry *ce;
	struct aicomm_struct ac;

	if ((slot = d_bbox_add(world.map.object.spawned, 0, 0, 1, 1)) < 0)
		return -1;
	
	fprintf(stderr, "Spawning object %i...\n", map_id);
	ce = &world.map.object.entry[slot];
	ce->sprite = d_sprite_load(d_map_prop(world.map.map->object[map_id].ref, "sprite"), 0, DARNIT_PFORMAT_RGB5A1);
	d_bbox_move(world.map.object.not_spawned, ce->map_id, INT_MAX, INT_MAX);
	d_bbox_resize(world.map.object.not_spawned, ce->map_id, 1, 1);
	d_sprite_hitbox(ce->sprite, &x, &y, &w, &h);

	ce->x = (world.map.map->object[slot].x * world.map.map->layer->tile_w) << 8;
	ce->y = (world.map.map->object[slot].y * world.map.map->layer->tile_h) << 8;
	d_bbox_move(world.map.object.spawned, slot, (ce->x >> 8) + x, (ce->y >> 8) + y);
	d_bbox_resize(world.map.object.spawned, slot, w, h);
	ce->l = world.map.map->object[slot].l;
	fprintf(stderr, "Using AI %s\n",  d_map_prop(world.map.map->object[map_id].ref, "ai"));
	
	
	ce->dx = ce->dy = 0;
	ce->self = slot;
	ce->dir = 0;
	ce->stat = NULL;
	ce->stats = 0;
	ce->state = NULL;
	ce->map_id = map_id;
	*((unsigned int *) (&ce->special_action)) = 0;

	d_sprite_activate(ce->sprite, 0);
	object_update_sprite(slot);
	
	strcpy(ce->ai, d_map_prop(world.map.map->object[map_id].ref, "ai"));
	ce->loop = d_dynlib_get(world.map.object.ai_lib, ce->ai);
	ac.msg = AICOMM_MSG_INIT;
	ac.from = -1;
	ac.self = ce->self;
	object_message_loop(ac);
	
	object_set_hitbox(slot);

	return slot;
}

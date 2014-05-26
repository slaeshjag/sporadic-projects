#include <darnit/darnit.h>
#include <string.h>
#include "common/aicomm.h"
#include "aicomm_f.h"
#include "aicomm_handlers.h"
#include "world.h"
#include "object.h"


void object_init() {
	char ai_name[64];
	world.map.object.not_spawned = NULL;
	
	/* Load AI code library */
	sprintf(ai_name, "bin/%s.ai", d_platform_string());
	fprintf(stderr, "Loading %s\n", ai_name);
	world.map.object.ai_lib = d_dynlib_open(ai_name);

	return;
}


void object_despawn_all() {
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

	/* TODO: Despawn objects that are not marked as no despawn */
	world.map.object.not_spawned = d_bbox_free(world.map.object.not_spawned);
	world.map.object.not_spawned = d_bbox_new(m->objects);
	d_bbox_set_indexkey(world.map.object.not_spawned);
	
	for (i = 0; i < (int) m->objects; i++) {
		d_bbox_add(world.map.object.not_spawned, m->object[i].x * m->layer->tile_w, m->object[i].y * m->layer->tile_h, 1, 1);
	}

	return;
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
			ac = world.map.object.entry[ac.from].loop(ac);

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


int object_spawn(int map_id) {
	int slot;
	struct character_entry *ce;
	struct aicomm_struct ac;

	if ((slot = object_slot_get()) < 0)
		return -1;
	
	ce = &world.map.object.entry[slot];
	ce->sprite = d_sprite_load(d_map_prop(world.map.map->object[map_id].ref, "sprite"), 0, DARNIT_PFORMAT_RGB5A1);

	ce->x = (world.map.map->object[slot].x * world.map.map->layer->tile_w) << 8;
	ce->y = (world.map.map->object[slot].y * world.map.map->layer->tile_h) << 8;
	ce->l = world.map.map->object[slot].l;
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
	character_message_loop(ac);
	
	object_set_hitbox(slot);

	return slot;
}

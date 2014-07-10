#include "object.h"
#include "aicomm_f.h"
#include "savefile.h"
#include "textbox.h"
#include "world.h"
#include <string.h>

#define	CHECK_FROM_VALID	\
	if (ac.from < 0 || ac.from >= OBJECT_MAX || !world.map.object.entry[ac.from].loop) {	\
		ac.msg = AICOMM_MSG_NOAI;							\
		ac.self = ac.from;								\
		ac.from = -1;									\
		return ac;									\
	}											

int character_get_character_looked_at(int src);
struct aicomm_struct character_message_next(struct aicomm_struct ac);
void character_update_sprite(int entry);
void character_despawn(int entry);

DARNIT_KEYS d_k;


struct aicomm_struct aicomm_f_diru(struct aicomm_struct ac) {
	CHECK_FROM_VALID;

	object_update_sprite(ac.from);
	return object_message_next(ac);
}


struct aicomm_struct aicomm_f_nspr(struct aicomm_struct ac) {
	CHECK_FROM_VALID;

	world.map.object.entry[ac.from].sprite = d_sprite_free(world.map.object.entry[ac.from].sprite);
	object_load_sprite(ac.from, ac.argp);

	return object_message_next(ac);
}


struct aicomm_struct aicomm_f_telp(struct aicomm_struct ac) {
	CHECK_FROM_VALID;

	if (world.new_state != STATE_TELEPORTING_E)
		object_silence_all(1);
	world.new_state = STATE_TELEPORTING_E;
	world.map.teleport.id = ac.from;

	/* Only compiling in gcc/mingw from now on (: */
	world.map.teleport.map = strdup(ac.argp);
	world.map.teleport.x = ac.arg[1];
	world.map.teleport.y = ac.arg[2];
	world.map.teleport.layer = ac.arg[3];

	return object_message_next(ac);
}


struct aicomm_struct aicomm_f_tpme(struct aicomm_struct ac) {
	//struct savefile_teleport_entry t;

	CHECK_FROM_VALID;
	#if 0
	
	/* TODO: Add teleport ID offset */
	t = ws.char_data->teleport.entry[ac.arg[0]];

	if (t.map == ws.active_world) {
		ac.ce[ac.from]->x = (t.x << 8) * ws.camera.tile_w;
		ac.ce[ac.from]->y = (t.y << 8) * ws.camera.tile_h;
		ac.ce[ac.from]->l = t.l;
		return character_message_next(ac);
	}

	ws.new_state = WORLD_STATE_CHANGEMAP;
	
	ws.char_data->teleport.to.slot = ac.ce[ac.from]->slot;
	strcpy(ws.char_data->teleport.to.ai, ac.ce[ac.from]->ai);
	ws.char_data->teleport.to.x = t.x;
	ws.char_data->teleport.to.y = t.y;
	ws.char_data->teleport.to.l = t.l;
	ac.ce[ac.from]->map = t.map;
	ws.char_data->teleport.to.dungeon = t.map;
	#endif
	fprintf(stderr, "STUB: aicomm_f_tpme()\n");

	return object_message_next(ac);
}


struct aicomm_struct aicomm_f_folm(struct aicomm_struct ac) {
	world.map.cam.follow = ac.from;
	ac.self = ac.from;
	ac.from = -1;
	ac.msg = AICOMM_MSG_NEXT;

	return object_message_next(ac);
}


struct aicomm_struct aicomm_f_setp(struct aicomm_struct ac) {
	world.map.cam.player = ac.from;

	return object_message_next(ac);
}


struct aicomm_struct aicomm_f_getp(struct aicomm_struct ac) {
	ac.self = ac.from;
	ac.from = world.map.cam.player;
	
	return ac;
}


struct aicomm_struct aicomm_f_kill(struct aicomm_struct ac) {
	object_despawn(ac.self);
	return object_message_next(ac);
}


struct aicomm_struct aicomm_f_spwn(struct aicomm_struct ac) {
	//int x, y;
#if 0
	x = ac.arg[1] * ws.camera.tile_w;
	y = ac.arg[2] * ws.camera.tile_h;
	object_spawn_entry(ac.arg[0], ac.argp, x, y, ac.arg[3], ws.dm->grid[4].id, -1);
#else
	fprintf(stderr, "STUB: aicomm_f_spwn\n");
#endif

	return object_message_next(ac);
}


struct aicomm_struct aicomm_f_getf(struct aicomm_struct ac) {
	ac.self = ac.from;
	ac.from = object_get_character_looked_at(ac.self);

	return ac;
}


struct aicomm_struct aicomm_f_camn(struct aicomm_struct ac) {
	//ws.camera.jump = 1;

	return object_message_next(ac);
}


struct aicomm_struct aicomm_f_tbox(struct aicomm_struct ac) {
	struct textbox_properties *tp;

	tp = ac.argp;
	if (tp)
		textbox_add_message(tp->message, tp->question, tp->icons, tp->face, ac.from);
	return object_message_next(ac);
}


struct aicomm_struct aicomm_f_invm(struct aicomm_struct ac) {
	ac.self = ac.from;
	ac.from = -1;
	ac.msg = AICOMM_MSG_INVM;

	return ac;
}


struct aicomm_struct aicomm_f_txte(struct aicomm_struct ac) {
	#if 0
	ac.arg[0] = texteffect_add(ac.argp, ac.arg[0], ac.arg[1], ac.arg[2], ac.arg[3], ac.arg[4], ac.arg[5], ac.arg[6]);
	#endif
	
	fprintf(stderr, "STUB: aicomm_f_txte\n");

	ac.self = ac.from;
	ac.from = -1;
	ac.msg = AICOMM_MSG_TXTE;

	return ac;
}


struct aicomm_struct aicomm_f_keys(struct aicomm_struct ac) {
	d_k = d_keys_get();
	ac.self = ac.from;
	ac.from = -1;
	ac.msg = AICOMM_MSG_KEYS;
	ac.argp = &d_k;

	return ac;
}


struct aicomm_struct aicomm_f_skey(struct aicomm_struct ac) {
	ac.self = ac.from;
	ac.from = -1;
	ac.msg = AICOMM_MSG_SKEY;
	d_keys_set(*((DARNIT_KEYS *) ac.argp));
	return ac;
}


struct aicomm_struct aicomm_f_sela(struct aicomm_struct ac) {
	CHECK_FROM_VALID;

	ac.self = ac.from;
	ac.from = -1;
	ac.msg = AICOMM_MSG_SELA;
	if (ac.arg[0] < 0 || ac.arg[0] >= OBJECT_MAX) {
		ac.msg = AICOMM_MSG_NOAI;
		return ac;
	}

	world.map.object.entry[ac.arg[0]].l = ac.arg[1];

	return ac;
}

struct aicomm_struct aicomm_f_prop(struct aicomm_struct ac) {
	CHECK_FROM_VALID;

	ac.self = ac.from;
	ac.from = -1;
	ac.msg = AICOMM_MSG_PROP;

	if (world.map.object.entry[ac.self].map_id < 0)
		ac.argp = NULL;
	else
		ac.argp = (void *) d_map_prop(world.map.map->object[world.map.object.entry[ac.self].map_id].ref, ac.argp);
	return ac;
}


struct aicomm_struct aicomm_f_dummy(struct aicomm_struct ac) {
	return ac;
}

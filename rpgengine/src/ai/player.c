#include "aicomm.h"
#include "player.h"
#include "aicommon.h"
#include "engine_api.h"
#include "common.h"

#include <string.h>
#define	ZERO_KEYS(k)		memset(&k, 0, sizeof(k));

static DARNIT_KEYS darnit_keys;


void player_init_stats(struct character_entry *ce) {
	ce->stat[CHAR_STAT_HP].cur = 10;
	ce->stat[CHAR_STAT_HP].max = 15;
	ce->stat[CHAR_STAT_HP].name = "hp";

}


static void player_init(struct aicomm_struct ac, struct player_state *ps) {
	int self = ac.self;

	ps->msg = aicom_msgbuf_new(32);
	ac.from = self;

	engine_api_set_player(ps->msg, self, self);
	engine_api_follow_me(ps->msg, self);
	engine_api_camera_jump(ps->msg, self);

	
	engine_api_request_preload(ps->msg, "res/testsprite.mts", CHARACTER_RES_ANIMATION, 0, 0, self);
	engine_api_preload_unload(ps->msg, "res/testsprite.mts", self);

	ac.ce[ac.self].stat = malloc(sizeof(*ac.ce[ac.self].stat) * CHAR_STAT_TOTAL);
	ac.ce[ac.self].stats = CHAR_STAT_TOTAL - 1;
	player_init_stats(&ac.ce[ac.self]);

	ac.ce[self].special_action.solid = 1;
	ac.ce[self].special_action.nodespawn = 1;

	return;
}


static void player_handle_send(struct aicomm_struct ac, struct player_state *ps) {
	int t;

	if (ac.self == ac.from)
		return;
	ac.arg[1] += PLAYER_PROG_OFFSET;

	/* Get progress */
	if (ac.arg[0] == 1) {
		if (ac.arg[1] >= (signed) ac.ce[ac.self].save.is || ac.arg[1] < 0)
			ac.arg[0] = 0;
		else
			ac.arg[0] = ac.ce[ac.self].save.i[ac.arg[1]];
	} else if (ac.arg[0] == 2) {	/* Set progress */
		if (ac.arg[1] >= (signed) ac.ce[ac.self].save.is || ac.arg[1] < 0)
			ac.arg[0] = 0;
		else {
			ac.ce[ac.self].save.i[ac.arg[1]] = ac.arg[2];
			ac.arg[0] = ac.arg[2];
		}
	} else 
		ac.arg[0] = 0;
	
	ac.arg[1] -= PLAYER_PROG_OFFSET;
	t = ac.self;
	ac.self = ac.from;
	ac.from = t;
	aicom_msgbuf_push(ps->msg, ac);

	return;
}


static void player_loop(struct aicomm_struct ac, struct player_state *ps) {
	int self = ac.self;
	int n;
	static DARNIT_KEYS keys;

	keys = darnit_keys;
	engine_api_get_keys(ps->msg, ac.self);

	ac.ce[self].dx = ac.ce[self].dy = 0;
	n = -1;
	if (ps->freeze)
		goto nomove;
	if (keys.left) {
		ac.ce[self].dx = PLAYER_SPEED * -1;
		ac.ce[self].dir = 0;
		n = 0;
	}
	
	if (keys.right) {
		ac.ce[self].dx = PLAYER_SPEED;
		ac.ce[self].dir = 2;
		n = 0;
	}
	
	if (keys.up) {
		ac.ce[self].dir = 1;
		ac.ce[self].dy = PLAYER_SPEED * -1;
		n = 0;
	}
	
	if (keys.down) {
		ac.ce[self].dir = 3;
		ac.ce[self].dy = PLAYER_SPEED;
		n = 0;
	}

	if (keys.select)
		engine_api_spawn(ps->msg, ac.self, 0, (ac.ce[ac.self].x >> 8) / 32 - 2,
			(ac.ce[ac.self].y >> 8) / 32 - 2, ac.ce[ac.self].l, "box_ai");


	if (keys.y) {
		engine_api_text_effect(ps->msg, ac.self, 2000, (ac.ce[ac.self].x >> 8) + 16,
			(ac.ce[ac.self].y >> 8) - 64, 400, 255, 127, 127, "Fiskmåsar i sjön\n+10 XP");
		engine_api_set_keys(ps->msg, &darnit_keys, ac.self);
	}

	if (keys.x) 
		engine_api_replace_sprite(ps->msg, ac.self, "res/box.spr");
//	if (keys.a)
		

	if (keys.BUTTON_ACCEPT) {
		engine_api_request_faced(ps->msg, ac.self);
		ZERO_KEYS(keys);
		keys.BUTTON_ACCEPT = 1;
		engine_api_set_keys(ps->msg, &keys, ac.self);
	}

	nomove:
	
	if (n < 0) {
		if (ac.ce[self].special_action.animate) {
			ac.ce[self].special_action.animate = 0;
			engine_api_direction_update(ps->msg, ac.self);
			return;
		}
	}

	engine_api_get_keys(ps->msg, ac.self);
	ac.ce[self].special_action.animate = 1;

	return;
}


struct aicomm_struct EXPORT_THIS player_ai(struct aicomm_struct ac) {
	struct player_state *ps;
	int argv[8];

	if (ac.msg == AICOMM_MSG_INIT) {
		ac.ce[ac.self].state = malloc(sizeof(struct player_state));
		ps = ac.ce[ac.self].state;
		ps->init = 0;
		ps->freeze = 0;
		memset(&darnit_keys, 0, sizeof(darnit_keys));
		player_init(ac, ps);
	} else if (ac.msg == AICOMM_MSG_LOOP) {
		player_loop(ac, ac.ce[ac.self].state);
	} else if (ac.msg == AICOMM_MSG_MAPE) {
		ps = ac.ce[ac.self].state;
		if (ac.arg[1] & MAP_FLAG_TELEPORT) {
			engine_api_teleport_table(ps->msg, ac.self, ((unsigned) ac.arg[1]) >> 14);
			aicom_msgbuf_push(ps->msg, ac);
		}
	} else if (ac.msg == AICOMM_MSG_SILE) {
		ps = ac.ce[ac.self].state;
		ps->freeze = ac.arg[0];
		ac.from = ac.self;
		ZERO_KEYS(darnit_keys);
	} else if (ac.msg == AICOMM_MSG_SEND) {
		player_handle_send(ac, ac.ce[ac.self].state);
	} else if (ac.msg == AICOMM_MSG_GETF) {
		ps = ac.ce[ac.self].state;
		argv[0] = 0;
		engine_api_send(ps->msg, ac.self, ac.from, NULL, argv, 1);
	} else if (ac.msg == AICOMM_MSG_KEYS) {
		darnit_keys = *((DARNIT_KEYS *) ac.argp);
	} else if (ac.msg == AICOMM_MSG_DESTROY) {
		ps = ac.ce[ac.self].state;
		aicom_msgbuf_free(ps->msg);
		free(ps);
		ac.msg = AICOMM_MSG_DONE;
		return ac;
	}

	ps = ac.ce[ac.self].state;

	return aicom_msgbuf_pop(ps->msg);
}



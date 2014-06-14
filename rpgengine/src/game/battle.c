#include <math.h>

#include "world.h"
#include "battle.h"
#include "party.h"

#define	BATTLE_PARTY(party)		((party) ? world.battle.party2 : world.battle.party1)

void battle_calc_apply_stats(int src_party, int src_member, int dst_party, int dst_member, int move, float scale) {
	struct party_s *a, *b;
	float att;

	a = BATTLE_PARTY(src_party);
	b = BATTLE_PARTY(dst_party);

	if (a->member[src_member].move.move[move].type == PARTY_MOVE_TYPE_RANGED) {
		att = a->member[src_member].cur_stat.s_att * scale;
		att *= att;
		att /= b->member[dst_member].cur_stat.s_def;
	} else if (a->member[src_member].move.move[move].type == PARTY_MOVE_TYPE_PHYSICAL) {
		att = a->member[src_member].cur_stat.att * scale;
		att *= att;
		att /= b->member[dst_member].cur_stat.def;
	}
	
	att *= a->member[src_member].move.move[move].power;
	b->member[dst_member].cur_stat.hp -= att;
}


float battle_calc_scale(int src_party, int src_member, int dst_party, int dst_member, int move, int x, int y) {
	struct party_move_s *movep;
	int cx, cy, cw, ch, obj;
	float scale;

	/* Disqualify marty members that shouldn't be affected */
	obj = BATTLE_PARTY(dst_party)->member[dst_member].battle.map_object;
	if (obj >= 0)
		return 0.0f;
	movep = &BATTLE_PARTY(src_party)->member[src_member].move.move[move];
	if (src_party == dst_party) {
		if (!movep->positive_effect)
			if (!movep->friendly_fire)
				return 0.0f;
	} else {
		if (movep->positive_effect)
			if (!movep->friendly_fire)
				return 0.0f;
	}

	if (movep->spread_type == PARTY_MOVE_SPREAD_TYPE_NO_DECAY)
		return 1.0f;

	d_sprite_hitbox(world.map.object.entry[obj].sprite, &cx, &cy, &cw, &ch);
	cx += (world.map.object.entry[obj].x >> 8);
	cy += (world.map.object.entry[obj].y >> 8);;
	cx += cw / 2;
	cy += ch / 2;

	cx -= x;
	cy -= y;

	if (movep->spread_type == PARTY_MOVE_SPREAD_TYPE_NONE) {
		if (x != cx || y != cy)
			return 0.0f;
		else
			return 1.0f;
	}

	cx *= cx;
	cy *= cy;
	scale = cx + cy;
	scale = sqrtf(scale);
	/* Range chack */
	if (scale >= movep->spread_radius)
		return 0.0f;
	/* Calculate a scale based off of the remaining spread types */
	scale = movep->spread_radius - scale;
	scale = movep->spread_radius / scale;
	if (movep->spread_type == PARTY_MOVE_SPREAD_TYPE_LINEAR)
		return scale;
	if (movep->spread_type == PARTY_MOVE_SPREAD_TYPE_SLOW_DECAY)
		return logf(scale * (M_E - 1.0f) + 1.0f);
	if (movep->spread_type == PARTY_MOVE_SPREAD_TYPE_SHARP_DECAY)
		return powf(M_E, scale) / M_E;
	
	return 0.0f;
}


void battle_attack(int src_party, int src_member, int x, int y, int angle, int move) {
	

}

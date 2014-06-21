#include <math.h>

#include "world.h"
#include "battle.h"
#include "party.h"
#include "ui/menu.h"

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


float battle_calc_scale(int src_party, int src_member, int dst_party, int dst_member, int move, int x, int y, int angle) {
	struct party_move_s *movep;
	int cx, cy, cw, ch, obj, ta;
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
	cy += (world.map.object.entry[obj].y >> 8);
	cx += cw / 2;
	cy += ch / 2;

	cx -= x;
	cy -= y;
	
	/* Test angle */
	if (cx < 0) {
		if (cy < 0) {			/* Quadrant 2 */
			ta = atanf((float) cy / (float) cx) * 180.f / M_PI + 90.f;
		} else if (cy > 0) {		/* Quadrant 3 */
			ta = -1 * atanf((float) cy / (float) cx) * 180.f / M_PI + 180.f;
		} else
			ta = 180;
	} else if (cx > 0) {
		if (cy < 0) {			/* Quadrant 1 */
			ta = -1 * atanf((float) cy / (float) cx) * 180.f / M_PI;
		} else if (cy > 0) {		/* Quadrant 4 */
			ta = 360.f - atanf((float) cy / (float) cx) * 180.f / M_PI;
		} else
			ta = 0;
	} else {
		if (cy < 0)
			ta = 90;
		else if (cy > 0)
			ta = 270;
		else
			return 1.0f;
	}

	/* Check if this object is outside the angle */
	if (ta - angle <= -movep->spread_angle || ta - angle >= movep->spread_angle)
		return 0.0f;

	if (movep->spread_type == PARTY_MOVE_SPREAD_TYPE_NONE) {
		if (cx || cy)
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
	int i;
	float scale;

	for (i = 0; i < world.battle.party1->members; i++) {
		scale =  battle_calc_scale(src_party, src_member, 0, i, move, x, y, angle);
		if (scale <= 0.0f)
			continue;

		battle_calc_apply_stats(src_party, src_member, 0, i, move, scale);
	}

	for (i = 0; i < world.battle.party2->members; i++) {
		scale =  battle_calc_scale(src_party, src_member, 1, i, move, x, y, angle);
		if (scale <= 0.0f)
			continue;

		battle_calc_apply_stats(src_party, src_member, 1, i, move, scale);
	}
}


void battle_ui_update() {
	int i, x, y, w, cx, cy;

	/* Party 1 */
	for (i = 0; i < world.battle.party1->members; i++) {
		x = BATTLE_SCREEN_MARGIN;
		y = world.config.face_h;
		y += d_font_glyph_hs(world.config.font);
		y += world.config.tile_h;
		y += BATTLE_SCREEN_MARGIN;
		y *= i;
		y += BATTLE_SCREEN_MARGIN;

		w = world.battle.party1->member[i].cur_stat.hp * BATTLE_HPMP_LENGTH / world.battle.party1->member[i].base_stat.calculated.hp;
		cx = world.config.tile_w * 6;
		cy = world.config.tile_h;
		if (w) {
			d_render_tile_move(world.battle.ui.hp_mp_meters, i * 2, x, y);
			d_render_tile_tilesheet_coord_set(world.battle.ui.hp_mp_meters, i * 2, cx, cy, w, BATTLE_HPMP_HEIGHT);
			d_render_tile_size_set(world.battle.ui.hp_mp_meters, i * 2, w, BATTLE_HPMP_HEIGHT);
		} else
			d_render_tile_clear(world.battle.ui.hp_mp_meters, i * 2);
		cx += w;
		x += w;
		w = BATTLE_HPMP_LENGTH - w;
		cy += BATTLE_HPMP_HEIGHT;
		if (w) {
			d_render_tile_move(world.battle.ui.hp_mp_meters, i * 2 + 1, x, y);
			d_render_tile_tilesheet_coord_set(world.battle.ui.hp_mp_meters, i * 2 + 1, cx, cy, w, BATTLE_HPMP_HEIGHT);
			d_render_tile_size_set(world.battle.ui.hp_mp_meters, i * 2 + 1, w, BATTLE_HPMP_HEIGHT);
		} else
			d_render_tile_clear(world.battle.ui.hp_mp_meters, i * 2 + 1);
	}
}


void battle_draw_ui() {
	battle_ui_update();
	
	d_render_tile_draw(world.battle.ui.hp_mp_meters, 2);
	if (world.battle.party1->member[0].cur_stat.hp >= 1.f)
		world.battle.party1->member[0].cur_stat.hp -= 0.2;
}


void battle_start() {
	int i;
	struct party_s *p;

	world.battle.ui.party1_stat = menu_new_container(BATTLE_STATS_WIDTH, d_platform_get().screen_h / world.config.tile_h);
	world.battle.ui.party2_stat = menu_new_container(BATTLE_STATS_WIDTH, d_platform_get().screen_h / world.config.tile_h);
	p = world.battle.party1;
	for (i = 0; i < PARTY_MAX_SIZE; i++) {
		
	}
		
	/* TODO: Init all the data structures used in battle */
}


void battle_end() {
	/* TODO: Clean up data structures used in battle */
}


void battle_init() {
	world.battle.ui.hp_mp_meters = d_render_tile_new(PARTY_MAX_SIZE * 2 * 2, world.config.ts_sys);

	return;
}

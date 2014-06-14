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


void battle_attack(int src_party, int src_member, int x, int y, int move) {
	

}

#ifndef __PARTY_H__
#define	__PARTY_H__

#include <stdint.h>

#define	PARTY_MAX_SIZE				4
#define	PARTY_NAME_LENGTH			32
#define	PARTY_FACE_LENGTH			48
#define	PARTY_MOVE_DESC_LENGTH			128
#define	PARTY_MEMBER_MOVE_CAP			32

struct party_stat_s {
	float					hp;
	float					mp;
	float					att;
	float					def;
	float					s_att;
	float					s_def;
	float					agil;
};


struct party_base_stat_s {
	struct party_stat_s			bias;
	struct party_stat_s			multiple;
	struct party_stat_s			range;

	struct party_stat_s			calculated;
};


enum party_move_spread_type_e {
	PARTY_MOVE_SPREAD_TYPE_LINEAR,
	PARTY_MOVE_SPREAD_TYPE_SHARP_DECAY,
	PARTY_MOVE_SPREAD_TYPE_SLOW_DECAY,
	PARTY_MOVE_SPREAD_TYPE_NO_DECAY,
	PARTY_MOVE_SPREAD_TYPE_NONE,
};


enum party_move_type_e {
	PARTY_MOVE_TYPE_RANGED,
	PARTY_MOVE_TYPE_PHYSICAL,
	PARTY_MOVE_TYPE_NONE,
};


struct party_move_s {
	char					name[PARTY_NAME_LENGTH];
	char					desc[PARTY_MOVE_DESC_LENGTH];

	enum party_move_spread_type_e		spread_type;
	int					spread_angle;
	float					spread_radius;
	float					power;
	int					positive_effect;
	int					friendly_fire;
	
	int					minimum_exp;
	enum party_move_type_e			type;
	struct party_stat_s			src_rel_effect;
	struct party_stat_s			src_abs_effect;
	struct party_stat_s			dst_rel_effect;
	struct party_stat_s			dst_abs_effect;

	/* TODO: Paths to animations etc. */
};


struct party_move_table_s {
	/* Might want to make this fixed-size... */
	struct party_move_s			move[PARTY_MEMBER_MOVE_CAP];
	int					moves;
};


struct party_member_battle_state_s {
	int					map_object;
	int					ready;
};


struct party_member_s {
	char					name[PARTY_NAME_LENGTH];
	char					face[PARTY_FACE_LENGTH];
	int					key;


	int32_t					exp;

	struct party_base_stat_s		base_stat;
	struct party_stat_s			cur_stat;
	struct party_move_table_s		move;
	struct party_member_battle_state_s	battle;
};


struct party_s {
	struct party_member_s			member[PARTY_MAX_SIZE];
	int					members;
	int					key_cnt;

};


struct party_s *party_new();
void party_calculate_stats(struct party_s *p);
void party_member_add_exp(struct party_s *p, int key, int32_t exp);
int party_member_add(struct party_s *p, const char *member_descr, int exp);
void party_member_leave(struct party_s *p, int member_key);
struct party_s *party_destroy(struct party_s *p);

#endif

#ifndef __PARTY_H__
#define	__PARTY_H__

#include <stdint.h>

#define	PARTY_NAME_LENGTH		32
#define	PARTY_FACE_LENGTH		48
#define	PARTY_MOVE_DESC_LENGTH		128
#define	PARTY_MEMBER_MOVE_CAP		32

struct party_stat_s {
	float				hp;
	float				mp;
	float				att;
	float				def;
	float				s_att;
	float				s_def;
	float				agil;
};


struct party_base_stat_s {
	struct party_stat_s		bias;
	struct party_stat_s		multiple;
	struct party_stat_s		range;

	struct party_stat_s		calculated;
};


enum party_move_spread_type_e {
	PARTY_MOVE_SPREAD_TYPE_LINEAR,
	PARTY_MOVE_SPREAD_TYPE_SHARP_DECAY,
	PARTY_MOVE_SPREAD_TYPE_SLOW_DECAY,
	PARTY_MOVE_SPREAD_TYPE_NONE,
};


struct party_move_s {
	char				name[PARTY_NAME_LENGTH];
	char				desc[PARTY_MOVE_DESC_LENGTH];

	enum party_move_spread_type_e	spead_type;
	float				spread_radius;
	/* If the move has a positive effect or negative */
	int				sign;
	
	int				minimum_exp;
	struct party_stat_s		effect;

	/* TODO: Paths to animations etc. */
};


struct party_move_table_s {
	/* Might want to make this fixed-size... */
	struct party_move_s		move[PARTY_MEMBER_MOVE_CAP];
	int				moves;
};

struct party_member_s {
	char				name[PARTY_NAME_LENGTH];
	char				face[PARTY_FACE_LENGTH];
	int				key;

	int				map_object;

	int32_t				exp;

	struct party_base_stat_s	base_stat;
	struct party_move_table_s	move;
};




struct party_s {
	struct party_member_s		*member;
	int				members;
	int				key_cnt;

};


struct party_s *party_new();
void party_calculate_stats(struct party_s *p);
void party_member_set_exp(struct party_s *p, int key, int32_t exp);
int party_member_add(struct party_s *p, const char *member_descr);
void party_member_leave(struct party_s *p, int member_key);
struct party_s *party_destroy(struct party_s *p);

#endif

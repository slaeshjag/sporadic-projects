#ifndef __PARTY_H__
#define	__PARTY_H__

#include <stdint.h>

struct party_stat_s {
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


struct party_member_s {
	char				*name;
	char				*face;
	int				key;

	int32_t				exp;
	struct party_base_stat_s	base_stat;
};


struct party_s {
	struct party_member_s		*member;
	int				members;
	int				key_cnt;

};


#endif

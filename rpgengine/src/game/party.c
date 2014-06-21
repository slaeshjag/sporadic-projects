#include <darnit/darnit.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "party.h"

struct party_s *party_new() {
	struct party_s *p;

	p = malloc(sizeof(*p));
	p->members = 0;
	p->key_cnt = 0;
	
	return p;
}


static int party_member_lookup(struct party_s *p, int key) {
	int i;

	for (i = 0; i < p->members; i++)
		if (p->member[i].key == key)
			return i;
	return -1;
}


static void party_read_stat_line(DARNIT_FILE *f, struct party_stat_s *s) {
	char buff[512];

	d_file_getln(buff, 512, f);
	sscanf(buff, "%f %f %f %f %f %f %f", &s->hp, &s->mp, &s->att, &s->def, &s->s_att, &s->s_def, &s->agil);
	return;
}


void party_calculate_stats(struct party_s *p) {
	int i, j;
	float n, *vec_r, *vec_mult, *vec_c;

	for (i = 0; i < p->members; i++) {
		p->member[i].base_stat.calculated = p->member[i].base_stat.bias;

		/* Ugly vector multiplication woop woop */
		vec_r = (float *) &p->member[i].base_stat.range;
		vec_c = (float *) &p->member[i].base_stat.calculated;
		vec_mult = (float *) &p->member[i].base_stat.multiple;
		for (j = 0; j < (signed) (sizeof(struct party_stat_s) / sizeof(float)); j++) {
			n = M_PI_2 / vec_r[j] * p->member[i].exp;
			if (n > M_PI_2)
				n = M_PI_2;
			vec_c[j] += sinf(n) * vec_mult[j];
		}
	}

	return;
}


void party_member_add_exp(struct party_s *p, int key, int32_t exp) {
	int slot;

	if ((slot = party_member_lookup(p, key)) < 0)
		return;
	p->member[slot].exp += exp;
	party_calculate_stats(p);
	return;
}


int party_member_add(struct party_s *p, const char *member_descr, int exp) {
	char name[128], face[128], buff[512];
	int slot = p->members++;
	DARNIT_FILE *f;

	if (p->members >= PARTY_MAX_SIZE)
		return -1;
	
	p->member[slot].key = p->key_cnt++;
	p->member[slot].exp = exp;

	if (!(f = d_file_open(member_descr, "r"))) {
		fprintf(stderr, "Unable to open party member descriptor %s\n", member_descr);
		return -1;
	}
	
	d_file_getln(buff, 512, f);
	sscanf(buff, "%s %s", name, face);
	party_read_stat_line(f, &p->member[slot].base_stat.bias);
	party_read_stat_line(f, &p->member[slot].base_stat.multiple);
	party_read_stat_line(f, &p->member[slot].base_stat.range);
	party_calculate_stats(p);

	return p->member[slot].key;
}


void party_member_leave(struct party_s *p, int member_key) {
	int i, d;

	if ((i = party_member_lookup(p, member_key)) < 0)
		return;

	d = p->members - i;
	if (d) {
		memmove(&p->member[i], &p->member[i + 1], sizeof(*p->member) * d);
		p->members--;
	}

	return;
}


struct party_s *party_destroy(struct party_s *p) {

	while (p->members)
		party_member_leave(p, p->member->key);
	free(p);
	return NULL;
}

#ifndef __WORLD_H__
#define	__WORLD_H__

#include "map.h"
#include "rpgtest.h"

void world_init();

struct world_s {
	struct map_s		map;
	enum state_e		state;
	enum state_e		new_state;
} world;

#endif

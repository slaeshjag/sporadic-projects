#ifndef __WORLD_H__
#define	__WORLD_H__

#include"map.h"

void world_init();

struct world_s {
	struct map_s		map;
} world;

#endif

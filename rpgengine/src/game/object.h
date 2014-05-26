#ifndef __OBJECT_H__
#define	__OBJECT_H__

#include <darnit/darnit.h>
#include "common/aicomm.h"

#define	OBJECT_MAX			128

struct object_s {
	DARNIT_BBOX			*not_spawned;
	DARNIT_BBOX			*spawned;
	struct character_entry		entry[OBJECT_MAX];
	DARNIT_DYNLIB			*ai_lib;
};

void object_init();
void object_reset();
void object_refresh();

#endif

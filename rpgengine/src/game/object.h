#ifndef __OBJECT_H__
#define	__OBJECT_H__

#include <darnit/darnit.h>
#include "common/aicomm.h"

#define	OBJECT_MAX			128

struct object_s {
	DARNIT_BBOX			*not_spawned;
	DARNIT_BBOX			*spawned;
	struct character_entry		entry[OBJECT_MAX];
	unsigned int			buff1[OBJECT_MAX];
	unsigned int			buff2[OBJECT_MAX];
	DARNIT_DYNLIB			*ai_lib;
};

void object_init();
void object_reset();
void object_refresh();
struct aicomm_struct object_message_next(struct aicomm_struct ac);

#endif

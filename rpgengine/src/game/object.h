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
int object_spawn(int map_id);
void object_loop();
void object_reset();
void object_render_layer(int l);
void object_refresh();
void object_message_loop(struct aicomm_struct ac);
void object_update_sprite(int entry);
void object_tell_all(struct aicomm_struct ac);
void object_load_sprite(int obj, const char *path);
int object_get_character_looked_at(int src);
struct aicomm_struct object_message_next(struct aicomm_struct ac);
void object_silence_all(int silence);
void object_despawn(int entry);

#endif

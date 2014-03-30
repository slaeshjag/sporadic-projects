#ifndef __MESSAGE_H__
#define	__MESSAGE_H__

#include "object.h"

void message_obj_init(struct object_state *os, int id);
void message_obj_destroy(struct object_state *os, int id);
void message_obj_collision(struct object_state *os, int id, int from);
void message_obj_loop(struct object_state *os, int id, int from);


#endif

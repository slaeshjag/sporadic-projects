#include "message.h"
#include "object.h"


void message_loop(struct object_state *os, struct object_message_s msg) {
	while (msg.msg != O_MSG_DONE) {
		if (msg.to < 0 || msg.to >= os->max_objects)
			return;
		if (!os->o[msg.to].props.present)
			return;
		if (!os->o[msg.to].ai)
			return;
		msg = os->o[msg.to].ai(msg);
	}

	return;
}


void message_obj_noargs(struct *object_state *os, enum object_message msg, int id, int from) {
	struct object_message_s msg;

	msg.to = id;
	msg.from = -1;
	msg.msg = msg;
	message_loop(os, msg);
	return;
}


void message_obj_init(struct object_state *os, int id) {
	message_obj_noargs(os, O_MSG_INIT, id, -1);
	return;
}


void message_obj_destroy(struct object_state *os, int id) {
	message_obj_noargs(os, O_MSG_KILL, id, -1);
	return;
}


void message_obj_collision(struct object_state *os, int id, int from) {
	message_obj_noargs(os, O_MSG_COLL, id, from);
	return;
}

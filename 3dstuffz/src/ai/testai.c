#include "../api/object.h"


struct object_message_s testai(struct object_message_s msg) {
	int ret;

	switch (msg.msg) {
		case O_MSG_INIT:
			msg.msg = O_MSG_NEXT;
			ret = msg.from;
			msg.from = msg.to;
			msg.to = ret;
			return msg;
			break;
		case O_MSG_KILL:
			msg.msg = O_MSG_NEXT;
			ret = msg.from;
			msg.from = msg.to;
			msg.to = ret;
			return msg;
			break;
		default:
			break;
	}

	msg.msg = O_MSG_DONE;
	return msg;
}

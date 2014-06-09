#include "aicommon.h"
#include "aicomm.h"
#include <darnit/darnit.h>

struct aicomm_struct stair_ai(struct aicomm_struct ac) {

	switch (ac.msg) {
		case AICOMM_MSG_INIT:
			ac.msg = AICOMM_MSG_PROP;
			ac.argp = "layer_to";
			ac.from = ac.self;
			ac.ce[ac.self].state = malloc(sizeof(int));
			return ac;
			break;
		case AICOMM_MSG_PROP:
			*((int *) (ac.ce[ac.self].state)) = atoi(ac.argp);
		case AICOMM_MSG_LOOP:
		default:
			ac.from = ac.self;
			ac.msg = AICOMM_MSG_DONE;
			return ac;
		case AICOMM_MSG_COLL:
			ac.msg = AICOMM_MSG_SELA;
			ac.from = ac.self;
			ac.arg[1] = *((int *) ac.ce[ac.self].state);
			return ac;
			break;
		case AICOMM_MSG_DESTROY:
			free(ac.ce[ac.self].state);
			ac.ce[ac.self].state = NULL;
			ac.from = ac.self;
			ac.msg = AICOMM_MSG_DONE;
			return ac;
			break;
	}

	ac.from = ac.self;
	ac.msg = AICOMM_MSG_DONE;

	return ac;
}

#include "aicommon.h"
#include "aicomm.h"

/* Han teleporterar taliban!		**
** Han teleporterar taliban		**
** Han teleporterar ta-li-baaaan	**
** Han teleporterar taliban!		*/

struct internal {
	int state;
	const char *map;
	int p;
	int x;
	int y;
	int l;
};


struct aicomm_struct EXPORT_THIS teleporter_ai(struct aicomm_struct ac) {
	struct internal *i;
	int from;

	if (!ac.ce->state || ac.msg == AICOMM_MSG_INIT) {
		ac.ce[ac.self].state = malloc(sizeof(*i));
		i = ac.ce[ac.self].state;
		i->state = 0;
	}

	i = ac.ce[ac.self].state;
	from = ac.from;
	ac.from = ac.self;
	
	if (ac.msg == AICOMM_MSG_INIT) {
		ac.ce[ac.self].special_action.animate = 0;
		ac.msg = AICOMM_MSG_PROP;
		ac.argp = "teleport_map";
		return ac;
	} else if (ac.msg == AICOMM_MSG_PROP) {
		if (!i->state) {
			i->map = ac.argp;
			ac.argp = "teleport_x";
			i->state++;
			fprintf(stderr, "Got map %s\n", i->map);
			return ac;
		} else if (i->state == 1) {
			i->x = atoi(ac.argp);
			ac.argp = "teleport_y";
			i->state++;
			return ac;
		} else if (i->state == 2) {
			i->y = atoi(ac.argp);
			ac.argp = "teleport_layer";
			i->state++;
			return ac;
		} else if (i->state == 3) {
			i->l = atoi(ac.argp);
			ac.msg = AICOMM_MSG_DONE;
			i->state++;
			return ac;
		}
	} else if (ac.msg == AICOMM_MSG_COLL) {
		if (ac.arg[0] != i->p)
			goto nope;
		fprintf(stderr, "teleport %i to %i %i %i %s!\n", ac.arg[0], i->x, i->y, i->l, i->map);
		ac.msg = AICOMM_MSG_TELP;
		ac.argp = (void *) i->map;
		ac.arg[1] = i->x;
		ac.arg[2] = i->y;
		ac.arg[3] = i->l;
		ac.from = ac.arg[0];
		return ac;
	} else if (ac.msg == AICOMM_MSG_LOOP) {
		ac.msg = AICOMM_MSG_GETP;
		return ac;
	} else if (ac.msg == AICOMM_MSG_GETP) {
		i->p = from;
		ac.msg = AICOMM_MSG_DONE;
		return ac;
	} else if (ac.msg == AICOMM_MSG_DESTROY) {
		free(ac.ce[ac.self].state);
		ac.ce[ac.self].state = NULL;
	}

	nope:
	ac.msg = AICOMM_MSG_DONE;

	return ac;
}

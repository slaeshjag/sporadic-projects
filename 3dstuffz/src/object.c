#include <darnit/darnit.h>
#include "object.h"


void object_state_init(int max_objects) {
	
	return;
}


struct object *object_new(float x, float y, float z, const char *ai) {
	struct object *o;

	o = malloc(sizeof(*o));
	o->x = x;
	o->y = y;
	o->z = z;
	o->vel_x = o->vel_y = o->vel_z = 0;

	/* TODO: Initialize object with AI */

	return o;
}

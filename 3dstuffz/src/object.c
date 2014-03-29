#include <darnit/darnit.h>
#include "object.h"
#include "message.h"


void object_state_init(int max_objects, struct object_state *os, const char *lib) {
	int i;

	os->o = malloc(sizeof(*os->o) * max_objects);
	os->max_objects = max_objects;
	os->bbox = d_bbox_new(max_objects);
	os->lib = d_dynlib_open(lib);

	for (i = 0; i < os->max_objects; i++)
		os->o[i].props.present = 1;
	os->o_cache = malloc(sizeof(*os->o_cache) * max_objects);

	return;
}


struct object *object_new(float x, float y, float z, const char *ai, struct object_state *os) {
	struct object *o;

	o = malloc(sizeof(*o));
	o->x = x;
	o->y = y;
	o->z = z;
	o->vel_x = o->vel_y = o->vel_z = 0;
	o->data = NULL;

	/* Default properties */
	o->props.present = 1;
	o->props.gravity = 1;
	o->props.solid = 1;

	/* TODO: Initialize object with AI */
	o->ai = d_dynlib_get(os->lib, ai);
	if (o->ai)
		message_obj_init(os, o->id);


	return o;
}


void object_free(int obj, struct object_state *os) {
	if (!os->o[obj].props.present)
		return;
	d_bbox_delete(os, obj);
	os->o[obj].props.present = 0;
	
	/* TODO: Call AI destructor */

	return;
}


void object_loop(struct object_state *os) {
	int i, max;
	
	max = d_bbox_test(os->bbox, os->active_area.x, os->active_area.z, os->active_area.w, os->active_area.d, os->o_cache, os->max_objects);
	for (i = 0; i < max; i++) {
		/* TODO: Loop through objects here */
	}

	return;
}

#include "object.h"
#include "ai.h"

struct Object obj;

struct ObjectFunction object_func_define[] = {
	{ "test", ai_test_init, ai_test_kill, ai_test_loop },
	{ NULL, ai_test_init, ai_test_kill, ai_test_loop },
};



struct ObjectFunction object_func_lookup(const char *str) {
	int i;

	for (i = 0; object_func_define[i].name; i++)
		if (!strcmp(str, object_func_define[i].name))
			return object_func_define[i];
	fprintf(stderr, "Warning: Unable to find AI %s\n", str);
	return object_func_define[i];
}


int object_init() {
	obj.obj = c_dynalloc_init(OBJECT_CAP, sizeof(struct ObjectEntry));

	return obj.obj ? 1 : 0;
}


int object_spawn(const char *sprite, const char *ai, int x, int y, int l, DARNIT_MAP_OBJECT *mobj) {
	int id;
	struct ObjectEntry *oe;

	id = c_dynalloc_alloc(obj.obj);
	if (id >= OBJECT_CAP) {
		/* Overflowing is bad, mmkay? */
		c_dynalloc_release(obj.obj, id);
		return -1;
	}
	oe = c_dynalloc_get(obj.obj, id);
	oe->sprite = d_sprite_load(sprite, 0, DARNIT_PFORMAT_RGB5A1);
	oe->mobj = mobj;
	oe->pos_x = x * 1000, oe->pos_y = y * 1000, oe->layer = l;
	oe->vel_x = oe->vel_y = 0;
	oe->solid = 0;

	oe->func = object_func_lookup(ai);
	oe->func.init(id);
	
	return id;
}


void object_kill(int id) {
	struct ObjectEntry *oe;

	if (!(oe = c_dynalloc_get(obj.obj, id)))
		return;
	oe->func.kill(id);
	d_sprite_free(oe->sprite);
	c_dynalloc_release(obj.obj, id);
}


void object_nuke() {
	int i;

	for (i = 0; i < 0; i++) {
		object_kill(i);
	}

	return;
}


void object_loop() {
	int i, j, cnt;
	struct ObjectEntry *oe;

	cnt = c_dynalloc_entries(obj.obj);
	for (i = 0; j < cnt; i++) {
		if (!(oe = c_dynalloc_get(obj.obj, i)))
			continue;
		/* TODO: Handle movement, collision detection */
		oe->func.loop(i);
	}
}

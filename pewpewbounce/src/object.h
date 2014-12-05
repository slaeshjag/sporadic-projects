#ifndef __OBJECT_H__
#define	__OBJECT_H__

#define	OBJECT_CAP		4096

#include <copypasta/copypasta.h>
#include <darnit/darnit.h>

#include "ai.h"



struct ObjectFunction {
	const char		*name;

	void			(*init)(int id);
	void			(*kill)(int id);
	void			(*loop)(int id);
	void			(*collide)(int mid, int fid, int xvelc, int yvelc, int xdir, int ydir);
	void			(*collide_map)(int id, int xdir, int ydir);
	/* TODO: Fill in */
};


struct ObjectEntry {
	/* All coordinates in millipixels */
	int			pos_x;
	int			pos_y;
	int			layer;
	int			rotation;

	/* Velocity in pixels/second */
	int			vel_x;
	int			vel_y;

	struct ObjectFunction	func;
	DARNIT_SPRITE		*sprite;
	DARNIT_MAP_OBJECT	*mobj;

	void			*data;

	int			solid;
	int			id;
};


struct Object {
	C_DYNALLOC		*obj;
	DARNIT_BBOX		*bbox;
};

int object_spawn(const char *sprite, const char *ai, int x, int y, int l, DARNIT_MAP_OBJECT *mobj);
void object_nuke();
void object_loop();
void object_render(int layer);

extern struct Object obj;

#endif

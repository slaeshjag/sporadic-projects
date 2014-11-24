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
	/* TODO: Fill in */
};


struct ObjectEntry {
	/* All coordinates in millipixels */
	int			pos_x;
	int			pos_y;
	int			layer;
	int			rotation;

	/* Velocity in pixels/second (millipixels per millisecond :B) */
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

extern struct Object obj;

#endif

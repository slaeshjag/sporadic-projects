#ifndef __OBJECT_H__
#define	__OBJECT_H__

#include <darnit/darnit.h>

struct object_props {
	unsigned int			gravity	: 1;
	unsigned int			solid	: 1;
};


struct object {
	float				x;
	float				y;
	float				z;
	
	struct object_props		props;

	float				vel_x;
	float				vel_y;
	float				vel_z;
};


struct {
	struct object			*o;
	int				max_objects;
	DARNIT_BBOX			*bbox;
} object_state;


#endif

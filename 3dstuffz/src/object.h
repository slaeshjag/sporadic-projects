#ifndef __OBJECT_H__
#define	__OBJECT_H__

#include <darnit/darnit.h>
#include "api/object.h"


struct object_state {
	struct object			*o;
	int				max_objects;
	DARNIT_BBOX			*bbox;

	void				*lib;
};


#endif

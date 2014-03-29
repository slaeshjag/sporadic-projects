#ifndef __API_OBJECT_H__
#define	__API_OBJECT_H__


enum object_message {
	O_MSG_INIT,
	O_MSG_LOOP,
	O_MSG_DONE,
	O_MSG_NEXT,
	O_MSG_COLL,
	O_MSG_KILL,
};


struct object_area {
	float				x;
	float				y;
	float				z;
	float				w;
	float				h;
	float				d;
};


struct object_message_s {
	int				from;
	int				to;

	enum object_message		msg;
	int				arg[8];
	void				*argp;

	struct object_state		*os;
};


struct object_props {
	unsigned int			gravity	: 1;
	unsigned int			solid	: 1;
	unsigned int			present	: 1;
};


struct object {
	int				id;

	float				x;
	float				y;
	float				z;
	float				vel_x;
	float				vel_y;
	float				vel_z;
	
	struct object_props		props;

	void				*data;
	struct object_message_s		(*ai)(struct object_message_s msg);
};


struct object_state {
	struct object			*o;
	unsigned int			*o_cache;
	int				max_objects;
	void				*bbox;
	struct object_area		active_area;

	void				*lib;
};


#endif

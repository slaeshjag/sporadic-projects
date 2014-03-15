#ifndef __MESH_H__
#define	__MESH_H__

#include "heightmap.h"

struct mesh_vertex {
	float			x;
	float			y;
	float			z;
};


struct mesh_triangle {
	struct mesh_vertex	p[3];
};


struct mesh {
	struct mesh_triangle	*tri;
	int			tri_width;
	int			tri_depth;
};


struct mesh *mesh_alloc(int width, int depth, int detail);
void mesh_heightmap_apply(struct mesh *m, struct heightmap *hm);

#endif

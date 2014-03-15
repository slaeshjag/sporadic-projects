#include "mesh.h"
#include "heightmap.h"
#include <stdlib.h>
#include <stdio.h>

static void mesh_tri_set(struct mesh *m, int i, int p, float x, float y, float z) {
	m->tri[i].p[p].x = x;
	m->tri[i].p[p].z = z;
	m->tri[i].p[p].y = y;
	return;
}


/* Width in OpenGL length units, height in OpenGL legnth units, detail in	**
**	vertices per OpenGL length unit						*/
struct mesh *mesh_alloc(int width, int depth, int detail) {
	int vert_x, vert_z, i, j, k;
	struct mesh *m;
	float x1, x2, z1, z2, y1, y2, y3, y4;
	struct heightmap *hm;

	
	vert_x = width * detail * 2;
	vert_z = depth * detail * 2;
	hm = heightmap_generate(vert_x + 1, vert_z + 1, detail);

	m = malloc(sizeof(*m));
	m->tri_width = vert_x;
	m->tri_depth = vert_z / 2;

	m->tri = malloc(sizeof(*m->tri) * m->tri_width * m->tri_depth);

	for (i = 0; i < vert_z / 2; i++)
		for (j = 0; j < vert_x / 2; j++) {
			y1 = hm->height[hm->w * i + j * 2];
			y2 = hm->height[hm->w * i + j * 2 + hm->w];
			y3 = hm->height[hm->w * i + j * 2 + hm->w + 1];
			y4 = hm->height[hm->w * i + j * 2 + 1];
			k = j * 2 + i * vert_x;
			z1 = 1.0f / detail * (i % detail) + i / detail - (float)width/2;
			x1 = 1.0f / detail * (j % detail) + j / detail - (float)depth/2;
			z2 = z1 + 1.0f / detail;
			x2 = x1 + 1.0f / detail;

			/* Tri 1 in pair */
			mesh_tri_set(m, k, 0, x1, y1, z1);
			mesh_tri_set(m, k, 1, x1, y2, z2);
			mesh_tri_set(m, k, 2, x2, y3, z2);

			/* Tri 2 in pair */
			mesh_tri_set(m, k + 1, 0, x2, y4, z1);
			mesh_tri_set(m, k + 1, 1, x1, y1, z1);
			mesh_tri_set(m, k + 1, 2, x2, y3, z2);
		}
	return m;
}

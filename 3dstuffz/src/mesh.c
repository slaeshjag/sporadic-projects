#include "mesh.h"
#include "heightmap.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define	ABSF(x)			(x) < 0 ? ((x) * -1.0f) : (x)

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
	m->detail = detail;

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


float mesh_collide_test_small(struct mesh *m, float x, float y, float z, float h, float r) {
	int center, c, i, j, cc;
	int cx, cz, d;
	float fx, fz, max_dy, fdy;

	max_dy = -HUGE_VALF;
	r *= r;
	cx = (x * m->detail * 2);
	cz = (z * m->detail);
	
	center = (cx + cz * m->tri_width + 0.5f);
	d = ((r * m->detail) + 0.5f);
	c = center - (d * m->tri_width * 2.0f + d);
	
	for (j = 0; j < d + 1; j++)
		for (i = 0; i < d * 2 + 1; i++) {
			cc = c + d * 4 * j + i;
			if (cc < 0 || cc > m->tri_width * m->tri_depth)
				continue;
			fx = x - m->tri[cc].p[0].x;
			fz = z - m->tri[cc].p[0].z;
			if (fx * fx + fz * fz < r) {	/* Within circle, test collision */
				fdy = y - m->tri[cc].p[0].y;
				if (fdy > max_dy)
					if (fdy > 0 && y + h > m->tri[cc].p[0].y)
						max_dy = fdy;
			}
		}
	return max_dy;
}

#include "heightmap.h"
#include <stdlib.h>
#include <math.h>

#define	ABSF(x)			(x) < 0 ? ((x) * -1.0f) : (x)
struct heightmap *heightmap_generate(int points_w, int points_d, int detail) {
	struct heightmap *hm;
	int i, j, k;

	hm = malloc(sizeof(*hm));

	hm->w = points_w;
	hm->h = points_d;

	hm->height = malloc(sizeof(*hm->height) * points_w * points_d);

	for (i = 0; i < points_d; i++)
		for (j = 0; j < points_w; j++) {
			k = i * points_w + j;
			hm->height[k] = (sinf(2 * M_PI / points_w * j) * cosf(2 * M_PI / points_d * i)) * 1.0f;
		}
	return hm;
}


/* mapchunker.c - Steven Arnow <s@rdw.se>,  2014 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#include <copypasta/copypasta.h>


struct vector3 {
	float			x;
	float			y;
	float			z;
};


struct quadface {
	struct vector3		vec[4];
};


int chunks_w;
int chunks_h;
float chunk_w;
float chunk_h;

struct quadface **chunk;

void add_vector(C_DYNALLOC *vec, float x, float y, float z) {
	struct vector3 *vec3;
	int i;

	i = c_dynalloc_alloc(vec);
	vec3 = c_dynalloc_get(vec, i);
	vec3->x = x, vec3->y = y, vec3->z = z;
	return;
}


void find_vectors(C_DYNALLOC *vec, FILE *fp, float chunk_side) {
	char buff[512];
	float x, y, z;
	float min_x, min_z, max_x, max_z, w, h;
	int wi, hi;
	max_x = max_z = -HUGE_VAL;
	min_x = min_z = HUGE_VAL;
	
	while (!feof(fp)) {
		*buff = 0;
		fscanf(fp, "%s", buff);
		if (strcmp(buff, "v")) {
			fgets(buff, 512, fp);
			continue;
		}

		fscanf(fp, "%f %f %f\n", &x, &y, &z);
		add_vector(vec, x, y, z);
		if (x > max_x) max_x = x;
		if (z > max_z) max_z = z;
		if (x < min_x) min_x = x;
		if (z < min_z) min_z = z;
	}

	fprintf(stderr, "min: %f, %f;; max: %f, %f\n", min_x, min_z, max_x, max_z);
	w = (max_x - min_x) / chunk_side;
	wi = w;
	if (wi < w)
		wi++;
	h = (max_z - min_z) / chunk_side;
	hi = h;
	if (hi < h)
		hi++;
	chunks_w = wi;
	chunks_h = hi;
	chunk_w = (max_x - min_x) / chunks_w;
	chunk_h = (max_z - min_z) / chunks_h;

	return;
}


void find_face(C_DYNALLOC *vec, C_DYNALLOC **chunk, FILE *fp) {
	char buff[512];
	int v[4], i, qi;
	struct quadface q, *qp;
	rewind(fp);

	while (!feof(fp)) {
		*buff = 0;
		fscanf(fp, "%s", buff);
		if (strcmp(buff, "f")) {
			fgets(buff, 512, fp);
			continue;
		}
		
		v[0] = v[1] = v[2] = v[3] = 0;
		fscanf(fp, "%i %i %i %i\n", v[0], v[1], v[2], v[3]);
		for (i = 0; i < 4; i++)
			q.vec[i] = *((struct vector3 *) c_dynalloc_get(vec, v[i] - 1));
		qi = c_dynalloc_alloc(chunk[chunk_lookup(q.vec[0])]);
		*((struct quadface *) c_dynalloc_get(chunk[chunk_lookup(q.vec[0])], qi)) = q;

	}
	
	return;
}


int main(int argc, char **argv) {
	C_DYNALLOC *vec, *face;
	int i;
	FILE *fp;

	vec = c_dynalloc_init(200, sizeof(struct vector3));
	face = c_dynalloc_init(200, sizeof(struct quadface));
	if (!(fp = fopen(argv[1], "r")))
		fprintf(stderr, "Unable to open %s\n", argv[1]);
	find_vectors(vec, fp, atof(argv[2]));
	chunk = malloc(sizeof(*chunk) * chunks_w * chunks_h);
	for (i = 0; i < chunks_w * chunks_h; i++)
		chunk[i] = c_dynalloc_init(20, sizeof(*chunk[i]));
	find_face(vec, chunk, fp);
	
	fprintf(stderr, "Found %i vertices\n", c_dynalloc_entries(vec));
	fprintf(stderr, "This would become %i by %i chunks\n", chunks_w, chunks_h);
		
	return 0;
}

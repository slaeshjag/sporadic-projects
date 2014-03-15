#ifndef __HEIGHTMAP_H__
#define	__HEIGHTMAP_H__

struct heightmap {
	float			*height;
	int			w;
	int			h;
};

struct heightmap *heightmap_generate(int w, int h, int detail);

#endif

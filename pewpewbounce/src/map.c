#include <darnit/darnit.h>
#include "map.h"
#include "object.h"

struct Map map_s;

int map_unload() {
	int i;

	object_nuke();
	d_map_unload(map_s.map);
	map_s.map = NULL;
}


void map_follow_me(int id) {
	map_s.camera.follow = id;
}


void map_camera() {
	struct ObjectEntry *oe;
	int x, y, w, h, map_w, map_h;
	
	if (!map_s.map)
		return;
	if (map_s.camera.follow < 0)
		return;
	
	map_w = map_s.map->layer->tile_w * map_s.map->layer->tilemap->w;
	map_h = map_s.map->layer->tile_h * map_s.map->layer->tilemap->h;

	if (!(oe = c_dynalloc_get(obj.obj, map_s.camera.follow))) {
		fprintf(stderr, "Object doesn't exist\n");
		return (map_s.camera.follow = -1, ((void) 0));
	}

	d_sprite_hitbox(oe->sprite, &x, &y, &w, &h);
	x += oe->pos_x / 1000;
	y += oe->pos_y / 1000;
	x += w/2, y += h/2;

	map_s.camera.x += (x - map_s.camera.x - (int)d_platform_get().screen_w / 2) / 10; 
	map_s.camera.y += (y - map_s.camera.y - (int)d_platform_get().screen_h / 2) / 10;
	if (map_s.camera.x < 0)
		map_s.camera.x = 0;
	if (map_s.camera.y < 0)
		map_s.camera.y = 0;

	if (map_s.camera.x + d_platform_get().screen_w > map_w)
		map_s.camera.x = map_w - d_platform_get().screen_w;
	if (map_s.camera.y + d_platform_get().screen_h > map_h)
		map_s.camera.y = map_h - d_platform_get().screen_h;
	
	map_s.camera.lim_lx = map_s.camera.x - d_platform_get().screen_w / 2;
	map_s.camera.lim_ly = map_s.camera.y - d_platform_get().screen_w / 2;
	map_s.camera.lim_bx = d_platform_get().screen_w * 2;
	map_s.camera.lim_by = d_platform_get().screen_h * 2;
}


int map_load(const char *map) {
	int i, x, y, l;

	if (map_s.map)
		map_unload();
	if (!(map_s.map = d_map_load(map)))
		return 0;
	
	map_s.camera.x = map_s.camera.y = 0;
	map_s.camera.follow = -1;
	
	for (i = 0; i < map_s.map->objects; i++) {
		const char *sprite, *ai;
	
		l = map_s.map->object[i].l;
		x = map_s.map->object[i].x * map_s.map->layer[l].tile_w + map_s.map->layer[l].tile_w / 2;
		y = map_s.map->object[i].y * map_s.map->layer[l].tile_h + map_s.map->layer[l].tile_h / 2;
		sprite = d_map_prop(map_s.map->object[i].ref, "sprite");
		ai = d_map_prop(map_s.map->object[i].ref, "ai");
		object_spawn(sprite, ai, x, y, l, &map_s.map->object[i]);
	}

	return 1;
}


unsigned int map_get_blocki(int i, int layer) {
	return map_s.map->layer[layer].tilemap->data[i];
}


unsigned int map_get_block(int x, int y, int layer) {
	int tw, th, i;

	if (layer >= map_s.map->layers || layer < 0)
		return ~0;
	tw = map_s.map->layer[layer].tile_w;
	th = map_s.map->layer[layer].tile_h;
	x /= tw;
	y /= th;

	if (x > map_s.map->layer[layer].tilemap->w || y > map_s.map->layer[layer].tilemap->h || x < 0 || y < 0)
		return ~0;
	i = x + y * map_s.map->layer[layer].tilemap->w;
	return map_get_blocki(i, layer);
}


int map_kill_blocki(int i, int layer) {
	map_s.map->layer[layer].tilemap->data[i] = 0;
	d_tilemap_recalc(map_s.map->layer[layer].tilemap);

	/* TODO: Animate */
}


int map_get_solid(int x, int y, int dx, int dy, int layer) {
	int x2, y2, tw, th, i, i2, tx, ty;
	unsigned int mask;

	if (dx && dy)
		fprintf(stderr, "ERROR\n");
	if (map_s.map->layers <= layer)
		return 0;
	
	x2 = x + dx;
	y2 = y + dy;

	tw = map_s.map->layer[layer].tile_w;
	th = map_s.map->layer[layer].tile_h;
	x /= tw, x2 /= tw;
	y /= th, y2 /= th;
	i = x + y * map_s.map->layer[layer].tilemap->w;
	i2 = x2 + y2 * map_s.map->layer[layer].tilemap->w;
	if (i == i2)
		return 0;
	mask = dy?(dy<0?0x10000:0x40000):(dx<0?80000:0x20000);
	if (map_s.map->layer[layer].tilemap->data[i2] & mask)
		return 1 + i2;
	return 0;
}


int map_check_area(int x, int y, int dx, int dy, int layer, int w, int h) {
	int i;

	if (w)
		w -=1;
	if (h)
		h -= 1;
	if (w)
		for (;; w -= map_s.map->layer[layer].tile_w) {
			if (w < 0) w = 0;
			if ((i = map_get_solid(x + w, y, dx, dy, layer)))
				return i;
			if (!w)
				break;
		}
	if (h)
		for (;; h -= map_s.map->layer[layer].tile_h) {
			if (h < 0) h = 0;
			if ((i = map_get_solid(x, y + h, dx, dy, layer)))
				return i;
			if (!h)
				break;
		}
	return 0;
}


void map_render() {
	int i;
	
	if (!map_s.map)
		return;
	map_camera();
	for (i = 0; i < map_s.map->layers; i++) {
		d_tilemap_camera_move(map_s.map->layer[i].tilemap, map_s.camera.x, map_s.camera.y);
		d_render_offset(map_s.camera.x, map_s.camera.y);
		d_tilemap_draw(map_s.map->layer[i].tilemap);
		object_render(i);
	}

	d_render_offset(0, 0);

	return;
}

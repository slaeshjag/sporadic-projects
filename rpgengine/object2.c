#include <darnit/darnit.h>
#include <string.h>
#include "object.h"
#include "world.h"
#include "aicomm.h"
#include "aicomm_f.h"
#include "aicomm_handlers.h"

void character_expand_entries();
void character_update_sprite(int entry);


int character_get_character_looked_at(int src) {
	int x, y, w, h, xt, yt, n, i;
	unsigned int t[9];

	if (src < 0 || src >= ws.char_data->max_entries)
		return -1;
	if (!ws.char_data->entry[src])
		return -1;
	d_sprite_hitbox(ws.char_data->entry[src]->sprite, &x, &y, &w, &h);
	x += (ws.char_data->entry[src]->x >> 8);
	y += (ws.char_data->entry[src]->y >> 8);

	switch (ws.char_data->entry[src]->dir) {
		case 0:	/* West */
			xt = x - (w >> 1);
			yt = y + (h >> 1);
			break;
		case 1:	/* North */
			xt = x + (w >> 1);
			yt = y - (h >> 1);
			break;
		case 2:	/* East */
			xt = x + w;
			yt = y + (h >> 1);
			break;
		case 3:	/* South */
			xt = x + (w >> 1);
			yt = y + h;
			break;
		case 4: /* North-west */
			xt = x - (w >> 1);
			yt = y - (h >> 1);
			break;
		case 5:	/* North-east */
			xt = x + w;
			yt = y - (h >> 1);
			break;
		case 6:	/* South-east */
			xt = x + w;
			yt = y + h;
			break;
		case 7:	/* South-west */
			xt = x - (w >> 1);
			yt = y + h;
			break;
		default:
			return -1;
			break;
	}

	n = d_bbox_test(ws.char_data->bbox, xt, yt, (w >> 1), (h >> 1), t, 9);
	if (!n)
		return -1;
	for (i = 0; i < 9; i++)
		if (ws.char_data->entry[t[i]]->map == ws.dm->grid[4].id)
			return t[i];
	return -1;
}
			


int character_load_ai_lib(const char *fname) {
	int i;

	i = ws.char_data->ai_libs++;
	ws.char_data->ai_lib = realloc(ws.char_data->ai_lib, 
		sizeof(*ws.char_data->ai_lib) * ws.char_data->ai_libs);
	if (!(ws.char_data->ai_lib[i].lib = d_dynlib_open(fname))) {
		ws.char_data->ai_libs--;
		return 0;
	}

	ws.char_data->ai_lib[i].ainame = malloc(strlen(fname) + 1);
	strcpy(ws.char_data->ai_lib[i].ainame, fname);

	return 1;
}


void character_tell_all(struct aicomm_struct ac) {
	int i;

	for (i = 0; i < ws.char_data->max_entries; i++)
		ac.self = i, character_message_loop(ac);
	return;
}


int character_test_map(int entry, int dx, int dy) {
	int x, y, w, h, x2, y2, t1, t2, t3, t4, dir, d1, d2;
	struct character_entry *ce;
	struct aicomm_struct ac;
	
	if (d_keys_get().l)
		return 0;

	ce = ws.char_data->entry[entry];
	d_sprite_hitbox(ce->sprite, NULL, NULL, &w, &h);
	x2 = ((ce->x + dx) >> 8);
	y2 = ((ce->y + dy) >> 8);
	x = (ce->x >> 8);
	y = (ce->y >> 8);
	if (x < 0 || x2 < 0 || y < 0 || y2 < 0)
		return 1;
	
	if (!dx) {
		x2 += (w - 1);
		y += (dy > 0) ? h - 1 : 0;
		y2 += (dy > 0) ? h - 1 : 0;
		t1 = world_calc_tile(x, y, ce->l);
		t2 = world_calc_tile(x, y2, ce->l);
		t3 = world_calc_tile(x2, y, ce->l);
		t4 = world_calc_tile(x2, y2, ce->l);
		dir = (((dy < 0) ? 0x8 : 0x2) << 16);
	} else if (!dy) {
		y2 += (h - 1);
		x += (dx > 0) ? w - 1 : 0;
		x2 += (dx > 0) ? w - 1 : 0;
		t1 = world_calc_tile(x, y, ce->l);
		t2 = world_calc_tile(x2, y, ce->l);
		t3 = world_calc_tile(x, y2, ce->l);
		t4 = world_calc_tile(x2, y2, ce->l);
		dir = (((dx < 0) ? 0x4 : 0x1) << 16);
	}
		
	if (t1 < 0 || t2 < 0 || t3 < 0 || t4 < 0)
		return 1;
	if (t1 == t2 && t3 == t4)
		return 0;

	d1 = world_get_tile_i(t2, ce->l);
	d2 = world_get_tile_i(t4, ce->l);

	ac.from = -1;
	ac.msg = AICOMM_MSG_MAPE;
	ac.self = entry;
	if (d1 & MAP_FLAG_EVENT) {
		ac.arg[0] = t2;
		ac.arg[1] = d1;
		character_message_loop(ac);
	}

	if (d2 & MAP_FLAG_EVENT) {
		ac.arg[0] = t4;
		ac.arg[1] = d2;
		character_message_loop(ac);
	}
	
	return ((d1 & dir) || (d2 & dir));
}


int character_test_collision(int entry, int dx, int dy) {
	struct aicomm_struct ac;
	struct character_entry *ce;
	int x, y, w, h, e, i, s, n;

	if (d_keys_get().l)
		return 0;

	ce = ws.char_data->entry[entry];
	d_sprite_hitbox(ce->sprite, &x, &y, &w, &h);
	x += ((ce->x + dx) >> 8);
	y += ((ce->y + dy) >> 8);
	n = d_bbox_test(ws.char_data->bbox, x, y, w, h, (unsigned *) ws.char_data->collision, 
		ws.char_data->max_entries);
	
	ac.msg = AICOMM_MSG_COLL;
	ac.from = -1;
	
	for (i = s = 0; i < n; i++) {
		e = ws.char_data->collision[i];
		if (ws.char_data->entry[e]->l != ws.char_data->entry[ce->self]->l)
			continue;
		if (e == ce->self)
			continue;
		if (ws.char_data->entry[e]->map != ws.dm->grid[4].id)
			continue;
		ac.arg[0] = ce->self;
		ac.self = e;
		character_message_loop(ac);
		ac.arg[0] = e;
		ac.self = ce->self;
		character_message_loop(ac);
		if (ws.char_data->entry[entry]->special_action.solid
		    && ws.char_data->entry[e]->special_action.solid)
			s = 1;
		
	}

	return s;
}


void character_handle_movement(int entry) {
	int dx, dy;
	struct character_entry *e;

	e = ws.char_data->entry[entry];
	dx = e->dx;
	dy = e->dy;

	if (!dx && !dy)
		return;

	dx *= d_last_frame_time();
	dy *= d_last_frame_time();
	dx /= 1000;
	dy /= 1000;

	while (dx) {
		if (!character_test_collision(entry, dx, 0) && !character_test_map(entry, dx, 0)) {
			ws.char_data->entry[entry]->x += dx;
			break;
		}
		if (!(dx / 256))
			break;
		dx += (dx < 0) ? 256 : -256;
	}

	while (dy) {
		if (!character_test_collision(entry, 0, dy) && !character_test_map(entry, 0, dy)) {
			ws.char_data->entry[entry]->y += dy;
			break;
		}
		if (!(dy / 256))
			break;
		dy += (dy < 0) ? 256 : -256;
	}

	character_update_sprite(entry);

	e->dx = 0, e->dy = 0;
	
	return;
}


int character_find_visible() {
	return d_bbox_test(ws.char_data->bbox, ws.camera.x - 96, ws.camera.y - 96,
		ws.camera.screen_w + 192, ws.camera.screen_h + 192, 
		(unsigned *) ws.char_data->collision, ws.char_data->max_entries);
}
	

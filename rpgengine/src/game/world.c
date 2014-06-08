#include "map.h"
#include "world.h"
#include "textbox.h"

void world_init() {
	int n;

	/* TODO: Init world.config */
	world.config.tile_w = world.config.tile_h = 32;
	world.config.face_w = world.config.face_h = 64;
	world.config.font = d_font_load("res/font.ttf", WORLD_FONT_SIZE, 512, 512);
	world.config.ts_sys = d_render_tilesheet_load(WORLD_UI_PATH, world.config.tile_w, world.config.tile_h, DARNIT_PFORMAT_RGB5A1);

	map_init();

	n = world.config.tile_h * 4;
	textbox_init(d_platform_get().screen_w, n, 0, d_platform_get().screen_h - n, 10, 10, 10, 10);
	return;
}

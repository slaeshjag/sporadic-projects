#ifndef __WORLD_H__
#define	__WORLD_H__

#include "map.h"
#include "rpgtest.h"

#define	WORLD_FONT_SIZE		28
#define	WORLD_UI_PATH		"res/system_ui.png"

void world_init();


struct world_config_s {
	int			tile_w;
	int			tile_h;
	int			face_w;
	int			face_h;

	DARNIT_FONT		*font;
	DARNIT_TILESHEET	*ts_sys;
};


struct world_s {
	struct map_s		map;
	struct textbox		*textbox;
	enum state_e		state;
	enum state_e		new_state;

	struct world_config_s	config;
} world;

#endif

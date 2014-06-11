#ifndef __MENU_H__
#define	__MENU_H__

#include <darnit/darnit.h>

#include "settings.h"
#include "palette.h"
#include "text.h"
#include "list.h"

enum menu_widget_type_e {
	MENU_WIDGET_TYPE_TEXT,
	MENU_WIDGET_TYPE_LIST,
};


struct menu_widget_s {
	union {
		struct menu_widget_text_s	text;
		struct menu_widget_list_s	list;
	} widget;

	int					x;
	int					y;
	enum menu_widget_type_e			type;
	int					hidden;

	void					(*draw)(struct menu_widget_s *);
	void					(*destroy)(struct menu_widget_s *);
};


struct menu_s {
	int					w_t;
	int					h_t;

	int					display_x;
	int					display_y;

	struct menu_widget_s			*widget;
	int					widgets;

	DARNIT_TILEMAP				*tm;
};


struct menu_config_s {
	DARNIT_TILESHEET			*ts_sys;
	int					tile_w;
	int					tile_h;
};



void menu_init(const char *ts_sys, int tile_w, int tile_h);
struct menu_s *menu_new_container(int w_t, int h_t);
int menu_new_widget_slot(struct menu_s *m);
void menu_set_position(struct menu_s *m, int x, int y);
void menu_indicate_scroll(struct menu_s *m, int scroll);
void menu_set_widget_hidden(struct menu_s *m, int slot, int hidden);
void menu_draw(struct menu_s *m);
void menu_destroy(struct menu_s *m);


extern struct menu_config_s ui_config;

#endif

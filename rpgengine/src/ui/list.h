#ifndef __MENU_LIST_H__
#define	__MENU_LIST_H__

#include <darnit/darnit.h>

#include "menu.h"

struct menu_widget_list_s {
	DARNIT_TEXT_SURFACE	*surface;
	DARNIT_TILE		*pointer;

	int			selection;
	int			top_selection;
	int			selection_h;
	int			selection_w;

	char			*option_buff;
	DARNIT_SPRITE		**option_icon;
	int			options;

	struct menu_s		*m;
};

int menu_new_widget_list(struct menu_s *m, int x, int y, int list_w, int list_h, const char *options, const char *icons, DARNIT_FONT *font);

#endif

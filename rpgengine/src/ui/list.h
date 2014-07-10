#ifndef __MENU_LIST_H__
#define	__MENU_LIST_H__

#include <darnit/darnit.h>

#include "menu.h"

struct menu_widget_list_s {
	DARNIT_TEXT_SURFACE	*surface;
	DARNIT_TILE		*pointer;
	DARNIT_FONT		*font;

	int			selection;
	int			top_selection;
	int			selection_h;
	int			selection_w;

	int			status;

	char			*option_buff;
	DARNIT_SPRITE		**option_icon;
	int			options;

	struct menu_s		*m;
	struct menu_event_s	*e;
};

int menu_new_widget_list(struct menu_s *m, int x, int y, int list_w, int list_h, const char *options, const char *icons, DARNIT_FONT *font);
void menu_list_get_selection(struct menu_s *m, int slot, int *selection, int *activated);
void menu_list_await_selection(struct menu_s *m, int slot, int activate);

#endif

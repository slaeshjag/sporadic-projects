#ifndef __MENU_TEXT_H__
#define	__MENU_TEXT_H__

#include <darnit/darnit.h>

struct menu_s;
struct menu_widget_s;

struct menu_widget_text_s {
	DARNIT_TEXT_SURFACE			*surface;
};


int menu_new_widget_text(struct menu_s *m, int x, int y, int w, int glyphs, DARNIT_FONT *font);
DARNIT_TEXT_SURFACE *menu_get_surface_from_text(struct menu_s *m, int slot);
void menu_destroy_widget_text(struct menu_widget_s *widget);


#endif

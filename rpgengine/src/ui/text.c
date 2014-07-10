#include <darnit/darnit.h>

#include "menu.h"
#include "text.h"


void menu_draw_widget_text(struct menu_widget_s *widget) {
	d_text_surface_draw(widget->widget.text.surface);

	return;
}


void menu_destroy_widget_text(struct menu_widget_s *widget) {
	d_text_surface_free(widget->widget.text.surface);
	return;
}


int menu_new_widget_text(struct menu_s *m, int x, int y, int w, int glyphs, DARNIT_FONT *font) {
	int slot;
	
	slot = menu_new_widget_slot(m);
	m->widget[slot].type = MENU_WIDGET_TYPE_TEXT;
	m->widget[slot].x = x;
	m->widget[slot].y = y;
	m->widget[slot].widget.text.surface = d_text_surface_color_new(font, glyphs, w, 0, 0);
	
	m->widget[slot].draw = menu_draw_widget_text;
	m->widget[slot].destroy = menu_destroy_widget_text;

	return slot;
}


DARNIT_TEXT_SURFACE *menu_get_surface_from_text(struct menu_s *m, int slot) {
	return m->widget[slot].widget.text.surface;
}


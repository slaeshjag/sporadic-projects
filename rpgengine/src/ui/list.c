#include <darnit/darnit.h>
#include <string.h>

#include "menu.h"
#include "list.h"
#include "event.h"

void menu_list_update_selection(struct menu_widget_s *l) {
	char buff[128], *next;
	unsigned char *p, *ps;
	int i, n, py_center;
	struct menu_widget_list_s *ls = &l->widget.list;
	d_text_surface_reset(l->widget.list.surface);

	/* Check bounds */
	if (ls->selection >= ls->options)
		ls->selection = 0;
	if (ls->selection < 0)
		ls->selection = ls->options - 1;
	if (ls->selection - ls->selection_h + 1 > ls->top_selection && ls->options > ls->selection_h)
		ls->top_selection = ls->selection - ls->selection_h + 1;
	if (ls->selection < ls->top_selection)
		ls->top_selection = ls->selection;
	
	i = 0;
	if (ls->top_selection)
		i |= 2;
	if (ls->top_selection + ls->selection_h < ls->options)
		i |= 1;
	menu_indicate_scroll(ls->m, i);
	
	/* Seek selections */

	next = ls->option_buff;
	p = &menu_color_palette[PALETTE_DEFAULT_TEXT * 4];
	ps = &menu_color_palette[PALETTE_SELECTED_TEXT * 4];
	for (i = 0; i < ls->top_selection + ls->selection_h; i++) {
		if (i >= ls->options)
			break;
		if (i < ls->top_selection)
			goto iter;

		sscanf(next, "%[^\n]", buff);
		if (i == ls->selection)
			d_text_surface_color_next(ls->surface, ps[0], ps[1], ps[2]);
		else
			d_text_surface_color_next(ls->surface, p[0], p[1], p[2]);
		d_text_surface_string_append(ls->surface, buff);
		d_text_surface_char_append(ls->surface, "\n");

		iter:
		next = strchr(next, '\n') + 1;
	}

	/* Update pointing arrows */
	n = ls->selection - ls->top_selection;
	py_center = d_font_glyph_hs(ls->font) / 2 - ui_config.tile_h / 2;
	py_center += d_font_glyph_hs(ls->font) * n;
	d_render_tile_move(ls->pointer, 0, MENU_LIST_ARROW_PAD - ui_config.tile_w, py_center);
	d_render_tile_move(ls->pointer, 1, ls->selection_w - MENU_LIST_ARROW_PAD, py_center);
	
	/* Update icon positions */
	for (i = ls->top_selection; i < ls->options && i < ls->selection_h + ls->top_selection; i++) {
		if (!ls->option_icon[i])
			continue;
		py_center = d_font_glyph_hs(ls->font) / 2 - d_sprite_height(ls->option_icon[i]) / 2;
		py_center += d_font_glyph_hs(ls->font) * (i - ls->top_selection);
		d_sprite_hitbox(ls->option_icon[i], &n, NULL, NULL, NULL);
		d_sprite_move(ls->option_icon[i], -n + MENU_LIST_ARROW_PAD, py_center);
		if (i == ls->selection)
			d_sprite_animate_start(ls->option_icon[i]);
		else
			d_sprite_animate_stop(ls->option_icon[i]);
	}
}


void menu_draw_widget_list(struct menu_widget_s *w) {
	struct menu_event_status_s s;
	struct menu_widget_list_s *ls = &w->widget.list;
	int i;

	if (w->widget.list.status == -1) {
		/* Logic stuff should probably have their own function.. */
		s = menu_event_listen(w->widget.list.e);
		w->widget.list.status = s.activate - 1;
	
		w->widget.list.selection += s.scroll;
		if (s.scroll)
			menu_list_update_selection(w);
	}

	d_text_surface_draw(w->widget.list.surface);
	d_render_tile_draw(w->widget.list.pointer, 2);
	for (i = ls->top_selection; i < ls->top_selection + ls->selection_h && i < ls->options; i++)
		if (ls->option_icon[i])
			d_sprite_draw(ls->option_icon[i]);
	
	return;
}


void menu_destroy_widget_list(struct menu_widget_s *l) {
	int i;
	d_text_surface_free(l->widget.list.surface);
	d_render_tile_free(l->widget.list.pointer);
	
	for (i = 0; i < l->widget.list.options; i++)
		l->widget.list.option_icon[i] = d_sprite_free(l->widget.list.option_icon[i]);
	free(l->widget.list.option_icon);
	free(l->widget.list.option_buff);
	menu_event_destroy(l->widget.list.e);

	return;
}


int menu_new_widget_list(struct menu_s *m, int x, int y, int list_w, int list_h, const char *options, const char *icons, DARNIT_FONT *font) {
	int slot, i, slot_w, o, n, list_w_p;
	char *next, path[128];

	slot = menu_new_widget_slot(m);
	m->widget[slot].type = MENU_WIDGET_TYPE_LIST;
	m->widget[slot].x = x;
	m->widget[slot].y = y;
	m->widget[slot].hidden = 0;
	m->widget[slot].draw = menu_draw_widget_list;
	m->widget[slot].destroy = menu_destroy_widget_list;
	
	/* Count number of options/selections */
	for (m->widget[slot].widget.list.options = 1, next = (char *) options; (next = strchr(next, '\n')); m->widget[slot].widget.list.options++)
		if (next)
			next++;

	m->widget[slot].widget.list.status = -1;
	m->widget[slot].widget.list.selection = 0;
	m->widget[slot].widget.list.top_selection = 0;
	m->widget[slot].widget.list.selection_h = list_h;
	m->widget[slot].widget.list.selection_w = list_w;
	m->widget[slot].widget.list.font = font;
	m->widget[slot].widget.list.m = m;
	m->widget[slot].widget.list.e = menu_event_listener_new();

	m->widget[slot].widget.list.option_buff = strdup(options);

	/* Load in list option icons */
	m->widget[slot].widget.list.option_icon = malloc(sizeof(*m->widget[slot].widget.list.option_icon) * m->widget[slot].widget.list.options);
	next = (char *) icons;
	for (i = 0; i < m->widget[slot].widget.list.options; i++) {
		if (!next || !(*next))
			break;
		sscanf(next, "%[^\t\n]", path);
		m->widget[slot].widget.list.option_icon[i] = d_sprite_load(path, 0, DARNIT_PFORMAT_RGB5A1);
		next = strchr(next, '\n');
		if (next)
			next++;
	}

	for (; i < m->widget[slot].widget.list.options; i++)
		m->widget[slot].widget.list.option_icon[i] = NULL;
	slot_w = 0;
	for (i = 0; i < m->widget[slot].widget.list.options; i++) {
		if (!m->widget[slot].widget.list.option_icon[i])
			continue;
		d_sprite_hitbox(m->widget[slot].widget.list.option_icon[i], &o, NULL, &n, NULL);
		if (n - o > slot_w)
			slot_w = o + n;
	}

	/* Create the text surface that'll present the options to be selected from */
	list_w_p = list_w - slot_w - MENU_LIST_ARROW_PAD * 2;
	slot_w += MENU_LIST_ARROW_PAD;
	m->widget[slot].widget.list.surface = d_text_surface_color_new(font, 128 * list_h, list_w_p, slot_w, 0);

	/* Create tile buff for selection arrows */
	m->widget[slot].widget.list.pointer = d_render_tile_new(2, ui_config.ts_sys);
	d_render_tile_set(m->widget[slot].widget.list.pointer, 0, 12);
	d_render_tile_set(m->widget[slot].widget.list.pointer, 1, 13);

	menu_list_update_selection(&m->widget[slot]);
	return slot;
}


void menu_list_get_selection(struct menu_s *m, int slot, int *selection, int *activated) {
	if (selection)
		*selection = m->widget[slot].widget.list.selection;
	if (activated)
		*activated = m->widget[slot].widget.list.status + 1;
	return;
}


void menu_list_await_selection(struct menu_s *m, int slot, int activated) {
	m->widget[slot].widget.list.status = activated - 1;

	return;
}
	

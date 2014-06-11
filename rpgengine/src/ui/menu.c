#include <darnit/darnit.h>

#include "menu.h"

struct menu_config_s ui_config;

unsigned char menu_color_palette[1024] = {
	/* R, G, B, A */
	0x00, 0x00, 0x00, 0xFF,
	0x00, 0x00, 0xAA, 0xFF,
	0x00, 0xAA, 0x00, 0xFF,
	0x00, 0xAA, 0xAA, 0xFF,
	0xAA, 0x00, 0x00, 0xFF,
	0xAA, 0x00, 0xAA, 0xFF,
	0xAA, 0x55, 0x00, 0xFF,
	0xAA, 0xAA, 0xAA, 0xFF,
	0x55, 0x55, 0x55, 0xFF,
	0x55, 0x55, 0xAA, 0xFF,
	0xAA, 0x55, 0xAA, 0xFF,
	0x55, 0xFF, 0xFF, 0xFF,
	0xFF, 0x55, 0x55, 0xFF,
	0xFF, 0x55, 0xFF, 0xFF,
	0xFF, 0xFF, 0x55, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF,
};


void menu_init(const char *ts_sys, int tile_w, int tile_h) {
	ui_config.ts_sys = d_render_tilesheet_load(ts_sys, tile_w, tile_h, DARNIT_PFORMAT_RGB5A1);

	return;
}


static void menu_init_background(struct menu_s *m) {
	int i;
	m->tm = d_tilemap_new(256, ui_config.ts_sys, 0xFF, m->w_t, m->h_t);

	for (i = 1; i < m->w_t * m->h_t; i++)
		m->tm->data[i] = 5;
	for (i = 1; i < m->w_t - 1; i++)
		m->tm->data[i] = 2;
	for (i = m->w_t * (m->h_t - 1); i < m->w_t * m->h_t; i++)
		m->tm->data[i] = 8;
	for (i = 0; i < m->w_t * m->h_t; i += m->w_t)
		m->tm->data[i] = 4;
	for (i = m->w_t - 1; i < m->w_t * m->h_t; i += m->w_t)
		m->tm->data[i] = 6;
	m->tm->data[0] = 1;
	m->tm->data[m->w_t - 1] = 3;
	m->tm->data[m->w_t * (m->h_t - 1)] = 7;
	m->tm->data[m->w_t * m->h_t - 1] = 9;

	d_tilemap_recalc(m->tm);

	return;
}


int menu_new_widget_slot(struct menu_s *m) {
	m->widget = realloc(m->widget, sizeof(*m->widget) * (m->widgets + 1));
	m->widget[m->widgets].hidden = 0;
	return m->widgets++;
}


struct menu_s *menu_new_container(int w_t, int h_t) {
	struct menu_s *m;

	m = malloc(sizeof(*m));

	m->w_t = w_t, m->h_t = h_t;
	m->display_x = m->display_y = 0;
	m->widget = NULL;
	m->widgets = 0;
	
	menu_init_background(m);

	return m;
}


void menu_set_position(struct menu_s *m, int x, int y) {
	fprintf(stderr, "menu set position: %i %i\n", x, y);
	d_tilemap_camera_move(m->tm, -x, -y);

	m->display_x = x;
	m->display_y = y;

	return;
}


void menu_set_widget_hidden(struct menu_s *m, int slot, int hidden) {
	m->widget[slot].hidden = hidden;

	return;
}


void menu_indicate_scroll(struct menu_s *m, int scroll) {
	int update = 0;

	if (scroll & 1) {
		if (m->tm->data[m->tm->w * m->tm->h - 1] != 10)
			m->tm->data[m->tm->w * m->tm->h - 1] = 10, update = 1;
	} else {
		if (m->tm->data[m->tm->w * m->tm->h - 1] != 9)
		m->tm->data[m->tm->w * m->tm->h - 1] = 9, update = 1;
	}

	if (scroll & 2) {
		if (m->tm->data[m->tm->w - 1] != 11)
			m->tm->data[m->tm->w * m->tm->h - 1] = 11, update = 1;
	} else {
		if (m->tm->data[m->tm->w - 1] != 3)
		m->tm->data[m->tm->w * m->tm->h - 1] = 3, update = 1;
	}

	if (update)
		d_tilemap_recalc(m->tm);

	return;
}


void menu_draw(struct menu_s *m) {
	int i;

	d_tilemap_draw(m->tm);
	
	for (i = 0; i < m->widgets; i++)
		if (!m->widget[i].hidden) {
			d_render_offset(-m->display_x - m->widget[i].x, -m->display_y - m->widget[i].y);
			m->widget[i].draw(&m->widget[i]);
		}

	return;
}


void menu_destroy(struct menu_s *m) {
	fprintf(stderr, "STUB: menu_destroy()\n");
	return;
}

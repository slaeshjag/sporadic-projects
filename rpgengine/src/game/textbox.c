#define	_EMIT_PALETTE
#include "textbox.h"
#include "world.h"
#include "aicomm.h"
#include "object.h"
#include "ui/menu.h"
#include <string.h>
#include <limits.h>

#define	TB_SURFACE	(menu_get_surface_from_text(tb->tb_m, tb->text_slot))


void textbox_init(int w, int h, int x, int y, int pad_x, int pad_y, int pad_x2, int pad_y2) {
	struct textbox *tb;
	DARNIT_FILE *f;
	int i;

	tb = malloc(sizeof(*tb));

	tb->message = NULL;
	tb->char_pos = 0;
	tb->row = 0;
	tb->tb_m = menu_new_container(w / world.config.tile_w, h / world.config.tile_h);
	menu_set_position(tb->tb_m, x, y);

	/* TODO: Implement */
	tb->qb = NULL;

	tb->face = d_render_tile_new(1, NULL);
	tb->face_ts = NULL;
	w -= (w % world.config.tile_w);
	x += (w % world.config.tile_w) / 2;
	h -= (h % world.config.tile_h);
	y += (h % world.config.tile_h);

	tb->surface_w = (w - pad_x - pad_x2);
	tb->text_slot = menu_new_widget_text(tb->tb_m, pad_x, pad_y, tb->surface_w, 2048, world.config.font);
	tb->rows = (h - pad_y - pad_y2) / d_font_glyph_hs(world.config.font);
	tb->w = w, tb->h = h, tb->x = x, tb->y = y;

	tb->ms_per_char = DEFAULT_TEXT_SPEED;
	tb->dt = 0;
	if ((f = d_file_open("res/PALETTE.VGA", "rb"))) {
		d_file_read(menu_color_palette, 1024, f);
		d_file_close(f);
		for (i = 3; i < 1024; menu_color_palette[i] = 0xFF, i += 4);
	}

	world.textbox = tb;

	return;
}


void textbox_loop() {
	struct textbox *tb;
	unsigned char *p = menu_color_palette;
	struct aicomm_struct ac;
	int i, next, qb_sel, qb_act;
	DARNIT_KEYS k;

	/* TODO: Init */
	tb = world.textbox;
	next = 0;
	if (!tb->message)
		return;

	if (tb->qb)
		menu_list_get_selection(tb->qb, tb->qb_id, &qb_sel, &qb_act);
	
	/* Improve or somth.. */
	tb->dt += d_last_frame_time();
	for (; tb->dt > tb->ms_per_char && tb->message[tb->char_pos]; tb->dt -= tb->ms_per_char) {
		if (d_keys_get().BUTTON_ACCEPT || qb_act) {
			tb->dt = INT_MAX;
			next = 1;
			k = d_keys_zero();
			k.BUTTON_ACCEPT = 1;
			d_keys_set(k);
		}

		if (tb->row == tb->rows) {
			menu_indicate_scroll(tb->tb_m, 1);
			
			tb->dt = 0;
			break;
		}
			
		if (tb->message[tb->char_pos]) {
			if (tb->message[tb->char_pos] == ' ') {
				tb->char_pos++;
				d_text_surface_char_append(TB_SURFACE, " ");
				if (d_text_surface_pos(TB_SURFACE) + d_font_word_w(world.config.font, 
				    &tb->message[tb->char_pos], NULL) >= tb->current_surface_w) {
					d_text_surface_char_append(TB_SURFACE, "\n");
					d_text_surface_offset_next_set(TB_SURFACE, tb->pad_start);
					tb->row++;
				}
			} else if (tb->message[tb->char_pos] == '\n') {
				tb->char_pos++;
				d_text_surface_char_append(TB_SURFACE, "\n");
				d_text_surface_offset_next_set(TB_SURFACE, tb->pad_start);
				tb->row++;
			} else if (tb->message[tb->char_pos] == '\x01') {
				tb->char_pos++;
				i = (((unsigned) tb->message[tb->char_pos]) << 2);
				d_text_surface_color_next(TB_SURFACE, p[i], p[i+1], p[i+2]);
				tb->char_pos++;
				tb->dt += tb->ms_per_char;
			} else if (tb->message[tb->char_pos] == '\x02') {
				tb->char_pos++;
				i = ((unsigned char) tb->message[tb->char_pos]);
				tb->dt += tb->ms_per_char;
				tb->ms_per_char = i << 2;
				tb->char_pos++;
			} else if (tb->message[tb->char_pos] == '\x03') {
				tb->char_pos++;
				tb->dt += tb->ms_per_char;
				tb->ms_per_char = DEFAULT_TEXT_SPEED;
			} else
				tb->char_pos += d_text_surface_char_append(TB_SURFACE, 
					&tb->message[tb->char_pos]);
		} else
			break;
	}

	if (!tb->message[tb->char_pos]) {
		if (d_keys_get().BUTTON_ACCEPT || next || qb_act) {
			k = d_keys_zero();
			k.BUTTON_ACCEPT = 1;
			d_keys_set(k);
			
			/* Close textbox */
			free(tb->message), tb->message = NULL;
			d_render_tilesheet_free(tb->face_ts);
			tb->qb = menu_destroy(tb->qb);
			tb->face_ts = NULL;

			ac.msg = AICOMM_MSG_BOXR;
			ac.from = -1;
			ac.arg[0] = (qb_act >= 0) ? qb_sel : -1;
			ac.self = tb->char_pingback;
			object_message_loop(ac);

			ac.msg = AICOMM_MSG_SILE;
			ac.arg[0] = 0;
			object_tell_all(ac);
			return;
		}
	}

	if (next) {
		menu_indicate_scroll(tb->tb_m, 0);
		d_text_surface_reset(TB_SURFACE);
		d_text_surface_offset_next_set(TB_SURFACE, tb->pad_start);
		if (tb->qb)
			menu_list_await_selection(tb->qb, tb->qb_id, 0);
		tb->row = 0;
		next = 0;
	}

	return;
}


static int textbox_options(const char *str) {
	int i;
	char *next;

	next = (char *) str;
	for (i = 0; next; next = strchr(next, '\n'), i++)
		if (next)
			next++;
	return i;
}


void textbox_add_message(const char *message, const char *question, const char *face, int pingback) {
	struct textbox *tb = world.textbox;
	struct aicomm_struct ac;
	int blol, w, h, wt, ht;

	if (tb)
		free(tb->message), tb->message = NULL;
	tb->char_pos = 0;
	tb->row = 0;
	tb->dt = 0;
	tb->char_pingback = pingback;
	if (tb->face_ts)
		d_render_tilesheet_free(tb->face_ts), tb->face_ts = NULL;
	
	if (face) {
		tb->face_ts = d_render_tilesheet_load(face, world.config.face_w, world.config.face_h, DARNIT_PFORMAT_RGB5A1);
		d_render_tile_tilesheet(tb->face, tb->face_ts);
		d_render_tile_move(tb->face, 0, tb->x + 8, (tb->h - world.config.face_h) / 2 + tb->y);
		d_render_tile_set(tb->face, 0, 0);
		blol = world.config.face_w + 16;
	} else
		blol = 0;

	tb->current_surface_w = tb->surface_w - blol;
	tb->pad_start = blol;
	d_text_surface_reset(TB_SURFACE);

	tb->message = malloc(strlen(message) + 1);
	strcpy(tb->message, message);
	
	d_text_surface_offset_next_set(TB_SURFACE, tb->pad_start);

	ac.from = -1;
	ac.msg = AICOMM_MSG_SILE;
	ac.arg[0] = 1;
	object_tell_all(ac);

	/* Figure out question box */

	if (question) {
		w = 0;
		h = d_font_string_geometrics_o(world.config.font, question, d_platform_get().screen_w / 2, &w);
		w += world.config.tile_w * 2 + 5;
		h += 16;
		wt = w / world.config.tile_w;
		if (w % world.config.tile_w)
			wt++;
		ht = h / world.config.tile_h;
		if (h % world.config.tile_h)
			ht++;
	
		tb->qb = menu_new_container(wt, ht);
		menu_set_position(tb->qb, (d_platform_get().screen_w - wt * world.config.tile_w), (d_platform_get().screen_h - (ht + tb->tb_m->tm->h) * world.config.tile_h));
		h = textbox_options(question);
		tb->qb_id = menu_new_widget_list(tb->qb, 0, 0, wt * world.config.tile_w, h, question, NULL, world.config.font);
	} else
		tb->qb = NULL;
	d_keys_set(d_keys_get());

	return;
}


void textbox_draw() {
	struct textbox *tb = world.textbox;
	
	if (!tb->message)
		return;
	menu_draw(tb->tb_m);
	if (tb->qb)
		menu_draw(tb->qb);
	d_render_offset(0, 0);
	
	d_render_tile_draw(tb->face, 1);

	return;
}


void textbox_destroy() {
	struct textbox *tb = world.textbox;
	
	free(tb->message);
	menu_destroy(tb->tb_m);
	free(tb);
	world.textbox = NULL;

	return;
}


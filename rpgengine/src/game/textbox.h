#ifndef __TEXTBOX_H__
#define	__TEXTBOX_H__

#include <darnit/darnit.h>

#include "ui/menu.h"

#define	DEFAULT_TEXT_SPEED	30

struct textbox {
	char			*message;
	unsigned int		char_pos;
	unsigned int		row;
	unsigned int		rows;
	DARNIT_TILESHEET	*face_ts;
	DARNIT_TILE		*face;
	
	/* Textbox */
	struct menu_s		*tb_m;
	int			text_slot;

	/* Question box */
	struct menu_s		*qb;
	int			qb_id;

	unsigned int		surface_w;
	unsigned int		current_surface_w;
	unsigned int		pad_start;

	int			x;
	int			y;
	int			w;
	int			h;

	int			ms_per_char;
	int			dt;
	int			char_pingback;
};

void textbox_init(int w, int h, int x, int y, int pad_x, int pad_y, int pad_x2, int pad_y2);
void textbox_loop();
void textbox_add_message(const char *message, const char *question, const char *icons, const char *face, int pingback);
void textbox_draw();
void textbox_destroy();


#endif

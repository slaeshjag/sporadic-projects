#include <darnit/darnit.h>
#include <string.h>
#include "object.h"
#include "world.h"
#include "aicomm.h"
#include "aicomm_f.h"
#include "aicomm_handlers.h"

void character_expand_entries();
void character_update_sprite(int entry);


int character_load_ai_lib(const char *fname) {
	int i;

	i = ws.char_data->ai_libs++;
	ws.char_data->ai_lib = realloc(ws.char_data->ai_lib, 
		sizeof(*ws.char_data->ai_lib) * ws.char_data->ai_libs);
	if (!(ws.char_data->ai_lib[i].lib = d_dynlib_open(fname))) {
		ws.char_data->ai_libs--;
		return 0;
	}

	ws.char_data->ai_lib[i].ainame = malloc(strlen(fname) + 1);
	strcpy(ws.char_data->ai_lib[i].ainame, fname);

	return 1;
}


int character_find_visible() {
	return d_bbox_test(ws.char_data->bbox, ws.camera.x - 96, ws.camera.y - 96,
		ws.camera.screen_w + 192, ws.camera.screen_h + 192, 
		(unsigned *) ws.char_data->collision, ws.char_data->max_entries);
}
	

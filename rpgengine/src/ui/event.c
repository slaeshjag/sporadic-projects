#include <darnit/darnit.h>

#include "menu.h"
#include "event.h"


struct menu_event_s *menu_event_listener_new() {
	struct menu_event_s *e;

	e = calloc(sizeof(*e), 1);
	return e;
}


struct menu_event_status_s menu_event_listen(struct menu_event_s *e) {
	struct menu_event_status_s s;
	int d = 0;
	s.scroll = 0;
	s.activate = 0;
	DARNIT_KEYS k;

	k = d_keys_get();
	if (!k.down && !k.up && !k.left && !k.right) {
		e->direction = 0;
		e->triggered = 0;
		e->timeout = 0;
		e->repeat = 0;
	}

	if (d_keys_get().left) {
		s.scroll = -MENU_EVENT_STEP_BIG;
		k = d_keys_zero();
		k.left = 1;
		d_keys_set(k);
		e->direction = 0;
	}

	if (d_keys_get().right) {
		s.scroll = MENU_EVENT_STEP_BIG;
		k = d_keys_zero();
		k.right = 1;
		d_keys_set(k);
		e->direction = 0;
	}

	if (d_keys_get().BUTTON_ACCEPT) {
		d_keys_zero();
		k.BUTTON_ACCEPT = 1;
		d_keys_set(k);
		s.activate = 1;
	}
	
	if (d_keys_get().BUTTON_CANCEL) {
		d_keys_zero();
		k.BUTTON_CANCEL = 1;
		d_keys_set(k);
		s.activate = 1;
	}

	if (d_keys_get().up)
		d = -1;
	if (d_keys_get().down)
		d = 1;
	if (d) {
		if (e->direction != d || !e->triggered) {
			e->direction = d;
			e->triggered = 1;
			e->timeout = MENU_EVENT_ASCROLL_TIMEOUT;
			e->repeat = 0;
			s.scroll = d;
		} else {
			e->timeout -= d_last_frame_time();
			if (e->timeout <= 0) {
				e->timeout = 0;
				e->repeat -= d_last_frame_time();
				if (e->repeat <= 0) {
					s.scroll = d;
					e->repeat += MENU_EVENT_AREPEAT_TIMEOUT;
				}
			}
		}
	}

	return s;
}


void *menu_event_destroy(struct menu_event_s *e) {
	free(e);
	return NULL;
}

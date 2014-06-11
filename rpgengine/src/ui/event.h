#ifndef __MENU_EVENT_H__
#define	__MENU_EVENT_H__



struct menu_event_s {
	int				direction;
	int				triggered;
	int				timeout;
	int				repeat;
};


struct menu_event_status_s {
	int				scroll;
	int				activate;
};


struct menu_event_s *menu_event_listener_new();
struct menu_event_status_s menu_event_listen(struct menu_event_s *e);
void *menu_event_destroy(struct menu_event_s *e);


#endif

#ifndef __MENU_EVENT_H__
#define	__MENU_EVENT_H__

#define	MENU_EVENT_STEP_BIG		5
#define	MENU_EVENT_ASCROLL_TIMEOUT	500
#define	MENU_EVENT_AREPEAT_TIMEOUT	100


struct menu_event_s {
	int				direction;
	int				triggered;
	int				timeout;
	int				repeat;
};


struct menu_event_status_s {
	int				scroll;
};


struct menu_event_s *menu_event_listener_new();
struct menu_event_status_s menu_event_listen(struct menu_event_s *e);
void *menu_event_destroy(struct menu_event_s *e);


#endif

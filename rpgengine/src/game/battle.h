#ifndef __BATTLE_H__
#define	__BATTLE_H__

#include <darnit/darnit.h>
#include "party.h"

#define	BATTLE_SCREEN_MARGIN		10
#define	BATTLE_HPMP_LENGTH		128
#define	BATTLE_HPMP_HEIGHT		16

struct battle_ui_s {
	DARNIT_TILE			*hp_mp_meters;
};


struct battle_s {
	struct party_s			*party1;
	struct party_s			*party2;
	
	struct battle_ui_s		ui;
};


void battle_init();


#endif

#ifndef __AI_H__
#define	__AI_H__

#define	AI_PLAYER_VEL		300
#define	AI_PLAYER_VEL_BOOST	500
#define	AI_PLAYER_BOUNCE_TIME	100

void ai_test_init(int id);
void ai_test_kill(int id);
void ai_test_loop(int id);
void ai_test_collide(int mid, int fid, int xvelc, int yvelc, int xdir, int ydir);
void ai_test_collide_map(int id, int xdir, int ydir, int maptile);

void ai_player_init(int id);
void ai_player_kill(int id);
void ai_player_loop(int id);
void ai_player_collide(int mid, int fid, int xvelc, int yvelc, int xdir, int ydir);
void ai_player_collide_map(int id, int xdir, int ydir, int maptile);


struct AiPlayerState {
	int		dir;
	int		bounce;
};

#endif

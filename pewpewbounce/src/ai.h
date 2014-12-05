#ifndef __AI_H__
#define	__AI_H__


void ai_test_init(int id);
void ai_test_kill(int id);
void ai_test_loop(int id);
void ai_test_collide(int mid, int fid, int xvelc, int yvelc, int xdir, int ydir);
void ai_test_collide_map(int id, int xdir, int ydir);

void ai_player_loop(int id);

#endif

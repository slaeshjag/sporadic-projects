#ifndef __AI_CRAWLER_H__
#define	__AI_CRAWLER_H__

#define	AI_CRAWLER_DAMAGE	2

void ai_crawler_init(int id);
void ai_crawler_kill(int id);
void ai_crawler_loop(int id);
void ai_crawler_collide(int mid, int fid, int xvelc, int yvelc, int xdir, int ydir);
void ai_crawler_collide_map(int id, int xdir, int ydir, int maptile);

#endif

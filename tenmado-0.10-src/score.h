/* $Id: score.h,v 1.5 2002/07/10 11:27:15 oohara Exp $ */

#ifndef __TENMADO_SCORE_H__
#define __TENMADO_SCORE_H__

void clear_score(void);
int get_score(void);
int get_stage_score(int stage);
int add_score(int delta);
int show_score(void);
int add_damage_score(int hit_point, int damage);

#endif /* not __TENMADO_SCORE_H__ */

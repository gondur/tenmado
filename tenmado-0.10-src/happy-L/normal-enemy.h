/* $Id: normal-enemy.h,v 1.17 2002/06/16 15:44:11 oohara Exp $ */

#ifndef __TENMADO_NORMAL_ENEMY_H__
#define __TENMADO_NORMAL_ENEMY_H__

/* what */
#define NORMAL_BALL_SMALL 1
#define NORMAL_BALL_MEDIUM 2
#define NORMAL_BALL_LARGE 3
#define NORMAL_STING 4
/* time_shoot of "horizon" should be 1 */
#define NORMAL_HORIZON 5
#define NORMAL_PLENTY 6
#define NORMAL_BURST 7

/* type
 * these values must be one of 2^n
 */
/* gets hit by ATTR_OBSTACLE */
#define ENEMY_TYPE_WEAK 1
/* counts as ATTR_OBSTACLE as well as ATTR_ENEMY */
#define ENEMY_TYPE_OBSTACLE 2

tenm_object *normal_enemy_new(double x, double y, int what, int type,
                              int color, int time_no_escape, int signal_index,
                              int signal_suffix, int number_mode_move,
                              int number_mode_shoot, ...);

#endif /* not __TENMADO_NORMAL_ENEMY_H__ */

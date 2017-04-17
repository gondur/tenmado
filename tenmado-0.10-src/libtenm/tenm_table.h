/* $Id: tenm_table.h,v 1.16 2002/05/30 21:45:58 oohara Exp $ */

#ifndef __TENM_TABLE_H__
#define __TENM_TABLE_H__

#include "tenm_object.h"

int tenm_table_init(int size, int draw_min_priority, int draw_max_priority);
int tenm_table_add(tenm_object *object);
void tenm_table_delete(int i);
void tenm_table_clear_all(void);
int tenm_table_move(tenm_object *player, int turn_per_frame);
int tenm_table_detect_collision(tenm_object *player);
int tenm_table_do_action(tenm_object *player);
int tenm_table_draw(tenm_object *player);
void tenm_table_apply(int i, int (*func)(tenm_object *, int), int n);
void tenm_table_apply_all(int (*func)(tenm_object *, int), int n);

#endif /* not __TENM_TABLE_H__ */

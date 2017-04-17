/* $Id: util.h,v 1.14 2003/01/07 17:31:57 oohara Exp $ */

#ifndef __TENMADO_UTIL_H__
#define __TENMADO_UTIL_H__

#include "tenm_object.h"

int util_init(int width, int height);
void util_quit(void);
int draw_string(int x, int y, const char *string, int length);
int draw_string_int(int x, int y, const int *string, int length);
int in_window_object(const tenm_object *p);
int in_window_primitive(const tenm_primitive *p);

/* table manipulation function */
int delete_enemy_shot(tenm_object *my, int n);




#endif /* not __TENMADO_UTIL_H__ */

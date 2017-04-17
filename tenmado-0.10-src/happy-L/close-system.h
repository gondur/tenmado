/* $Id: close-system.h,v 1.6 2002/07/05 00:55:34 oohara Exp $ */

#ifndef __TENMADO_CLOSE_SYSTEM_H__
#define __TENMADO_CLOSE_SYSTEM_H__

#include "tenm_object.h"

/* close_system_ball_new direction */
#define CLOSE_SYSTEM_VERTICAL_FORWARD 1
#define CLOSE_SYSTEM_VERTICAL_BACKWARD -1
#define CLOSE_SYSTEM_HORIZONTAL_FORWARD 2
#define CLOSE_SYSTEM_HORIZONTAL_BACKWARD -2

tenm_object *close_system_wall_new(int n);
tenm_object *close_system_ball_new(int t, int n, int direction, 
                                   int time_shoot, int color);

#endif /* not __TENMADO_CLOSE_SYSTEM_H__ */

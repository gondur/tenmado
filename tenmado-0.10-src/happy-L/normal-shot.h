/* $Id: normal-shot.h,v 1.4 2002/06/12 09:14:25 oohara Exp $ */

#ifndef __TENMADO_NORMAL_SHOT_H__
#define __TENMADO_NORMAL_SHOT_H__

#include "tenm_object.h"

tenm_object *normal_shot_angle_new(double x, double y, double speed, int theta,
                                   int color, int life);
tenm_object *normal_shot_point_new(double x, double y, double speed,
                             double target_x, double target_y,
                             int color, int life);
tenm_object *normal_shot_new(double x, double y,
                             double speed_x, double speed_y,
                             int color, int life);

#endif /* not __TENMADO_NORMAL_SHOT_H__ */

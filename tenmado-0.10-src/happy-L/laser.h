/* $Id: laser.h,v 1.5 2002/05/19 07:40:30 oohara Exp $ */

#ifndef __TENMADO_LASER_H__
#define __TENMADO_LASER_H__

#include "tenm_object.h"

tenm_object *laser_angle_new(double x, double y, double speed, int theta,
                              double length, int color, int life);
tenm_object *laser_point_new(double x, double y, double speed,
                             double target_x, double target_y,
                             double length, int color, int life);
tenm_object *laser_new(double x, double y,
                       double speed_x, double speed_y,
                       double length_x, double length_y,
                       int color, int life);

#endif /* not __TENMADO_LASER_H__ */

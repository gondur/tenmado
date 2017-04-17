/* $Id: rotate-laser.h,v 1.2 2002/05/23 23:48:33 oohara Exp $ */

#ifndef __TENMADO_ROTATE_LASER_H__
#define __TENMADO_ROTATE_LASER_H__

#include "tenm_object.h"

tenm_object *rotate_laser_new(double x, double y,
                              double speed_x, double speed_y,
                              int theta, double speed_theta,
                              double length, int color, int life);

#endif /* not __TENMADO_ROTATE_LASER_H__ */

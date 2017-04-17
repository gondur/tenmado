/* $Id: balloon.h,v 1.4 2002/06/21 20:19:59 oohara Exp $ */

#ifndef __TENMADO_BALLOON_H__
#define __TENMADO_BALLOON_H__

#include "tenm_object.h"

tenm_object *balloon_new(double x, double radius, double speed_y,
                         double speed_r, int time_swell,
                         int time_swell_initial, int time_wait);

#endif /* __TENMADO_BALLOON_H__ */

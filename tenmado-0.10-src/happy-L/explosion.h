/* $Id: explosion.h,v 1.2 2002/05/11 02:29:00 oohara Exp $ */

#ifndef __TENMADO_EXPLOSION_H__
#define __TENMADO_EXPLOSION_H__
tenm_object *explosion_new(double x, double y, double dx, double dy,
                           int size_debris, int number_debris,
                           int color, double speed_debris, int life);

#endif /* not __TENMADO_EXPLOSION_H__ */

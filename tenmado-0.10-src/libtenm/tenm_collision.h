/* $Id: tenm_collision.h,v 1.21 2002/05/08 10:24:03 oohara Exp $ */

#ifndef __TENM_COLLISION_H__
#define __TENM_COLLISION_H__

#include "tenm_primitive.h"

/* don't use tenm_* here to avoid creating new primitives */
int tenm_same_point(double a_x, double a_y, double b_x, double b_y);
int tenm_point_nearer(double a_x, double a_y,
                      double b_x, double b_y, double n);
int tenm_line_nearer(double p_x, double p_y,
                     double a_x, double a_y, double b_x, double b_y,
                     double n);
int tenm_same_side(double p_x, double p_y, double q_x, double q_y,
                   double a_x, double a_y, double b_x, double b_y);
int tenm_same_side_vertical(double p_x, double p_y, double q_x, double q_y,
                            double a_x, double a_y, double b_x, double b_y,
                            double on_line_x, double on_line_y);

int tenm_collided_primitive(const tenm_primitive *p,
                            const tenm_primitive *q);

#endif /* not __TENM_COLLISION_H__ */

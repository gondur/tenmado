/* $Id: tenm_object.h,v 1.44 2003/02/11 11:19:37 oohara Exp $ */

#ifndef __TENM_OBJECT_H__
#define __TENM_OBJECT_H__

#include "tenm_primitive.h"

struct _tenm_mass
{
  /* number of primitives */
  int n;
  tenm_primitive **p;
};
typedef struct _tenm_mass tenm_mass;

struct _tenm_object
{
  int table_index;
  char *name;
  int attr;
  int hit_mask;
  int hit_point;
  double x;
  double y;
  /* number of count */
  int n;
  int *count;
  /* number of count_d */
  int n_d;
  double *count_d;
  tenm_mass *mass;
  /* pointer for these functions must be void because tenm_object is not
   * yet defined
   */
  int (*move)(void *, double);
  int (*hit)(void *, void *);
  /* action other than move (such as shoot) */
  int (*act)(void *, const void *);
  int (*draw)(void *, int);
  /* there is no need to put *tenm_image in this struct */
};
typedef struct _tenm_object tenm_object;

tenm_mass *tenm_mass_new(int number_primitive, tenm_primitive **p);
void tenm_mass_delete(tenm_mass *p);


tenm_object *tenm_object_new(const char *name, int attr, int hit_mask,
                             int hit_point, double x, double y,
                             int number_count, int *count,
                             int number_count_d, double *count_d,
                             int number_primitive, tenm_primitive **p,
                             int (*move)(tenm_object *, double),
                             int (*hit)(tenm_object *, tenm_object *),
                             int (*act)(tenm_object *, const tenm_object *),
                             int (*draw)(tenm_object *, int));
void tenm_object_delete(tenm_object *p);

int tenm_collided_mass(tenm_mass *p, tenm_mass *q);
int tenm_move_mass(tenm_mass *p, double dx, double dy);

#endif /* not __TENM_OBJECT_H__ */

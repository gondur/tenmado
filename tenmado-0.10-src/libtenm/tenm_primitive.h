/* $Id: tenm_primitive.h,v 1.6 2003/02/11 11:14:23 oohara Exp $ */

#ifndef __TENM_PRIMITIVE_H__
#define __TENM_PRIMITIVE_H__

struct _tenm_primitive
{
  int klass;
  void (*delete)(void *);
};
typedef struct _tenm_primitive tenm_primitive;

/* "parent" must be first to to use inheritance in C (not C++) */
struct _tenm_point
{
  tenm_primitive parent;
  
  double x;
  double y;
};
typedef struct _tenm_point tenm_point;

struct _tenm_circle
{
  tenm_primitive parent;

  tenm_point *center;
  /* radius */
  double r;
};
typedef struct _tenm_circle tenm_circle;

struct _tenm_segment
{
  tenm_primitive parent;

  /* each end of the segment */
  tenm_point *a;
  tenm_point *b;
};
typedef struct _tenm_segment tenm_segment;

struct _tenm_polygon
{
  tenm_primitive parent;

  /* number of vertexes */
  int n;
  /* coordinates of vertexes */
  tenm_point **v;
};
typedef struct _tenm_polygon tenm_polygon;

/* these values must be one of 3^n so that we can add any two of them
 * and get a unique number */
#define TENM_POINT 1
#define TENM_CIRCLE 3
#define TENM_SEGMENT 9
#define TENM_POLYGON 27

tenm_point *tenm_point_new(double x, double y);
void tenm_point_delete(tenm_point *p);
tenm_circle *tenm_circle_new(double center_x, double center_y, double radius);
void tenm_circle_delete(tenm_circle *p);
tenm_segment *tenm_segment_new(double a_x, double a_y, double b_x, double b_y);
void tenm_segment_delete(tenm_segment *p);
tenm_polygon *tenm_polygon_new(int number_vertex, ...);
void tenm_polygon_delete(tenm_polygon *p);

int tenm_move_primitive(tenm_primitive *p, double dx, double dy);

#endif /* not __TENM_PRIMITIVE_H__ */

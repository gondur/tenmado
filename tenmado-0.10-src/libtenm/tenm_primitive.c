/* $Id: tenm_primitive.c,v 1.8 2003/01/14 02:58:24 oohara Exp $ */
#include <stdio.h>
/* malloc */
#include <stdlib.h>
/* for tenm_polygon_new */
#include <stdarg.h>

#include "tenm_primitive.h"

#define NEAR_ZERO 0.0001

tenm_point *
tenm_point_new(double x, double y)
{
  tenm_point *temp;
  temp = (tenm_point *) malloc(sizeof(tenm_point));
  if (temp == NULL)
  {
    fprintf(stderr, "tenm_point_new: malloc failed\n");
    return NULL;
  }
  
  ((tenm_primitive *) temp)->klass = TENM_POINT;
  ((tenm_primitive *) temp)->delete = (void (*)(void *)) tenm_point_delete;
  temp->x = x;
  temp->y = y;
  return temp;
}

void
tenm_point_delete(tenm_point *p)
{
  /* sanity check */
  if (p == NULL)
  {
    fprintf(stderr, "tenm_point_delete: p is NULL\n");
    return;
  }

  free(p);
}

tenm_circle *
tenm_circle_new(double center_x, double center_y, double radius)
{
  tenm_circle *temp;

  /* sanity check */
  if (radius < NEAR_ZERO)
  {
    fprintf(stderr, "tenm_circle_new: radius is non-positive (%f)\n", radius);
    return NULL;
  }

  temp = (tenm_circle *) malloc(sizeof(tenm_circle));
  if (temp == NULL)
  {
    fprintf(stderr, "tenm_circle_new: malloc failed\n");
    return NULL;
  }
  
  ((tenm_primitive *) temp)->klass = TENM_CIRCLE;
  ((tenm_primitive *) temp)->delete = (void (*)(void *)) tenm_circle_delete;
  temp->center = tenm_point_new(center_x, center_y);
  if (temp->center == NULL)
  {
    fprintf(stderr, "tenm_circle_new: tenm_point_new failed\n");
    free(temp);
    return NULL;
  }
  temp->r = radius;
  return temp;
}

void tenm_circle_delete(tenm_circle *p)
{
  /* sanity check */
  if (p == NULL)
  {
    fprintf(stderr, "tenm_circle_delete: p is NULL");
    return;
  }
  if (p->center == NULL)
  {
    fprintf(stderr, "tenm_circle_delete: p->center is NULL");
    return;
  }
  
  tenm_point_delete(p->center);
  free(p);
}

tenm_segment *
tenm_segment_new(double a_x, double a_y, double b_x, double b_y)
{
  tenm_segment *temp;
  temp = (tenm_segment *) malloc(sizeof(tenm_segment));
  if (temp == NULL)
  {
    fprintf(stderr, "tenm_segment_new: malloc failed\n");
    return NULL;
  }

  ((tenm_primitive *) temp)->klass = TENM_SEGMENT;
  ((tenm_primitive *) temp)->delete = (void (*)(void *)) tenm_segment_delete;
  temp->a = tenm_point_new(a_x, a_y);
  if (temp->a == NULL)
  {
    fprintf(stderr, "tenm_segment_new: tenm_point_new(a) failed\n");
    free(temp);
    return NULL;
  }
  temp->b = tenm_point_new(b_x, b_y);
  if (temp->b == NULL)
  {
    fprintf(stderr, "tenm_segment_new: tenm_point_new(b) failed\n");
    tenm_point_delete(temp->a);
    free(temp);
    return NULL;
  }

  return temp;
}

void
tenm_segment_delete(tenm_segment *p)
{
  if (p == NULL)
  {
    fprintf(stderr, "tenm_segment_delete: p is NULL\n");
    return;
  }
  if (p->a == NULL)
  {
    fprintf(stderr, "tenm_segment_delete: p->a is NULL\n");
    return;
  }
  if (p->b == NULL)
  {
    fprintf(stderr, "tenm_segment_delete: p->b is NULL\n");
    return;
  }

  tenm_point_delete(p->b);
  tenm_point_delete(p->a);
  free(p);
}

/* ... is 3 or more of (double x, double y) */
tenm_polygon *
tenm_polygon_new(int number_vertex, ...)
{
  va_list ap;
  int i;
  int j;
  double x;
  double y;
  tenm_polygon *temp;

  /* sanity check */
  if (number_vertex < 3)
  {
    fprintf(stderr, "tenm_polygon_new: strange number_vertex (%d)\n",
            number_vertex);
    return NULL;
  }
  
  temp = (tenm_polygon *) malloc(sizeof(tenm_polygon));
  if (temp == NULL)
  {
    fprintf(stderr, "tenm_polygon_new: malloc failed\n");
    return NULL;
  }
  temp->v = (tenm_point **) malloc(sizeof(tenm_point *) * number_vertex);
  if (temp->v == NULL)
  {
    fprintf(stderr, "tenm_polygon_new: malloc(v) failed\n");
    free(temp);
    return NULL;
  }

  ((tenm_primitive *) temp)->klass = TENM_POLYGON;
  ((tenm_primitive *) temp)->delete = (void (*)(void *)) tenm_polygon_delete;

  va_start(ap, number_vertex);
  for (i = 0; i < number_vertex; i++)
  {
    x = va_arg(ap, double);
    y = va_arg(ap, double);
    temp->v[i] = tenm_point_new(x, y);
    if (temp->v[i] == NULL)
    {
      fprintf(stderr, "tenm_polygon_new: tenm_point_new(%d) failed\n", i);
      for (j = 0; j < i; j++)
        tenm_point_delete(temp->v[j]);
      free(temp);
      va_end(ap);
      return NULL;
    }
  }
  temp->n = number_vertex;
  va_end(ap);
  return temp;
}

void
tenm_polygon_delete(tenm_polygon *p)
{
  int j;
  if (p == NULL)
  {
    fprintf(stderr, "tenm_polygon_delete: p is NULL\n");
    return;
  }
  if (p->n < 3)
  {
    fprintf(stderr, "tenm_polygon_delete: strange p->n (%d)\n", p->n);
    return;
  }
  if (p->v == NULL)
  {
    fprintf(stderr, "tenm_polygon_delete: p->v is NULL\n");
    return;
  }
  
  for (j = 0; j < p->n; j++)
  {
    if (p->v[j] == NULL)
    {
      fprintf(stderr, "tenm_polygon_delete: p->v[%d] is NULL\n", j);
      return;
    }
    tenm_point_delete(p->v[j]);
  }

  free(p->v);
  free(p);
}

int
tenm_move_primitive(tenm_primitive *p, double dx, double dy)
{
  int i;

  if (p == NULL)
  {
    fprintf(stderr, "tenm_move_primitive: p is NULL\n");
    return 1;
  }
  switch (p->klass)
  {
  case TENM_POINT:
    ((tenm_point *) p)->x += dx;
    ((tenm_point *) p)->y += dy;
    return 0;
    break;
  case TENM_CIRCLE:
    if (((tenm_circle *) p)->center == NULL)
    {
      fprintf(stderr, "tenm_move_primitive: "
              "((tenm_circle *) p)->center is NULL\n");
      return 1;
    }
    ((tenm_circle *) p)->center->x += dx;
    ((tenm_circle *) p)->center->y += dy;
    return 0;
    break;
  case TENM_SEGMENT:
    if (((tenm_segment *) p)->a == NULL)
    {
      fprintf(stderr, "tenm_move_primitive: "
              "((tenm_segment *) p)->a is NULL\n");
      return 1;
    }
    if (((tenm_segment *) p)->b == NULL)
    {
      fprintf(stderr, "tenm_move_primitive: "
              "((tenm_segment *) p)->b is NULL\n");
      return 1;
    }
    ((tenm_segment *) p)->a->x += dx;
    ((tenm_segment *) p)->a->y += dy;
    ((tenm_segment *) p)->b->x += dx;
    ((tenm_segment *) p)->b->y += dy;
    return 0;
    break;
  case TENM_POLYGON:
    if (((tenm_polygon *) p)->v == NULL)
    {
      fprintf(stderr, "tenm_move_primitive: "
              "((tenm_polygon *) p)->v is NULL\n");
      return 1;
    }
    if (((tenm_polygon *) p)->n < 3)
    {
      fprintf(stderr, "tenm_move_primitive: "
              "strange ((tenm_polygon *) p)->n (%d)\n",
              ((tenm_polygon *) p)->n);
      return 1;
    }
    for (i = 0; i < ((tenm_polygon *) p)->n; i++)
    {
      if (((tenm_polygon *) p)->v[i] == NULL)
      {
        fprintf(stderr, "tenm_move_primitive: "
                "((tenm_polygon *) p)->v[%d] is NULL\n", i);
        return 1;
      }
      ((tenm_polygon *) p)->v[i]->x += dx;
      ((tenm_polygon *) p)->v[i]->y += dy;
    }
    return 0;
    break;
  default:
    fprintf(stderr, "tenm_move_primitive: strange primitive found (%d)\n",
            p->klass);
    return 1;
    break;
  }
  /* should not reach here */
  fprintf(stderr, "tenm_move_primitive: should not reach here\n");
  return 1;
}

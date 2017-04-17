/* $Id: tenm_collision.c,v 1.83 2003/01/14 01:03:14 oohara Exp $ */
#include <stdio.h>
/* malloc */
#include <stdlib.h>
/* for tenm_polygon_new */
#include <stdarg.h>

#include "tenm_collision.h"

#define NEAR_ZERO 0.0001

static double tenm_same_side_temp(double p_x, double p_y,
                                  double a_x, double a_y,
                                  double b_x, double b_y);
static double tenm_same_side_vertical_temp(double p_x, double p_y,
                                           double a_x, double a_y,
                                           double b_x, double b_y,
                                           double on_line_x, double on_line_y);

static int tenm_collided_point_point(const tenm_point *p,
                                     const tenm_point *q);
static int tenm_collided_point_circle(const tenm_point *p,
                                      const tenm_circle *q);
static int tenm_collided_point_segment(const tenm_point *p,
                                       const tenm_segment *q);
static int tenm_collided_point_polygon(const tenm_point *p,
                                       const tenm_polygon *q);
static int tenm_collided_circle_circle(const tenm_circle *p,
                                       const tenm_circle *q);
static int tenm_collided_circle_segment(const tenm_circle *p,
                                        const tenm_segment *q);
static int tenm_collided_circle_segment2(double p_x, double p_y, double p_r,
                                         double q_a_x, double q_a_y,
                                         double q_b_x, double q_b_y);
static int tenm_collided_circle_polygon(const tenm_circle *p,
                                        const tenm_polygon *q);
static int tenm_collided_segment_segment(const tenm_segment *p,
                                         const tenm_segment *q);
static int tenm_collided_segment_segment2(double p_a_x, double p_a_y,
                                          double p_b_x, double p_b_y,
                                          double q_a_x, double q_a_y,
                                          double q_b_x, double q_b_y);
static int tenm_collided_segment_polygon(const tenm_segment *p,
                                         const tenm_polygon *q);
static int tenm_collided_polygon_polygon(const tenm_polygon *p,
                                         const tenm_polygon *q);

int
tenm_same_point(double a_x, double a_y, double b_x, double b_y)
{
  /* assume that a point (really a pixel) is a circle whose diameter is 1 */
  return tenm_point_nearer(a_x, a_y, b_x, b_y, 1.0);
}

/* checks if the distance between point A(a_x, a_y) and point B(b_x, b_y) is
 * equal to or smaller than n
 * return 1 (true) or 0 (false)
 */
int
tenm_point_nearer(double a_x, double a_y, double b_x, double b_y, double n)
{
  /* distance^2 = temp = temp1 + temp2 */
  double temp = 0;
  double temp1 = 0;
  double temp2 = 0;

  /* distance can't be negative*/
  if (n < -NEAR_ZERO)
  {
    fprintf(stderr, "tenm_point_nearer: n is negative (%f)\n", n);
    return 0;
  }
  
  if ((n > -NEAR_ZERO) && (n < NEAR_ZERO))
    return tenm_same_point(a_x, a_y, b_x, b_y);

  temp1 = a_x - b_x;
  temp1 = temp1 * temp1;
  temp2 = a_y - b_y;
  temp2 = temp2 * temp2;
  temp = temp1 + temp2;

  if (n * n >= temp)
    return 1;
  return 0;
}

/* checks if the distance between the point P and the line is equal to
 * or smaller than n
 * return 1 (true) or 0 (false)
 */
int
tenm_line_nearer(double p_x, double p_y,
                 double a_x, double a_y, double b_x, double b_y, double n)
{
  /* distance^2 = temp/over */
  double temp = 0;
  double over = 0;
  /* over = over1 + over2 */
  double over1 = 0;
  double over2 = 0;

  /* distance can't be nagative */
  if (n < -NEAR_ZERO)
  {
    fprintf(stderr, "tenm_line_nearer: n is negative (%f)\n", n);
    return 0;
  }
  
  /* check if line is really a point */
  if(tenm_same_point(a_x, a_y, b_x, b_y))
  {
    return tenm_point_nearer(p_x, p_y, a_x, a_y, n);
  }
  
  temp = (a_x - p_x) * (b_y - a_y) - (a_y - p_y) * (b_x - a_x);
  temp = temp * temp;
  over1= b_x - a_x;
  over1 = over1 * over1;
  over2 = b_y - a_y;
  over2 = over2 * over2;
  over = over1 + over2;

  if (over * n * n >= temp)
    return 1;
  return 0;
}

/* checks if the two points P(p_x, p_y) and Q(q_x, q_y) are on the same
 * side of the line which passes A(a_x, a_y) and B(b_x, b_y)
 * "on the line" counts as either side
 * return 1 (true) or 0 (false)
 */
int
tenm_same_side(double p_x, double p_y, double q_x, double q_y,
               double a_x, double a_y, double b_x, double b_y)
{
  double temp_p;
  double temp_q;

  /* check if we really have a line */
  if (tenm_same_point(a_x, a_y, b_x, b_y))
  {
    fprintf(stderr, "tenm_same_side: not a line\n");
    return 0;
  }

  temp_p = tenm_same_side_temp(p_x, p_y, a_x, a_y, b_x, b_y);
  temp_q = tenm_same_side_temp(q_x, q_y, a_x, a_y, b_x, b_y);
  if ((temp_p >= 0) && (temp_q >= 0))
    return 1;
  if ((temp_p <= 0) && (temp_q <= 0))
    return 1;
  return 0;
}

/* local-only function used in tenm_same_side */
static double
tenm_same_side_temp(double p_x, double p_y,
                    double a_x, double a_y, double b_x, double b_y)
{
  return (b_y - a_y) * p_x - (b_x - a_x) * p_y
    -((b_y - a_y) * a_x - (b_x - a_x) * a_y);
}

/* checks if the two points P(p_x, p_y) and Q(q_x, q_y) are on the same
 * side of the line L
 * such that:
 * 1) L passes the point (on_line_x, on_line_y)
 * 2) L is vertical to the line which passes (a_x, a_y) and (b_x, b_y)
 * "on the line" counts as either side
 * return 1 (true) or 0 (false)
 */
int
tenm_same_side_vertical(double p_x, double p_y, double q_x, double q_y,
                        double a_x, double a_y, double b_x, double b_y,
                        double on_line_x, double on_line_y)
{
  double temp_p;
  double temp_q;

  /* check if we really have a line */
  if (tenm_same_point(a_x, a_y, b_x, b_y))
  {
    fprintf(stderr, "tenm_same_side_vertical: not a line\n");
    return 0;
  }

  temp_p = tenm_same_side_vertical_temp(p_x, p_y,
                                        a_x, a_y, b_x, b_y,
                                        on_line_x, on_line_y);
  temp_q = tenm_same_side_vertical_temp(q_x, q_y,
                                        a_x, a_y, b_x, b_y,
                                        on_line_x, on_line_y);
  if ((temp_p >= 0) && (temp_q >= 0))
    return 1;
  if ((temp_p <= 0) && (temp_q <= 0))
    return 1;
  return 0;
}

/* local-only function used in tenm_same_side_vertical */
static double
tenm_same_side_vertical_temp(double p_x, double p_y,
                             double a_x, double a_y, double b_x, double b_y,
                             double on_line_x, double on_line_y)
{
  return (b_x - a_x) * p_x + (b_y - a_y) * p_y
    -((b_x - a_x) * on_line_x + (b_y - a_y) * on_line_y);
}

/* optimized under the assumption that p and q are usually not collided */
int
tenm_collided_primitive(const tenm_primitive *p,
                        const tenm_primitive *q)
{
  const tenm_primitive *p_temp;
  const tenm_primitive *q_temp;
  
  if (p == NULL)
  {
    fprintf(stderr, "tenm_collided_primitive: p is NULL\n");
    return 0;
  }
  if (q == NULL)
  {
    fprintf(stderr, "tenm_collided_primitive: q is NULL\n");
    return 0;
  }

  switch (p->klass + q->klass)
  {
  case TENM_POINT + TENM_POINT:
    return tenm_collided_point_point((const tenm_point *) p,
                                     (const tenm_point *) q);
    break;
  case TENM_POINT + TENM_CIRCLE:
    if (p->klass == TENM_POINT)
    {
      p_temp = p;
      q_temp = q;
    }
    else if (q->klass == TENM_POINT)
    {
      p_temp = q;
      q_temp = p;
    }
    else
    {
      fprintf(stderr, "tenm_collided_primitive: strange pointer found in "
              "TENM_POINT + TENM_CIRCLE\n");
      return 0;
    }
    return tenm_collided_point_circle((const tenm_point *) p_temp,
                                      (const tenm_circle *) q_temp);
    break;
  case TENM_POINT + TENM_SEGMENT:
    if (p->klass == TENM_POINT)
    {
      p_temp = p;
      q_temp = q;
    }
    else if (q->klass == TENM_POINT)
    {
      p_temp = q;
      q_temp = p;
    }
    else
    {
      fprintf(stderr, "tenm_collided_primitive: strange pointer found in "
              "TENM_POINT + TENM_SEGMENT\n");
      return 0;
    }
    return tenm_collided_point_segment((const tenm_point *) p_temp,
                                       (const tenm_segment *) q_temp);
    break;
  case TENM_POINT + TENM_POLYGON:
    if (p->klass == TENM_POINT)
    {
      p_temp = p;
      q_temp = q;
    }
    else if (q->klass == TENM_POINT)
    {
      p_temp = q;
      q_temp = p;
    }
    else
    {
      fprintf(stderr, "tenm_collided_primitive: strange pointer found in "
              "TENM_POINT + TENM_POLYGON\n");
      return 0;
    }
    return tenm_collided_point_polygon((const tenm_point *) p_temp,
                                       (const tenm_polygon *) q_temp);
    break;
  case TENM_CIRCLE + TENM_CIRCLE:
    return tenm_collided_circle_circle((const tenm_circle *) p,
                                       (const tenm_circle *) q);
    break;
  case TENM_CIRCLE + TENM_SEGMENT:
    if (p->klass == TENM_CIRCLE)
    {
      p_temp = p;
      q_temp = q;
    }
    else if (q->klass == TENM_CIRCLE)
    {
      p_temp = q;
      q_temp = p;
    }
    else
    {
      fprintf(stderr, "tenm_collided_primitive: strange pointer found in "
              "TENM_CIRCLE + TENM_SEGMENT\n");
      return 0;
    }
    return tenm_collided_circle_segment((const tenm_circle *) p_temp,
                                        (const tenm_segment *) q_temp);
    break;
  case TENM_CIRCLE + TENM_POLYGON:
    if (p->klass == TENM_CIRCLE)
    {
      p_temp = p;
      q_temp = q;
    }
    else if (q->klass == TENM_CIRCLE)
    {
      p_temp = q;
      q_temp = p;
    }
    else
    {
      fprintf(stderr, "tenm_collided_primitive: strange pointer found in "
              "TENM_CIRCLE + TENM_POLYGON\n");
      return 0;
    }
    return tenm_collided_circle_polygon((const tenm_circle *) p_temp,
                                        (const tenm_polygon *) q_temp);
    break;
  case TENM_SEGMENT + TENM_SEGMENT:
    return tenm_collided_segment_segment((const tenm_segment *) p,
                                         (const tenm_segment *) q);
    break;
  case TENM_SEGMENT + TENM_POLYGON:
    if (p->klass == TENM_SEGMENT)
    {
      p_temp = p;
      q_temp = q;
    }
    else if (q->klass == TENM_SEGMENT)
    {
      p_temp = q;
      q_temp = p;
    }
    else
    {
      fprintf(stderr, "tenm_collided_primitive: strange pointer found in "
              "TENM_SEGMENT + TENM_POLYGON\n");
      return 0;
    }
    return tenm_collided_segment_polygon((const tenm_segment *) p_temp,
                                         (const tenm_polygon *) q_temp);
    break;
  case TENM_POLYGON + TENM_POLYGON:
    return tenm_collided_polygon_polygon((const tenm_polygon *) p,
                                         (const tenm_polygon *) q);
    break;
  default:
    fprintf(stderr, "tenm_collided_primitive: strange combination of "
            "primitives (%d and %d)\n", p->klass, q->klass);
    return 0;
    break;
  }
  
  return 0;
}

static int
tenm_collided_point_point(const tenm_point *p,
                          const tenm_point *q)
{
  return tenm_same_point(p->x, p->y, q->x, q->y);
}

static int
tenm_collided_point_circle(const tenm_point *p,
                           const tenm_circle *q)
{
  return tenm_point_nearer(p->x, p->y, q->center->x, q->center->y, q->r);
}

static int
tenm_collided_point_segment(const tenm_point *p,
                            const tenm_segment *q)
{
  /* assume that a point (really a pixel) is a circle whose diameter is 1 */
  return tenm_collided_circle_segment2(p->x, p->y, 0.5,
                                       q->a->x, q->a->y, q->b->x, q->b->y);
}

static int
tenm_collided_point_polygon(const tenm_point *p,
                            const tenm_polygon *q)
{
  /* I know that the best algorithm is O(n * log(n)), but it is hard
   * to implement */
  int i;
  int j;
  int k;
  int l;
  
  if (q->n <= 0)
  {
    fprintf(stderr, "tenm_collided_point_polygon: strange point number in q "
            "(%d)\n", q->n);
    return 0;
  }
  if (q->n == 1)
    return tenm_same_point(p->x, p->y, q->v[0]->x, q->v[0]->y);
  if (q->n == 2)
    return tenm_collided_circle_segment2(p->x, p->y, 0.5,
                                         q->v[0]->x, q->v[0]->y,
                                         q->v[1]->x, q->v[1]->y);

  for (i = 0; i < q->n; i++)
    for (j = i + 1; j < q->n; j++)
    {
      for (k = 0; k < q->n; k++)
      {
        if (tenm_same_point(q->v[k]->x, q->v[k]->y, q->v[i]->x, q->v[i]->y))
          continue;
        if (tenm_same_point(q->v[k]->x, q->v[k]->y, q->v[j]->x, q->v[j]->y))
          continue;

        if (q->n >= 4)
        {
          for (l = 0;(l < q->n)&&((tenm_same_point(q->v[l]->x, q->v[l]->y,
                                                   q->v[i]->x, q->v[i]->y))
                                  ||(tenm_same_point(q->v[l]->x, q->v[l]->y,
                                                     q->v[j]->x, q->v[j]->y))
                                  ||(tenm_same_point(q->v[l]->x, q->v[l]->y,
                                                     q->v[k]->x, q->v[k]->y)));
               l++)
            ;

          if ((l < q->n) && (! tenm_same_side(q->v[k]->x, q->v[k]->y,
                                              q->v[l]->x, q->v[l]->y,
                                              q->v[i]->x, q->v[i]->y,
                                              q->v[j]->x, q->v[j]->y)))
            break;
        }
        
        if (! tenm_same_side(p->x, p->y, q->v[k]->x, q->v[k]->y,
                             q->v[i]->x, q->v[i]->y, q->v[j]->x, q->v[j]->y))
          return 0;
      }
    }
  return 1;
}

static int
tenm_collided_circle_circle(const tenm_circle *p,
                            const tenm_circle *q)
{
  return tenm_point_nearer(p->center->x, p->center->y,
                           q->center->x, q->center->y, p->r + q->r);
}

static int
tenm_collided_circle_segment(const tenm_circle *p,
                             const tenm_segment *q)
{
  return tenm_collided_circle_segment2(p->center->x, p->center->y, p->r,
                                       q->a->x, q->a->y, q->b->x, q->b->y);
}

static int
tenm_collided_circle_segment2(double p_x, double p_y, double p_r,
                              double q_a_x, double q_a_y,
                              double q_b_x, double q_b_y)
{
  if (tenm_point_nearer(p_x, p_y, q_a_x, q_a_y, p_r))
    return 1;
  if (tenm_point_nearer(p_x, p_y, q_b_x, q_b_y, p_r))
    return 1;

  if (! tenm_line_nearer(p_x, p_y,
                         q_a_x, q_a_y, q_b_x, q_b_y, p_r))
    return 0;
  if (tenm_same_side_vertical(q_a_x, q_a_y, q_b_x, q_b_y,
                              q_a_x, q_a_y, q_b_x, q_b_y,
                              p_x, p_y))
    return 0;
  return 1;
}

static int
tenm_collided_circle_polygon(const tenm_circle *p,
                             const tenm_polygon *q)
{
  int i;
  int j;
  int temp;

  /* first, assume a sufficiently large rectangle */
  temp = 1;
  for (i = 0; i < q->n; i++)
    if (p->center->x + p->r >= q->v[i]->x)
    {
      temp = 0;
      break;
    }
  if (temp == 1)
    return 0;

  temp = 1;
  for (i = 0; i < q->n; i++)
    if (p->center->x - p->r <= q->v[i]->x)
    {
      temp = 0;
      break;
    }
  if (temp == 1)
    return 0;

  temp = 1;
  for (i = 0; i < q->n; i++)
    if (p->center->y + p->r >= q->v[i]->y)
    {
      temp = 0;
      break;
    }
  if (temp == 1)
    return 0;

  temp = 1;
  for (i = 0; i < q->n; i++)
    if (p->center->y - p->r <= q->v[i]->y)
    {
      temp = 0;
      break;
    }
  if (temp == 1)
    return 0;

  /* now, the expensive check */
  for (i = 0; i < q->n; i++)
    for (j = i + 1; j < q->n; j++)
      if (tenm_collided_circle_segment2(p->center->x, p->center->y, p->r,
                                        q->v[i]->x, q->v[i]->y,
                                        q->v[j]->x, q->v[j]->y))
          return 1;
  
  return tenm_collided_point_polygon(p->center, q);
}
static int
tenm_collided_segment_segment(const tenm_segment *p,
                              const tenm_segment *q)
{
  return tenm_collided_segment_segment2(p->a->x, p->a->y, p->b->x, p->b->y,
                                        q->a->x, q->a->y, q->b->x, q->b->y);
}

static int
tenm_collided_segment_segment2(double p_a_x, double p_a_y,
                               double p_b_x, double p_b_y,
                               double q_a_x, double q_a_y,
                               double q_b_x, double q_b_y)
{
  /* we need this point-on-segment checks first because tenm_same_side
   * counts "on the line" as either side
   */
  if (tenm_collided_circle_segment2(p_a_x, p_a_y, 0.5,
                                    q_a_x, q_a_y, q_b_x, q_b_y))
    return 1;
  if (tenm_collided_circle_segment2(p_b_x, p_b_y, 0.5,
                                    q_a_x, q_a_y, q_b_x, q_b_y))
    return 1;
  if (tenm_collided_circle_segment2(q_a_x, q_a_y, 0.5,
                                    p_a_x, p_a_y, p_b_x, p_b_y))
    return 1;
  if (tenm_collided_circle_segment2(q_b_x, q_b_y, 0.5,
                                    p_a_x, p_a_y, p_b_x, p_b_y))
    return 1;

  if (tenm_same_side(p_a_x, p_a_y, p_b_x, p_b_y,
                     q_a_x, q_a_y, q_b_x, q_b_y))
    return 0;
  if (tenm_same_side(q_a_x, q_a_y, q_b_x, q_b_y,
                     p_a_x, p_a_y, p_b_x, p_b_y))
    return 0;
  return 1;
}

static int
tenm_collided_segment_polygon(const tenm_segment *p,
                              const tenm_polygon *q)
{
  int i;
  int j;
  int temp;

  /* first, assume a sufficiently large rectangle */
  temp = 1;
  for (i = 0; i < q->n; i++)
  {
    if (p->a->x >= q->v[i]->x)
    {
      temp = 0;
      break;
    }
    if (p->b->x >= q->v[i]->x)
    {
      temp = 0;
      break;
    }
  }
  if (temp == 1)
    return 0;

  temp = 1;
  for (i = 0; i < q->n; i++)
  {
    if (p->a->x <= q->v[i]->x)
    {
      temp = 0;
      break;
    }
    if (p->b->x <= q->v[i]->x)
    {
      temp = 0;
      break;
    }
  }
  if (temp == 1)
    return 0;

  temp = 1;
  for (i = 0; i < q->n; i++)
  {
    if (p->a->y >= q->v[i]->y)
    {
      temp = 0;
      break;
    }
    if (p->b->y >= q->v[i]->y)
    {
      temp = 0;
      break;
    }
  }
  if (temp == 1)
    return 0;

  temp = 1;
  for (i = 0; i < q->n; i++)
  {
    if (p->a->y <= q->v[i]->y)
    {
      temp = 0;
      break;
    }
    if (p->b->y <= q->v[i]->y)
    {
      temp = 0;
      break;
    }
  }
  if (temp == 1)
    return 0;

  /* now, the expensive check */
  for (i = 0; i < q->n; i++)
    for (j = i + 1; j < q->n; j++)
      if (tenm_collided_segment_segment2(p->a->x, p->a->y,
                                         p->b->x, p->b->y,
                                         q->v[i]->x, q->v[i]->y,
                                         q->v[j]->x, q->v[j]->y))
        return 1;

  if (tenm_collided_point_polygon(p->a, q))
    return 1;
  if (tenm_collided_point_polygon(p->b, q))
    return 1;
  return 0;
}

static int
tenm_collided_polygon_polygon(const tenm_polygon *p, const tenm_polygon *q)
{
  int i;
  int j;
  int k;
  int l;
  int temp;

  /* first, assume a sufficiently large rectangle */
  temp = 1;
  for (i = 0; i < p->n; i++)
    for (j = 0; j < q->n; j++)
      if (p->v[i]->x >= q->v[j]->x)
      {
        temp = 0;
        break;
      }
  if (temp == 1)
    return 0;

  temp = 1;
  for (i = 0; i < p->n; i++)
    for (j = 0; j < q->n; j++)
      if (p->v[i]->x <= q->v[j]->x)
      {
        temp = 0;
        break;
      }
  if (temp == 1)
    return 0;

  temp = 1;
  for (i = 0; i < p->n; i++)
    for (j = 0; j < q->n; j++)
      if (p->v[i]->y >= q->v[j]->y)
      {
        temp = 0;
        break;
      }
  if (temp == 1)
    return 0;

  temp = 1;
  for (i = 0; i < p->n; i++)
    for (j = 0; j < q->n; j++)
      if (p->v[i]->y <= q->v[j]->y)
      {
        temp = 0;
        break;
      }
  if (temp == 1)
    return 0;

  /* now, the expensive check */
  for (i = 0; i < p->n; i++)
    for (j = i + 1; j < p->n; j++)
      for (k = 0; k < q->n; k++)
        for (l = k + 1; l < q->n; l++)
          if (tenm_collided_segment_segment2(p->v[i]->x, p->v[i]->y,
                                             p->v[j]->x, p->v[j]->y,
                                             q->v[k]->x, q->v[k]->y,
                                             q->v[l]->x, q->v[l]->y))
            return 1;

  for (i = 0; i < p->n; i++)
    if (tenm_collided_point_polygon(p->v[i], q))
      return 1;
  for (i = 0; i < q->n; i++)
    if (tenm_collided_point_polygon(q->v[i], p))
      return 1;
  return 0;
}

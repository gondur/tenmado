/* $Id: util.c,v 1.39 2003/01/12 00:37:37 oohara Exp $ */

/* NOT_HAVE_POSIX */
#include <config.h>

#include <stdio.h>
/* malloc */
#include <stdlib.h>

#include "tenm_graphic.h"
#include "const.h"
#include "tenm_table.h"
#include "explosion.h"
#include "tenm_primitive.h"
#include "tenm_collision.h"
#include "tenm_object.h"

#include "util.h"

#ifdef NOT_HAVE_POSIX
#define FONTDIR "./happy-L/image/"
#else /* not NOT_HAVE_POSIX */
#define FONTDIR DATADIR "/games/tenmado/image/"
#endif

static tenm_image *font = NULL;
static tenm_primitive *window = NULL;
static double window_width = 0.0;
static double window_height = 0.0;
static char *buffer = NULL;
static int buffer_size = 0;

static int in_window_point(double x, double y);

/* return 0 on success, 1 on error */
int
util_init(int width, int height)
{
  font = tenm_load_image(1, FONTDIR "/font-medium.png",
                         1, 95,
                         tenm_map_color(255, 255, 255));

  window= (tenm_primitive *) tenm_polygon_new(4,
                                              0.0, 0.0,
                                              0.0, (double) height,
                                              (double) width, 0.0,
                                              (double) width, (double) height);
  if (window == NULL)
  {
    tenm_image_delete(font);
    fprintf(stderr, "util_init: tenm_polygon_new failed\n");
    return 1;
  }
  window_width = width;
  window_height = height;

  return 0;
}

void
util_quit(void)
{
  if (font != NULL)
  {
    tenm_image_delete(font);
    font = NULL;
  }
  if (window != NULL)
  {
    (window->delete)(window);
    window = NULL;
  }
  window_width = 0.0;
  window_height = 0.0;
  if (buffer != NULL)
  {
    free(buffer);
    buffer = NULL;
  }
  buffer_size = 0;
}

int
draw_string(int x, int y, const char *string, int length)
{
  /* sanity check */
  if (font == NULL)
    return 0;
  if (string == NULL)
    return 0;
  if (length <= 0)
    return 0;

  if (tenm_draw_string(x, y, font, string, length) != 0)
  {
    fprintf(stderr, "draw_string: tenm_draw_string failed\n");
    return 1;
  }
  return 0;
}

int
draw_string_int(int x, int y, const int *string, int length)
{
  int i;
  char *buffer_temp;

  /* sanity check */
  if (font == NULL)
    return 0;
  if (string == NULL)
    return 0;
  if (length <= 0)
    return 0;

  /* don't substitute buffer directly, or you will be in a trouble
   * if realloc fails (you don't need data in buffer, but you still
   * need to free buffer in any case) */
  if ((buffer == NULL) || (length + 1 > buffer_size))
  {
    if (buffer == NULL)
      buffer_temp = (char *) malloc(sizeof(char) * (length + 1));
    else
      buffer_temp = (char *) realloc(buffer, sizeof(char) * (length + 1));
    if (buffer_temp == NULL)
    {
      fprintf(stderr, "draw_string_int: memory allocation to buffer failed\n");
      return 1;
    }
    buffer = buffer_temp;
    buffer_size = length + 1;
  }

  /* stupid way to get a pointer to char
   * (char * and int * are incompatible)
   */
  for (i = 0; i < length; i++)
    buffer[i] = (char) string[i];
  buffer[length] = '\0';

  if (tenm_draw_string(x, y, font, buffer, length) != 0)
  {
    fprintf(stderr, "draw_string_int: tenm_draw_string failed\n");
    return 1;
  }
  return 0;
}

/* return 1 (true) or 0 (false) */
int
in_window_object(const tenm_object *p)
{
  int i;

  if (window == NULL)
  {
    fprintf(stderr, "in_window_primitive: window is NULL\n");
    return 0;
  }
  if (p == NULL)
  {
    fprintf(stderr, "in_window_primitive: p is NULL\n");
    return 0;
  }
  if (p->mass == NULL)
  {
    fprintf(stderr, "in_window_primitive: p->mass is NULL\n");
    return 0;
  }
  for (i = 0; i < p->mass->n; i++)
    if (in_window_primitive(p->mass->p[i]))
      return 1;
  return 0;
}

/* optimized under the assumption that p is usually in the window
 * return 1 (true) or 0 (false) */
int
in_window_primitive(const tenm_primitive *p)
{
  double temp_x;
  double temp_y;
  double temp_r;
  int i;

  if (window == NULL)
  {
    fprintf(stderr, "in_window_primitive: window is NULL\n");
    return 0;
  }
  if (p == NULL)
  {
    fprintf(stderr, "in_window_primitive: p is NULL\n");
    return 0;
  }
  
  switch (p->klass)
  {
  case TENM_POINT:
    return in_window_point(((const tenm_point *) p)->x,
                           ((const tenm_point *) p)->y);
    break;
  case TENM_CIRCLE:
    temp_x = ((const tenm_circle *) p)->center->x;
    temp_y = ((const tenm_circle *) p)->center->y;
    temp_r = ((const tenm_circle *) p)->r;
    if ((temp_x + temp_r >= 0.0) && (temp_x - temp_r < window_width)
        && (temp_y >= 0.0) && (temp_y < window_height))
      return 1;
    if ((temp_y + temp_r >= 0.0) && (temp_y - temp_r < window_height)
        && (temp_x >= 0.0) && (temp_x < window_width))
      return 1;
    return tenm_collided_primitive(p, window);
    break;
  case TENM_SEGMENT:
    temp_x = ((const tenm_segment *) p)->a->x;
    temp_y = ((const tenm_segment *) p)->a->y;
    if (in_window_point(temp_x, temp_y))
      return 1;
    temp_x = ((const tenm_segment *) p)->b->x;
    temp_y = ((const tenm_segment *) p)->b->y;
    if (in_window_point(temp_x, temp_y))
      return 1;
    return tenm_collided_primitive(p, window);
    break;
  case TENM_POLYGON:
    for (i = 0; i < ((const tenm_polygon *) p)->n; i++)
    {
      temp_x = ((const tenm_polygon *) p)->v[i]->x;
      temp_y = ((const tenm_polygon *) p)->v[i]->y;
      if (in_window_point(temp_x, temp_y))
        return 1;
    }
    return tenm_collided_primitive(p, window);
    break;
  default:
    fprintf(stderr, "primitive_in_window: strange primitive found (%d)\n",
            p->klass);
    return 0;
    break;
  }
  /* should not reach here */
  return 0;
}

/* return 1 (true) or 0 (false) */
static int
in_window_point(double x, double y)
{
  /* sanity check */
  if (window == NULL)
  {
    fprintf(stderr, "point_in_window: window is NULL\n");
    return 0;
  }

  if ((x < 0) || (x >= window_width))
    return 0;
  if ((y < 0) || (y >= window_height))
    return 0;
  return 1;
}

int
delete_enemy_shot(tenm_object *my, int n)
{
  if (my == NULL)
    return 0;
  if (!(my->attr & ATTR_ENEMY_SHOT))
    return 0;
  tenm_table_add(explosion_new(my->x, my->y,
                               0.0, 0.0,
                               1, 20, my->count[0], 2.0, 8));
  return 1;
}

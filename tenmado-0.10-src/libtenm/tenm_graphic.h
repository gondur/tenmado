/* $Id: tenm_graphic.h,v 1.37 2003/01/03 23:15:26 oohara Exp $ */

#ifndef __TENM_GRAPHIC_H__
#define __TENM_GRAPHIC_H__

#include <SDL.h>

#include "tenm_primitive.h"
#include "tenm_object.h"

struct _tenm_image
{
  /* number of images */
  int n;
  SDL_Surface **image;
};
typedef struct _tenm_image tenm_image;

typedef Uint32 tenm_color;

/* for flag in tenm_graphic_init */
#define TENM_FULLSCREEN 1

int tenm_graphic_init(int width, int height, int flag, const char *name);
int tenm_draw_line(int a_x, int a_y, int b_x, int b_y, int width,
                   tenm_color color);
int tenm_draw_circle(int center_x, int center_y, int radius, int width,
                     tenm_color color);
int tenm_draw_point(int x, int y, tenm_color color);
int tenm_draw_rectangle(int x, int y, int w, int h, tenm_color color);
tenm_color tenm_map_color(int red, int green, int blue);
int tenm_clear_window(tenm_color color);
int tenm_redraw_window(void);
int tenm_draw_primitive(tenm_primitive *p, tenm_color color);
int tenm_draw_mass(tenm_mass *p, tenm_color color);

tenm_image *tenm_load_image(int number_file, ...);
void tenm_image_delete(tenm_image *p);
int tenm_draw_image(int x, int y, tenm_image *image, int number);
int tenm_draw_string(int x, int y, tenm_image *font, const char *string,
                     int length);




#endif /* not __TENM_GRAPHIC_H__ */

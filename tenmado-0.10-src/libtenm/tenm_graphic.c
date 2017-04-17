/* $Id: tenm_graphic.c,v 1.128 2003/01/14 03:02:55 oohara Exp $ */
#include <stdio.h>
/* malloc  */
#include <stdlib.h>
#include <math.h>
/* for tenm_load_image */
#include <stdarg.h>
/* isprint */
#include <ctype.h>

#include <SDL.h>
#include <SDL_image.h>

#include "tenm_math.h"
#include "tenm_primitive.h"
#include "tenm_object.h"
#include "tenm_sdl_init.h"

#include "tenm_graphic.h"

/* number of printable characters */
#define TENM_FONT_NUMBER_CHAR 95

static SDL_Surface *dpy = NULL;

static int tenm_font_index(int c);
static int tenm_draw_line2(int a_x, int a_y, int b_x, int b_y, int width,
                           tenm_color color, int xy_reversed);

/* return 0 on success, 1 on error */
int
tenm_graphic_init(int width, int height, int flag, const char *name)
{
  Uint32 sdl_flag;

  /* sanity check */
  if (name == NULL)
  {
    fprintf(stderr, "tenm_graphic_init: name is NULL\n");
    return 1;
  }
  if (SDL_WasInit(SDL_INIT_VIDEO) != 0)
  {
    fprintf(stderr, "tenm_graphic_init: warning: video is already "
            "initialized\n");
    return 0;
  }

  if (tenm_sdl_init(SDL_INIT_VIDEO) != 0)
  {
    fprintf(stderr, "tenm_graphic_init: cannot initialize SDL video\n");
    return 1;
  }

  if (dpy != NULL)
  {
    fprintf(stderr, "tenm_graphic_init: "
            "dpy is already set\n");
    /* this is a fatal error --- if we are here, video is not
     * initialized before tenm_graphic_init() is called, so
     * the situation is weird enough
     */
    /* dpy should not be freed here --- it is the job of SDL_Quit() */
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    return 1;
  }

  if (flag & TENM_FULLSCREEN)
    sdl_flag = SDL_FULLSCREEN;
  else
    sdl_flag = 0;
  dpy = SDL_SetVideoMode(width, height, 0, sdl_flag);
  if (dpy == NULL)
  {
    fprintf(stderr, "tenm_graphic_init: "
            "SDL_SetVideoMode failed (%s)\n", SDL_GetError());
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    return 1;
  }

  SDL_WM_SetCaption(name, name);
  /* ignore the return value of SDL_ShowCursor() because the cursor
   * may be out of the window when it is called
   */
  SDL_ShowCursor(SDL_DISABLE);

  return 0;
}

/* draw a line between A(a_x, a_y) and B(b_x, b_y)
 * return 0 on success, 1 on error
 */
int
tenm_draw_line(int a_x, int a_y, int b_x, int b_y, int width,
               tenm_color color)
{
  int dx;
  int dy;
  int dx_abs;
  int dy_abs;

  /* sanity check */
  if (width <= 0)
  {
    fprintf(stderr, "tenm_draw_line: width is non-positive (%d)\n", width);
    return 1;
  }

  dx = b_x - a_x;
  dy = b_y - a_y;
  if (dx >= 0)
    dx_abs = dx;
  else
    dx_abs = -dx;
  if (dy >= 0)
    dy_abs = dy;
  else
    dy_abs = -dy;

  if ((dx == 0) && (dy == 0))
    return tenm_draw_point(a_x, a_y, color);

  if (dx_abs >= dy_abs)
  {
    if (dx >= 0)
      return tenm_draw_line2(a_x, a_y, b_x, b_y, width, color, 0);
    else
      return tenm_draw_line2(b_x, b_y, a_x, a_y, width, color, 0);
  }
  else
  {
    if (dy >= 0)
      return tenm_draw_line2(a_y, a_x, b_y, b_x, width, color, 1);
    else
      return tenm_draw_line2(b_y, b_x, a_y, a_x, width, color, 1);
  }

  /* should not reach here */
  fprintf(stderr, "tenm_draw_line: should not reach here\n");
  return 1;
}

/* draw a circle whose center is (center_x, center_y)
 * with a line whose width is width (arg 4)
 * note that the center of the circle is always blank,
 * even if width == radius
 * return 0 on success, 1 on error
 */
int
tenm_draw_circle(int center_x, int center_y, int radius, int width,
                 tenm_color color)
{
  int i;
  int x_outer;
  int x_inner;
  int y;
  int error_tilde_outer;
  int error_tilde_inner;
  int status = 0;

  /* sanity check */
  if (radius <= 0)
  {
    fprintf(stderr, "tenm_draw_circle: radius is non-positive (%d)\n", radius);
    return 1;
  }
  if ((width <= 0) || (width > radius))
  {
    fprintf(stderr, "tenm_draw_circle: width is out of range (%d)\n", width);
    return 1;
  }

  /* the Bresenham algorithm with the "wide line" hack */
  x_outer = radius;
  x_inner = radius - width + 1;
  y = 0;
  /* the next point is either (x, y + 1) or (x - 1, y + 1) */

  /* ((x)^2 + (y + 1)^2 - radius^2) + ((x - 1)^2 + (y + 1)^2 - radius^2) */
  error_tilde_outer = 3 - 2 * radius;
  error_tilde_inner = 3 - 2 * (radius - width + 1);
  while (x_outer >= y)
  {
    for (i = x_outer; (i >= x_inner) && (i >= 0); i--)
    {  
      if (tenm_draw_point(center_x + i,
                          center_y + y,
                          color) != 0)
        status = 1;
      if (tenm_draw_point(center_x + i,
                          center_y - y,
                          color) != 0)
        status = 1;
      if (tenm_draw_point(center_x - i,
                          center_y + y,
                          color) != 0)
        status = 1;
      if (tenm_draw_point(center_x - i,
                          center_y - y,
                          color) != 0)
        status = 1;
    
      if (tenm_draw_point(center_x + y,
                          center_y + i,
                          color) != 0)
        status = 1;
      if (tenm_draw_point(center_x + y,
                          center_y - i,
                          color) != 0)
        status = 1;
      if (tenm_draw_point(center_x - y,
                          center_y + i,
                          color) != 0)
        status = 1;
      if (tenm_draw_point(center_x - y,
                          center_y - i,
                          color) != 0)
        status = 1;
    }
    
    if (error_tilde_outer >= 0)
    {
      /* the next point is (x - 1, y + 1) */
      /* error_tilde(next) - error_tilde
       * = (((x - 1)^2 + (y + 2)^2 - radius^2)
       *    + ((x - 2)^2 + (y + 2)^2 - radius^2))
       *   - (((x)^2 + (y + 1)^2 - radius^2)
       *      + ((x - 1)^2 + (y + 1)^2 - radius^2))
       */
      error_tilde_outer += (-4) * x_outer + 4 * y + 10;
      x_outer--;
    }
    else
    {
      /* the next point is (x, y + 1) */
      /* error_tilde(next) - error_tilde
       * = (((x)^2 + (y + 2)^2 - radius^2)
       *    + ((x - 1)^2 + (y + 2)^2 - radius^2))
       *   - (((x)^2 + (y + 1)^2 - radius^2)
       *      + ((x - 1)^2 + (y + 1)^2 - radius^2))
       */
      error_tilde_outer += 4 * y + 6;
    }

    if (x_inner >= y)
    {
      if (error_tilde_inner >= 0)
      {
        error_tilde_inner += (-4) * x_inner + 4 * y + 10;
        x_inner--;
      }
      else
      {
        error_tilde_inner += 4 * y + 6;
      }
    }

    y++;

  }

  return status;
}

/* you must _NOT_ lock dpy before calling this function
 * return 0 on success, 1 on error
 */
int
tenm_draw_point(int x, int y, tenm_color color)
{
  return tenm_draw_rectangle(x, y, 1, 1, color);
}

/* you must _NOT_ lock dpy before calling this function
 * return 0 on success, 1 on error
 */
int
tenm_draw_rectangle(int x, int y, int w, int h, tenm_color color)
{
  SDL_Rect rec;

  /* sanity check */
  if ((x + (w - 1) < 0) || (x - (w - 1) >= dpy->w))
    return 0;
  if ((y + (h - 1) < 0) || (y - (h - 1) >= dpy->h))
    return 0;

  rec.x = x;
  rec.y = y;
  rec.w = w;
  rec.h = h;
  if (SDL_FillRect(dpy, &rec, color) != 0)
  {
    fprintf(stderr, "tenm_draw_rectangle: SDL_FillRect failed "
            "(%s) (x = %d, y = %d, w = %d, h = %d)\n", SDL_GetError(),
            x, y, w, h);
    return 1;
  }

  return 0;
}

tenm_color
tenm_map_color(int red, int green, int blue)
{
  /* I can't fix gcc "different width" warnings here */
  return SDL_MapRGB(dpy->format, red, green, blue);
}

/* return 0 on success, 1 on error */
int
tenm_clear_window(tenm_color color)
{
  if (SDL_FillRect(dpy, NULL, color) != 0)
  {
    fprintf(stderr, "tenm_clear_window: SDL_FillRect failed (%s)\n",
            SDL_GetError());
    return 1;
  }
  return 0;
}

/* return 0 on success, 1 on error */
int
tenm_redraw_window(void)
{
  if (SDL_Flip(dpy) != 0)
  {
    fprintf(stderr, "tenm_redraw_window: SDL_Flip failed (%s)\n",
            SDL_GetError());
    return 1;
  }
  return 0;
}

/* return 0 on success, 1 on error */
int
tenm_draw_primitive(tenm_primitive *p, tenm_color color)
{
  int i;
  int j;
  int status = 0;

  if (p == NULL)
  {
    fprintf(stderr, "tenm_draw_primitive: p is NULL\n");
    return 0;
  }

  switch (p->klass)
  {
  case TENM_POINT:
    if (tenm_draw_point((int) ((tenm_point *) p)->x,
                        (int) ((tenm_point *) p)->y,
                        color) != 0)
      return 1;
    else
      return 0;
    break;
  case TENM_CIRCLE:
    if (tenm_draw_circle((int) ((tenm_circle *) p)->center->x,
                         (int) ((tenm_circle *) p)->center->y,
                         (int) ((tenm_circle *) p)->r,
                         1, color) != 0)
      return 1;
    else
      return 0;
    break;
  case TENM_SEGMENT:
    if (tenm_draw_line((int) ((tenm_segment *) p)->a->x,
                       (int) ((tenm_segment *) p)->a->y,
                       (int) ((tenm_segment *) p)->b->x,
                       (int) ((tenm_segment *) p)->b->y,
                       1, color) != 0)
      return 1;
    else
      return 0;
    break;
  case TENM_POLYGON:
    for (i = 0; i < ((tenm_polygon *) p)->n; i++)
      for (j = i + 1; j < ((tenm_polygon *) p)->n; j++)
        if (tenm_draw_line((int) ((tenm_polygon *) p)->v[i]->x,
                           (int) ((tenm_polygon *) p)->v[i]->y,
                           (int) ((tenm_polygon *) p)->v[j]->x,
                           (int) ((tenm_polygon *) p)->v[j]->y,
                           1, color) != 0)
          status = 1;
    return status;
    break;
  default:
    fprintf(stderr, "tenm_draw_primitive: strange primitive (%d)\n", p->klass);
    return 1;
    break;
  }
  /* should not reach here */
  fprintf(stderr, "tenm_draw_primitive: should not reach here\n");
  return 1;
}

/* return 0 on success, 1 on error */
int
tenm_draw_mass(tenm_mass *p, tenm_color color)
{
  int i;
  int status = 0;

  /* sanity check */
  if (p == NULL)
  {
    fprintf(stderr, "tenm_draw_mass: p is NULL\n");
    return 1;
  }
  if (p->n <= 0)
  {
    fprintf(stderr, "tenm_draw_mass: p->n is non-negative\n");
    return 1;
  }
  if (p->p == NULL)
  {
    fprintf(stderr, "tenm_draw_mass: p->p is NULL\n");
    return 1;
  }

  for (i = 0; i < p->n; i++)
  {    
    if (tenm_draw_primitive(p->p[i], color))
    {
      fprintf(stderr, "tenm_draw_mass: tenm_draw_primitive failed (%d)\n", i);
      status = 1;
    }
  }

  return status;
}

/* load image files and split them
 * ... is one or more of
 * (const char *file_name, int row, int column, tenm_color transparent_color)
 * return NULL on error
 */
tenm_image *
tenm_load_image(int number_file, ...)
{
  int i;
  int j;
  int k;
  int l;
  const char *file_name;
  int row;
  int column;
  tenm_color transparent_color;
  int width;
  int height;
  int suffix;
  va_list ap;
  tenm_image *temp;
  SDL_Surface **temp_surface;
  SDL_Surface *master;
  SDL_Rect temp_rect_src;
  SDL_Rect temp_rect_dest;

  /* sanity check */
  if (number_file <= 0)
  {
    fprintf(stderr, "tenm_load_image: strange number_file (%d)\n",
            number_file);
    return NULL;
  }

  temp = (tenm_image *) malloc(sizeof(tenm_image));
  if (temp == NULL)
  {
    fprintf(stderr, "tenm_load_image: malloc failed\n");
    return NULL;
  }
  temp->n = 0;
  temp->image = NULL;

  va_start(ap, number_file);
  for (i = 0; i < number_file; i++)
  {
    /* you can't use char in va_arg */
    file_name = (const char *) va_arg(ap, const int *);
    row = va_arg(ap, int);
    column = va_arg(ap, int);
    transparent_color = va_arg(ap, tenm_color);

    /* sanity check */
    if (file_name == NULL)
    {
      fprintf(stderr, "tenm_load_image: file_name is NULL (i = %d)\n", i);
      if (temp->image != NULL)
      {
        for (l = 0; l < temp->n; l++)
          SDL_FreeSurface(temp->image[l]);
        free(temp->image);
      }
      free(temp);
      va_end(ap);
      return NULL;
    }    
    if (row <= 0)
    {
      fprintf(stderr, "tenm_load_image: row is non-positive (%d) "
              "(i = %d)\n", row, i);
      if (temp->image != NULL)
      {
        for (l = 0; l < temp->n; l++)
          SDL_FreeSurface(temp->image[l]);
        free(temp->image);
      }
      free(temp);
      va_end(ap);
      return NULL;
    }
    if (column <= 0)
    {
      fprintf(stderr, "tenm_load_image: column is non-positive (%d) "
              "(i = %d)\n", column, i);
      if (temp->image != NULL)
      {
        for (l = 0; l < temp->n; l++)
          SDL_FreeSurface(temp->image[l]);
        free(temp->image);
      }
      free(temp);
      va_end(ap);
      return NULL;
    }

    master = IMG_Load(file_name);
    if (master == NULL)
    {
      fprintf(stderr, "tenm_load_image: IMG_Load(%s) failed (%s) (%i)\n",
              file_name, IMG_GetError(), i);
      if (temp->image != NULL)
      {
        for (l = 0; l < temp->n; l++)
          SDL_FreeSurface(temp->image[l]);
        free(temp->image);
      }
      free(temp);
      va_end(ap);
      return NULL;
    }
 
    width = master->w / column;
    if (master->w % column != 0)
      fprintf(stderr, "tenm_load_image: warning: width (%d) of image (%s) "
              "cannot be divided by %d\n", master->w, file_name, column);
    height = master->h / row;
    if (master->h % row != 0)
      fprintf(stderr, "tenm_load_image: warning: height (%d) of image (%s) "
              "cannot be divided by %d\n", master->h, file_name, row);

    /* don't substitute tenm->image directly, or you will be in a trouble
     * if realloc fails  */
    if (temp->image == NULL)
      temp_surface = (SDL_Surface **) malloc(sizeof(SDL_Surface *)
                                             * row * column);
    else
      temp_surface = (SDL_Surface **) realloc(temp->image,
                                              sizeof(SDL_Surface *)
                                              * (row * column + temp->n));
    if (temp_surface == NULL)
    {
      fprintf(stderr, "tenm_load_image: memory allocation to image "
              "(%d) failed\n", i);
      SDL_FreeSurface(master);
      if (temp->image != NULL)
      {
        for (l = 0; l < temp->n; l++)
          SDL_FreeSurface(temp->image[l]);
        free(temp->image);
      }
      free(temp);
      return NULL;
    }
    temp->image = temp_surface;

    for (j = 0; j < row; j++)
      for (k = 0; k < column; k++)
      {
        suffix = temp->n + j * column + k;
        /* plain malloc is not enough */
        temp->image[suffix] = SDL_CreateRGBSurface(0, width, height,
                                                   dpy->format->BitsPerPixel,
                                                   dpy->format->Rmask,
                                                   dpy->format->Gmask,
                                                   dpy->format->Bmask,
                                                   dpy->format->Amask);
        if (temp->image[suffix] == NULL)
        {
          fprintf(stderr, "tenm_load_image: SDL_CreateRGBSurface(%d) "
                  "failed (%s)\n", suffix, SDL_GetError());
          SDL_FreeSurface(master);
          for (l = 0; l < suffix; l++)
            SDL_FreeSurface(temp->image[l]);
          free(temp->image);
          free(temp);
          return NULL;
        }
        if (dpy->format->BitsPerPixel == 8)
          SDL_SetPalette(temp->image[suffix], SDL_LOGPAL|SDL_PHYSPAL,
                         dpy->format->palette->colors,
                         0, dpy->format->palette->ncolors);
        temp_rect_src.x = width * k;
        temp_rect_src.y = row * j;
        temp_rect_src.w = width;
        temp_rect_src.h = height;
        temp_rect_dest.x = 0;
        temp_rect_dest.y = 0;
        temp_rect_dest.w = width;
        temp_rect_dest.h = height;
        if (SDL_BlitSurface(master, &temp_rect_src,
                            temp->image[suffix], &temp_rect_dest) != 0)
        {
          fprintf(stderr, "tenm_load_image: SDL_BlitSurface failed (%s)\n",
                  SDL_GetError());
          SDL_FreeSurface(master);
          /* we need to free temp->image[suffix] too */
          for (l = 0; l <= suffix; l++)
            SDL_FreeSurface(temp->image[l]);
          free(temp->image);
          free(temp);
          return NULL;
        }
        if (SDL_SetColorKey(temp->image[suffix], SDL_SRCCOLORKEY,
                            transparent_color) != 0)
        {
          fprintf(stderr, "tenm_load_image: SDL_SetColorKey failed (%s)\n",
                  SDL_GetError());
          SDL_FreeSurface(master);
          /* we need to free temp->image[suffix] too */
          for (l = 0; l <= suffix; l++)
            SDL_FreeSurface(temp->image[l]);
          free(temp->image);
          free(temp);
          return NULL;
        }
      }
    temp->n += row * column;
    SDL_FreeSurface(master);
  }

  va_end(ap);

  return temp;
}

void
tenm_image_delete(tenm_image *p)
{
  int i;
  /* sanity check */
  if (p == NULL)
  {
    fprintf(stderr, "tenm_image_delete: p is NULL\n");
    return;
  }
  if (p->n <= 0)
  {
    fprintf(stderr, "tenm_image_delete: p->n is non-positive (%d)\n", p->n);
    return;
  }
  if (p->image == NULL)
  {
    fprintf(stderr, "tenm_image_delete: p->image is NULL\n");
    return;
  }

  for (i = 0; i < p->n; i++)
    SDL_FreeSurface(p->image[i]);
  free(p->image);
  free(p);
}

/* return 0 on success, 1 on error */
int
tenm_draw_image(int x, int y, tenm_image *image, int number)
{
  SDL_Rect temp;

  /* sanity check */
  if (image == NULL)
  {
    fprintf(stderr, "tenm_draw_image: image is NULL\n");
    return 1;
  }
  if (image->n <= 0)
  {
    fprintf(stderr, "tenm_draw_image: image->n is non-positive\n");
    return 1;
  }
  if (image->image == NULL)
  {
    fprintf(stderr, "tenm_draw_image: image->image is NULL\n");
    return 1;
  }
  if ((number < 0) || (number >= image->n))
  {
    fprintf(stderr, "tenm_draw_image: number (%d) is out of range "
            "(0 -- %d)\n", number, image->n - 1);
    return 1;
  }
  
  temp.x = x;
  temp.y = y;
  temp.w = image->image[number]->w;
  temp.h = image->image[number]->h;

  if (SDL_BlitSurface(image->image[number], NULL, dpy, &temp) != 0)
  {
    fprintf(stderr, "tenm_draw_image: SDL_BlitSurface failed (%s)\n",
            SDL_GetError());
    return 1;
  }

  return 0;
}

/* return 0 on success, 1 on error */
int
tenm_draw_string(int x, int y, tenm_image *font, const char *string,
                 int length)
{
  int i;
  int suffix;
  int temp_x;
  int status = 0;

  /* sanity check */
  if (font == NULL)
  {
    fprintf(stderr, "tenm_draw_string: font is NULL\n");
    return 1;
  }
  if (font->n < TENM_FONT_NUMBER_CHAR)
  {
    fprintf(stderr, "tenm_draw_string: not enough images in font "
            "(%d) (%d required)\n", font->n, TENM_FONT_NUMBER_CHAR);
    return 1;
  }
  if (font->image == NULL)
  {
    fprintf(stderr, "tenm_draw_string: font->image is NULL\n");
    return 1;
  }
  if (string == NULL)
  {
    fprintf(stderr, "tenm_draw_string: string is NULL\n");
    return 1;
  }
  if (length <= 0)
  {
    fprintf(stderr, "tenm_draw_string: length is non-positive (%d)\n", length);
    return 1;
  }

  temp_x = x;
  for (i = 0; (i < length) && (string[i] != '\0'); i++)
  {
    if (!isprint(string[i]))
    {
      fprintf(stderr, "tenm_draw_string: char %d unprintable (\\x%x)\n",
              i, string[i]);
      status = 1;
      continue;
    }
    suffix = tenm_font_index(string[i]);
    if ((suffix < 0) || (suffix >= font->n))
    {
      fprintf(stderr, "tenm_draw_string: tenm_font_index(%d) returned "
              "strange number (%d)\n", i, suffix);
      status = 1;
      continue;
    }

    if (tenm_draw_image(temp_x, y - font->image[suffix]->h / 2,
                        font, suffix) != 0)
    {
      fprintf(stderr, "tenm_draw_string: tenm_draw_image(%d) failed\n", i);
      status = 1;
      continue;
    }
    
    temp_x += font->image[suffix]->w;
  }
  return status;
}

/* order of characters (ASCII order)
 *    !"#$%&'()*+,-./0123456789:;<=>?
 *   @ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_
 *   `abcdefghijklmnoprqstuvwxyz{|}~
 */

/* return a non-negative index for the character c
 * return -1 on error
 */
static int
tenm_font_index(int c)
{
  /* sanity check */
  if (!isprint(c))
  {
    fprintf(stderr, "tenm_font_index: unprintable char (\\x%x)\n", c);
    return -1;
  }
  
  /* don't assume ASCII */
  switch (c)
  {
  case ' ':
    return 0;
    break;
  case '!':
    return 1;
    break;
  case '"':
    return 2;
    break;
  case '#':
    return 3;
    break;
  case '$':
    return 4;
    break;
  case '%':
    return 5;
    break;
  case '&':
    return 6;
    break;
  case '\'':
    return 7;
    break;
  case '(':
    return 8;
    break;
  case ')':
    return 9;
    break;
  case '*':
    return 10;
    break;
  case '+':
    return 11;
    break;
  case ',':
    return 12;
    break;
  case '-':
    return 13;
    break;
  case '.':
    return 14;
    break;
  case '/':
    return 15;
    break;
  case '0':
    return 16;
    break;
  case '1':
    return 17;
    break;
  case '2':
    return 18;
    break;
  case '3':
    return 19;
    break;
  case '4':
    return 20;
    break;
  case '5':
    return 21;
    break;
  case '6':
    return 22;
    break;
  case '7':
    return 23;
    break;
  case '8':
    return 24;
    break;
  case '9':
    return 25;
    break;
  case ':':
    return 26;
    break;
  case ';':
    return 27;
    break;
  case '<':
    return 28;
    break;
  case '=':
    return 29;
    break;
  case '>':
    return 30;
    break;
  case '?':
    return 31;
    break;
  case '@':
    return 32;
    break;
  case 'A':
    return 33;
    break;
  case 'B':
    return 34;
    break;
  case 'C':
    return 35;
    break;
  case 'D':
    return 36;
    break;
  case 'E':
    return 37;
    break;
  case 'F':
    return 38;
    break;
  case 'G':
    return 39;
    break;
  case 'H':
    return 40;
    break;
  case 'I':
    return 41;
    break;
  case 'J':
    return 42;
    break;
  case 'K':
    return 43;
    break;
  case 'L':
    return 44;
    break;
  case 'M':
    return 45;
    break;
  case 'N':
    return 46;
    break;
  case 'O':
    return 47;
    break;
  case 'P':
    return 48;
    break;
  case 'Q':
    return 49;
    break;
  case 'R':
    return 50;
    break;
  case 'S':
    return 51;
    break;
  case 'T':
    return 52;
    break;
  case 'U':
    return 53;
    break;
  case 'V':
    return 54;
    break;
  case 'W':
    return 55;
    break;
  case 'X':
    return 56;
    break;
  case 'Y':
    return 57;
    break;
  case 'Z':
    return 58;
    break;
  case '[':
    return 59;
    break;
  case '\\':
    return 60;
    break;
  case ']':
    return 61;
    break;
  case '^':
    return 62;
    break;
  case '_':
    return 63;
    break;
  case '`':
    return 64;
    break;
  case 'a':
    return 65;
    break;
  case 'b':
    return 66;
    break;
  case 'c':
    return 67;
    break;
  case 'd':
    return 68;
    break;
  case 'e':
    return 69;
    break;
  case 'f':
    return 70;
    break;
  case 'g':
    return 71;
    break;
  case 'h':
    return 72;
    break;
  case 'i':
    return 73;
    break;
  case 'j':
    return 74;
    break;
  case 'k':
    return 75;
    break;
  case 'l':
    return 76;
    break;
  case 'm':
    return 77;
    break;
  case 'n':
    return 78;
    break;
  case 'o':
    return 79;
    break;
  case 'p':
    return 80;
    break;
  case 'q':
    return 81;
    break;
  case 'r':
    return 82;
    break;
  case 's':
    return 83;
    break;
  case 't':
    return 84;
    break;
  case 'u':
    return 85;
    break;
  case 'v':
    return 86;
    break;
  case 'w':
    return 87;
    break;
  case 'x':
    return 88;
    break;
  case 'y':
    return 89;
    break;
  case 'z':
    return 90;
    break;
  case '{':
    return 91;
    break;
  case '|':
    return 92;
    break;
  case '}':
    return 93;
    break;
  case '~':
    return 94;
    break;
  default:
    fprintf(stderr, "tenm_font_index: unknown char (\\x%x)\n", c);
    return -1;
    break;
  }
  /* should not reach here */
  fprintf(stderr, "tenm_font_index: should not reach here\n");
  return -1;
}

/* draw a line between A(a_x, a_y) and B(b_x, b_y)
 * assumes b_x > a_x and abs(b_x - a_x) >= abs(b_y - a_x)
 * return 0 on success, 1 on error
 */
static int
tenm_draw_line2(int a_x, int a_y, int b_x, int b_y, int width,
                tenm_color color, int xy_reversed)
{
  int x;
  int y;
  int x2;
  int y2;
  int dx;
  int dy_abs;
  /* 2 * abs(b_x - a_x) * (y - y_real_line) */
  int error_tilde;
  int error_tilde2;
  int y_changed;
  int x2_changed;
  int status = 0;
  int range;

  /* sanity check */
  if (b_x <= a_x)
  {
    fprintf(stderr, "tenm_draw_line2: not (b_x <= a_x)\n");
    return 1;
  }
  if ((b_x - a_x < a_y - b_y) || (b_x - a_x < b_y - a_y))
  {
    fprintf(stderr, "tenm_draw_line2: not "
            "((b_x - a_x < a_y - b_y) || (b_x - a_x < b_y - a_y))\n");
    return 1;
  }
  if (width <= 0)
  {
    fprintf(stderr, "tenm_draw_line2: width is non-positive (%d)\n", width);
    return 1;
  }

  dx = b_x - a_x;
  if (a_y >= b_y)
    dy_abs = a_y - b_y;
  else
    dy_abs = b_y - a_y;
  /* to minimize error */
  if (dy_abs == 0)
    range = width;
  else
    range = (int) (((double) (dx * width))
                   / tenm_sqrt(dx * dx + dy_abs * dy_abs));
  if (range <= 0)
    range = 1;

  /* the Bresenham algorithm with the "wide line" hack */
  x = a_x;
  y = a_y;
  /* the next point is:
   * either (x + 1, y) or (x + 1, y + 1) if b_y >= a_y
   * either (x + 1, y) or (x + 1, y - 1) otherwise
   */
  error_tilde = 0;
  y_changed = 0;

  while (x <= b_x)
  {
    /* Bresenham again */
    x2 = 0;
    y2 = 0;
    /* not very accurate, but who cares? */
    error_tilde2 = 0;
    x2_changed = 0;

    while (y2 < range)
    {
      if (xy_reversed)
      {
        if (tenm_draw_point(y + y2, x + x2, color) != 0)
          status = 1;
        if (tenm_draw_point(y - y2, x - x2, color) != 0)
          status = 1;
      }
      else
      {
        if (tenm_draw_point(x + x2, y + y2, color) != 0)
          status = 1;
        if (tenm_draw_point(x - x2, y - y2, color) != 0)
          status = 1;
      }

      if ((y_changed) && (x2_changed))
      {
        if (xy_reversed)
        {
          if (b_y >= a_y)
          {
            if (tenm_draw_point(y + y2 - 1, x + x2, color) != 0)
              status = 1;
            if (tenm_draw_point(y - y2, x- x2 - 1, color) != 0)
              status = 1;
          }
          else
          {
            if (tenm_draw_point(y + y2, x + x2 - 1, color) != 0)
              status = 1;
            if (tenm_draw_point(y - y2 + 1, x - x2, color) != 0)
              status = 1;
          }
        }
        else
        {
          if (b_y >= a_y)
          {
            if (tenm_draw_point(x + x2, y + y2 - 1, color) != 0)
              status = 1;
            if (tenm_draw_point(x - x2 - 1, y - y2, color) != 0)
              status = 1;
          }
          else
          {
            if (tenm_draw_point(x + x2 - 1, y + y2, color) != 0)
              status = 1;
            if (tenm_draw_point(x - x2, y - y2 + 1, color) != 0)
              status = 1;
          }
        }
      }

      y2++;
      error_tilde2 += 2 * dy_abs;
      if (error_tilde2 >= dx)
      {
        error_tilde2 -= 2 * dx;
        if (b_y >= a_y)
          x2--;
        else
          x2++;
        x2_changed = 1;
      }
      else
      {
        x2_changed = 0;
      }
    }

    x++;
    error_tilde += 2 * dy_abs;
    if (error_tilde >= dx)
    {
      error_tilde -= 2 * dx;
      if (b_y >= a_y)
        y++;
      else
        y--;
      y_changed = 1;
    }
    else
    {
      y_changed = 0;
    }
  }

  return status;
}

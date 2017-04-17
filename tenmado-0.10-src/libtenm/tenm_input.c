/* $Id: tenm_input.c,v 1.91 2004/02/15 14:40:48 oohara Exp $ */

#include <stdio.h>
/* atexit, malloc */
#include <stdlib.h>
/* for tenm_polygon_new */
#include <stdarg.h>
/* errno */
#include <errno.h>
/* strerror */
#include <string.h>

#include <SDL.h>

/* tenm_redraw_window() */
#include "tenm_graphic.h"
/* tenm_pow2() */
#include "tenm_math.h"
#include "tenm_sdl_init.h"

#include "tenm_input.h"

static int number_key_mask = 0;
static tenm_key *key_mask = NULL;

static void (*focus_handler)(int) = NULL;

static SDL_Joystick *js = NULL;
static int *joystick_axis_set = NULL;
static tenm_key *joystick_axis_mask = NULL;
static int *joystick_button_set = NULL;
static tenm_key *joystick_button_mask = NULL;
static int neutral_range = 8192;

static int key_status;

static int tenm_add_key_status(tenm_key key, int delta);
static void tenm_key_quit(void);
static void tenm_joystick_quit(void);

/* there is no init() for keyboard because it is initialized
 * along with video and there is tenm_graphic_init()
 */

/* ... is 0 -- TENM_NUMBER_KEY_MAX (16) of (tenm_key key)
 * return 0 on success, 1 on error
 */
int
tenm_set_key(int number_key, ...)
{
  int i;
  va_list ap;
  tenm_key *temp_key_mask = NULL;

  /* sanity check */
  if (number_key <= -1)
  {
    fprintf(stderr, "tenm_set_key: number_key is negative\n");
    return 1;
  }

  if (number_key > TENM_NUMBER_KEY_MAX)
  {
    fprintf(stderr, "tenm_set_key: warning: registering only the first "
            "%d keys and ignoring the rest\n", TENM_NUMBER_KEY_MAX);
    number_key = TENM_NUMBER_KEY_MAX;
  }

  /* initialize SDL if necessary */
  if (tenm_sdl_init(SDL_INIT_VIDEO) != 0)
  {
    fprintf(stderr, "tenm_set_key: cannot initialize SDL video \n");
    return 1;
  }

  errno = 0;
  if (atexit(tenm_key_quit) != 0)
  {
    fprintf(stderr, "tenm_set_key: cannot register tenm_key_quit "
            "to exit");
    if (errno != 0)
      fprintf(stderr, " (%s)", strerror(errno));
    fprintf(stderr, "\n");
    tenm_key_quit();
    return 1;
  }

  /* don't substitute key_mask directly, or you will be in a trouble
   * if realloc fails  */
  if (key_mask == NULL)
    temp_key_mask = (tenm_key *) malloc(sizeof(tenm_key) * number_key);
  else
    temp_key_mask = (tenm_key *) realloc(key_mask,
                                         sizeof(tenm_key) * number_key);
  if (temp_key_mask == NULL)
  {
    fprintf(stderr, "tenm_event_init: memory allocation to key_mask failed\n");
    tenm_key_quit();
    return 1;
  }

  key_mask = temp_key_mask;
  number_key_mask = number_key;
  va_start(ap, number_key);
  for (i = 0; i < number_key; i++)
    key_mask[i] = va_arg(ap, tenm_key);
  va_end(ap);

  return 0;
}

void
tenm_set_focus_handler(void (*handler)(int))
{
  focus_handler = (void (*)(int)) handler;
}

/* range is the maximum of the absolute value of event.jaxis.axis
 * that is considered to be neutral
 * return TENM_JOYSTICK_INIT_SUCCESS, TENM_JOYSTICK_INIT_NO_JOYSTICK
 * or TENM_JOYSTICK_INIT_ERROR
 */
int
tenm_joystick_init(int range)
{
  int i;

  /* sanity check */
  if (range <= 0)
  {
    fprintf(stderr, "tenm_joystick_init: strange range (%d)\n", range);
    return TENM_JOYSTICK_INIT_ERROR;
  }
  
  if (SDL_WasInit(SDL_INIT_JOYSTICK) != 0)
  {
    fprintf(stderr, "tenm_joystick_init: warning: joystick is already "
            "initialized\n");
    return TENM_JOYSTICK_INIT_SUCCESS;
  }

  if (tenm_sdl_init(SDL_INIT_JOYSTICK) != 0)
  {
    fprintf(stderr, "tenm_joystick_init: cannot initialize SDL joystick "
            "(%s)\n", SDL_GetError());
    return TENM_JOYSTICK_INIT_ERROR;
  }

  errno = 0;
  if (atexit(tenm_joystick_quit) != 0)
  {
    fprintf(stderr, "tenm_joystick_init: cannot register tenm_joystick_quit "
            "to exit");
    if (errno != 0)
      fprintf(stderr, " (%s)", strerror(errno));
    fprintf(stderr, "\n");
    tenm_joystick_quit();
    return TENM_JOYSTICK_INIT_ERROR;
  }

  if (SDL_NumJoysticks() <= 0)
  {
    fprintf(stderr, "tenm_joystick_init: no joystick found\n");
    /* this is not a fatal error --- you should fall back to a keyboard */
    return TENM_JOYSTICK_INIT_NO_JOYSTICK;
  }

  if (js != NULL)
  {
    fprintf(stderr, "tenm_joystick_init: js is already set\n");
    /* this is a fatal error --- if we are here, joystick is not
     * initialized before tenm_joystick_init() is called, so
     * the situation is weird enough
     */
    tenm_joystick_quit();
    return TENM_JOYSTICK_INIT_ERROR;
  }
  js = SDL_JoystickOpen(0);
  if (js == NULL)
  {
    fprintf(stderr, "tenm_joystick_init: cannot open joystick 1 (%s)\n",
            SDL_GetError());
    tenm_joystick_quit();
    return TENM_JOYSTICK_INIT_ERROR;
  }

  if (SDL_JoystickEventState(SDL_ENABLE) != SDL_ENABLE)
  {
    fprintf(stderr, "tenm_set_key: cannot enable SDL joystick event "
            "processing (%s)\n", SDL_GetError());
    tenm_joystick_quit();
    return TENM_JOYSTICK_INIT_ERROR;
  }

  if (joystick_axis_set != NULL)
  {
    fprintf(stderr, "tenm_joystick_init: warning: joystick_axis_set is "
            "not NULL, freeing\n");
    free(joystick_axis_set);
    joystick_axis_set = NULL;
  }
  joystick_axis_set = (int *) malloc(sizeof(int) * 4);
  if (joystick_axis_set == NULL)
  {
    fprintf(stderr, "tenm_joystick_init: malloc(joystick_axis_set) failed\n");
    tenm_joystick_quit();
    return TENM_JOYSTICK_INIT_ERROR;
  }
  for (i = 0; i < 4; i++)
    joystick_axis_set[i] = 0;

  if (joystick_axis_mask != NULL)
  {
    fprintf(stderr, "tenm_joystick_init: warning: joystick_axis_mask is "
            "not NULL, freeing\n");
    free(joystick_axis_mask);
    joystick_axis_mask = NULL;
  }
  joystick_axis_mask = (tenm_key *) malloc(sizeof(tenm_key) * 4);
  if (joystick_axis_mask == NULL)
  {
    fprintf(stderr, "tenm_joystick_init: malloc(joystick_axis_mask) failed\n");
    tenm_joystick_quit();
    return TENM_JOYSTICK_INIT_ERROR;
  }

  if (joystick_button_set != NULL)
  {
    fprintf(stderr, "tenm_joystick_init: warning: joystick_button_set is "
            "not NULL, freeing\n");
    free(joystick_button_set);
    joystick_button_set = NULL;
  }
  joystick_button_set = (int *) malloc(sizeof(int) * (TENM_NUMBER_KEY_MAX -4));
  if (joystick_button_set == NULL)
  {
    fprintf(stderr, "tenm_joystick_init: malloc(joystick_button_set) "
            "failed\n");
    tenm_joystick_quit();
    return TENM_JOYSTICK_INIT_ERROR;
  }
  for (i = 0; i < TENM_NUMBER_KEY_MAX - 4; i++)
    joystick_button_set[i] = 0;

  if (joystick_button_mask != NULL)
  {
    fprintf(stderr, "tenm_joystick_init: warning: joystick_button_mask is "
            "not NULL, freeing\n");
    free(joystick_button_mask);
    joystick_button_mask = NULL;
  }
  joystick_button_mask = (tenm_key *) malloc(sizeof(tenm_key)
                                             * (TENM_NUMBER_KEY_MAX - 4));
  if (joystick_button_mask == NULL)
  {
    fprintf(stderr, "tenm_joystick_init: malloc(joystick_button_mask) "
            "failed\n");
    tenm_joystick_quit();
    return TENM_JOYSTICK_INIT_ERROR;
  }

  neutral_range = range;

  return TENM_JOYSTICK_INIT_SUCCESS;
}

/* translate a direction of a joystick axis into a key
 * return 0 on success, 1 on error
 */
int
tenm_joystick_map_axis(int direction, tenm_key key)
{
  /* sanity check */
  if ((direction < 0) || (direction >= 4))
  {
    fprintf(stderr, "tenm_joystick_map_axis: strange direction (%d)\n",
            direction);
    return 1;
  }

  if ((joystick_axis_set == NULL) || (joystick_axis_mask == NULL))
  {
    fprintf(stderr, "tenm_joystick_map_axis: joystick is not initialized\n");
    return 1;
  }

  joystick_axis_set[direction] = 1;
  joystick_axis_mask[direction] = key;

  return 0;
}

/* translate a button of a joystick axis into a key
 * return 0 on success, 1 on error
 */
int
tenm_joystick_map_button(int button_index, tenm_key key)
{
  /* sanity check */
  if ((button_index < 0) || (button_index >= 12))
  {
    fprintf(stderr, "tenm_joystick_map_button: strange button_index (%d)\n",
            button_index);
    return 1;
  }

  if ((joystick_button_set == NULL) || (joystick_button_mask == NULL))
  {
    fprintf(stderr, "tenm_joystick_map_button: joystick is not initialized\n");
    return 1;
  }

  joystick_button_set[button_index] = 1;
  joystick_button_mask[button_index] = key;

  return 0;
}

int
tenm_get_key_status(void)
{
  return key_status;
}

/* return 1 if the program should quit, 0 if not */
int
tenm_event_handle(void)
{
  SDL_Event event;

  while (SDL_PollEvent(&event) > 0)
  {
    if (event.type == SDL_QUIT)
      return 1;

    if (event.type == SDL_VIDEOEXPOSE)
    {
      tenm_redraw_window();
    }
    else if (event.type == SDL_ACTIVEEVENT)
    {
      if (event.active.gain == 1)
      {
        /* the mouse cursor entered the window */
        if (focus_handler != NULL)
          (*focus_handler)(1);
      }
      else
      {
        /* the mouse cursor left the window */
        if (focus_handler != NULL)
          (*focus_handler)(0);
      }
    }
    else if (event.type == SDL_KEYDOWN)
    {
      tenm_add_key_status(event.key.keysym.sym, 1);
    }
    else if (event.type == SDL_KEYUP)
    {
      tenm_add_key_status(event.key.keysym.sym, -1);
    }
    else if (event.type == SDL_JOYAXISMOTION)
    {
      if ((joystick_axis_set == NULL)
          || (joystick_axis_mask == NULL))
      {
        fprintf(stderr, "tenm_event_handle: SDL_JOYAXISMOTION event happened "
                "while joystick is not initialized, ignoring\n");
        continue;
      }

      if (event.jaxis.axis == 0)
      {
        /* left/right (mutually exclusive) */
        if (joystick_axis_set[TENM_JOYSTICK_LEFT])
        {
          if (event.jaxis.value < -neutral_range)
            tenm_add_key_status(joystick_axis_mask[TENM_JOYSTICK_LEFT], 1);
          else
            tenm_add_key_status(joystick_axis_mask[TENM_JOYSTICK_LEFT], -1);
        }
        if (joystick_axis_set[TENM_JOYSTICK_RIGHT])
        {
          if (event.jaxis.value > neutral_range)
            tenm_add_key_status(joystick_axis_mask[TENM_JOYSTICK_RIGHT], 1);
          else
            tenm_add_key_status(joystick_axis_mask[TENM_JOYSTICK_RIGHT], -1);
        }
      }
      else if (event.jaxis.axis == 1)
      {
        /* up/down (mutually exclusive) */
        if (joystick_axis_set[TENM_JOYSTICK_UP])
        {
          if (event.jaxis.value < -neutral_range)
            tenm_add_key_status(joystick_axis_mask[TENM_JOYSTICK_UP], 1);
          else
            tenm_add_key_status(joystick_axis_mask[TENM_JOYSTICK_UP], -1);
        }
        if (joystick_axis_set[TENM_JOYSTICK_DOWN])
        {
          if (event.jaxis.value > neutral_range)
            tenm_add_key_status(joystick_axis_mask[TENM_JOYSTICK_DOWN], 1);
          else
            tenm_add_key_status(joystick_axis_mask[TENM_JOYSTICK_DOWN], -1);
        }
      }
    }
    else if (event.type == SDL_JOYBUTTONDOWN)
    {
      if ((joystick_button_set == NULL)
          || (joystick_button_mask == NULL))
      {
        fprintf(stderr, "tenm_event_handle: SDL_JOYBUTTONDOWN event happened "
                "while joystick is not initialized, ignoring\n");
        continue;
      }

      /* event.jbutton.button is always nonnegative because it is unsigned */
      if ((event.jbutton.button < TENM_NUMBER_KEY_MAX - 4)
          && (joystick_button_set[event.jbutton.button]))
        tenm_add_key_status(joystick_button_mask[event.jbutton.button], 1);
    }
    else if (event.type == SDL_JOYBUTTONUP)
    {
      if ((joystick_button_set == NULL)
          || (joystick_button_mask == NULL))
      {
        fprintf(stderr, "tenm_event_handle: SDL_JOYBUTTONUP event happened "
                "while joystick is not initialized, ignoring\n");
        continue;
      }

      /* event.jbutton.button is always nonnegative because it is unsigned */
      if ((event.jbutton.button < TENM_NUMBER_KEY_MAX - 4)
          && (joystick_button_set[event.jbutton.button]))
        tenm_add_key_status(joystick_button_mask[event.jbutton.button], -1);
    }
  }
  return 0;
}

/* positive delta means the key is pressed
 * negative delta means the key is released
 */
static int
tenm_add_key_status(tenm_key key, int delta)
{
  int i;
  int value = 0;

  for (i = 0; i < number_key_mask; i++)
    if (key == key_mask[i])
      value = tenm_pow2(i);

  if (value == 0)
  {    
    /* unknown key */
    return 0;
  }

  if (delta > 0)
    key_status |= value;
  else if (delta < 0)
    key_status &= ~value;
  else
    return 1;
  return 0;
}

static void
tenm_key_quit(void)
{
  number_key_mask = 0;
  if (key_mask != NULL)
  {
    free(key_mask);
    key_mask = NULL;
  }
}

static void
tenm_joystick_quit(void)
{
  if (js != NULL)
  {
    SDL_JoystickClose(js);
    js = NULL; 
  }
  if (joystick_axis_set != NULL)
  {
    free(joystick_axis_set);
    joystick_axis_set = NULL;
  }
  if (joystick_axis_mask != NULL)
  {
    free(joystick_axis_mask);
    joystick_axis_mask = NULL;
  }
  if (joystick_button_set != NULL)
  {
    free(joystick_button_set);
    joystick_button_set = NULL;
  }
  if (joystick_button_mask != NULL)
  {
    free(joystick_button_mask);
    joystick_button_mask = NULL;
  }
  neutral_range = 8192;

  if (SDL_WasInit(SDL_INIT_JOYSTICK) != 0)
    SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
}

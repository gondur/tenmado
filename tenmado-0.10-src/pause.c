/* $Id: pause.c,v 1.14 2002/07/10 11:01:37 oohara Exp $ */

#include <stdio.h>

/* WINDOW_WIDTH */
#include "const.h"
/* draw_string() */
#include "util.h"
/* tenm_redraw_window() */
#include "tenm_graphic.h"

#include "pause.h"

static int paused = 0;
static int paused_modify_ok = 1;

static int show_pause_message(void);

void
clear_pause(void)
{
  paused = 0;
  paused_modify_ok = 1;
}

/* return 1 if the game is paused, 0 if not */
int
do_pause(int pause_key_pressed)
{
  if (pause_key_pressed)
  {
    if (paused_modify_ok)
    {
      paused_modify_ok = 0;
      if (paused)
      {
        paused = 0;
      }
      else
      {
        paused = 1;
        if (show_pause_message() != 0)
          fprintf(stderr, "do_pause: show_pause_message failed\n");
      }
    }
  }
  else
  {
    paused_modify_ok = 1;
  }

  if (paused)
    return 1;

  return 0;
}

/* pause the game if the mouse cursor is out of the window
 * note that moving the mouse cursor into the window does not
 * continue the game
 */
void
pause_by_mouse(int gain)
{
  if ((gain != 1) && (!(paused)))
  {
    paused = 1;
    if (show_pause_message() != 0)
      fprintf(stderr, "pause_by_mouse: show_pause_message failed\n");
  }
}

/* return 0 on success, 1 on error */
static int
show_pause_message(void)
{
  int status = 0;

  if (draw_string(10, WINDOW_HEIGHT - 40, "paused --- press p "
                  "to continue", 30) != 0)
  {
    fprintf(stderr, "show_pause_message: draw_string failed\n");
    status = 1;
  }
  if (tenm_redraw_window() != 0)
  {
    fprintf(stderr, "show_pause_message: tenm_redraw_window failed\n");
    status = 1;
  }

  return status;
}

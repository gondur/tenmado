/* $Id: title.c,v 1.21 2009/11/14 02:11:12 oohara Exp $ */

/* VERSION */
#include <config.h>

#include <stdio.h>
/* strlen */
#include <string.h>

#include "tenm_input.h"
#include "tenm_graphic.h"
#include "util.h"
#include "esc-ok.h"
#include "tenm_timer.h"
/* COPYRIGHT_STRING */
#include "const.h"

#include "title.h"

/* return
 * 0 if a real game is requested
 * 1 if a tutorial demo is requested
 * 2 if a high score list is requested
 * 3 if the program should quit
 */
int
title(void)
{
  int i;
  int choice = 0;
  int delay = 6;
  int a_x;
  int a_y;
  int b_x;
  int b_y;
  tenm_color color;
  const char *string = NULL;
  char temp[32];

  /* we don't need to pause in the title */
  tenm_set_focus_handler((void (*)(int)) NULL);

  while (1 == 1)
  {
    if (tenm_event_handle() != 0)
    {
      choice = 3;
      break;
    }
    /* quit if ESC is pressed */
    if (tenm_get_key_status() & 32)
    {
      if (get_esc_ok())
      {
        /* this is completely useless, but let's be pedantic */
        set_esc_ok(0);
        choice = 3;
        break;
      }
    }
    else
    {
      set_esc_ok(1);
    }

    /* proceed if space if pressed */
    if (tenm_get_key_status() & 16)
      break;

    /* move the cursor */
    if (tenm_get_key_status() & 1)
    {
      if (delay <= 0)
      {
        choice--;
        if (choice < 0)
          choice += 4;
        delay = 6;
      }
      else
      {
        delay--;
      }
    }
    else if (tenm_get_key_status() & 2)
    {
      if (delay <= 0)
      {
        choice++;
        if (choice >= 4)
          choice -= 4;
        delay = 6;
      }
      else
      {
        delay--;
      }
    }
    else
    {
      delay = 0;
    }

    tenm_clear_window(tenm_map_color(253, 245, 230));

    for (i = 0; i < 3; i++)
    {
      if (i == 0)
      {
        a_x = 480;
        a_y = 560;
        b_x = 0;
        b_y = 436;
        color = tenm_map_color(0, 63, 255);
      }
      else if (i == 1)
      {
        a_x = 0;
        a_y = 436;
        b_x = 356;
        b_y = 80;
        color = tenm_map_color(127, 0, 255);
      }
      else
      {
        a_x = 356;
        a_y = 80;
        b_x = 480;
        b_y = 560;
        color = tenm_map_color(255, 0, 50);
      }

      if (tenm_draw_line(a_x, a_y, b_x, b_y, 1, color) != 0)
      {
        fprintf(stderr, "title: tenm_draw_line failed (%d)", i);
      }
    }

    if (draw_string(80, 90, "tenmado", 7) != 0)
      fprintf(stderr, "title: draw_string (title) failed\n");

    /* don't use "string" here --- what we need is a buffer,
     * not a pointer
     */
    sprintf(temp, "version %.20s", VERSION);
    if (draw_string(80, 120, temp, (int) strlen(temp)) != 0)
      fprintf(stderr, "title: draw_string (version) failed\n");

    for (i = 0; i < 4; i++)
    {
      if (i == 0)
      {
        string = "start a game";
      }
      else if (i == 1)
      {
        string = "tutorial";
      }
      else if (i == 2)
      {
        string = "high score";
      }
      else
      {
        string = "quit";
      }

      if (draw_string(180, 300 + 30 * i, string, (int) strlen(string)) != 0)
        fprintf(stderr, "title: draw_string (menu %d) failed\n", i + 1);
    }

    if (draw_string(160, 300 + 30 * choice, ">", 1) != 0)
      fprintf(stderr, "title: draw_string (cursor) failed\n");

    if (draw_string(60, 600, COPYRIGHT_STRING,
                    (int) strlen(COPYRIGHT_STRING)) != 0)
      fprintf(stderr, "title: draw_string (copyright) failed\n");

    tenm_redraw_window();

    tenm_wait_next_frame();
  }

  return choice;
}

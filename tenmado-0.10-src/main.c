/* $Id: main.c,v 1.226 2003/09/17 01:12:43 oohara Exp $ */

#include <stdio.h>
/* rand, exit */
#include <stdlib.h>
/* time */
#include <time.h>

#include "const.h"
#include "tenm_graphic.h"
#include "tenm_input.h"
#include "tenm_math.h"
#include "tenm_table.h"
#include "tenm_timer.h"
#include "util.h"
#include "image.h"
/* pause_by_mouse */
#include "pause.h"
#include "loop.h"
#include "title.h"
#include "option.h"
#include "bac_dump.h"
/* DATABASE_DIR */
#include "high-score.h"

/* note that delay granularity is 10 ms */
#define NUM_WAIT 3

int
main(int argc, char *argv[])
{
  int choice;
  int temp;
  int graphic_flag;
  const option *op = NULL;

  if (set_option(argc, argv) != 0)
  {
    fprintf(stderr, "main: set_option failed\n");
    return 1;
  }
  op = get_option();
  if (op == NULL)
  {
    fprintf(stderr, "main: get_option failed\n");
    return 1;
  }

  if (op->help != 0)
  {
    do_help();
    return 0;
  }
  if (op->version != 0)
  {
    do_version();
    return 0;
  }

  if (op->dump >= 0)
  {
    if (dump_high_score(op->dump) != 0)
    {
      fprintf(stderr, "main: dump_high_score failed\n");
      return 1;
    }
    return 0;
  }
  if (op->undump >= 0)
  {
    if (undump_high_score(op->undump) != 0)
    {
      fprintf(stderr, "main: undump_high_score failed\n");
      return 1;
    }
    return 0;
  }
  if (op->undump_default != 0)
  {
    if (read_default_score() != 0)
    {
      fprintf(stderr, "main: read_default_score() failed\n");
      return 1;
    }
    return 0;
  }

  srand((unsigned int) time(NULL));

  if (op->full_screen != 0)
    graphic_flag = TENM_FULLSCREEN;
  else
    graphic_flag = 0;
  tenm_graphic_init(WINDOW_WIDTH, WINDOW_HEIGHT, graphic_flag, "tenmado");
  tenm_clear_window(tenm_map_color(255, 239, 213));
  tenm_timer_init(NUM_WAIT);
  if (tenm_math_init(810, 11) != 0)
    fprintf(stderr, "main: tenm_math_init failed, continuing (can be slow)\n");
  util_init(WINDOW_WIDTH, WINDOW_HEIGHT);
  image_init();
  tenm_table_init(256, -1, 1);
  tenm_set_key(7, TENM_KEY_UP, TENM_KEY_DOWN, TENM_KEY_RIGHT, TENM_KEY_LEFT,
               TENM_KEY_SPACE, TENM_KEY_ESCAPE, TENM_KEY_p);


  temp = tenm_joystick_init(8192);
  if (temp == TENM_JOYSTICK_INIT_ERROR)
  {
    fprintf(stderr, "main: tenm_joystick_init failed\n");
    return 1;
  }
  else if (temp == TENM_JOYSTICK_INIT_NO_JOYSTICK)
  {
    fprintf(stderr, "main: don't worry, just use the keyboard\n");
  }
  else
  {
    tenm_joystick_map_axis(TENM_JOYSTICK_UP, TENM_KEY_UP);
    tenm_joystick_map_axis(TENM_JOYSTICK_DOWN, TENM_KEY_DOWN);
    tenm_joystick_map_axis(TENM_JOYSTICK_LEFT, TENM_KEY_LEFT);
    tenm_joystick_map_axis(TENM_JOYSTICK_RIGHT, TENM_KEY_RIGHT);
    tenm_joystick_map_button(0, TENM_KEY_SPACE);
  }

  choice = title();
  while (choice != 3)
  {
    if ((choice == 0) || (choice == 1))
    {
      if (main_loop(choice) != 0)
        break;
    }
    else
    {
      if (show_high_score() != 0)
        break;
    }
    choice = title();
  }

  image_quit();
  util_quit();

  return 0;
}

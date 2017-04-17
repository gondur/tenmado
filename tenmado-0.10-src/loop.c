/* $Id: loop.c,v 1.53 2002/12/25 12:30:42 oohara Exp $ */

#include <stdio.h>

#include "tenm_object.h"
#include "pause.h"
#include "tenm_table.h"
#include "player.h"
#include "score.h"
#include "ship.h"
#include "chain.h"
#include "hit-status.h"
#include "stage.h"
#include "tenm_timer.h"
#include "tenm_input.h"
#include "scheduler.h"
#include "const.h"
#include "tenm_graphic.h"
#include "result.h"
#include "esc-ok.h"
#include "option.h"

#include "loop.h"

/*
static int draw_all_mass(tenm_object *my, int n);
*/

/* returns 1 if the program should quit, 0 if not */
int
main_loop(int tutorial)
{
  /* for debugging only */
  int t_start = 0;

  int i;
  int t = 0;
  int frame_passed = 0;
  int quit_needed = 0;
  const option *op = NULL;
  tenm_object *player;

  op = get_option();
  if (op == NULL)
  {
    fprintf(stderr, "main_loop: get_option failed\n");
    /* we should have checked the options */
    return 1;
  }

  clear_pause();

  /* we don't need to pause if it is a tutorial */
  if (tutorial == 0)
    tenm_set_focus_handler((void (*)(int)) pause_by_mouse);
  else
    tenm_set_focus_handler((void (*)(int)) NULL);

  tenm_table_clear_all();
  player = player_new(tutorial);

  clear_score();
  set_ship(5);
  clear_chain();
  clear_hit_status();

  if (tutorial == 0)
  {
    if ((op->stage >= 1) && (op->stage <= 5))
      set_stage(op->stage);
    else
      set_stage(1);
  }
  else
  {  
    set_stage(7);
  }
  
  tenm_timer_reset();

  while (1 == 1)
  {
    /* quit the program if a SDL_QUIT event happened 
     * (for example, if a SIGINT signal (sent by Ctrl+c) is received)
     */
    if (tenm_event_handle() != 0)
    {
      quit_needed = 1;
      break;
    }
    /* back to the title if ESC is pressed  */
    if (tenm_get_key_status() & 32)
    {
      if (get_esc_ok())
      {
        set_esc_ok(0);
        break;
      }
    }
    else
    {
      set_esc_ok(1);
    }

    /* pause if it is a real game and if p is pressed
     * we must check "tutorial" first to suppress unnecessary
     * pause message
     */
    if ((tutorial == 0) && (do_pause(tenm_get_key_status() & 64)))
    {
      frame_passed++;
      /* this wait is necessary to save CPU time */
      tenm_wait_next_frame();
      continue;
    }

    for (i = 1; i <= 30; i++)
    {
      if (tenm_table_detect_collision(player) != 0)
        player = NULL;
      if (tenm_table_move(player, 30) != 0)
        player = NULL;
    }

    /* scheduler() must be called before tenm_table_do_action()
     * to clear action_needed flag correctly */
    if (scheduler(t + t_start) == SCHEDULER_NEXT_STAGE)
    {
      clear_chain();
      /* t is incremented at the end of the loop */
      t = -1;
      /* "stage 6" means ending, "stage 7" means tutorial */
      if (get_stage() > 5)
        break;
      /* you can't go to the next stage if you use --stage option */
      if ((op->stage >= 1) && (op->stage <= 5))
        break;
      add_stage(1);
    }

    if (tenm_table_do_action(player) != 0)
      player = NULL;
    calculate_chain();


    /* pure white is bad for your eyes */
    tenm_clear_window(tenm_map_color(253, 245, 230));

    tenm_table_draw(player);
    show_chain();
    show_hit_status();
    show_score();
    show_ship();

    /* for those who want to see the world as it is */
    /* note that
     * (1) the player is not in the table
     * (2) this is slow
     */
    /*
    tenm_table_apply_all((int (*)(tenm_object *, int)) draw_all_mass, 0);
    */

    tenm_redraw_window();

    tenm_wait_next_frame();
    if (op->slow != 0)
    {
      /* wait one more frame */
      tenm_wait_next_frame();
    }

    if (player == NULL)
      break;
    t++;
    frame_passed++;
  }

  if (frame_passed > 0)
  {
    printf("average fps: %f\n", tenm_calculate_fps(frame_passed));
    fflush(stdout);
  }

  tenm_table_clear_all();
  if (player != NULL)
    tenm_object_delete(player);

  if ((!(tutorial)) && (!(quit_needed)))
  {
    /* note that show_result() does not use table */
    if (show_result() != 0)
      quit_needed = 1;
  }

  return quit_needed;
}

#if 0
static int
draw_all_mass(tenm_object *my, int n)
{
  /* sanity check */
  if (my == NULL)
    return 0;

  if (my->mass == NULL)
    return 0;
  if (my->attr == 0)
    return 0;

  if (tenm_draw_mass(my->mass, tenm_map_color(0, 0, 0)) != 0)
  {
    fprintf(stderr, "draw_all_mass: tenm_draw_mass failed (%d)\n",
            my->table_index);
  }

  return 0;
}
#endif /* 0 */

/* $Id: result.c,v 1.44 2004/09/04 17:28:36 oohara Exp $ */

/* VERSION, NOT_HAVE_POSIX */
#include <config.h>

#include <stdio.h>
/* strlen */
#include <string.h>
/* free */
#include <stdlib.h>

#include "stage.h"
#include "score.h"
#include "tenm_graphic.h"
#include "util.h"
#include "tenm_input.h"
#include "tenm_timer.h"
#include "esc-ok.h"
#include "high-score.h"
#include "bac_array.h"
/* bac_compare_entry */
#include "bac_entry.h"
#include "option.h"

#include "result.h"

static int show_rank(bac_array *array_player, bac_array **array_list,
                     int stage, int y);

/* return 1 if the program should quit, 0 if not */
int
show_result(void)
{
  int i;
  int y;
  char temp[32];
  int stage_score[6];
  bac_array *array_player = NULL;
  bac_array **array_list = NULL;
  int no_entry;
  const option *op = NULL;

  /* no need to detect focus loss */
  tenm_set_focus_handler((void (*)(int)) NULL);

  op = get_option();
  if (op == NULL)
  {
    fprintf(stderr, "show_result: get_option failed\n");
    /* we should have checked the options */
    return 1;
  }

  /* no need for high score list stuff if --slow or --stage is specified */
  if ((op->slow == 0) && ((op->stage < 1) || (op->stage > 5)))
  {  
    for (i = 1; i <= 6; i++)
      stage_score[i - 1] = get_stage_score(i);
    array_player = make_high_score(get_score(), stage_score);
    if (array_player == NULL)
    {
      fprintf(stderr, "show_result: make_high_score failed\n");
    }

    no_entry = 0;
    array_list = read_high_score(0, &no_entry);
    if (array_list == NULL)
    {
      if (no_entry != 0)
      {
        printf("show_result: at least one high score file is empty\n");
        fflush(stdout);
      }
      else
      {
        fprintf(stderr, "show_result: read_high_score failed\n");
      }
    }
  }

  tenm_timer_reset();

  tenm_clear_window(tenm_map_color(253, 245, 230));

  if (draw_string(100, 90, "tenmado result", 14) != 0)
    fprintf(stderr, "show_result: draw_string (line 1) failed\n");

  sprintf(temp, "version %.20s", VERSION);
  if (draw_string(100, 120, temp, (int) strlen(temp)) != 0)
    fprintf(stderr, "show_result: draw_string (line 2) failed\n");

#ifndef NOT_HAVE_POSIX
  if (array_player != NULL)
  {
    if (((bac_entry_ver_0 *) (array_player->p[0]))->user_name_available)
      sprintf(temp, "player:  %-12.12s",
               ((bac_entry_ver_0 *) array_player->p[0])->user_name);
    else
      sprintf(temp, "player:  (uid %5d)",
               ((bac_entry_ver_0 *) array_player->p[0])->uid);

    if (draw_string(100, 180, temp, (int) strlen(temp)) != 0)
      fprintf(stderr, "show_result: draw_string (line 3) failed\n");
  }
#endif /* not NOT_HAVE_POSIX */

  sprintf(temp, "total score: %8d", get_score());
  if (draw_string(100, 210, temp, (int) strlen(temp)) != 0)
    fprintf(stderr, "show_result: draw_string (line 4) failed\n");
  if ((array_player != NULL) && (array_list != NULL))
  {
    if (show_rank(array_player, array_list, 0, 210)  != 0)
          fprintf(stderr, "show_result: show_rank (line 4) failed\n");
  }

  for (i = 1; i <= 4; i++)
  {
    y = 240 + i * 30;
    sprintf(temp, "stage %1d:     %8d", i, get_stage_score(i));
    if (draw_string(100, y, temp, (int) strlen(temp)) != 0)
      fprintf(stderr, "show_result: draw_string (line %d) failed\n", i + 4);
    if ((array_player != NULL) && (array_list != NULL))
    {
      if (show_rank(array_player, array_list, i, y)  != 0)
        fprintf(stderr, "show_result: show_rank (line %d) failed\n", i + 4);
    }
  }

  sprintf(temp, "final stage: %8d", get_stage_score(5));
  if (draw_string(100, 390, temp, (int) strlen(temp)) != 0)
    fprintf(stderr, "show_result: draw_string (line 9) failed\n");
  if ((array_player != NULL) && (array_list != NULL))
  {
    if (show_rank(array_player, array_list, 5, 390) != 0)
      fprintf(stderr, "show_result: show_rank (line 9) failed\n");
  }

  if (get_stage() == 6)
  {
    /* the player cleared the entire game */
    sprintf(temp, "ship bonus:  %8d", get_stage_score(6));
    if (draw_string(100, 420, temp, (int) strlen(temp)) != 0)
      fprintf(stderr, "show_result: draw_string (line 10) failed\n");
    /* there is no score list for stage 6 */
  }

  if (draw_string(160, 600, "press ESC to quit", 17) != 0)
    fprintf(stderr, "show_result: draw_string (ESC instruction) failed\n");

  tenm_redraw_window();

  if (array_player != NULL)
  {
    if (save_high_score(array_player) != 0)
      fprintf(stderr, "show_result: save_high_score failed\n");
    if (array_list != NULL)
    {
      if (truncate_high_score(array_list) != 0)
        fprintf(stderr, "show_result: truncate_high_score failed\n");
    }
  }

  while (1 == 1)
  {
    /* quit the program if a SDL_QUIT event happened
     * (for example, if a SIGINT signal (sent by Ctrl+c) is received)
     */
    if (tenm_event_handle() != 0)
    {
      if (array_list != NULL)
      {
        for (i = 0; i < 6; i++)
          bac_array_delete(array_list[i]);
        free(array_list);
      }
      if (array_player != NULL)
        bac_array_delete(array_player);
      return 1;
    }

    /* back to the title if ESC is pressed */
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
    
    /* this wait is necessary to save CPU time */
    tenm_wait_next_frame();
  }

  if (array_list != NULL)
  {
    for (i = 0; i < 6; i++)
      bac_array_delete(array_list[i]);
    free(array_list);
  }
  if (array_player != NULL)
    bac_array_delete(array_player);
  return 0;
}

/* return 0 on success, 1 on error */
static int
show_rank(bac_array *array_player, bac_array **array_list,
          int stage, int y)
{
  int i;
  int status = 0;
  char temp[8];

  /* sanity check */
  if (array_player == NULL)
    return 0;
  if (array_player->n < 6)
    return 0;
  if (array_list == NULL)
    return 0;

  for (i = 0; i < high_score_list_size(stage); i++)
  {
    if ((i >= (array_list[stage])->n)
        || (bac_compare_entry(&(array_player->p[stage]),
                              &((array_list[stage])->p[i])) < 0))
    {
      sprintf(temp, "(#%3d)", i + 1);
      temp[8 - 1] = '\0';
      if (draw_string(300, y, temp, (int) strlen(temp)) != 0)
      {
        fprintf(stderr, "show_rank: draw_string (stage %d) failed\n", stage);
        status = 1;
      }
      break;
    }
  }

  return status;
}

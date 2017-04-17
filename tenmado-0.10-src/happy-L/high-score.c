/* $Id: high-score.c,v 1.121 2003/01/13 01:49:19 oohara Exp $ */

/* VERSION, HAVE_LIBDB3, HAVE_LIBDB_3, NOT_HAVE_POSIX */
#include <config.h>

#include <stdio.h>
/* time, struct tm */
#include <time.h>
/* malloc */
#include <stdlib.h>
/* strlen */
#include <string.h>

#ifndef NOT_HAVE_POSIX
/* getuid */
#include <sys/types.h>
/* getuid */
#include <unistd.h>
/* getpwuid */
#include <pwd.h>
#endif /* not NOT_HAVE_POSIX */

#include "bac_entry.h"
#include "bac_save.h"
#include "bac_dump.h"
#include "tenm_input.h"
#include "bac_array.h"
#include "bac_load.h"
#include "tenm_graphic.h"
#include "util.h"
#include "esc-ok.h"
#include "tenm_timer.h"
#include "const.h"
#include "bac_truncate.h"
#include "bac_string_to_array.h"

#include "high-score.h"

#ifdef NOT_HAVE_POSIX
#define DATABASE_DIR "./save/"
#define DEFAULT_DIR "./happy-L/default/"
#else /* not NOT_HAVE_POSIX */
#define DATABASE_DIR LOCALSTATEDIR "/games/tenmado/"
#define DEFAULT_DIR DATADIR "/games/tenmado/default/"
#endif /* not NOT_HAVE_POSIX */

#define DATABASE_MODE 0644

static char *high_score_file_name(int stage);
static char *default_file_name(int stage);

int
high_score_list_size(int stage)
{
  /* sanity check */
  if ((stage < 0) || (stage > 5))
    return 1;

  if (stage == 0)
    return 100;

  return 20;
}

bac_array *
make_high_score(int total_score, const int *stage_score)
{
  bac_entry *entry;
  bac_array *array = NULL;
  bac_array *temp_array = NULL;
  int score_sort = 0;
  int user_name_available = 0;
  const char *user_name = NULL;
  int i;
  uid_t u;

#ifndef NOT_HAVE_POSIX
  struct passwd *pw = NULL;
#endif /* not NOT_HAVE_POSIX */

  /* sanity check */
  if (stage_score == NULL)
    return NULL;

#ifdef NOT_HAVE_POSIX
  u = 0;
  user_name_available = 0;
  user_name = NULL;
#else /* not NOT_HAVE_POSIX */
  u = getuid();
  pw = getpwuid(u);
  if (pw == NULL)
  {
    user_name_available = 0;
    user_name = NULL;
  }
  else
  {
    user_name_available = 1;
    user_name = (const char *) (pw->pw_name);
  }
#endif /* not NOT_HAVE_POSIX */

  /* there is no high score list for stage 6 */
  for (i = 0; i < 6; i++)
  {
    if (i == 0)
      score_sort = total_score;
    else
      score_sort = stage_score[i - 1];

    entry = bac_entry_new(0, 0, time(NULL), u, user_name_available,
                          user_name, score_sort, total_score, 6, stage_score);
    if (entry == NULL)
    {
      fprintf(stderr, "make_high_score: bac_entry_new(%d) failed\n", i);
      if (array != NULL)
        bac_array_delete(array);
      return NULL;
    }

    temp_array = bac_array_add_entry(array, entry);
    if (temp_array == NULL)
    {
      fprintf(stderr, "make_high_score: bac_array_add_entry(%d) failed\n", i);
      (entry->delete)(entry);
      if (array != NULL)
        bac_array_delete(array);
      return NULL;
    }
    array = temp_array;
  }

  return array;
}

/* return 0 on success, 1 on error
 * assumes DATABASE_DIR already exists --- if not, it is an error
 */
int
save_high_score(bac_array *array)
{
  int i;
  char *file = NULL;

  /* sanity check */
  if (array == NULL)
    return 0;
  if (array->n < 6)
    return 0;

  /* there is no high score list for stage 6 */
  for (i = 0; i < 6; i++)
  {
    file = high_score_file_name(i);
    if (file == NULL)
    {
      fprintf(stderr, "save_high_score: high_score_file_name(%d) failed\n",
              i);
      return 1;
    }

    if (bac_save(DATABASE_DIR, file, (array->p)[i], DATABASE_MODE) != 0)
    {
      fprintf(stderr, "save_high_score: bac_save(%d) failed\n", i);
      free(file);
      return 1;
    }

    free(file);
  }

  return 0;
}

/* stage 0 means total score
 * return 0 on success, 1 on error
 * assumes there is at least one entry
 */
int
dump_high_score(int stage)
{
  char *file = NULL;

  file = high_score_file_name(stage);
  if (file == NULL)
  {
    fprintf(stderr, "dump_high_score: high_score_file_name failed\n");
    return 1;
  }

  if (bac_dump(DATABASE_DIR, file, DATABASE_MODE) != 0)
  {
    fprintf(stderr, "dump_high_score: bac_dump failed\n");
    free(file);
    return 1;
  }

  free(file);
  return 0;
}

/* stage 0 means total score
 * return 0 on success, 1 on error
 */
int
undump_high_score(int stage)
{
  char *file = NULL;

  file = high_score_file_name(stage);
  if (file == NULL)
  {
    fprintf(stderr, "undump_high_score: high_score_file_name failed\n");
    return 1;
  }

  if (bac_undump(DATABASE_DIR, file, DATABASE_MODE) != 0)
  {
    fprintf(stderr, "dump_high_score: bac_dump failed\n");
    free(file);
    return 1;
  }

  free(file);
  return 0;
}

/* return 1 if the program should quit, 0 if not */
int
show_high_score(void)
{
  int i;
  int stage = 0;
  int delay_stage = 6;
  int rank = 0;
  int delay_rank = 6;
  int quit_needed = 0;
  bac_array **p = NULL;
  bac_entry *entry;
  char string[40];
  int timer_cursor = 0;
  struct tm *broken_time = NULL;
  int no_entry;

  /* no need to detect focus loss */
  tenm_set_focus_handler((void (*)(int)) NULL);

  no_entry = 0;
  p = read_high_score(BAC_LOAD_FLAG_NO_DELETE, &no_entry);
  if (p == NULL)
  {
    if (no_entry != 0)
    {
      printf("show_high_score: at least one list is empty\n");
      fflush(stdout);
      /* continue and show a blank list */
    }
    else
    {
      /* real error */
      fprintf(stderr, "show_high_score: read_high_score failed\n");
      return 1;
    }
  }

  while (1 == 1)
  {
    if (tenm_event_handle() != 0)
    {
      quit_needed = 1;
      break;
    }
    /* return to the title if ESC is pressed */
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

    if (tenm_get_key_status() & 2)
    {
      if ((delay_rank <= 0) || (tenm_get_key_status() & 16))
      {
        if ((p != NULL) && (rank + 1 + 9 < (p[stage])->n)
            && (rank + 1 + 9 < high_score_list_size(stage)))
          rank++;
        delay_rank = 6;
      }
      else
      {
        delay_rank--;
      }
    }
    else if (tenm_get_key_status() & 1)
    {
      if ((delay_rank <= 0) || (tenm_get_key_status() & 16))
      {
        if (rank - 1 >= 0)
          rank--;
        delay_rank = 6;
      }
      else
      {
        delay_rank--;
      }
    }
    else
    {
      delay_rank = 0;
    }

    if (tenm_get_key_status() & 4)
    {
      if (delay_stage <= 0)
      {
        stage++;
        if (stage > 5)
          stage -= 6;
        delay_stage = 6;
        rank = 0;
      }
      else
      {
        delay_stage--;
      }
    }
    else if (tenm_get_key_status() & 8)
    {
      if (delay_stage <= 0)
      {
        stage--;
        if (stage < 0)
          stage += 6;
        delay_stage = 6;
        rank = 0;
      }
      else
      {
        delay_stage--;
      }
    }
    else
    {
      delay_stage = 0;
    }

    tenm_clear_window(tenm_map_color(253, 245, 230));

    sprintf(string, "tenmado high score (version %.9s)", VERSION);
    string[40 - 1] = '\0';
    if (draw_string(30, 30, string, (int) strlen(string)) != 0)
      fprintf(stderr, "show_high_score: draw_string (high score title) "
              "failed\n");

    if (stage == 0)
      sprintf(string, "total score");
    else if (stage == 5)
      sprintf(string, "final stage");
    else if ((stage >= 1) && (stage <= 4))
      sprintf(string, "stage %d", stage);
    else
      sprintf(string, "stage ?");
    string[40 - 1] = '\0';
    if (draw_string(180, 60, string, (int) strlen(string)) != 0)
      fprintf(stderr, "show_high_score: draw_string (stage) failed\n");

    if (timer_cursor < 20)
    {
      if (draw_string(60, 60, "<", 1) != 0)
        fprintf(stderr, "show_high_score: draw_string "
                "(left cursor) failed\n");
      if (draw_string(WINDOW_WIDTH - 60, 60, ">", 1) != 0)
        fprintf(stderr, "show_high_score: draw_string "
                "(right cursor) failed\n");
      if (rank > 0)
      {
        if (draw_string(240, 90, "^", 1) != 0)
          fprintf(stderr, "show_high_score: draw_string "
                  "(up cursor) failed\n");
      }
      if ((p != NULL) && (rank + 10 < (p[stage])->n)
          && (rank + 10 < high_score_list_size(stage)))
      {
        if (draw_string(240, 420, "v", 1) != 0)
          fprintf(stderr, "show_high_score: draw_string "
                  "(down cursor) failed\n");
      }
    }
    timer_cursor++;
    if (timer_cursor >= 40)
      timer_cursor = 0;

    if (p != NULL)
    {
      for (i = 0; (i < 10) && (i + rank< (p[stage])->n); i++)
      {
        entry = (p[stage])->p[i + rank];
        sprintf(string, "%3d  %8d  ", i + rank + 1,
                 ((bac_entry_ver_0 *) entry)->score_sort);
#ifndef NOT_HAVE_POSIX
        if ((((bac_entry_ver_0 *) entry)->user_name_available)
            && (((bac_entry_ver_0 *) entry)->user_name != NULL))
          sprintf(string + strlen(string),
                  "%-12.12s", ((bac_entry_ver_0 *) entry)->user_name);
        else
          sprintf(string + strlen(string),
                  "(uid %5d) ", ((bac_entry_ver_0 *) entry)->uid);
#endif /* not NOT_HAVE_POSIX */
        broken_time = localtime(&(((bac_entry_ver_0 *) entry)->when));
        sprintf(string + strlen(string),
                "  %4d/%2d/%2d", broken_time->tm_year + 1900,
                 broken_time->tm_mon + 1, broken_time->tm_mday);
        string[40 - 1] = '\0';
        if (draw_string(30, 120 + 30 * i, string, (int) strlen(string)) != 0)
          fprintf(stderr, "show_high_score: draw_string (#%d) failed\n",
                  i + rank + 1);
      }
    }

    if (draw_string(110, 570, "press space for fast up/down", 28) != 0)
      fprintf(stderr, "show_high_score: draw_string (space instruction) "
              "failed\n");
    if (draw_string(160, 600, "press ESC to quit", 17) != 0)
      fprintf(stderr, "show_high_score: draw_string (ESC instruction) "
              "failed\n");

    tenm_redraw_window();

    tenm_wait_next_frame();
  }

  if (p != NULL)
  {
    for (i = 0; i < 6; i++)
      bac_array_delete(p[i]);
    free(p);
  }

  return quit_needed;
}

/* assumes every high score list has at least one entry
 * --- if not, read no entry at all and returns NULL
 */
bac_array **
read_high_score(int flag, int *no_entry)
{
  int i;
  int j;
  char *file;
  bac_array **temp = NULL;

  temp = (bac_array **) malloc(sizeof(bac_array *) * 6);
  if (temp == NULL)
  {
    fprintf(stderr, "read_high_score: malloc(temp) failed\n");
    return NULL;
  }

  for (i = 0; i < 6; i++)
  {
    file = high_score_file_name(i);
    if (file == NULL)
    {
      fprintf(stderr, "read_high_score: high_score_file_name(%d) "
              "failed\n", i);
      for (j = 0; j < i; j++)
        bac_array_delete(temp[j]);
      free(temp);
      return NULL;
    }
    temp[i] = bac_load(DATABASE_DIR, file, high_score_list_size(i), flag,
                       DATABASE_MODE, no_entry);
    if (temp[i] == NULL)
    {
      if (*no_entry != 0)
      {
        printf("read_high_score: no entry in %s%s\n", DATABASE_DIR, file);
        fflush(stdout);
      }
      else
      {
        fprintf(stderr, "read_high_score: bac_load(%d) failed\n", i);
      }
      free(file);
      for (j = 0; j < i; j++)
        bac_array_delete(temp[j]);
      free(temp);
      return NULL;
    }
    free(file);
  }

  return temp;
}

/* return 0 on success, 1 on error */
int
truncate_high_score(bac_array **array_list)
{
  int stage;
  int size;
  char *file = NULL;

  /* sanity check */
  if (array_list == NULL)
    return 0;

  for (stage = 0; stage < 6; stage++)
  {
    if (array_list[stage] == NULL)
      continue;
    size = high_score_list_size(stage);
    if ((array_list[stage])->n <= size)
      continue;
    file = high_score_file_name(stage);
    if (file == NULL)
    {
      fprintf(stderr, "truncate_high_score: high_score_file_name(%d) "
              "failed\n", stage);
      return 1;
    }
    if (bac_truncate(DATABASE_DIR, file,
                     (array_list[stage])->p[size - 1], DATABASE_MODE) != 0)
    {
      fprintf(stderr, "truncate_high_score: bac_truncate(%d) failed\n", stage);
      free(file);
      return 1;
    }
    free(file);
  }

  return 0;
}

/* return 0 on success, 1 on error */
int
read_default_score(void)
{
  int i;
  char *file_default;
  char *file_list;
  int stage;
  FILE *fp = NULL;
  bac_array **array_list = NULL;
  bac_array *array_default = NULL;
  int no_entry;

  /* see if there is already an entry */
  no_entry = 0;
  array_list = read_high_score(BAC_LOAD_FLAG_NO_SORT, &no_entry);
  if (array_list != NULL)
  {
    for (i = 0; i < 6; i++)
      bac_array_delete(array_list[i]);
    free(array_list);
    return 0;
  }
  if (no_entry == 0)
  {
    fprintf(stderr, "read_default_score: read_high_score failed\n");
    return 1;
  }

  printf("read_default_score: reading default score files\n");
  fflush(stdout);

  for (stage = 0; stage < 6; stage++)
  {
    file_default = default_file_name(stage);
    if (file_default == NULL)
    {
      fprintf(stderr, "read_default_score: default_file_name(%d) failed\n",
              stage);
      return 1;
    }
    file_list = high_score_file_name(stage);
    if (file_list == NULL)
    {
      fprintf(stderr, "read_default_score: high_score_file_name(%d) failed\n",
              stage);
      free(file_default);
      return 1;
    }
    fp = fopen(file_default, "r");
    if (fp == NULL)
    {
      fprintf(stderr, "read_default_score: fopen(%d) failed\n",
              stage);
      free(file_list);
      free(file_default);
      return 1;
    }
    array_default = bac_file_to_array(fp);
    if (array_default == NULL)
    {
      fprintf(stderr, "read_default_score: bac_file_to_array(%d) failed\n",
              stage);
      fclose(fp);
      free(file_list);
      free(file_default);
      return 1;
    }
    for (i = 0; i < array_default->n; i++)
    {
      if (bac_save(DATABASE_DIR, file_list, array_default->p[i],
                   DATABASE_MODE) != 0)
      {
        fprintf(stderr, "read_default_score: bac_save(stage %d entry %d) "
                "failed\n", stage, i);
        bac_array_delete(array_default);
        fclose(fp);
        free(file_list);
        free(file_default);
        return 1;
      }
    }
    bac_array_delete(array_default);
    fclose(fp);
    free(file_list);
    free(file_default);
  }

  return 0;
}

/* return value is relative to DATABASE_DIR */
static char *
high_score_file_name(int stage)
{
  char *temp;

  /* sanity check */
  if ((stage < 0) || (stage > 5))
    return NULL;

  temp = (char *) malloc(sizeof(char) * 20);
  if (temp == NULL)
  {
    fprintf(stderr, "high_score_file_name: malloc failed\n");
    return NULL;
  }

#if (defined(HAVE_LIBDB3) || defined(HAVE_LIBDB_3))
  if (stage == 0)
    sprintf(temp, "score-total.db3");
  else
    sprintf(temp, "score-stage-%d.db3", stage);
#else /* not (defined(HAVE_LIBDB3) || defined(HAVE_LIBDB_3)) */
  if (stage == 0)
    sprintf(temp, "score-total.txt");
  else
    sprintf(temp, "score-stage-%d.txt", stage);
#endif /* not (defined(HAVE_LIBDB3) || defined(HAVE_LIBDB_3)) */

  /* be paranoid */
  temp[20 - 1] = '\0';

  return temp;
}

/* return value is absolute */
static char *
default_file_name(int stage)
{
  size_t size;
  char *temp;

  /* sanity check */
  if ((stage < 0) || (stage > 5))
    return NULL;

  size = strlen(DEFAULT_DIR) + 20;
  temp = (char *) malloc(sizeof(char) * size);
  if (temp == NULL)
  {
    fprintf(stderr, "high_score_file_name: malloc failed\n");
    return NULL;
  }

  sprintf(temp, DEFAULT_DIR);
  /* be paranoid */
  temp[size - 1] = '\0';
  if (stage == 0)
    sprintf(temp + strlen(temp),
             "score-total.txt");
  else
    sprintf(temp + strlen(temp),
             "score-stage-%d.txt", stage);
  temp[size - 1] = '\0';

  return temp;
}

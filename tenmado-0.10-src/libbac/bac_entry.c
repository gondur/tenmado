/* $Id: bac_entry.c,v 1.44 2003/01/14 03:53:22 oohara Exp $ */

#include <stdio.h>
/* malloc */
#include <stdlib.h>
/* strdup, strlen */
#include <string.h>
/* isprint */
#include <ctype.h>
/* CHAR_BIT */
#include <limits.h>

#include "bac_entry.h"

static void bac_entry_delete(bac_entry *p);
static char *bac_entry_escape(const char *p);
static int bac_compare_entry_ver_0(const void *a, const void *b);

/* user_name and score_stage are duplicated --- you must free them
 */
bac_entry *
bac_entry_new(int score_entry_version, int stage_data_version,
              time_t when, uid_t uid, int user_name_available,
              const char *user_name, int score_sort, int score_total,
              int number_stage, const int *score_stage)
{
  int i;
  bac_entry_ver_0 *temp = NULL;

  /* sanity check */
  if (score_entry_version != 0)
  {
    fprintf(stderr, "bac_entry_new: not supported score_entry_version "
            "(%d)\n", score_entry_version);
    return NULL;
  }
  if ((user_name_available != 0) && (user_name == NULL))
  {
    fprintf(stderr, "bac_entry_new: user_name is NULL\n");
    return NULL;
  }
  if (number_stage <= 0)
  {
    fprintf(stderr, "bac_entry_new: non-positive number_stage (%d)\n",
            number_stage);
    return NULL;
  }
  if (score_stage == NULL)
  {
    fprintf(stderr, "bac_entry_new: score_stage is NULL\n");
    return NULL;
  }

  temp = (bac_entry_ver_0 *) malloc(sizeof(bac_entry_ver_0));
  if (temp == NULL)
  {
    fprintf(stderr, "bac_entry_new: malloc(temp) failed\n");
    return NULL;
  }
  temp->score_stage = (int *) malloc(sizeof(int) * number_stage);
  if (temp->score_stage == NULL)
  {
    fprintf(stderr, "bac_entry_new: malloc(temp->score_stage) failed\n");
    free(temp);
    return NULL;
  }

  ((bac_entry *) temp)->klass = score_entry_version;
  ((bac_entry *) temp)->delete = (void (*)(void *)) bac_entry_delete;

  temp->stage_data_version = stage_data_version;
  temp->when = when;
  temp->uid = uid;
  temp->user_name_available = user_name_available;
  if (user_name_available != 0)
  {
    temp->user_name = bac_entry_escape(user_name);
    if (temp->user_name == NULL)
    {
      fprintf(stderr, "bac_entry_new: bac_entry_escape failed\n");
      free(temp->score_stage);
      free(temp);
      return NULL;
    }
  }
  temp->score_sort = score_sort;
  temp->score_total = score_total;
  temp->number_stage = number_stage;
  for (i = 0; i < number_stage; i++)
    temp->score_stage[i] = score_stage[i];

  return ((bac_entry *) temp);
}

static void
bac_entry_delete(bac_entry *p)
{
  /* sanity check */
  if (p == NULL)
  {
    fprintf(stderr, "bac_entry_delete: p is NULL\n");
    return;
  }
  if (p->klass != 0)
  {
    fprintf(stderr, "bac_entry_new: not supported score_entry_version "
            "(%d)\n", p->klass);
    return;
  }

  if ((((bac_entry_ver_0 *) p)->user_name_available != 0)
      &&(((bac_entry_ver_0 *) p)->user_name != NULL))
    free(((bac_entry_ver_0 *) p)->user_name);

  free(((bac_entry_ver_0 *) p)->score_stage);
  free(p);
}

static char *
bac_entry_escape(const char *p)
{
  int i;
  int length;
  int length_allocated = 0;
  int length_used;
  char *temp = NULL;
  char *temp2 = NULL;

  /* sanity check */
  if (p == NULL)
  {
    fprintf(stderr, "bac_entry_escape: p is NULL\n");
    return NULL;
  }

  length = strlen(p);
  length_used = 0;
  /* +1 is for the trailing \0 */
  temp = (char *) malloc(sizeof(char) * (length + 1));
  if (temp == NULL)
  {
    fprintf(stderr, "bac_entry_escape: malloc failed\n");
    return NULL;
  }
  length_allocated = length + 1;

  for (i = 0; i < length; i++)
  {
    /* usually, \t and \n are non-printable, but there may be
     * some problematic locale */
    if ((isprint(p[i])) && (p[i] != '\t') && (p[i] != '\n'))
    {
      temp[length_used] = p[i];
      length_used++;
    }
    else
    {
      fprintf(stderr, "bac_entry_escape: unprintable char found "
              "(\\x%x), escaping\n", p[i]);
      /* who said char is 8 bit? */
      temp2 = (char *) realloc(temp,
                               sizeof(char) * (length_allocated + strlen("\\")
                                               + CHAR_BIT * sizeof(char)));
      if (temp2 == NULL)
      {
        fprintf(stderr, "bac_entry_escape: realloc failed\n");
        free(temp);
        return NULL;
      }
      temp = temp2;
      length_allocated += strlen("\\") + CHAR_BIT * sizeof(char);
      sprintf(temp + length_used, "\\x%x", p[i]);
      /* we allocated enough memory for the trailing \0,
       * so we can just use strlen here */
      length_used = strlen(temp);
    }
  }

  temp[length_allocated - 1] = '\0';
  return temp;
}

/* for qsort()
 * return a positive value if a (arg 1) should be after b (arg 2)
 *        a negative value if a (arg 1) should be before b (arg 2)
 *        0 if a (arg 1) and b (arg 2) are the same
 */

/* this void * should be bac_entry ** */
int
bac_compare_entry(const void *a, const void *b)
{
  const bac_entry *a_temp = *((const bac_entry * const *) a);
  const bac_entry *b_temp = *((const bac_entry * const *) b);

  /* this is a nonsense because we assume klass == 0 */
  if (a_temp->klass > b_temp->klass)
    return -1;
  else if (a_temp->klass < b_temp->klass)
    return 1;

  return bac_compare_entry_ver_0(a, b);
}

/* this void * should be bac_entry ** (no ver_0 here) */
static int
bac_compare_entry_ver_0(const void *a, const void *b)
{
  int i;

  const bac_entry_ver_0 *a_temp = ((const bac_entry_ver_0 *)
                                   *((const bac_entry * const *) a));
  const bac_entry_ver_0 *b_temp = ((const bac_entry_ver_0 *)
                                   *((const bac_entry * const *) b));

  if (a_temp->stage_data_version > b_temp->stage_data_version)
    return -1;
  else if (a_temp->stage_data_version < b_temp->stage_data_version)
    return 1;
  else if (a_temp->score_sort > b_temp->score_sort)
    return -1;
  else if (a_temp->score_sort < b_temp->score_sort)
    return 1;
  /* older entry comes first */
  else if (difftime(a_temp->when, b_temp->when) < 0)
    return -1;
  else if (difftime(a_temp->when, b_temp->when) > 0)
    return 1;
  else if (a_temp->score_total > b_temp->score_total)
    return -1;
  else if (a_temp->score_total < b_temp->score_total)
    return 1;

  for (i = 1; (i <= a_temp->number_stage) && (i <= b_temp->number_stage); i++)
  {
    if (a_temp->score_stage[i - 1] > b_temp->score_stage[i - 1])
      return -1;
    else if (a_temp->score_stage[i - 1] < b_temp->score_stage[i - 1])
      return 1;
  }

  if ((a_temp->user_name_available) && (b_temp->user_name_available)
      && (a_temp->user_name != NULL) && (b_temp->user_name != NULL))
  {
    if (strcmp(a_temp->user_name, b_temp->user_name) != 0)
      return strcmp(a_temp->user_name, b_temp->user_name);
  }

  /* so that non-root users are favored */
  if (a_temp->uid > b_temp->uid)
    return -1;
  else if (a_temp->uid < b_temp->uid)
    return 1;

  return 0;
}

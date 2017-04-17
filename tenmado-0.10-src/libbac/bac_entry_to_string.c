/* $Id: bac_entry_to_string.c,v 1.24 2003/01/14 03:56:36 oohara Exp $ */

#include <stdio.h>
/* malloc */
#include <stdlib.h>
/* strdup, strlen */
#include <string.h>
/* time_t */
#include <time.h>
/* uid_t */
#include <sys/types.h>
/* CHAR_BIT */
#include <limits.h>
/* isprint */
#include <ctype.h>

#include "bac_entry_to_string.h"

char *
bac_entry_to_string(bac_entry *entry)
{
  int i;
  char *temp = NULL;
  char *temp2 = NULL;
  int length = 0;
  int length_used = 0;

  /* sanity check */
  if (entry == NULL)
  {
    fprintf(stderr, "bac_entry_to_string: entry is NULL\n");
    return NULL;
  }
  if (entry->klass != 0)
  {
    fprintf(stderr, "bac_entry_to_string: not supported score_entry_version "
            "(%d)\n", entry->klass);
    return NULL;
  }
  if ((((bac_entry_ver_0 *) entry)->user_name_available != 0)
      && (((bac_entry_ver_0 *) entry)->user_name == NULL))
  {
    fprintf(stderr, "bac_entry_to_string: entry->user_name is NULL\n");
    return NULL;
  }
  if (((bac_entry_ver_0 *) entry)->number_stage <= 0)
  {
    fprintf(stderr, "bac_entry_to_string: entry->number_stage is "
            "non-positive\n");
    return NULL;
  }
  if (((bac_entry_ver_0 *) entry)->score_stage == NULL)
  {
    fprintf(stderr, "bac_entry_to_string: entry->score_stage is NULL\n");
    return NULL;
  }

  if (isprint('\n'))
  {
    fprintf(stderr, "bac_entry_to_string: \\n is printable\n");
    return NULL;
  }
  if (isprint('\t'))
  {
    fprintf(stderr, "bac_entry_to_string: \\t is printable\n");
    return NULL;
  }

  /* entry_version */
  length += strlen("Entry-Version") + strlen("\t") + strlen("\n");
  length += CHAR_BIT * sizeof(int);
  /* stage_data_version */
  length += strlen("Stage-Data-Version") + strlen("\t") + strlen("\n");
  length += CHAR_BIT * sizeof(int);
  /* when */
  length += strlen("When") + strlen("\t") + strlen("\n");
  length += CHAR_BIT * sizeof(time_t);
  /* uid */
  length += strlen("Uid") + strlen("\t") + strlen("\n");
  length += CHAR_BIT * sizeof(uid_t);
  /* there is no field for user_name_available */
  if (((bac_entry_ver_0 *) entry)->user_name_available != 0)
  {
    /* user_name */
    length += strlen("User-Name") + strlen("\t") + strlen("\n");
    length += strlen(((bac_entry_ver_0 *) entry)->user_name);
  }
  /* score_sort */
  length += strlen("Score-Sort") + strlen("\t") + strlen("\n");
  length += CHAR_BIT * sizeof(int);
  /* score_total */
  length += strlen("Score-Total") + strlen("\t") + strlen("\n");
  length += CHAR_BIT * sizeof(int);
  /* number_stage */
  length += strlen("Number-Stage") + strlen("\t") + strlen("\n");
  length += CHAR_BIT * sizeof(int);
  /* score_stage */
  length += (strlen("Score-Stage") + strlen("\t") * 2 + strlen("\n"))
    * ((bac_entry_ver_0 *) entry)->number_stage;
  length += CHAR_BIT * sizeof(int) * 2
    * ((bac_entry_ver_0 *) entry)->number_stage;
  /* terminating \n */
  length += strlen("\n");
  /* trailing \0 */
  length += 1;

  temp = (char *) malloc(sizeof(char) * length);
  if (temp == NULL)
  {
    fprintf(stderr, "bac_entry_to_string: malloc failed\n");
    return NULL;
  }

  sprintf(temp, "Entry-Version\t%d\n"
           "Stage-Data-Version\t%d\n"
           "When\t%d\n"
           "Uid\t%d\n",
           0,
           ((bac_entry_ver_0 *) entry)->stage_data_version,
           (int) (((bac_entry_ver_0 *) entry)->when),
           (int) (((bac_entry_ver_0 *) entry)->uid));
  /* be paranoid */
  temp[length - 1] = '\0';
  length_used = strlen(temp);
  if (length_used >= length)
  {
    fprintf(stderr, "bac_entry_to_string: temp is too short\n");
    free(temp);
    return NULL;
  }
  if (((bac_entry_ver_0 *) entry)->user_name_available != 0)
  {
    sprintf(temp + length_used,
             "User-Name\t%s\n",
             ((bac_entry_ver_0 *) entry)->user_name);
    temp[length - 1] = '\0';
    length_used = strlen(temp);
    if (length_used >= length)
    {
      fprintf(stderr, "bac_entry_to_string: temp is too short\n");
      free(temp);
      return NULL;
    }
  }
  sprintf(temp + length_used,
           "Score-Sort\t%d\n"
           "Score-Total\t%d\n"
           "Number-Stage\t%d\n",
           ((bac_entry_ver_0 *) entry)->score_sort,
           ((bac_entry_ver_0 *) entry)->score_total,
           ((bac_entry_ver_0 *) entry)->number_stage);
  temp[length - 1] = '\0';
  for (i = 1; i <= ((bac_entry_ver_0 *) entry)->number_stage; i++)
  {
    length_used = strlen(temp);
    if (length_used >= length)
    {
      fprintf(stderr, "bac_entry_to_string: temp is too short\n");
      free(temp);
      return NULL;
    }
    sprintf(temp + length_used,
             "Score-Stage\t%d\t%d\n", i,
             ((bac_entry_ver_0 *) entry)->score_stage[i - 1]);
    temp[length - 1] = '\0';
  }
  
  length_used = strlen(temp);
  if (length_used + 1 >= length)
  {
    fprintf(stderr, "bac_entry_to_string: temp is too short\n");
    free(temp);
    return NULL;
  }
  temp[length_used] = '\n';
  temp[length_used + 1] = '\0';

  /* free unused memory */
  /* +2 (not +1) is for the trailing \0 */
  temp2 = (char *) realloc(temp, sizeof(char) * ((size_t) (length_used + 2)));
  if (temp2 == NULL)
  {
    fprintf(stderr, "bac_entry_to_string: realloc failed\n");
    free(temp);
    return NULL;
  }
  temp = temp2;

  return temp;
}

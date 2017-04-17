/* $Id: bac_dump.c,v 1.14 2003/01/14 03:50:15 oohara Exp $ */

/* NOT_HAVE_POSIX */
#include <config.h>

#include <stdio.h>
/* free */
#include <stdlib.h>

#ifndef NOT_HAVE_POSIX
/* seteuid */
#include <sys/types.h>
/* seteuid */
#include <unistd.h>
#endif /* not NOT_HAVE_POSIX */

#include "bac_array.h"
#include "bac_load.h"
#include "bac_entry_to_string.h"
#include "bac_string_to_array.h"
#include "bac_save.h"

#include "bac_dump.h"

/* return 0 on success, 1 on error */
int
bac_dump(const char *dir, const char *file, int mode)
{
  int i;
  bac_array *array = NULL;
  char *string = NULL;
  int no_entry;

  /* sanity check */
  if (dir == NULL)
  {
    fprintf(stderr, "bac_dump: dir is NULL\n");
    return 1;
  }
  if (file == NULL)
  {
    fprintf(stderr, "bac_dump: file is NULL\n");
    return 1;
  }

  no_entry = 0;
  array = bac_load(dir, file, 1, BAC_LOAD_FLAG_NO_SORT, mode, &no_entry);
  if ((array == NULL) || (array->p == NULL))
  {
    if (no_entry != 0)
    {
      /* don't use stdout here */
      fprintf(stderr, "bac_dump: there is no entry\n");
      return 0;
    }
    else
    {
      fprintf(stderr, "bac_dump: bac_load failed\n");
      return 1;
    }
    /* should not reach here */
    return 1;
  }

  for (i = 0; i < array->n; i++)
  {
    string = bac_entry_to_string(array->p[i]);
    if (string == NULL)
    {
      fprintf(stderr, "bac_dump: bac_entry_to_string failed\n");
      bac_array_delete(array);
      return 1;
    }
    printf("%s", string);
    fflush(stdout);
    free(string);
    string = NULL;
  }

  bac_array_delete(array);
  return 0;
}

/* return 0 on success, 1 on error
 * only the superuser may undump
 * assumes dir (arg 1) already exists --- if not, it is an error
 */
int
bac_undump(const char *dir, const char *file, int mode)
{
  int i;
  bac_array *array = NULL;

  /* sanity check */
  if (dir == NULL)
  {
    fprintf(stderr, "bac_undump: dir is NULL\n");
    return 1;
  }
  if (file == NULL)
  {
    fprintf(stderr, "bac_undump: file is NULL\n");
    return 1;
  }

#ifndef NOT_HAVE_POSIX
  /* permission check
   * seteuid may be unnecessary, but in case (there is fakeroot)
   */
  if ((geteuid() != 0) || (seteuid(0) != 0))
  {
    fprintf(stderr, "bac_umdump: only the superuser may undump\n");
    return 1;
  }
#endif  /* not NOT_HAVE_POSIX */

  array = bac_file_to_array(stdin);
  if ((array == NULL) || (array->p == NULL))
  {
    fprintf(stderr, "bac_umdump: bac_parse failed\n");
    return 1;
  }

  for (i = 0; i < array->n; i++)
  {
    if (bac_save(dir, file, array->p[i], mode) != 0)
    {
      fprintf(stderr, "bac_umdump: bac_save failed\n");
      bac_array_delete(array);
      return 1;
    }
  }

  bac_array_delete(array);
  return 0;
}

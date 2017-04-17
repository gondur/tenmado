/* $Id: bac_truncate.c,v 1.36 2009/11/14 02:14:49 oohara Exp $ */

/* HAVE_LIBDB3, HAVE_LIBDB_3 */
#include <config.h>

/* remove */
#include <stdio.h>
/* qsort, malloc */
#include <stdlib.h>
/* memset, strncpy, strerror */
#include <string.h>
/* errno */
#include <errno.h>

#if (defined(HAVE_LIBDB3) || defined(HAVE_LIBDB_3))
/* libdb3 */
#include <db.h>
#endif /* (defined(HAVE_LIBDB3) || defined(HAVE_LIBDB_3)) */

#include "bac_array.h"
#include "bac_entry.h"
#include "bac_string_to_array.h"

#if !(defined(HAVE_LIBDB3) || defined(HAVE_LIBDB_3))
#include "bac_load.h"
#include "bac_save.h"
#endif /* !(defined(HAVE_LIBDB3) || defined(HAVE_LIBDB_3)) */

#include "bac_truncate.h"

#if (defined(HAVE_LIBDB3) || defined(HAVE_LIBDB_3))
static int bac_truncate_one_step(bac_entry *entry, const char *string,
                                 DBC *cursorp);
#endif /* (defined(HAVE_LIBDB3) || defined(HAVE_LIBDB_3)) */

/* delete all entries that are (strictly) below entry (arg 3)
 * in the database
 * return 0 on success, 1 on error
 * assumes there is at least one entry --- if not, it is an error
 */
int
bac_truncate(const char *dir, const char *file, bac_entry *entry, int mode)
{
#if (defined(HAVE_LIBDB3) || defined(HAVE_LIBDB_3))
  DB *dbp = NULL;
  DB_ENV *dbenvp = NULL;
  DBC *cursorp = NULL;
  DBT key;
  DBT data;
  int status;

  /* sanity check */
  if (dir == NULL)
  {
    fprintf(stderr, "bac_truncate: dir is NULL\n");
    return 1;
  }
  if (file == NULL)
  {
    fprintf(stderr, "bac_truncate: file is NULL\n");
    return 1;
  }
  if (entry == NULL)
  {
    fprintf(stderr, "bac_truncate: entry is NULL\n");
    return 1;
  }

  /* this must be before db_create */
  status = db_env_create(&dbenvp, 0);
  if (status != 0)
  {
    fprintf(stderr, "bac_truncate: db_env_create failed (%s)\n",
            db_strerror(status));
    return 1;
  }
  /* this must be before DB->open */
  /* this directory must be present on the system before
   * this program is called (DB_CREATE creates only files in that
   * directory, not the directory itself)
   */
  /* DB_INIT_CDB and DB_INIT_MPOOL means Concurrent Data Store */
  status = dbenvp->open(dbenvp, dir,
                        DB_INIT_CDB | DB_INIT_MPOOL | DB_CREATE, mode);
  if (status != 0)
  {
    fprintf(stderr, "bac_truncate: DBENV->open failed (%s)\n",
            db_strerror(status));
    dbenvp->close(dbenvp, 0);
    return 1;
  }
  status = db_create(&dbp, dbenvp, 0);
  if (status != 0)
  {
    fprintf(stderr, "bac_truncate: db_create failed (%s)\n",
            db_strerror(status));
    dbenvp->close(dbenvp, 0);
    return 1;
  }
  status = dbp->open(dbp, file, NULL, DB_RECNO,
                     DB_CREATE, mode);
  if (status != 0)
  {
    fprintf(stderr, "bac_truncate: DB->open failed (%s)\n",
            db_strerror(status));
    dbp->close(dbp, 0);
    dbenvp->close(dbenvp, 0);
    return 1;
  }
  status = dbp->cursor(dbp, NULL, &cursorp, DB_WRITECURSOR);
  if (status != 0)
  {
    fprintf(stderr, "bac_truncate: DB->cursor failed (%s)\n",
            db_strerror(status));
    dbp->close(dbp, 0);
    dbenvp->close(dbenvp, 0);
    return 1;
  }

  /* for compatibility with later version of libdb */
  memset(&key, 0, sizeof(key));
  memset(&data, 0, sizeof(data));
  status = cursorp->c_get(cursorp, &key, &data, DB_FIRST);
  if (status == DB_NOTFOUND)
  {
    /* there is no element in the database, so nothing to do */
    cursorp->c_close(cursorp);
    dbp->close(dbp, 0);
    dbenvp->close(dbenvp, 0);
    return 0;
  }
  else if (status != 0)
  {
    fprintf(stderr, "bac_truncate: DBcursor->c_get(DB_FIRST) failed (%s)\n",
            db_strerror(status));
    cursorp->c_close(cursorp);
    dbp->close(dbp, 0);
    dbenvp->close(dbenvp, 0);
    return 1;
  }
  if (((char *) (data.data))[(data.size / sizeof(char)) - 1] != '\0' )
  {
    fprintf(stderr, "bac_truncate: data.data is not null-terminated\n");
    cursorp->c_close(cursorp);
    dbp->close(dbp, 0);
    dbenvp->close(dbenvp, 0);
    return 1;
  }

  if (bac_truncate_one_step(entry, (char *) (data.data), cursorp) != 0)
  {
    fprintf(stderr, "bac_truncate: bac_truncate_one_step failed\n");
    cursorp->c_close(cursorp);
    dbp->close(dbp, 0);
    dbenvp->close(dbenvp, 0);
    return 1;
  }
  memset(&key, 0, sizeof(key));
  memset(&data, 0, sizeof(data));

  while(1 == 1)
  {
    status = cursorp->c_get(cursorp, &key, &data, DB_NEXT);
    if (status == DB_NOTFOUND)
    {
      /* no more entry in the database */
      break;
    }
    else if (status != 0)
    {
      fprintf(stderr, "bac_truncate: DBcursor->c_get(DB_NEXT) failed (%s)\n",
              db_strerror(status));
      cursorp->c_close(cursorp);
      dbp->close(dbp, 0);
      dbenvp->close(dbenvp, 0);
      return 1;
    }
    else
    {
      if (((char *) (data.data))[(data.size / sizeof(char)) - 1] != '\0' )
      {
        fprintf(stderr, "bac_truncate: data.data is not null-terminated\n");
        cursorp->c_close(cursorp);
        dbp->close(dbp, 0);
        dbenvp->close(dbenvp, 0);
        return 1;
      }
      if (bac_truncate_one_step(entry, (char *) (data.data), cursorp) != 0)
      {
        fprintf(stderr, "bac_truncate: bac_truncate_one_step failed\n");
        cursorp->c_close(cursorp);
        dbp->close(dbp, 0);
        dbenvp->close(dbenvp, 0);
        return 1;
      }
      memset(&key, 0, sizeof(key));
      memset(&data, 0, sizeof(data));
    }
  }

  cursorp->c_close(cursorp);
  dbp->close(dbp, 0);
  dbenvp->close(dbenvp, 0);
#else /* not (defined(HAVE_LIBDB3) || defined(HAVE_LIBDB_3)) */
  int i;
  int no_entry;
  bac_array *p = NULL;
  size_t length;
  char *file_abs = NULL;

  /* sanity check */
  if (dir == NULL)
  {
    fprintf(stderr, "bac_truncate: dir is NULL\n");
    return 1;
  }
  if (file == NULL)
  {
    fprintf(stderr, "bac_truncate: file is NULL\n");
    return 1;
  }
  if (entry == NULL)
  {
    fprintf(stderr, "bac_truncate: entry is NULL\n");
    return 1;
  }

  /* +1 is for the trailing \0 */
  length = strlen(dir) + strlen(file) + 1;
  file_abs = (char *) malloc(sizeof(char) * length);
  if (file_abs == NULL)
  {
    fprintf(stderr, "bac_truncate: malloc(file_abs) failed\n");
    return 1;
  }
  strncpy(file_abs, dir, length);
  file_abs[length - 1] = '\0';
  strncpy(file_abs + strlen(file_abs), file, length - strlen(file_abs));
  file_abs[length - 1] = '\0';

  p = bac_load(dir, file, 1, 0, mode, &no_entry);
  if (p == NULL)
  {
    free(file_abs);
    return 0;
  }

  errno = 0;
  if (remove(file_abs) != 0)
  {
    fprintf(stderr, "bac_truncate: remove failed");
    if (errno != 0)
      fprintf(stderr, " (%s)", strerror(errno));
    fprintf(stderr, "\n");
    bac_array_delete(p);
    free(file_abs);
    return 1;
  }

  for (i = 0; (i < p->n) && (bac_compare_entry((void *) (&(p->p[i])),
                                               (void *) (&entry)) <= 0); i++)
  {
    if (bac_save(dir, file, p->p[i], mode) != 0)
    {
      fprintf(stderr, "bac_truncate: bac_save(%d) failed\n", i);
      bac_array_delete(p);
      free(file_abs);
      return 1;
    }
  }
  bac_array_delete(p);
  free(file_abs);

#endif /* not (defined(HAVE_LIBDB3) || defined(HAVE_LIBDB_3)) */

  return 0;
}

#if (defined(HAVE_LIBDB3) || defined(HAVE_LIBDB_3))
/* return 0 on success, 1 on error */
static int
bac_truncate_one_step(bac_entry *entry, const char *string, DBC *cursorp)
{
  bac_array *temp;
  int compare_result;

  temp = bac_string_to_array(string);
  if (temp == NULL)
  {
    fprintf(stderr, "bac_truncate_one_step: bac_string_to_array failed\n");
    return 1;
  }
  qsort((void *) temp->p, (size_t) (temp->n), sizeof(bac_entry *),
        (int (*)(const void *, const void *)) bac_compare_entry);

  compare_result = bac_compare_entry((void *) (&(temp->p[0])),
                                     (void *) (&entry));
  if (compare_result > 0)
  {
    if (cursorp->c_del(cursorp, 0) != 0)
    {
      fprintf(stderr, "bac_truncate: DBcursor->c_del failed\n");
      if (temp != NULL)
        bac_array_delete(temp);
      return 1;
    }
  }
  if (temp != NULL)
    bac_array_delete(temp);

  return 0;
}
#endif /* (defined(HAVE_LIBDB3) || defined(HAVE_LIBDB_3)) */

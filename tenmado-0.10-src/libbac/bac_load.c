/* $Id: bac_load.c,v 1.89 2003/01/15 23:34:44 oohara Exp $ */

/* HAVE_LIBDB3, HAVE_LIBDB_3 */
#include <config.h>

#include <stdio.h>
/* qsort, malloc */
#include <stdlib.h>
/* memset, strerror */
#include <string.h>
/* errno
 * ENOENT (from POSIX.1)
 */
#include <errno.h>
/* stat (from POSIX.1) */
#include <sys/stat.h>

#if (defined(HAVE_LIBDB3) || defined(HAVE_LIBDB_3))
/* libdb3 */
#include <db.h>
#endif /* (defined(HAVE_LIBDB3) || defined(HAVE_LIBDB_3)) */

#include "bac_array.h"
#include "bac_entry.h"
#include "bac_string_to_array.h"

#include "bac_load.h"

#if (defined(HAVE_LIBDB3) || defined(HAVE_LIBDB_3))
static bac_array *bac_load_add_new_entry(bac_array *array, const char *string,
                                         int n, int flag, DBC *cursorp);
#endif /* (defined(HAVE_LIBDB3) || defined(HAVE_LIBDB_3)) */

/* return a new bac_array which consists of loaded entries
 * no_entry (arg 6) is set to 1 if bac_load() returns NULL because
 * there is no entry, 0 if not
 * if an error happens, load the entries that are loaded
 * before the error
 * assumes all data in the database are null-terminated
 * note that the returned array may have more than n entries in it
 */
bac_array *
bac_load(const char *dir, const char *file, int n, int flag, int mode,
         int *no_entry)
{
  bac_array *p = NULL;
#if (defined(HAVE_LIBDB3) || defined(HAVE_LIBDB_3))
  DB *dbp = NULL;
  DB_ENV *dbenvp = NULL;
  DBC *cursorp = NULL;
  DBT key;
  DBT data;
  u_int32_t dbflag = 0;
  int status;
  bac_array *p_temp = NULL;

  *no_entry = 0;

  /* sanity check */
  if (dir == NULL)
  {
    fprintf(stderr, "bac_load: dir is NULL\n");
    return NULL;
  }
  if (file == NULL)
  {
    fprintf(stderr, "bac_load: file is NULL\n");
    return NULL;
  }  
  if (n <= 0)
  {
    fprintf(stderr, "bac_load: n is non-positive\n");
    return NULL;
  }
  
  /* this must be before db_create */
  status = db_env_create(&dbenvp, 0);
  if (status != 0)
  {
    fprintf(stderr, "bac_load: db_env_create failed (%s)\n",
            db_strerror(status));
    return NULL;
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
    fprintf(stderr, "bac_load: DBENV->open(%s) failed (%s)\n", dir,
            db_strerror(status));
    dbenvp->close(dbenvp, 0);
    return NULL;
  }
  status = db_create(&dbp, dbenvp, 0);
  if (status != 0)
  {
    fprintf(stderr, "bac_load: db_create failed (%s)\n", db_strerror(status));
    dbenvp->close(dbenvp, 0);
    return NULL;
  }
  status = dbp->open(dbp, file, NULL, DB_RECNO,
                     DB_CREATE, mode);
  if (status != 0)
  {
    fprintf(stderr, "bac_load: DB->open(%s) failed (%s)\n", file,
            db_strerror(status));
    dbp->close(dbp, 0);
    dbenvp->close(dbenvp, 0);
    return NULL;
  }

  if (flag & BAC_LOAD_FLAG_NO_DELETE)
    dbflag = 0;
  else
    dbflag = DB_WRITECURSOR;
  status = dbp->cursor(dbp, NULL, &cursorp, dbflag);
  if (status != 0)
  {
    fprintf(stderr, "bac_load: DB->cursor failed (%s)\n", db_strerror(status));
    dbp->close(dbp, 0);
    dbenvp->close(dbenvp, 0);
    return NULL;
  }
  /* for compatibility with later version of libdb */
  memset(&key, 0, sizeof(key));
  memset(&data, 0, sizeof(data));
  status = cursorp->c_get(cursorp, &key, &data, DB_FIRST);
  if (status == DB_NOTFOUND)
  {
    /* there is no element in the database */
    /* be quiet, there may be --dump option
     * the calling function should produce an appropriate error message
     */
    *no_entry = 1;
    cursorp->c_close(cursorp);
    dbp->close(dbp, 0);
    dbenvp->close(dbenvp, 0);
    /* there is no special return value here --- after all,
     * you have no array
     */
    return NULL;
  }
  else if (status != 0)
  {
    fprintf(stderr, "bac_load: DBcursor->c_get(DB_FIRST) failed (%s)\n",
            db_strerror(status));
    cursorp->c_close(cursorp);
    dbp->close(dbp, 0);
    dbenvp->close(dbenvp, 0);
    return NULL;
  }
  if (((char *) (data.data))[(data.size / sizeof(char)) - 1] != '\0' )
  {
    fprintf(stderr, "bac_load: data.data is not null-terminated\n");
    cursorp->c_close(cursorp);
    dbp->close(dbp, 0);
    dbenvp->close(dbenvp, 0);
    return NULL;
  }

  p_temp = bac_load_add_new_entry(p, (char *) (data.data), n, flag, cursorp);
  if (p_temp == NULL)
  {
    fprintf(stderr, "bac_load: bac_load_add_new_entry failed\n");
    cursorp->c_close(cursorp);
    dbp->close(dbp, 0);
    dbenvp->close(dbenvp, 0);
    return NULL;
  }
  p = p_temp;

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
      fprintf(stderr, "bac_load: DBcursor->c_get(DB_NEXT) failed (%s)\n",
              db_strerror(status));
      cursorp->c_close(cursorp);
      dbp->close(dbp, 0);
      dbenvp->close(dbenvp, 0);
      return p;
    }
    else
    {
      if (((char *) (data.data))[(data.size / sizeof(char)) - 1] != '\0' )
      {
        fprintf(stderr, "bac_load: data.data is not null-terminated\n");
        cursorp->c_close(cursorp);
        dbp->close(dbp, 0);
        dbenvp->close(dbenvp, 0);
        return p;
      }

      p_temp = bac_load_add_new_entry(p, (char *) (data.data), n, flag,
                                      cursorp);
      if (p_temp == NULL)
      {
        fprintf(stderr, "bac_load: bac_load_add_new_entry failed\n");
        cursorp->c_close(cursorp);
        dbp->close(dbp, 0);
        dbenvp->close(dbenvp, 0);
        return p;
      }
      p = p_temp;

      memset(&key, 0, sizeof(key));
      memset(&data, 0, sizeof(data));
    }
  }

  cursorp->c_close(cursorp);
  dbp->close(dbp, 0);
  dbenvp->close(dbenvp, 0);

#else /* not (defined(HAVE_LIBDB3) || defined(HAVE_LIBDB_3)) */
  FILE *fp = NULL;
  size_t length;
  char *file_abs = NULL;
  struct stat buf;

  *no_entry = 0;

  /* sanity check */
  if (dir == NULL)
  {
    fprintf(stderr, "bac_load: dir is NULL\n");
    return NULL;
  }
  if (file == NULL)
  {
    fprintf(stderr, "bac_load: file is NULL\n");
    return NULL;
  }  
  if (n <= 0)
  {
    fprintf(stderr, "bac_load: n is non-positive\n");
    return NULL;
  }

  /* +1 is for the trailing \0 */
  length = strlen(dir) + strlen(file) + 1;
  file_abs = (char *) malloc(sizeof(char) * length);
  if (file_abs == NULL)
  {
    fprintf(stderr, "bac_load: malloc(file_abs) failed\n");
    return NULL;
  }
  strncpy(file_abs, dir, length);
  file_abs[length - 1] = '\0';
  strncpy(file_abs + strlen(file_abs), file, length - strlen(file_abs));
  file_abs[length - 1] = '\0';

  /* see if the file exists */
  errno = 0;
  if (stat(file_abs, &buf) != 0)
  {
    if (errno == ENOENT)
    {
      /* there is no element in the database --- in fact, no database */
      /* be quiet, there may be --dump option
       * the calling function should produce an appropriate error message
       */
      *no_entry = 1;
      free(file_abs);
      /* there is no special return value here --- after all,
       * you have no array
       */
      return NULL;
    }
    else
    {
      /* real error */
      fprintf(stderr, "bac_load: stat failed");
      if (errno != 0)
        fprintf(stderr, " (%s)", strerror(errno));
      fprintf(stderr, "\n");
      free(file_abs);
      return NULL;
    }
  }

  errno = 0;
  fp = fopen(file_abs, "r");
  if (fp == NULL)
  {
    fprintf(stderr, "bac_load: fopen failed");
    if (errno != 0)
      fprintf(stderr, " (%s)", strerror(errno));
    fprintf(stderr, "\n");
    free(file_abs);
    return NULL;
  }
  p = bac_file_to_array(fp);
  fclose(fp);
  free(file_abs);

  if ((p != NULL)
      &&(!((flag & BAC_LOAD_FLAG_NO_SORT) == BAC_LOAD_FLAG_NO_SORT)))
    qsort((void *) p->p, (size_t) (p->n), sizeof(bac_entry *),
          (int (*)(const void *, const void *)) bac_compare_entry);

#endif /* not (defined(HAVE_LIBDB3) || defined(HAVE_LIBDB_3)) */

  return p;
}

#if (defined(HAVE_LIBDB3) || defined(HAVE_LIBDB_3))
/* assumes array (arg 1) is sorted as long as it is not NULL
 */
static bac_array *
bac_load_add_new_entry(bac_array *array, const char *string, int n, int flag,
                       DBC *cursorp)
{
  bac_array *array_new = NULL;
  bac_array *array_temp = NULL;
  int in_nth;
  int status;

  array_new = bac_string_to_array(string);
  if (array_new == NULL)
  {
    fprintf(stderr, "bac_load_add_new_entry: bac_string_to_array failed\n");
    return NULL;
  }
  /* simple !(flag & BAC_LOAD_FLAG_NO_SORT) is wrong */
  if (!((flag & BAC_LOAD_FLAG_NO_SORT) == BAC_LOAD_FLAG_NO_SORT))
    qsort((void *) array_new->p, (size_t) (array_new->n), sizeof(bac_entry *),
          (int (*)(const void *, const void *)) bac_compare_entry);

  if ((array != NULL) && (array_new != NULL) && (array->n >= n)
      && (bac_compare_entry((void *) (&(array->p[array->n - 1])),
                            (void *) (&(array_new->p[0]))) <= 0))
  {
    /* no entry in array_new is above nth */
    /* false */
    in_nth = 0;
  }
  else
  {
    /* true */
    in_nth = 1;
  }

  if ((!(flag & BAC_LOAD_FLAG_NO_DELETE)) && (!in_nth))
  {
    status = cursorp->c_del(cursorp, 0);
    if (status != 0)
    {
      fprintf(stderr, "bac_load_add_new_entry: "
              "DBcursor->c_del failed (%s)\n", db_strerror(status));
      /* we may quit now because we return NULL regardless of flag */
      if (array_new != NULL)
        bac_array_delete(array_new);
      return NULL;
    }
  }

  if ((flag & BAC_LOAD_FLAG_LOAD_ALL) || (in_nth))
  {
    array_temp = bac_array_add_array(array, array_new);
    if (array_temp == NULL)
    {
      fprintf(stderr, "bac_load: bac_array_add_array failed\n");
      if (array_new != NULL)
        bac_array_delete(array_new);
      return NULL;
    }
    array = array_temp;
    if (!((flag & BAC_LOAD_FLAG_NO_SORT) == BAC_LOAD_FLAG_NO_SORT))
      qsort((void *) array->p, (size_t) (array->n), sizeof(bac_entry *),
            (int (*)(const void *, const void *)) bac_compare_entry);
  }

  return array;
}
#endif /* (defined(HAVE_LIBDB3) || defined(HAVE_LIBDB_3)) */

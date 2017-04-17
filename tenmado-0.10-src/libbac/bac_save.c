/* $Id: bac_save.c,v 1.25 2003/01/14 04:36:57 oohara Exp $ */

/* HAVE_LIBDB3, HAVE_LIBDB_3 */
#include <config.h>

/* FILE */
#include <stdio.h>
/* memset, strlen, strerror */
#include <string.h>
/* malloc, free */
#include <stdlib.h>
/* errno */
#include <errno.h>

#if (defined(HAVE_LIBDB3) || defined(HAVE_LIBDB_3))
/* libdb3 */
#include <db.h>
#endif /* (defined(HAVE_LIBDB3) || defined(HAVE_LIBDB_3)) */

#include "bac_entry.h"
#include "bac_entry_to_string.h"

#include "bac_save.h"

/* return 0 on success, 1 on error
 * assumes dir (arg 1) already exists --- if not, it is an error
*/
int
bac_save(const char *dir, const char *file, bac_entry *p, int mode)
{
#if (defined(HAVE_LIBDB3) || defined(HAVE_LIBDB_3))
  DB *dbp = NULL;
  DB_ENV *dbenvp = NULL;
  DBT key;
  DBT data;
  int status;

  /* sanity check */
  if (dir == NULL)
  {
    fprintf(stderr, "bac_save: dir is NULL\n");
    return 1;
  }
  if (file == NULL)
  {
    fprintf(stderr, "bac_save: file is NULL\n");
    return 1;
  }
  if (p == NULL)
  {
    fprintf(stderr, "bac_save: p is NULL\n");
    return 0;
  }
  if (p->klass != 0)
  {
    fprintf(stderr, "bac_save: not supported score_entry_version (%d)\n",
            p->klass);
    return 0;
  }

  /* this must be before db_create */
  status = db_env_create(&dbenvp, 0);
  if (status != 0)
  {
    fprintf(stderr, "bac_save: db_env_create failed (%s)\n",
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
    fprintf(stderr, "bac_save: DBENV->open(%s) failed (%s)\n", dir,
            db_strerror(status));
    dbenvp->close(dbenvp, 0);
    return 1;
  }
  status = db_create(&dbp, dbenvp, 0);
  if (status != 0)
  {
    fprintf(stderr, "bac_save: db_create failed (%s)\n", db_strerror(status));
    dbenvp->close(dbenvp, 0);
    return 1;
  }
  status = dbp->open(dbp, file, NULL, DB_RECNO,
                     DB_CREATE, mode);
  
  if (status != 0)
  {
    fprintf(stderr, "bac_save: DB->open(%s) failed (%s)\n", file,
            db_strerror(status));
    dbp->close(dbp, 0);
    dbenvp->close(dbenvp, 0);
    return 1;
  }

  /* for compatibility with later version of libdb */
  memset(&key, 0, sizeof(key));
  memset(&data, 0, sizeof(data));
  /* we don't set key because this is a Recno database */
  data.data = bac_entry_to_string(p);
  if (data.data == NULL)
  {
    fprintf(stderr, "bac_save: bac_entry_to_string failed\n");
    dbp->close(dbp, 0);
    dbenvp->close(dbenvp, 0);
    return 1;
  }  
  /* I want to save the trailing \0 too */
  data.size = sizeof(char) * (strlen(data.data) + 1);
  status = dbp->put(dbp, NULL, &key, &data, DB_APPEND);
  if (status != 0)
  {
    fprintf(stderr, "bac_save: DB->put failed (%s)\n", db_strerror(status));
    free(data.data);
    dbp->close(dbp, 0);
    dbenvp->close(dbenvp, 0);
    return 1;
  }

  free(data.data);
  status = dbp->close(dbp, 0);
  if (status != 0)
  {
    dbenvp->close(dbenvp, 0);
    fprintf(stderr, "bac_save: DB->close failed (%s)\n", db_strerror(status));
    return 1;
  }
  status = dbenvp->close(dbenvp, 0);
  if (status != 0)
  {
    fprintf(stderr, "bac_save: DBENV->close failed (%s)\n",
            db_strerror(status));
    return 1;
  }

#else /* not (defined(HAVE_LIBDB3) || defined(HAVE_LIBDB_3)) */
  FILE *fp = NULL;
  size_t length;
  char *file_abs = NULL;
  char *data = NULL;

  /* sanity check */
  if (dir == NULL)
  {
    fprintf(stderr, "bac_save: dir is NULL\n");
    return 1;
  }
  if (file == NULL)
  {
    fprintf(stderr, "bac_save: file is NULL\n");
    return 1;
  }
  if (p == NULL)
  {
    fprintf(stderr, "bac_save: p is NULL\n");
    return 0;
  }
  if (p->klass != 0)
  {
    fprintf(stderr, "bac_save: not supported score_entry_version (%d)\n",
            p->klass);
    return 0;
  }

  /* +1 is for the trailing \0 */
  length = strlen(dir) + strlen(file) + 1;
  file_abs = (char *) malloc(sizeof(char) * length);
  if (file_abs == NULL)
  {
    fprintf(stderr, "bac_save: malloc(file_abs) failed\n");
    return 1;
  }
  strncpy(file_abs, dir, length);
  file_abs[length - 1] = '\0';
  strncpy(file_abs + strlen(file_abs), file, length - strlen(file_abs));
  file_abs[length - 1] = '\0';

  data = bac_entry_to_string(p);
  if (data == NULL)
  {
    fprintf(stderr, "bac_save: bac_entry_to_string failed\n");
    free(file_abs);
    return 1;
  }

  errno = 0;
  fp = fopen(file_abs, "a");
  if (fp == NULL)
  {
    fprintf(stderr, "bac_save: fopen failed");
    if (errno != 0)
      fprintf(stderr, " (%s)", strerror(errno));
    fprintf(stderr, "\n");
    free(data);
    free(file_abs);
    return 1;
  }

  fputs(data, fp);
  fclose(fp);
  free(data);
  free(file_abs);
#endif /* not (defined(HAVE_LIBDB3) || defined(HAVE_LIBDB_3)) */

  return 0;
}

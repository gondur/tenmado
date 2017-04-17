/* $Id: bac_entry.h,v 1.15 2003/04/22 02:19:49 oohara Exp $ */

#ifndef __BAC_ENTRY_H__
#define __BAC_ENTRY_H__

/* NOT_HAVE_POSIX */
#include <config.h>

/* time_t */
#include <time.h>

#ifdef NOT_HAVE_POSIX
#define uid_t int
#else /* not NOT_HAVE_POSIX */
/* uid_t */
#include <sys/types.h>
#endif /* not NOT_HAVE_POSIX */

struct _bac_entry
{
  int klass;
  void (*delete)(void *);
};
typedef struct _bac_entry bac_entry;

/* "parent" must be first to to use inheritance in C (not C++) */
struct _bac_entry_ver_0
{
  bac_entry parent;

  int stage_data_version;
  time_t when;
  uid_t uid;
  int user_name_available;
  char *user_name;
  int score_sort;
  int score_total;
  int number_stage;
  int *score_stage;
};
typedef struct _bac_entry_ver_0 bac_entry_ver_0;

bac_entry *bac_entry_new(int score_entry_version, int stage_data_version,
                         time_t when, uid_t uid, int user_name_available,
                         const char *user_name, int score_sort,
                         int score_total, int number_stage,
                         const int *score_stage);
int bac_compare_entry(const void *a, const void *b);

#endif /* not __BAC_ENTRY_H__ */

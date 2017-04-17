/* $Id: bac_load.h,v 1.17 2003/01/11 21:41:56 oohara Exp $ */

#ifndef __BAC_LOAD_H__
#define __BAC_LOAD_H__

#include "bac_array.h"

/* for flag in bac_load */
/* don't remove an entry from the database even if it is below nth
 * (it does not get into the returned array unless BAC_LOAD_FLAG_LOAD_ALL is
 * also specified)
 * ignored if Berkeley DB is unavailable (no entry is removed from
 * the database anyway)
 */
#define BAC_LOAD_FLAG_NO_DELETE 1
/* load all entries in the database
 * they may still be removed from the database after read
 * --- use BAC_LOAD_FLAG_NO_DELETE too if you don't like this
 * ignored if Berkeley DB is unavailable (all entries are loaded anyway)
 */
#define BAC_LOAD_FLAG_LOAD_ALL 2
/* don't sort entries
 * implies BAC_LOAD_FLAG_NO_DELETE and BAC_LOAD_FLAG_LOAD_ALL
 */
#define BAC_LOAD_FLAG_NO_SORT (4 | BAC_LOAD_FLAG_NO_DELETE | BAC_LOAD_FLAG_LOAD_ALL)

bac_array *bac_load(const char *dir, const char *file, int n, int flag,
                    int mode, int *no_entry);

#endif /* not __BAC_LOAD_H__ */

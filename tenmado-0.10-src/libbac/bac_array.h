/* $Id: bac_array.h,v 1.7 2002/12/07 03:21:34 oohara Exp $ */

#ifndef __BAC_ARRAY_H__
#define __BAC_ARRAY_H__

#include "bac_entry.h"

struct _bac_array
{
  /* number of entries */
  int n;
  bac_entry **p;
};
typedef struct _bac_array bac_array;

void bac_array_delete(bac_array *p);
bac_array *bac_array_add_entry(bac_array *p, bac_entry *entry);
bac_array *bac_array_add_array(bac_array *p, bac_array *array);

#endif /* not __BAC_ARRAY_H__ */

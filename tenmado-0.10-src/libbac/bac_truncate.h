/* $Id: bac_truncate.h,v 1.3 2002/12/18 15:55:24 oohara Exp $ */

#ifndef __BAC_TRUNCATE_H__
#define __BAC_TRUNCATE_H__

#include "bac_entry.h"

int bac_truncate(const char *dir, const char *file, bac_entry *entry,
                 int mode);

#endif /* not __BAC_TRUNCATE_H__ */

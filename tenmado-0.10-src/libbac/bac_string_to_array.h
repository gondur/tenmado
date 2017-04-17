/* $Id: bac_string_to_array.h,v 1.4 2002/12/09 17:43:45 oohara Exp $ */

#ifndef __BAC_STRING_TO_ARRAY_H__
#define __BAC_STRING_TO_ARRAY_H__

#include "bac_array.h"

bac_array *bac_string_to_array(const char *string);
bac_array *bac_file_to_array(FILE *file);

#endif /* not __BAC_STRING_TO_ARRAY_H__ */

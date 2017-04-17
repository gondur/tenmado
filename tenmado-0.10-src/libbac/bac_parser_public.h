/* $Id: bac_parser_public.h,v 1.5 2002/12/08 11:32:36 oohara Exp $ */

#ifndef __BAC_PARSER_PUBLIC_H__
#define __BAC_PARSER_PUBLIC_H__

#include "bac_array.h"

/* yyparse */
int bac_parse(void);

bac_array *bac_parser_get_array(void);
void bac_parser_delete_array(void);

#endif /* not __BAC_PARSER_PUBLIC_H__ */

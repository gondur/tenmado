/* $Id: bac_lexical.h,v 1.6 2012/01/29 03:59:31 oohara Exp $ */

#ifndef __BAC_LEXICAL_H__
#define __BAC_LEXICAL_H__

/* FILE */
#include <stdio.h>

/* yylineno */
extern int bac_lineno;

/* yylex */
int bac_lex(void);
/* yylex_destroy */
int bac_lex_destroy(void);

void bac_read_from_file(FILE *file);
void bac_read_from_string(const char *string);
void bac_close_string(void);

#endif /* not __BAC_LEXICAL_H__ */

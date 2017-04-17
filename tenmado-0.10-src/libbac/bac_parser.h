/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     NEWLINE = 258,
     TAB = 259,
     LONG_INT = 260,
     STRING = 261,
     BEGIN_ENTRY_VERSION = 262,
     BEGIN_COMMENT = 263,
     BEGIN_STAGE_DATA_VERSION = 264,
     BEGIN_WHEN = 265,
     BEGIN_TOTAL_SCORE = 266,
     BEGIN_UID = 267,
     BEGIN_USER_NAME = 268,
     BEGIN_SCORE_SORT = 269,
     BEGIN_SCORE_TOTAL = 270,
     BEGIN_NUMBER_STAGE = 271,
     BEGIN_SCORE_STAGE = 272,
     END_OF_ENTRY = 273,
     LEXICAL_ERROR = 274
   };
#endif
/* Tokens.  */
#define NEWLINE 258
#define TAB 259
#define LONG_INT 260
#define STRING 261
#define BEGIN_ENTRY_VERSION 262
#define BEGIN_COMMENT 263
#define BEGIN_STAGE_DATA_VERSION 264
#define BEGIN_WHEN 265
#define BEGIN_TOTAL_SCORE 266
#define BEGIN_UID 267
#define BEGIN_USER_NAME 268
#define BEGIN_SCORE_SORT 269
#define BEGIN_SCORE_TOTAL 270
#define BEGIN_NUMBER_STAGE 271
#define BEGIN_SCORE_STAGE 272
#define END_OF_ENTRY 273
#define LEXICAL_ERROR 274




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 56 "bac_parser.y"
{
  int val_int;
  long int val_long_int;
  char *val_string;
}
/* Line 1489 of yacc.c.  */
#line 93 "bac_parser.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE bac_lval;


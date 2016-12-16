/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

#ifndef YY_YY_OX_BISON_H_INCLUDED
# define YY_YY_OX_BISON_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 1 "ox_bison.y" /* yacc.c:1909  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../../include/hiena/scanner_module.h"	/* HIENA HEADER */

typedef void* yyscan_t;			/* FLEX SETUP */

#line 53 "ox_bison.h" /* yacc.c:1909  */

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    INDENT = 258,
    LINE = 259,
    INDENTED_LINE = 260,
    BLANKLINE = 261,
    NEWLINE = 262,
    END = 263,
    INDENTED_LINE_RECURSIVE_AREA = 264,
    OUTLINE = 265,
    OUTLINE_BLOCK = 266,
    DIR_OF_OUTLINES = 267,
    DIR_OF_OUTLINE_BLOCKS = 268
  };
#endif
/* Tokens.  */
#define INDENT 258
#define LINE 259
#define INDENTED_LINE 260
#define BLANKLINE 261
#define NEWLINE 262
#define END 263
#define INDENTED_LINE_RECURSIVE_AREA 264
#define OUTLINE 265
#define OUTLINE_BLOCK 266
#define DIR_OF_OUTLINES 267
#define DIR_OF_OUTLINE_BLOCKS 268

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 17 "ox_bison.y" /* yacc.c:1909  */

    int    len;
    char * str;
    void * ob;

#line 97 "ox_bison.h" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif



int yyparse (yyscan_t *scanner, Hsp *hsp);

#endif /* !YY_YY_OX_BISON_H_INCLUDED  */

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

#ifndef YY_YY_FUDGE_BISON_H_INCLUDED
# define YY_YY_FUDGE_BISON_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 1 "fudge_bison.y" /* yacc.c:1909  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../../include/hiena/lookup_module.h"		/* HIENA HEADER */

typedef void* yyscan_t;			/* FLEX SETUP */

/* callback args */
#define nova(NOVARG) h->op->add_va(h, NOVARG)
#define sql(SQLSTR)  h->op->sql(h, SQLSTR, strlen(SQLSTR))

#line 57 "fudge_bison.h" /* yacc.c:1909  */

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    END = 258,
    NAME_SEG = 259,
    AT = 260,
    FLEXIVAL = 261,
    DOT = 262,
    LBRACE = 263,
    RBRACE = 264,
    SLASH = 265,
    ASSIGN = 266,
    EQ = 267,
    NOTEQ = 268,
    LT = 269,
    GT = 270,
    LTE = 271,
    GTE = 272,
    ERE = 273
  };
#endif
/* Tokens.  */
#define END 258
#define NAME_SEG 259
#define AT 260
#define FLEXIVAL 261
#define DOT 262
#define LBRACE 263
#define RBRACE 264
#define SLASH 265
#define ASSIGN 266
#define EQ 267
#define NOTEQ 268
#define LT 269
#define GT 270
#define LTE 271
#define GTE 272
#define ERE 273

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 23 "fudge_bison.y" /* yacc.c:1909  */

    int    c;
    char * str;

#line 110 "fudge_bison.h" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (yyscan_t *scanner, Hframe *h);

#endif /* !YY_YY_FUDGE_BISON_H_INCLUDED  */

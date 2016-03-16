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

#ifndef YY_RQL_RQL_TAB_H_INCLUDED
# define YY_RQL_RQL_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef RQLDEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define RQLDEBUG 1
#  else
#   define RQLDEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define RQLDEBUG 0
# endif /* ! defined YYDEBUG */
#endif  /* ! defined RQLDEBUG */
#if RQLDEBUG
extern int rqldebug;
#endif
/* "%code requires" blocks.  */
#line 1 "rql.y" /* yacc.c:1909  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../hiena.h"

#define rescol(VAR) hsp_sql_set_rescol(h, (void *)VAR)
#define push(HTOK, VAR) rql_push_expr(h, HTOK, (void *)VAR)
#define from(TARG) hsp_sql_set_selection(h, TARG)
#define do_select() hsp_sql_select_core(h)

typedef void *yyscan_t;		/* FLEX SETUP */

#line 67 "rql.tab.h" /* yacc.c:1909  */

/* Token type.  */
#ifndef RQLTOKENTYPE
# define RQLTOKENTYPE
  enum rqltokentype
  {
    END = 258,
    EOL = 259,
    SELECT = 260,
    ALL = 261,
    FROM = 262,
    WHERE = 263,
    IS = 264,
    NOT = 265,
    LIKE = 266,
    SEMICOLON = 267,
    COMMA = 268,
    STAR = 269,
    STRING_LITERAL = 270,
    IDENTIFIER = 271,
    EXPR_BINARY_IS = 272,
    EXPR_COLUMN = 273,
    TAB_SRC_CHILD = 274,
    TAB_SRC_PROP = 275,
    TAB_AXPA_CHILD = 276,
    TAB_AXPA_PROP = 277,
    TAB_SEL_CHILD = 278,
    TAB_SEL_PROP = 279
  };
#endif

/* Value type.  */
#if ! defined RQLSTYPE && ! defined RQLSTYPE_IS_DECLARED
typedef union RQLSTYPE RQLSTYPE;
union RQLSTYPE
{
#line 21 "rql.y" /* yacc.c:1909  */

    int   num;
    char *str;

#line 109 "rql.tab.h" /* yacc.c:1909  */
};
# define RQLSTYPE_IS_TRIVIAL 1
# define RQLSTYPE_IS_DECLARED 1
#endif



int rqlparse (yyscan_t *scanner, Hsp *h);

#endif /* !YY_RQL_RQL_TAB_H_INCLUDED  */

/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_GRAMMAR_GRAMMAR_TAB_H_INCLUDED
# define YY_GRAMMAR_GRAMMAR_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef GRAMMARDEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define GRAMMARDEBUG 1
#  else
#   define GRAMMARDEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define GRAMMARDEBUG 0
# endif /* ! defined YYDEBUG */
#endif  /* ! defined GRAMMARDEBUG */
#if GRAMMARDEBUG
extern int grammardebug;
#endif

/* Token kinds.  */
#ifndef GRAMMARTOKENTYPE
# define GRAMMARTOKENTYPE
  enum grammartokentype
  {
    GRAMMAREMPTY = -2,
    GRAMMAREOF = 0,                /* "end of file"  */
    GRAMMARerror = 256,            /* error  */
    GRAMMARUNDEF = 257,            /* "invalid token"  */
    DOTDOTDOT = 258,               /* DOTDOTDOT  */
    BADTOKEN = 259,                /* BADTOKEN  */
    STRUCT = 260,                  /* STRUCT  */
    UNION = 261,                   /* UNION  */
    ENUM = 262,                    /* ENUM  */
    DECLARATION_RESULT = 263,      /* DECLARATION_RESULT  */
    PARAM_RESULT = 264,            /* PARAM_RESULT  */
    SCOPERES = 265,                /* SCOPERES  */
    NUMBER = 266,                  /* NUMBER  */
    IDENTIFIER = 267,              /* IDENTIFIER  */
    STORAGE_CLASS_SPECIFIER = 268, /* STORAGE_CLASS_SPECIFIER  */
    TYPE_QUALIFIER = 269,          /* TYPE_QUALIFIER  */
    FUNCTION_SPECIFIER = 270,      /* FUNCTION_SPECIFIER  */
    TYPE_NAME = 271                /* TYPE_NAME  */
  };
  typedef enum grammartokentype grammartoken_kind_t;
#endif

/* Value type.  */
#if ! defined GRAMMARSTYPE && ! defined GRAMMARSTYPE_IS_DECLARED
union GRAMMARSTYPE
{
#line 27 "grammar.y"

  uint4 flags;
  TypeDeclarator *dec;
  vector<TypeDeclarator *> *declist;
  TypeSpecifiers *spec;
  vector<uint4> *ptrspec;
  Datatype *type;
  Enumerator *enumer;
  vector<Enumerator *> *vecenum;
  string *str;
  uintb *i;

#line 101 "grammar.tab.h"

};
typedef union GRAMMARSTYPE GRAMMARSTYPE;
# define GRAMMARSTYPE_IS_TRIVIAL 1
# define GRAMMARSTYPE_IS_DECLARED 1
#endif


extern GRAMMARSTYPE grammarlval;


int grammarparse (void);


#endif /* !YY_GRAMMAR_GRAMMAR_TAB_H_INCLUDED  */

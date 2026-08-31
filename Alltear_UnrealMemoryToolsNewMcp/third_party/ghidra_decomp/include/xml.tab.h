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

#ifndef YY_XML_XML_TAB_H_INCLUDED
# define YY_XML_XML_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef XMLDEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define XMLDEBUG 1
#  else
#   define XMLDEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define XMLDEBUG 0
# endif /* ! defined YYDEBUG */
#endif  /* ! defined XMLDEBUG */
#if XMLDEBUG
extern int xmldebug;
#endif

/* Token kinds.  */
#ifndef XMLTOKENTYPE
# define XMLTOKENTYPE
  enum xmltokentype
  {
    XMLEMPTY = -2,
    XMLEOF = 0,                    /* "end of file"  */
    XMLerror = 256,                /* error  */
    XMLUNDEF = 257,                /* "invalid token"  */
    CHARDATA = 258,                /* CHARDATA  */
    CDATA = 259,                   /* CDATA  */
    ATTVALUE = 260,                /* ATTVALUE  */
    COMMENT = 261,                 /* COMMENT  */
    CHARREF = 262,                 /* CHARREF  */
    NAME = 263,                    /* NAME  */
    SNAME = 264,                   /* SNAME  */
    ELEMBRACE = 265,               /* ELEMBRACE  */
    COMMBRACE = 266                /* COMMBRACE  */
  };
  typedef enum xmltokentype xmltoken_kind_t;
#endif

/* Value type.  */
#if ! defined XMLSTYPE && ! defined XMLSTYPE_IS_DECLARED
union XMLSTYPE
{
#line 122 "xml.y"

  int4 i;
  string *str;
  Attributes *attr;
  NameValue *pair;

#line 90 "xml.tab.h"

};
typedef union XMLSTYPE XMLSTYPE;
# define XMLSTYPE_IS_TRIVIAL 1
# define XMLSTYPE_IS_DECLARED 1
#endif


extern XMLSTYPE xmllval;


int xmlparse (void);


#endif /* !YY_XML_XML_TAB_H_INCLUDED  */

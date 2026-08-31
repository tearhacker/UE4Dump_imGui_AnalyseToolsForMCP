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

#ifndef YY_PCODE_PCODEPARSE_TAB_H_INCLUDED
# define YY_PCODE_PCODEPARSE_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef PCODEDEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define PCODEDEBUG 1
#  else
#   define PCODEDEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define PCODEDEBUG 0
# endif /* ! defined YYDEBUG */
#endif  /* ! defined PCODEDEBUG */
#if PCODEDEBUG
extern int pcodedebug;
#endif

/* Token kinds.  */
#ifndef PCODETOKENTYPE
# define PCODETOKENTYPE
  enum pcodetokentype
  {
    PCODEEMPTY = -2,
    PCODEEOF = 0,                  /* "end of file"  */
    PCODEerror = 256,              /* error  */
    PCODEUNDEF = 257,              /* "invalid token"  */
    OP_BOOL_OR = 258,              /* OP_BOOL_OR  */
    OP_BOOL_AND = 259,             /* OP_BOOL_AND  */
    OP_BOOL_XOR = 260,             /* OP_BOOL_XOR  */
    OP_EQUAL = 261,                /* OP_EQUAL  */
    OP_NOTEQUAL = 262,             /* OP_NOTEQUAL  */
    OP_FEQUAL = 263,               /* OP_FEQUAL  */
    OP_FNOTEQUAL = 264,            /* OP_FNOTEQUAL  */
    OP_GREATEQUAL = 265,           /* OP_GREATEQUAL  */
    OP_LESSEQUAL = 266,            /* OP_LESSEQUAL  */
    OP_SLESS = 267,                /* OP_SLESS  */
    OP_SGREATEQUAL = 268,          /* OP_SGREATEQUAL  */
    OP_SLESSEQUAL = 269,           /* OP_SLESSEQUAL  */
    OP_SGREAT = 270,               /* OP_SGREAT  */
    OP_FLESS = 271,                /* OP_FLESS  */
    OP_FGREAT = 272,               /* OP_FGREAT  */
    OP_FLESSEQUAL = 273,           /* OP_FLESSEQUAL  */
    OP_FGREATEQUAL = 274,          /* OP_FGREATEQUAL  */
    OP_LEFT = 275,                 /* OP_LEFT  */
    OP_RIGHT = 276,                /* OP_RIGHT  */
    OP_SRIGHT = 277,               /* OP_SRIGHT  */
    OP_FADD = 278,                 /* OP_FADD  */
    OP_FSUB = 279,                 /* OP_FSUB  */
    OP_SDIV = 280,                 /* OP_SDIV  */
    OP_SREM = 281,                 /* OP_SREM  */
    OP_FMULT = 282,                /* OP_FMULT  */
    OP_FDIV = 283,                 /* OP_FDIV  */
    OP_ZEXT = 284,                 /* OP_ZEXT  */
    OP_CARRY = 285,                /* OP_CARRY  */
    OP_BORROW = 286,               /* OP_BORROW  */
    OP_SEXT = 287,                 /* OP_SEXT  */
    OP_SCARRY = 288,               /* OP_SCARRY  */
    OP_SBORROW = 289,              /* OP_SBORROW  */
    OP_NAN = 290,                  /* OP_NAN  */
    OP_ABS = 291,                  /* OP_ABS  */
    OP_SQRT = 292,                 /* OP_SQRT  */
    OP_CEIL = 293,                 /* OP_CEIL  */
    OP_FLOOR = 294,                /* OP_FLOOR  */
    OP_ROUND = 295,                /* OP_ROUND  */
    OP_INT2FLOAT = 296,            /* OP_INT2FLOAT  */
    OP_FLOAT2FLOAT = 297,          /* OP_FLOAT2FLOAT  */
    OP_TRUNC = 298,                /* OP_TRUNC  */
    OP_NEW = 299,                  /* OP_NEW  */
    BADINTEGER = 300,              /* BADINTEGER  */
    GOTO_KEY = 301,                /* GOTO_KEY  */
    CALL_KEY = 302,                /* CALL_KEY  */
    RETURN_KEY = 303,              /* RETURN_KEY  */
    IF_KEY = 304,                  /* IF_KEY  */
    ENDOFSTREAM = 305,             /* ENDOFSTREAM  */
    LOCAL_KEY = 306,               /* LOCAL_KEY  */
    INTEGER = 307,                 /* INTEGER  */
    STRING = 308,                  /* STRING  */
    SPACESYM = 309,                /* SPACESYM  */
    USEROPSYM = 310,               /* USEROPSYM  */
    VARSYM = 311,                  /* VARSYM  */
    OPERANDSYM = 312,              /* OPERANDSYM  */
    JUMPSYM = 313,                 /* JUMPSYM  */
    LABELSYM = 314                 /* LABELSYM  */
  };
  typedef enum pcodetokentype pcodetoken_kind_t;
#endif

/* Value type.  */
#if ! defined PCODESTYPE && ! defined PCODESTYPE_IS_DECLARED
union PCODESTYPE
{
#line 28 "pcodeparse.y"

  uintb *i;
  string *str;
  vector<ExprTree *> *param;
  StarQuality *starqual;
  VarnodeTpl *varnode;
  ExprTree *tree;
  vector<OpTpl *> *stmt;
  ConstructTpl *sem;

  SpaceSymbol *spacesym;
  UserOpSymbol *useropsym;
  LabelSymbol *labelsym;
  OperandSymbol *operandsym;
  VarnodeSymbol *varsym;
  SpecificSymbol *specsym;

#line 149 "pcodeparse.tab.h"

};
typedef union PCODESTYPE PCODESTYPE;
# define PCODESTYPE_IS_TRIVIAL 1
# define PCODESTYPE_IS_DECLARED 1
#endif


extern PCODESTYPE pcodelval;


int pcodeparse (void);


#endif /* !YY_PCODE_PCODEPARSE_TAB_H_INCLUDED  */

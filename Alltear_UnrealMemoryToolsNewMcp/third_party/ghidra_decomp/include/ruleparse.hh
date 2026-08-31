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

#ifndef YY_RULEPARSE_RULEPARSE_HH_INCLUDED
# define YY_RULEPARSE_RULEPARSE_HH_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int ruleparsedebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    RIGHT_ARROW = 258,             /* RIGHT_ARROW  */
    LEFT_ARROW = 259,              /* LEFT_ARROW  */
    DOUBLE_RIGHT_ARROW = 260,      /* DOUBLE_RIGHT_ARROW  */
    DOUBLE_LEFT_ARROW = 261,       /* DOUBLE_LEFT_ARROW  */
    OP_BOOL_OR = 262,              /* OP_BOOL_OR  */
    OP_BOOL_AND = 263,             /* OP_BOOL_AND  */
    OP_BOOL_XOR = 264,             /* OP_BOOL_XOR  */
    OP_INT_OR = 265,               /* OP_INT_OR  */
    OP_INT_XOR = 266,              /* OP_INT_XOR  */
    OP_INT_AND = 267,              /* OP_INT_AND  */
    OP_INT_EQUAL = 268,            /* OP_INT_EQUAL  */
    OP_INT_NOTEQUAL = 269,         /* OP_INT_NOTEQUAL  */
    OP_FLOAT_EQUAL = 270,          /* OP_FLOAT_EQUAL  */
    OP_FLOAT_NOTEQUAL = 271,       /* OP_FLOAT_NOTEQUAL  */
    OP_INT_LESS = 272,             /* OP_INT_LESS  */
    OP_INT_LESSEQUAL = 273,        /* OP_INT_LESSEQUAL  */
    OP_INT_SLESS = 274,            /* OP_INT_SLESS  */
    OP_INT_SLESSEQUAL = 275,       /* OP_INT_SLESSEQUAL  */
    OP_FLOAT_LESS = 276,           /* OP_FLOAT_LESS  */
    OP_FLOAT_LESSEQUAL = 277,      /* OP_FLOAT_LESSEQUAL  */
    OP_INT_LEFT = 278,             /* OP_INT_LEFT  */
    OP_INT_RIGHT = 279,            /* OP_INT_RIGHT  */
    OP_INT_SRIGHT = 280,           /* OP_INT_SRIGHT  */
    OP_INT_ADD = 281,              /* OP_INT_ADD  */
    OP_INT_SUB = 282,              /* OP_INT_SUB  */
    OP_FLOAT_ADD = 283,            /* OP_FLOAT_ADD  */
    OP_FLOAT_SUB = 284,            /* OP_FLOAT_SUB  */
    OP_INT_SDIV = 285,             /* OP_INT_SDIV  */
    OP_INT_SREM = 286,             /* OP_INT_SREM  */
    OP_FLOAT_MULT = 287,           /* OP_FLOAT_MULT  */
    OP_FLOAT_DIV = 288,            /* OP_FLOAT_DIV  */
    OP_INT_MULT = 289,             /* OP_INT_MULT  */
    OP_INT_DIV = 290,              /* OP_INT_DIV  */
    OP_INT_REM = 291,              /* OP_INT_REM  */
    OP_BOOL_NEGATE = 292,          /* OP_BOOL_NEGATE  */
    OP_INT_NEGATE = 293,           /* OP_INT_NEGATE  */
    OP_INT_ZEXT = 294,             /* OP_INT_ZEXT  */
    OP_INT_CARRY = 295,            /* OP_INT_CARRY  */
    OP_INT_BORROW = 296,           /* OP_INT_BORROW  */
    OP_INT_SEXT = 297,             /* OP_INT_SEXT  */
    OP_INT_SCARRY = 298,           /* OP_INT_SCARRY  */
    OP_INT_SBORROW = 299,          /* OP_INT_SBORROW  */
    OP_FLOAT_NAN = 300,            /* OP_FLOAT_NAN  */
    OP_FLOAT_ABS = 301,            /* OP_FLOAT_ABS  */
    OP_FLOAT_SQRT = 302,           /* OP_FLOAT_SQRT  */
    OP_FLOAT_CEIL = 303,           /* OP_FLOAT_CEIL  */
    OP_FLOAT_FLOOR = 304,          /* OP_FLOAT_FLOOR  */
    OP_FLOAT_ROUND = 305,          /* OP_FLOAT_ROUND  */
    OP_FLOAT_INT2FLOAT = 306,      /* OP_FLOAT_INT2FLOAT  */
    OP_FLOAT_FLOAT2FLOAT = 307,    /* OP_FLOAT_FLOAT2FLOAT  */
    OP_FLOAT_TRUNC = 308,          /* OP_FLOAT_TRUNC  */
    OP_BRANCH = 309,               /* OP_BRANCH  */
    OP_BRANCHIND = 310,            /* OP_BRANCHIND  */
    OP_CALL = 311,                 /* OP_CALL  */
    OP_CALLIND = 312,              /* OP_CALLIND  */
    OP_RETURN = 313,               /* OP_RETURN  */
    OP_CBRANCH = 314,              /* OP_CBRANCH  */
    OP_CALLOTHER = 315,            /* OP_CALLOTHER  */
    OP_LOAD = 316,                 /* OP_LOAD  */
    OP_STORE = 317,                /* OP_STORE  */
    OP_PIECE = 318,                /* OP_PIECE  */
    OP_SUBPIECE = 319,             /* OP_SUBPIECE  */
    OP_COPY = 320,                 /* OP_COPY  */
    BADINTEGER = 321,              /* BADINTEGER  */
    BEFORE_KEYWORD = 322,          /* BEFORE_KEYWORD  */
    AFTER_KEYWORD = 323,           /* AFTER_KEYWORD  */
    REMOVE_KEYWORD = 324,          /* REMOVE_KEYWORD  */
    SET_KEYWORD = 325,             /* SET_KEYWORD  */
    ACTION_TICK = 326,             /* ACTION_TICK  */
    ISTRUE_KEYWORD = 327,          /* ISTRUE_KEYWORD  */
    ISFALSE_KEYWORD = 328,         /* ISFALSE_KEYWORD  */
    CHAR = 329,                    /* CHAR  */
    INTB = 330,                    /* INTB  */
    OP_IDENTIFIER = 331,           /* OP_IDENTIFIER  */
    VAR_IDENTIFIER = 332,          /* VAR_IDENTIFIER  */
    CONST_IDENTIFIER = 333,        /* CONST_IDENTIFIER  */
    OP_NEW_IDENTIFIER = 334,       /* OP_NEW_IDENTIFIER  */
    VAR_NEW_IDENTIFIER = 335,      /* VAR_NEW_IDENTIFIER  */
    DOT_IDENTIFIER = 336           /* DOT_IDENTIFIER  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 28 "ruleparse.y"

  char ch;
  string *str;
  int8 *big;
  int4 id;
  OpCode opcode;
  vector<OpCode> *opcodelist;
  ConstraintGroup *group;
  RHSConstant *rhsconst;

#line 156 "ruleparse.hh"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE ruleparselval;


int ruleparseparse (void);


#endif /* !YY_RULEPARSE_RULEPARSE_HH_INCLUDED  */

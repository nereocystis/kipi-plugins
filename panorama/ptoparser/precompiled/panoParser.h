/* A Bison parser, made by GNU Bison 2.6.4.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
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

#ifndef YY_YY_TMP_DIGIKAM_SC_BUILD_EXTRA_KIPI_PLUGINS_PANORAMA_PANOPARSER_H_INCLUDED
# define YY_YY_TMP_DIGIKAM_SC_BUILD_EXTRA_KIPI_PLUGINS_PANORAMA_PANOPARSER_H_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     PT_TOKEN_NUMBER_INT = 258,
     PT_TOKEN_NUMBER_FLOAT = 259,
     PT_TOKEN_STRING = 260,
     PT_TOKEN_HUGIN_KEYWORD = 261,
     PT_TOKEN_KEYWORD = 262,
     PT_TOKEN_KEYWORD_MULTICHAR = 263,
     PT_TOKEN_KEYWORD_CROPPING = 264,
     PT_TOKEN_COMMENT = 265,
     PT_TOKEN_EOL = 266,
     PT_TOKEN_SEP = 267,
     PT_TOKEN_INPUT_LINE = 268,
     PT_TOKEN_PANO_LINE = 269,
     PT_TOKEN_CONTROL_PT_LINE = 270,
     PT_TOKEN_OPTIMIZE_OPT_LINE = 271,
     PT_TOKEN_OPTIMIZE_VARS_LINE = 272,
     PT_TOKEN_KEYWORD_MASK = 273,
     PT_TOKEN_KEYWORD_PROJPARAMS = 274,
     PT_TOKEN_COMMA = 275,
     PT_TOKEN_REFERENCE = 276,
     PT_TOKEN_MASK_PT_LINE = 277,
     PT_TOKEN_ERROR = 278,
     PT_TOKEN_EOF = 279
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 2077 of yacc.c  */
#line 69 "ptoparser/panoParser.y"

    int     iVal;
    float   fVal;
    char    strVal[PT_TOKEN_MAX_LEN + 1];
    char    cVal;


/* Line 2077 of yacc.c  */
#line 89 "/tmp/digikam-sc/build/extra/kipi-plugins/panorama/panoParser.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_YY_TMP_DIGIKAM_SC_BUILD_EXTRA_KIPI_PLUGINS_PANORAMA_PANOPARSER_H_INCLUDED  */
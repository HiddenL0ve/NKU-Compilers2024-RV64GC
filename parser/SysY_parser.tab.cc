/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "parser/SysY_parser.y"

#include <fstream>
#include "SysY_tree.h"
#include "type.h"
Program ast_root;

void yyerror(char *s, ...);
int yylex();
int error_num = 0;
extern int line_number;
extern std::ofstream fout;
extern IdTable id_table;

#line 85 "SysY_parser.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "SysY_parser.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_STR_CONST = 3,                  /* STR_CONST  */
  YYSYMBOL_IDENT = 4,                      /* IDENT  */
  YYSYMBOL_FLOAT_CONST = 5,                /* FLOAT_CONST  */
  YYSYMBOL_INT_CONST = 6,                  /* INT_CONST  */
  YYSYMBOL_LEQ = 7,                        /* LEQ  */
  YYSYMBOL_GEQ = 8,                        /* GEQ  */
  YYSYMBOL_EQ = 9,                         /* EQ  */
  YYSYMBOL_NE = 10,                        /* NE  */
  YYSYMBOL_AND = 11,                       /* AND  */
  YYSYMBOL_OR = 12,                        /* OR  */
  YYSYMBOL_CONST = 13,                     /* CONST  */
  YYSYMBOL_IF = 14,                        /* IF  */
  YYSYMBOL_ELSE = 15,                      /* ELSE  */
  YYSYMBOL_WHILE = 16,                     /* WHILE  */
  YYSYMBOL_NONE_TYPE = 17,                 /* NONE_TYPE  */
  YYSYMBOL_INT = 18,                       /* INT  */
  YYSYMBOL_FLOAT = 19,                     /* FLOAT  */
  YYSYMBOL_FOR = 20,                       /* FOR  */
  YYSYMBOL_RETURN = 21,                    /* RETURN  */
  YYSYMBOL_BREAK = 22,                     /* BREAK  */
  YYSYMBOL_CONTINUE = 23,                  /* CONTINUE  */
  YYSYMBOL_ERROR = 24,                     /* ERROR  */
  YYSYMBOL_TODO = 25,                      /* TODO  */
  YYSYMBOL_THEN = 26,                      /* THEN  */
  YYSYMBOL_27_ = 27,                       /* ';'  */
  YYSYMBOL_28_ = 28,                       /* ','  */
  YYSYMBOL_29_ = 29,                       /* '('  */
  YYSYMBOL_30_ = 30,                       /* ')'  */
  YYSYMBOL_31_ = 31,                       /* '='  */
  YYSYMBOL_32_ = 32,                       /* '{'  */
  YYSYMBOL_33_ = 33,                       /* '}'  */
  YYSYMBOL_34_ = 34,                       /* '['  */
  YYSYMBOL_35_ = 35,                       /* ']'  */
  YYSYMBOL_36_ = 36,                       /* '+'  */
  YYSYMBOL_37_ = 37,                       /* '-'  */
  YYSYMBOL_38_ = 38,                       /* '!'  */
  YYSYMBOL_39_ = 39,                       /* '*'  */
  YYSYMBOL_40_ = 40,                       /* '/'  */
  YYSYMBOL_41_ = 41,                       /* '%'  */
  YYSYMBOL_42_ = 42,                       /* '<'  */
  YYSYMBOL_43_ = 43,                       /* '>'  */
  YYSYMBOL_YYACCEPT = 44,                  /* $accept  */
  YYSYMBOL_Program = 45,                   /* Program  */
  YYSYMBOL_Comp_list = 46,                 /* Comp_list  */
  YYSYMBOL_CompUnit = 47,                  /* CompUnit  */
  YYSYMBOL_Decl = 48,                      /* Decl  */
  YYSYMBOL_VarDecl = 49,                   /* VarDecl  */
  YYSYMBOL_ConstDecl = 50,                 /* ConstDecl  */
  YYSYMBOL_VarDef_list = 51,               /* VarDef_list  */
  YYSYMBOL_ConstDef_list = 52,             /* ConstDef_list  */
  YYSYMBOL_FuncDef = 53,                   /* FuncDef  */
  YYSYMBOL_VarDef = 54,                    /* VarDef  */
  YYSYMBOL_ConstDef = 55,                  /* ConstDef  */
  YYSYMBOL_ConstInitVal = 56,              /* ConstInitVal  */
  YYSYMBOL_VarInitVal = 57,                /* VarInitVal  */
  YYSYMBOL_ConstInitVal_list = 58,         /* ConstInitVal_list  */
  YYSYMBOL_VarInitVal_list = 59,           /* VarInitVal_list  */
  YYSYMBOL_FuncFParams = 60,               /* FuncFParams  */
  YYSYMBOL_FuncFParam = 61,                /* FuncFParam  */
  YYSYMBOL_Block = 62,                     /* Block  */
  YYSYMBOL_BlockItem_list = 63,            /* BlockItem_list  */
  YYSYMBOL_BlockItem = 64,                 /* BlockItem  */
  YYSYMBOL_Stmt = 65,                      /* Stmt  */
  YYSYMBOL_Exp = 66,                       /* Exp  */
  YYSYMBOL_Cond = 67,                      /* Cond  */
  YYSYMBOL_Lval = 68,                      /* Lval  */
  YYSYMBOL_PrimaryExp = 69,                /* PrimaryExp  */
  YYSYMBOL_IntConst = 70,                  /* IntConst  */
  YYSYMBOL_FloatConst = 71,                /* FloatConst  */
  YYSYMBOL_UnaryExp = 72,                  /* UnaryExp  */
  YYSYMBOL_FuncRParams = 73,               /* FuncRParams  */
  YYSYMBOL_Exp_list = 74,                  /* Exp_list  */
  YYSYMBOL_MulExp = 75,                    /* MulExp  */
  YYSYMBOL_AddExp = 76,                    /* AddExp  */
  YYSYMBOL_RelExp = 77,                    /* RelExp  */
  YYSYMBOL_EqExp = 78,                     /* EqExp  */
  YYSYMBOL_LAndExp = 79,                   /* LAndExp  */
  YYSYMBOL_LOrExp = 80,                    /* LOrExp  */
  YYSYMBOL_ConstExp = 81,                  /* ConstExp  */
  YYSYMBOL_Array = 82,                     /* Array  */
  YYSYMBOL_Array_list = 83,                /* Array_list  */
  YYSYMBOL_ConstArray = 84,                /* ConstArray  */
  YYSYMBOL_ConstArray_list = 85            /* ConstArray_list  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  20
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   302

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  44
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  42
/* YYNRULES -- Number of rules.  */
#define YYNRULES  108
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  200

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   281


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    38,     2,     2,     2,    41,     2,     2,
      29,    30,    39,    36,    28,    37,     2,    40,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    27,
      42,    31,    43,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    34,     2,    35,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    32,     2,    33,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    76,    76,    84,    89,    96,   100,   107,   111,   118,
     122,   131,   135,   144,   148,   157,   161,   169,   174,   179,
     183,   188,   192,   201,   203,   205,   207,   215,   217,   224,
     226,   230,   239,   243,   247,   254,   258,   268,   272,   280,
     284,   294,   298,   302,   308,   314,   319,   328,   332,   339,
     343,   351,   355,   363,   367,   371,   375,   379,   383,   387,
     391,   395,   399,   403,   411,   415,   420,   424,   432,   436,
     440,   444,   453,   460,   468,   469,   473,   497,   501,   505,
     512,   520,   524,   532,   533,   537,   541,   551,   555,   559,
     566,   570,   574,   578,   582,   590,   591,   595,   603,   604,
     612,   613,   619,   625,   629,   634,   642,   646,   651
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "STR_CONST", "IDENT",
  "FLOAT_CONST", "INT_CONST", "LEQ", "GEQ", "EQ", "NE", "AND", "OR",
  "CONST", "IF", "ELSE", "WHILE", "NONE_TYPE", "INT", "FLOAT", "FOR",
  "RETURN", "BREAK", "CONTINUE", "ERROR", "TODO", "THEN", "';'", "','",
  "'('", "')'", "'='", "'{'", "'}'", "'['", "']'", "'+'", "'-'", "'!'",
  "'*'", "'/'", "'%'", "'<'", "'>'", "$accept", "Program", "Comp_list",
  "CompUnit", "Decl", "VarDecl", "ConstDecl", "VarDef_list",
  "ConstDef_list", "FuncDef", "VarDef", "ConstDef", "ConstInitVal",
  "VarInitVal", "ConstInitVal_list", "VarInitVal_list", "FuncFParams",
  "FuncFParam", "Block", "BlockItem_list", "BlockItem", "Stmt", "Exp",
  "Cond", "Lval", "PrimaryExp", "IntConst", "FloatConst", "UnaryExp",
  "FuncRParams", "Exp_list", "MulExp", "AddExp", "RelExp", "EqExp",
  "LAndExp", "LOrExp", "ConstExp", "Array", "Array_list", "ConstArray",
  "ConstArray_list", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-160)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      31,    76,    26,    37,    47,    57,    31,  -160,  -160,  -160,
    -160,  -160,    79,    79,    71,    40,   107,  -160,    50,   126,
    -160,  -160,    -8,   128,  -160,   130,     6,    10,   245,    29,
    -160,    11,  -160,   102,    45,  -160,   264,    29,  -160,    39,
    -160,    79,  -160,   108,   119,   109,    99,  -160,   109,   100,
      51,  -160,  -160,    29,    72,    29,    29,    29,  -160,  -160,
    -160,  -160,  -160,  -160,  -160,    85,   134,    97,   245,  -160,
      65,  -160,   109,   103,   229,  -160,   134,  -160,   104,   264,
    -160,  -160,   127,   129,   146,  -160,   158,   109,  -160,   109,
      84,   132,   142,  -160,  -160,    58,  -160,  -160,  -160,    29,
      29,    29,    29,    29,  -160,  -160,  -160,   109,  -160,  -160,
      59,  -160,  -160,   139,   157,   164,   167,   102,   102,   251,
     174,   178,  -160,  -160,  -160,  -160,   181,  -160,  -160,   179,
     176,  -160,  -160,  -160,  -160,  -160,   168,   183,  -160,   245,
    -160,  -160,  -160,  -160,    85,    85,  -160,   264,  -160,   132,
     132,    29,    29,  -160,   182,  -160,  -160,  -160,  -160,  -160,
      29,  -160,    29,  -160,  -160,   132,   132,   185,   134,    13,
     171,   201,   204,   193,  -160,   197,  -160,   216,    29,    29,
      29,    29,    29,    29,    29,    29,   216,  -160,   210,   134,
     134,   134,   134,    13,    13,   171,   201,  -160,   216,  -160
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     2,     3,     5,     7,
       8,     6,     0,     0,     0,    24,     0,    13,    24,     0,
       1,     4,     0,     0,    15,     0,     0,     0,     0,     0,
     104,    26,     9,     0,     0,    10,     0,     0,   107,     0,
      11,     0,    12,     0,     0,     0,     0,    39,     0,     0,
      66,    73,    72,     0,     0,     0,     0,     0,    23,    34,
      69,    74,    71,    70,    83,    87,    64,     0,     0,   105,
      24,    14,     0,     0,     0,    27,   102,    31,     0,     0,
     108,    16,    41,    42,     0,    22,     0,     0,    18,     0,
       0,    67,     0,    32,    37,     0,    77,    78,    79,     0,
       0,     0,     0,     0,   103,    25,    20,     0,    29,    35,
       0,   106,    28,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    54,    48,    52,    55,     0,    49,    51,     0,
      69,    40,    21,    17,    76,    81,     0,    80,    68,     0,
      33,    84,    85,    86,    88,    89,    19,     0,    30,    43,
      44,     0,     0,    63,     0,    60,    61,    47,    50,    56,
       0,    75,     0,    38,    36,    45,    46,     0,    90,    95,
      98,   100,    65,     0,    62,     0,    82,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    53,    58,    91,
      93,    92,    94,    96,    97,    99,   101,    57,     0,    59
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -160,  -160,  -160,   220,   -73,  -160,  -160,     0,   214,  -160,
     195,   188,   -65,   -46,  -160,  -160,   -15,   145,   -35,  -160,
     110,  -159,   -22,    88,   -79,  -160,  -160,  -160,   -40,  -160,
    -160,    86,   -36,     8,    60,    56,  -160,   205,   -29,   -47,
     207,  -160
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     5,     6,     7,     8,     9,    10,    16,    23,    11,
      17,    24,    75,    58,   110,    95,    46,    47,   125,   126,
     127,   128,   129,   167,    60,    61,    62,    63,    64,   136,
     137,    65,    66,   169,   170,   171,   172,    77,    30,    31,
      38,    39
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      76,    76,    69,    91,    19,   130,    59,    67,    94,   109,
      85,   124,    49,    88,   112,    96,    97,    98,   188,    73,
     178,   179,   105,    36,    43,    44,    37,   197,    43,    44,
      14,    92,    59,    50,    51,    52,    45,   106,    76,   199,
      48,    15,    68,    76,     1,    29,    59,   130,     2,     3,
       4,    18,   132,   124,   133,   180,   181,    20,    53,   141,
     142,   143,    69,    43,    44,    55,    56,    57,   135,    27,
      79,    28,   146,    37,    29,    72,    50,    51,    52,    34,
      90,    28,   164,    22,    29,    29,   139,   147,    50,    51,
      52,   140,   148,   163,    12,    13,    28,   154,   130,    29,
      26,    53,   165,   166,    54,    93,    70,   130,    55,    56,
      57,    76,    82,    53,   134,   168,   168,    59,    19,   130,
      55,    56,    57,    83,    99,   100,   101,    86,    86,    87,
      89,    86,   104,   107,    32,    33,    69,    69,   175,   111,
     176,    84,   189,   190,   191,   192,   168,   168,   168,   168,
      50,    51,    52,    35,    33,    40,    41,    42,    41,     1,
     115,   113,   116,   114,   117,   118,    29,   119,   120,   121,
     102,   103,   138,   122,   149,    53,    43,    44,    84,   123,
     182,   183,    55,    56,    57,    50,    51,    52,   144,   145,
     193,   194,   150,   151,     1,   115,   152,   116,   161,   117,
     118,   155,   119,   120,   121,   156,   159,   160,   122,   174,
      53,   162,   184,    84,   157,   177,   185,    55,    56,    57,
      50,    51,    52,   186,   187,   198,    21,    25,    71,    81,
     115,   131,   116,    50,    51,    52,   158,   119,   120,   121,
     173,   196,    78,   122,   195,    53,    80,     0,    84,    50,
      51,    52,    55,    56,    57,    50,    51,    52,    53,     0,
       0,    74,   108,     0,     0,    55,    56,    57,    50,    51,
      52,     0,     0,     0,    53,     0,     0,    54,   153,     0,
      53,    55,    56,    57,     0,     0,     0,    55,    56,    57,
       0,     0,     0,    53,     0,     0,    74,     0,     0,     0,
      55,    56,    57
};

static const yytype_int16 yycheck[] =
{
      36,    37,    31,    50,     4,    84,    28,    29,    54,    74,
      45,    84,    27,    48,    79,    55,    56,    57,   177,    34,
       7,     8,    68,    31,    18,    19,    34,   186,    18,    19,
       4,    53,    54,     4,     5,     6,    30,    72,    74,   198,
      30,     4,    31,    79,    13,    34,    68,   126,    17,    18,
      19,     4,    87,   126,    89,    42,    43,     0,    29,    99,
     100,   101,    91,    18,    19,    36,    37,    38,    90,    29,
      31,    31,   107,    34,    34,    30,     4,     5,     6,    29,
      29,    31,   147,     4,    34,    34,    28,    28,     4,     5,
       6,    33,    33,   139,    18,    19,    31,   119,   177,    34,
      29,    29,   149,   150,    32,    33,     4,   186,    36,    37,
      38,   147,     4,    29,    30,   151,   152,   139,   118,   198,
      36,    37,    38,     4,    39,    40,    41,    28,    28,    30,
      30,    28,    35,    30,    27,    28,   165,   166,   160,    35,
     162,    32,   178,   179,   180,   181,   182,   183,   184,   185,
       4,     5,     6,    27,    28,    27,    28,    27,    28,    13,
      14,    34,    16,    34,    18,    19,    34,    21,    22,    23,
      36,    37,    30,    27,    35,    29,    18,    19,    32,    33,
       9,    10,    36,    37,    38,     4,     5,     6,   102,   103,
     182,   183,    35,    29,    13,    14,    29,    16,    30,    18,
      19,    27,    21,    22,    23,    27,    27,    31,    27,    27,
      29,    28,    11,    32,    33,    30,    12,    36,    37,    38,
       4,     5,     6,    30,    27,    15,     6,    13,    33,    41,
      14,    86,    16,     4,     5,     6,   126,    21,    22,    23,
     152,   185,    37,    27,   184,    29,    39,    -1,    32,     4,
       5,     6,    36,    37,    38,     4,     5,     6,    29,    -1,
      -1,    32,    33,    -1,    -1,    36,    37,    38,     4,     5,
       6,    -1,    -1,    -1,    29,    -1,    -1,    32,    27,    -1,
      29,    36,    37,    38,    -1,    -1,    -1,    36,    37,    38,
      -1,    -1,    -1,    29,    -1,    -1,    32,    -1,    -1,    -1,
      36,    37,    38
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    13,    17,    18,    19,    45,    46,    47,    48,    49,
      50,    53,    18,    19,     4,     4,    51,    54,     4,    51,
       0,    47,     4,    52,    55,    52,    29,    29,    31,    34,
      82,    83,    27,    28,    29,    27,    31,    34,    84,    85,
      27,    28,    27,    18,    19,    30,    60,    61,    30,    60,
       4,     5,     6,    29,    32,    36,    37,    38,    57,    66,
      68,    69,    70,    71,    72,    75,    76,    66,    31,    82,
       4,    54,    30,    60,    32,    56,    76,    81,    81,    31,
      84,    55,     4,     4,    32,    62,    28,    30,    62,    30,
      29,    83,    66,    33,    57,    59,    72,    72,    72,    39,
      40,    41,    36,    37,    35,    57,    62,    30,    33,    56,
      58,    35,    56,    34,    34,    14,    16,    18,    19,    21,
      22,    23,    27,    33,    48,    62,    63,    64,    65,    66,
      68,    61,    62,    62,    30,    66,    73,    74,    30,    28,
      33,    72,    72,    72,    75,    75,    62,    28,    33,    35,
      35,    29,    29,    27,    66,    27,    27,    33,    64,    27,
      31,    30,    28,    57,    56,    83,    83,    67,    76,    77,
      78,    79,    80,    67,    27,    66,    66,    30,     7,     8,
      42,    43,     9,    10,    11,    12,    30,    27,    65,    76,
      76,    76,    76,    77,    77,    78,    79,    65,    15,    65
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    44,    45,    46,    46,    47,    47,    48,    48,    49,
      49,    50,    50,    51,    51,    52,    52,    53,    53,    53,
      53,    53,    53,    54,    54,    54,    54,    55,    55,    56,
      56,    56,    57,    57,    57,    58,    58,    59,    59,    60,
      60,    61,    61,    61,    61,    61,    61,    62,    62,    63,
      63,    64,    64,    65,    65,    65,    65,    65,    65,    65,
      65,    65,    65,    65,    66,    67,    68,    68,    69,    69,
      69,    69,    70,    71,    72,    72,    72,    72,    72,    72,
      73,    74,    74,    75,    75,    75,    75,    76,    76,    76,
      77,    77,    77,    77,    77,    78,    78,    78,    79,    79,
      80,    80,    81,    82,    83,    83,    84,    85,    85
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     2,     1,     1,     1,     1,     3,
       3,     4,     4,     1,     3,     1,     3,     6,     5,     6,
       5,     6,     5,     3,     1,     4,     2,     3,     4,     2,
       3,     1,     2,     3,     1,     1,     3,     1,     3,     1,
       3,     2,     2,     4,     4,     5,     5,     3,     2,     1,
       2,     1,     1,     4,     1,     1,     2,     5,     5,     7,
       2,     2,     3,     2,     1,     1,     1,     2,     3,     1,
       1,     1,     1,     1,     1,     4,     3,     2,     2,     2,
       1,     1,     3,     1,     3,     3,     3,     1,     3,     3,
       1,     3,     3,     3,     3,     1,     3,     3,     1,     3,
       1,     3,     1,     3,     1,     2,     3,     1,     2
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]));
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* Program: Comp_list  */
#line 77 "parser/SysY_parser.y"
{
    (yyloc) = (yylsp[0]);
    ast_root = new __Program((yyvsp[0].comps));
    ast_root->SetLineNumber(line_number);
}
#line 1433 "SysY_parser.tab.c"
    break;

  case 3: /* Comp_list: CompUnit  */
#line 85 "parser/SysY_parser.y"
{
    (yyval.comps) = new std::vector<CompUnit>;
    ((yyval.comps))->push_back((yyvsp[0].comp_unit));
}
#line 1442 "SysY_parser.tab.c"
    break;

  case 4: /* Comp_list: Comp_list CompUnit  */
#line 90 "parser/SysY_parser.y"
{
    ((yyvsp[-1].comps))->push_back((yyvsp[0].comp_unit));
    (yyval.comps) = (yyvsp[-1].comps);
}
#line 1451 "SysY_parser.tab.c"
    break;

  case 5: /* CompUnit: Decl  */
#line 96 "parser/SysY_parser.y"
     {
    (yyval.comp_unit) = new CompUnit_Decl((yyvsp[0].decl)); 
    (yyval.comp_unit)->SetLineNumber(line_number);
}
#line 1460 "SysY_parser.tab.c"
    break;

  case 6: /* CompUnit: FuncDef  */
#line 100 "parser/SysY_parser.y"
        {
    (yyval.comp_unit) = new CompUnit_FuncDef((yyvsp[0].func_def)); 
    (yyval.comp_unit)->SetLineNumber(line_number);
}
#line 1469 "SysY_parser.tab.c"
    break;

  case 7: /* Decl: VarDecl  */
#line 107 "parser/SysY_parser.y"
        {
    (yyval.decl) = (yyvsp[0].decl); 
    (yyval.decl)->SetLineNumber(line_number);
}
#line 1478 "SysY_parser.tab.c"
    break;

  case 8: /* Decl: ConstDecl  */
#line 111 "parser/SysY_parser.y"
          {
    (yyval.decl) = (yyvsp[0].decl); 
    (yyval.decl)->SetLineNumber(line_number);
}
#line 1487 "SysY_parser.tab.c"
    break;

  case 9: /* VarDecl: INT VarDef_list ';'  */
#line 118 "parser/SysY_parser.y"
                    {
    (yyval.decl) = new VarDecl(Type::INT,(yyvsp[-1].defs)); 
    (yyval.decl)->SetLineNumber(line_number);
}
#line 1496 "SysY_parser.tab.c"
    break;

  case 10: /* VarDecl: FLOAT VarDef_list ';'  */
#line 122 "parser/SysY_parser.y"
                      {
    (yyval.decl)= new VarDecl(Type::FLOAT,(yyvsp[-1].defs));
    (yyval.decl)->SetLineNumber(line_number);
}
#line 1505 "SysY_parser.tab.c"
    break;

  case 11: /* ConstDecl: CONST INT ConstDef_list ';'  */
#line 131 "parser/SysY_parser.y"
                            {
    (yyval.decl) = new ConstDecl(Type::INT,(yyvsp[-1].defs)); 
    (yyval.decl)->SetLineNumber(line_number);
}
#line 1514 "SysY_parser.tab.c"
    break;

  case 12: /* ConstDecl: CONST FLOAT ConstDef_list ';'  */
#line 135 "parser/SysY_parser.y"
                              {
    (yyval.decl) = new ConstDecl(Type::FLOAT,(yyvsp[-1].defs)); 
    (yyval.decl)->SetLineNumber(line_number);
}
#line 1523 "SysY_parser.tab.c"
    break;

  case 13: /* VarDef_list: VarDef  */
#line 144 "parser/SysY_parser.y"
       {
    (yyval.defs)=new std::vector<Def>;
    ((yyval.defs))->push_back((yyvsp[0].def));
}
#line 1532 "SysY_parser.tab.c"
    break;

  case 14: /* VarDef_list: VarDef_list ',' VarDef  */
#line 148 "parser/SysY_parser.y"
                       {
   // $1=new std::vector<Def>;
    ((yyvsp[-2].defs))->push_back((yyvsp[0].def));
    (yyval.defs)=(yyvsp[-2].defs);
}
#line 1542 "SysY_parser.tab.c"
    break;

  case 15: /* ConstDef_list: ConstDef  */
#line 157 "parser/SysY_parser.y"
         {
    (yyval.defs)=new std::vector<Def>;
    ((yyval.defs))->push_back((yyvsp[0].def));
}
#line 1551 "SysY_parser.tab.c"
    break;

  case 16: /* ConstDef_list: ConstDef_list ',' ConstDef  */
#line 161 "parser/SysY_parser.y"
                           {
    ((yyvsp[-2].defs))->push_back((yyvsp[0].def));
    (yyval.defs)=(yyvsp[-2].defs);
}
#line 1560 "SysY_parser.tab.c"
    break;

  case 17: /* FuncDef: INT IDENT '(' FuncFParams ')' Block  */
#line 170 "parser/SysY_parser.y"
{
    (yyval.func_def) = new __FuncDef(Type::INT,(yyvsp[-4].symbol_token),(yyvsp[-2].formals),(yyvsp[0].block));
    (yyval.func_def)->SetLineNumber(line_number);
}
#line 1569 "SysY_parser.tab.c"
    break;

  case 18: /* FuncDef: INT IDENT '(' ')' Block  */
#line 175 "parser/SysY_parser.y"
{
    (yyval.func_def) = new __FuncDef(Type::INT,(yyvsp[-3].symbol_token),new std::vector<FuncFParam>(),(yyvsp[0].block)); 
    (yyval.func_def)->SetLineNumber(line_number);
}
#line 1578 "SysY_parser.tab.c"
    break;

  case 19: /* FuncDef: FLOAT IDENT '(' FuncFParams ')' Block  */
#line 179 "parser/SysY_parser.y"
                                      {
    (yyval.func_def) = new __FuncDef(Type::FLOAT,(yyvsp[-4].symbol_token),(yyvsp[-2].formals),(yyvsp[0].block));
    (yyval.func_def)->SetLineNumber(line_number);
}
#line 1587 "SysY_parser.tab.c"
    break;

  case 20: /* FuncDef: FLOAT IDENT '(' ')' Block  */
#line 184 "parser/SysY_parser.y"
{
    (yyval.func_def) = new __FuncDef(Type::FLOAT,(yyvsp[-3].symbol_token),new std::vector<FuncFParam>(),(yyvsp[0].block)); 
    (yyval.func_def)->SetLineNumber(line_number);
}
#line 1596 "SysY_parser.tab.c"
    break;

  case 21: /* FuncDef: NONE_TYPE IDENT '(' FuncFParams ')' Block  */
#line 188 "parser/SysY_parser.y"
                                          {
    (yyval.func_def) = new __FuncDef(Type::VOID,(yyvsp[-4].symbol_token),(yyvsp[-2].formals),(yyvsp[0].block));
    (yyval.func_def)->SetLineNumber(line_number);
}
#line 1605 "SysY_parser.tab.c"
    break;

  case 22: /* FuncDef: NONE_TYPE IDENT '(' ')' Block  */
#line 193 "parser/SysY_parser.y"
{
    (yyval.func_def) = new __FuncDef(Type::VOID,(yyvsp[-3].symbol_token),new std::vector<FuncFParam>(),(yyvsp[0].block)); 
    (yyval.func_def)->SetLineNumber(line_number);
}
#line 1614 "SysY_parser.tab.c"
    break;

  case 23: /* VarDef: IDENT '=' VarInitVal  */
#line 202 "parser/SysY_parser.y"
{(yyval.def) = new VarDef((yyvsp[-2].symbol_token),nullptr,(yyvsp[0].initval)); (yyval.def)->SetLineNumber(line_number);}
#line 1620 "SysY_parser.tab.c"
    break;

  case 24: /* VarDef: IDENT  */
#line 204 "parser/SysY_parser.y"
{(yyval.def) = new VarDef_no_init((yyvsp[0].symbol_token),nullptr); (yyval.def)->SetLineNumber(line_number);}
#line 1626 "SysY_parser.tab.c"
    break;

  case 25: /* VarDef: IDENT Array_list '=' VarInitVal  */
#line 206 "parser/SysY_parser.y"
{(yyval.def) = new VarDef((yyvsp[-3].symbol_token),(yyvsp[-2].expressions),(yyvsp[0].initval)); (yyval.def)->SetLineNumber(line_number);}
#line 1632 "SysY_parser.tab.c"
    break;

  case 26: /* VarDef: IDENT Array_list  */
#line 208 "parser/SysY_parser.y"
{(yyval.def) = new VarDef_no_init((yyvsp[-1].symbol_token),(yyvsp[0].expressions)); (yyval.def)->SetLineNumber(line_number);}
#line 1638 "SysY_parser.tab.c"
    break;

  case 27: /* ConstDef: IDENT '=' ConstInitVal  */
#line 216 "parser/SysY_parser.y"
{(yyval.def) = new ConstDef((yyvsp[-2].symbol_token),nullptr,(yyvsp[0].initval)); (yyval.def)->SetLineNumber(line_number);}
#line 1644 "SysY_parser.tab.c"
    break;

  case 28: /* ConstDef: IDENT ConstArray_list '=' ConstInitVal  */
#line 218 "parser/SysY_parser.y"
{(yyval.def) = new VarDef((yyvsp[-3].symbol_token),(yyvsp[-2].expressions),(yyvsp[0].initval)); (yyval.def)->SetLineNumber(line_number);}
#line 1650 "SysY_parser.tab.c"
    break;

  case 29: /* ConstInitVal: '{' '}'  */
#line 224 "parser/SysY_parser.y"
        {(yyval.initval) = new ConstInitVal(new std::vector<InitVal>()); (yyval.initval)->SetLineNumber(line_number);}
#line 1656 "SysY_parser.tab.c"
    break;

  case 30: /* ConstInitVal: '{' ConstInitVal_list '}'  */
#line 226 "parser/SysY_parser.y"
                        {
    (yyval.initval)=new ConstInitVal((yyvsp[-1].initvals));
   (yyval.initval)->SetLineNumber(line_number);
}
#line 1665 "SysY_parser.tab.c"
    break;

  case 31: /* ConstInitVal: ConstExp  */
#line 230 "parser/SysY_parser.y"
         {
    (yyval.initval)=new ConstInitVal_exp((yyvsp[0].expression));
    (yyval.initval)->SetLineNumber(line_number);
}
#line 1674 "SysY_parser.tab.c"
    break;

  case 32: /* VarInitVal: '{' '}'  */
#line 239 "parser/SysY_parser.y"
        {
    (yyval.initval) = new VarInitVal(new std::vector<InitVal>()); 
    (yyval.initval)->SetLineNumber(line_number);
}
#line 1683 "SysY_parser.tab.c"
    break;

  case 33: /* VarInitVal: '{' VarInitVal_list '}'  */
#line 243 "parser/SysY_parser.y"
                      {
    (yyval.initval)=new VarInitVal((yyvsp[-1].initvals));
    (yyval.initval)->SetLineNumber(line_number);
}
#line 1692 "SysY_parser.tab.c"
    break;

  case 34: /* VarInitVal: Exp  */
#line 247 "parser/SysY_parser.y"
    {
    (yyval.initval)=new VarInitVal_exp((yyvsp[0].expression));
    (yyval.initval)->SetLineNumber(line_number);
}
#line 1701 "SysY_parser.tab.c"
    break;

  case 35: /* ConstInitVal_list: ConstInitVal  */
#line 254 "parser/SysY_parser.y"
             {
    (yyval.initvals)=new std::vector<InitVal>;
    ((yyval.initvals))->push_back((yyvsp[0].initval));
}
#line 1710 "SysY_parser.tab.c"
    break;

  case 36: /* ConstInitVal_list: ConstInitVal_list ',' ConstInitVal  */
#line 258 "parser/SysY_parser.y"
                                   {
    ((yyvsp[-2].initvals))->push_back((yyvsp[0].initval));
    (yyval.initvals)=(yyvsp[-2].initvals);
}
#line 1719 "SysY_parser.tab.c"
    break;

  case 37: /* VarInitVal_list: VarInitVal  */
#line 268 "parser/SysY_parser.y"
           {
    (yyval.initvals)=new std::vector<InitVal>;
    ((yyval.initvals))->push_back((yyvsp[0].initval));
}
#line 1728 "SysY_parser.tab.c"
    break;

  case 38: /* VarInitVal_list: VarInitVal_list ',' VarInitVal  */
#line 272 "parser/SysY_parser.y"
                               {
    ((yyvsp[-2].initvals))->push_back((yyvsp[0].initval));
    (yyval.initvals)=(yyvsp[-2].initvals);
}
#line 1737 "SysY_parser.tab.c"
    break;

  case 39: /* FuncFParams: FuncFParam  */
#line 280 "parser/SysY_parser.y"
           {
    (yyval.formals) = new std::vector<FuncFParam>;
    ((yyval.formals))->push_back((yyvsp[0].formal));
}
#line 1746 "SysY_parser.tab.c"
    break;

  case 40: /* FuncFParams: FuncFParams ',' FuncFParam  */
#line 284 "parser/SysY_parser.y"
                           {
    ((yyvsp[-2].formals))->push_back((yyvsp[0].formal));
    (yyval.formals) = (yyvsp[-2].formals);
}
#line 1755 "SysY_parser.tab.c"
    break;

  case 41: /* FuncFParam: INT IDENT  */
#line 294 "parser/SysY_parser.y"
          {
    (yyval.formal) = new __FuncFParam(Type::INT,(yyvsp[0].symbol_token),nullptr);
    (yyval.formal)->SetLineNumber(line_number);
}
#line 1764 "SysY_parser.tab.c"
    break;

  case 42: /* FuncFParam: FLOAT IDENT  */
#line 298 "parser/SysY_parser.y"
            {
    (yyval.formal) = new __FuncFParam(Type::FLOAT,(yyvsp[0].symbol_token),nullptr);
    (yyval.formal)->SetLineNumber(line_number);
}
#line 1773 "SysY_parser.tab.c"
    break;

  case 43: /* FuncFParam: INT IDENT '[' ']'  */
#line 302 "parser/SysY_parser.y"
                 {
  std::vector<Expression>* t = new std::vector<Expression>;
    t->push_back(0);
    (yyval.formal) = new __FuncFParam(Type::INT,(yyvsp[-2].symbol_token),t);
    (yyval.formal)->SetLineNumber(line_number);
}
#line 1784 "SysY_parser.tab.c"
    break;

  case 44: /* FuncFParam: FLOAT IDENT '[' ']'  */
#line 308 "parser/SysY_parser.y"
                      {
    std::vector<Expression>* t = new std::vector<Expression>;
    t->push_back(0);
    (yyval.formal) = new __FuncFParam(Type::FLOAT,(yyvsp[-2].symbol_token),t);
    (yyval.formal)->SetLineNumber(line_number);
}
#line 1795 "SysY_parser.tab.c"
    break;

  case 45: /* FuncFParam: INT IDENT '[' ']' Array_list  */
#line 314 "parser/SysY_parser.y"
                             {
    (yyvsp[0].expressions)->insert((yyvsp[0].expressions)->begin(),nullptr);
    (yyval.formal) = new __FuncFParam(Type::INT,(yyvsp[-3].symbol_token),(yyvsp[0].expressions));
    (yyval.formal)->SetLineNumber(line_number);
}
#line 1805 "SysY_parser.tab.c"
    break;

  case 46: /* FuncFParam: FLOAT IDENT '[' ']' Array_list  */
#line 319 "parser/SysY_parser.y"
                               {
    (yyvsp[0].expressions)->insert((yyvsp[0].expressions)->begin(),nullptr);
    (yyval.formal) = new __FuncFParam(Type::FLOAT,(yyvsp[-3].symbol_token),(yyvsp[0].expressions));
    (yyval.formal)->SetLineNumber(line_number);
}
#line 1815 "SysY_parser.tab.c"
    break;

  case 47: /* Block: '{' BlockItem_list '}'  */
#line 328 "parser/SysY_parser.y"
                       {
    (yyval.block) = new __Block((yyvsp[-1].block_items));
    (yyval.block)->SetLineNumber(line_number);
}
#line 1824 "SysY_parser.tab.c"
    break;

  case 48: /* Block: '{' '}'  */
#line 332 "parser/SysY_parser.y"
        {
    (yyval.block) = new __Block(new std::vector<BlockItem>);
    (yyval.block)->SetLineNumber(line_number);
}
#line 1833 "SysY_parser.tab.c"
    break;

  case 49: /* BlockItem_list: BlockItem  */
#line 339 "parser/SysY_parser.y"
          {
    (yyval.block_items) = new std::vector<BlockItem>;
    ((yyval.block_items))->push_back((yyvsp[0].block_item));
}
#line 1842 "SysY_parser.tab.c"
    break;

  case 50: /* BlockItem_list: BlockItem_list BlockItem  */
#line 343 "parser/SysY_parser.y"
                         {
    ((yyvsp[-1].block_items))->push_back((yyvsp[0].block_item));
    (yyval.block_items) = (yyvsp[-1].block_items);
}
#line 1851 "SysY_parser.tab.c"
    break;

  case 51: /* BlockItem: Stmt  */
#line 351 "parser/SysY_parser.y"
     {
    (yyval.block_item) = new BlockItem_Stmt((yyvsp[0].stmt));
    (yyval.block_item)->SetLineNumber(line_number);
}
#line 1860 "SysY_parser.tab.c"
    break;

  case 52: /* BlockItem: Decl  */
#line 355 "parser/SysY_parser.y"
     {
    (yyval.block_item) = new BlockItem_Decl((yyvsp[0].decl));
    (yyval.block_item)->SetLineNumber(line_number);
}
#line 1869 "SysY_parser.tab.c"
    break;

  case 53: /* Stmt: Lval '=' Exp ';'  */
#line 363 "parser/SysY_parser.y"
                  {
    (yyval.stmt) = new assign_stmt((yyvsp[-3].expression), (yyvsp[-1].expression));
    (yyval.stmt)->SetLineNumber(line_number);
}
#line 1878 "SysY_parser.tab.c"
    break;

  case 54: /* Stmt: ';'  */
#line 367 "parser/SysY_parser.y"
    {
    (yyval.stmt) = new null_stmt();
    (yyval.stmt)->SetLineNumber(line_number);
}
#line 1887 "SysY_parser.tab.c"
    break;

  case 55: /* Stmt: Block  */
#line 371 "parser/SysY_parser.y"
      {
    (yyval.stmt) = new block_stmt((yyvsp[0].block));
    (yyval.stmt)->SetLineNumber(line_number);
}
#line 1896 "SysY_parser.tab.c"
    break;

  case 56: /* Stmt: Exp ';'  */
#line 375 "parser/SysY_parser.y"
        {
    (yyval.stmt) = new expr_stmt((yyvsp[-1].expression));
    (yyval.stmt)->SetLineNumber(line_number);
}
#line 1905 "SysY_parser.tab.c"
    break;

  case 57: /* Stmt: WHILE '(' Cond ')' Stmt  */
#line 379 "parser/SysY_parser.y"
                        {
    (yyval.stmt) = new while_stmt((yyvsp[-2].expression),(yyvsp[0].stmt));
    (yyval.stmt)->SetLineNumber(line_number);
}
#line 1914 "SysY_parser.tab.c"
    break;

  case 58: /* Stmt: IF '(' Cond ')' Stmt  */
#line 383 "parser/SysY_parser.y"
                                {
    (yyval.stmt) = new if_stmt((yyvsp[-2].expression),(yyvsp[0].stmt));
    (yyval.stmt)->SetLineNumber(line_number);
}
#line 1923 "SysY_parser.tab.c"
    break;

  case 59: /* Stmt: IF '(' Cond ')' Stmt ELSE Stmt  */
#line 387 "parser/SysY_parser.y"
                               {
    (yyval.stmt) = new ifelse_stmt((yyvsp[-4].expression),(yyvsp[-2].stmt),(yyvsp[0].stmt));
    (yyval.stmt)->SetLineNumber(line_number);
}
#line 1932 "SysY_parser.tab.c"
    break;

  case 60: /* Stmt: BREAK ';'  */
#line 391 "parser/SysY_parser.y"
          {
    (yyval.stmt) = new break_stmt();
    (yyval.stmt)->SetLineNumber(line_number);
}
#line 1941 "SysY_parser.tab.c"
    break;

  case 61: /* Stmt: CONTINUE ';'  */
#line 395 "parser/SysY_parser.y"
             {
    (yyval.stmt) = new continue_stmt();
    (yyval.stmt)->SetLineNumber(line_number);
}
#line 1950 "SysY_parser.tab.c"
    break;

  case 62: /* Stmt: RETURN Exp ';'  */
#line 399 "parser/SysY_parser.y"
               {
    (yyval.stmt) = new return_stmt((yyvsp[-1].expression));
    (yyval.stmt)->SetLineNumber(line_number);
}
#line 1959 "SysY_parser.tab.c"
    break;

  case 63: /* Stmt: RETURN ';'  */
#line 403 "parser/SysY_parser.y"
           {
    (yyval.stmt) = new return_stmt_void();
    (yyval.stmt)->SetLineNumber(line_number);
}
#line 1968 "SysY_parser.tab.c"
    break;

  case 64: /* Exp: AddExp  */
#line 411 "parser/SysY_parser.y"
       {(yyval.expression) = (yyvsp[0].expression); (yyval.expression)->SetLineNumber(line_number);}
#line 1974 "SysY_parser.tab.c"
    break;

  case 65: /* Cond: LOrExp  */
#line 415 "parser/SysY_parser.y"
       {(yyval.expression) = (yyvsp[0].expression); (yyval.expression)->SetLineNumber(line_number);}
#line 1980 "SysY_parser.tab.c"
    break;

  case 66: /* Lval: IDENT  */
#line 420 "parser/SysY_parser.y"
      {
    (yyval.expression)=new Lval((yyvsp[0].symbol_token),nullptr);
    (yyval.expression)->SetLineNumber(line_number);
}
#line 1989 "SysY_parser.tab.c"
    break;

  case 67: /* Lval: IDENT Array_list  */
#line 424 "parser/SysY_parser.y"
                 {
    (yyval.expression)=new Lval((yyvsp[-1].symbol_token),(yyvsp[0].expressions));
    (yyval.expression)->SetLineNumber(line_number);
}
#line 1998 "SysY_parser.tab.c"
    break;

  case 68: /* PrimaryExp: '(' Exp ')'  */
#line 432 "parser/SysY_parser.y"
           {
    (yyval.expression)=new PrimaryExp_branch((yyvsp[-1].expression));
     (yyval.expression)->SetLineNumber(line_number);
}
#line 2007 "SysY_parser.tab.c"
    break;

  case 69: /* PrimaryExp: Lval  */
#line 436 "parser/SysY_parser.y"
     {
    (yyval.expression) = (yyvsp[0].expression); 
    (yyval.expression)->SetLineNumber(line_number);
}
#line 2016 "SysY_parser.tab.c"
    break;

  case 70: /* PrimaryExp: FloatConst  */
#line 440 "parser/SysY_parser.y"
           {
    (yyval.expression) = (yyvsp[0].expression); 
    (yyval.expression)->SetLineNumber(line_number);
}
#line 2025 "SysY_parser.tab.c"
    break;

  case 71: /* PrimaryExp: IntConst  */
#line 444 "parser/SysY_parser.y"
         {
    (yyval.expression) = (yyvsp[0].expression); 
    (yyval.expression)->SetLineNumber(line_number);
}
#line 2034 "SysY_parser.tab.c"
    break;

  case 72: /* IntConst: INT_CONST  */
#line 453 "parser/SysY_parser.y"
          {
    (yyval.expression) = new IntConst((yyvsp[0].int_token));
    (yyval.expression)->SetLineNumber(line_number);
}
#line 2043 "SysY_parser.tab.c"
    break;

  case 73: /* FloatConst: FLOAT_CONST  */
#line 460 "parser/SysY_parser.y"
            {
    (yyval.expression) = new FloatConst((yyvsp[0].float_token));
    (yyval.expression)->SetLineNumber(line_number);
}
#line 2052 "SysY_parser.tab.c"
    break;

  case 74: /* UnaryExp: PrimaryExp  */
#line 468 "parser/SysY_parser.y"
           {(yyval.expression) = (yyvsp[0].expression);}
#line 2058 "SysY_parser.tab.c"
    break;

  case 75: /* UnaryExp: IDENT '(' FuncRParams ')'  */
#line 469 "parser/SysY_parser.y"
                          {
    (yyval.expression) = new Func_call((yyvsp[-3].symbol_token),(yyvsp[-1].expression));
    (yyval.expression)->SetLineNumber(line_number);
}
#line 2067 "SysY_parser.tab.c"
    break;

  case 76: /* UnaryExp: IDENT '(' ')'  */
#line 473 "parser/SysY_parser.y"
              {
    // 在sylib.h这个文件中,starttime()是一个宏定义
    // #define starttime() _sysy_starttime(__LINE__)
    // 我们在语法分析中将其替换为_sysy_starttime(line_number)
    // stoptime同理
    if((yyvsp[-2].symbol_token)->get_string() == "starttime"){
        auto params = new std::vector<Expression>;
        params->push_back(new IntConst(line_number));
        Expression temp = new FuncRParams(params);
        (yyval.expression) = new Func_call(id_table.add_id("_sysy_starttime"),temp);
        (yyval.expression)->SetLineNumber(line_number);
    }
    else if((yyvsp[-2].symbol_token)->get_string() == "stoptime"){
        auto params = new std::vector<Expression>;
        params->push_back(new IntConst(line_number));
        Expression temp = new FuncRParams(params);
        (yyval.expression) = new Func_call(id_table.add_id("_sysy_stoptime"),temp);
        (yyval.expression)->SetLineNumber(line_number);
    }
    else{
        (yyval.expression) = new Func_call((yyvsp[-2].symbol_token),nullptr);
        (yyval.expression)->SetLineNumber(line_number);
    }
}
#line 2096 "SysY_parser.tab.c"
    break;

  case 77: /* UnaryExp: '+' UnaryExp  */
#line 497 "parser/SysY_parser.y"
             {
    (yyval.expression) = new UnaryExp_plus((yyvsp[0].expression));
    (yyval.expression)->SetLineNumber(line_number);
}
#line 2105 "SysY_parser.tab.c"
    break;

  case 78: /* UnaryExp: '-' UnaryExp  */
#line 501 "parser/SysY_parser.y"
             {
    (yyval.expression) = new UnaryExp_neg((yyvsp[0].expression));
    (yyval.expression)->SetLineNumber(line_number);
}
#line 2114 "SysY_parser.tab.c"
    break;

  case 79: /* UnaryExp: '!' UnaryExp  */
#line 505 "parser/SysY_parser.y"
             {
    (yyval.expression) = new UnaryExp_not((yyvsp[0].expression));
    (yyval.expression)->SetLineNumber(line_number);
}
#line 2123 "SysY_parser.tab.c"
    break;

  case 80: /* FuncRParams: Exp_list  */
#line 512 "parser/SysY_parser.y"
         {
    (yyval.expression) = new FuncRParams((yyvsp[0].expressions));
    (yyval.expression)->SetLineNumber(line_number);
}
#line 2132 "SysY_parser.tab.c"
    break;

  case 81: /* Exp_list: Exp  */
#line 520 "parser/SysY_parser.y"
    {
    (yyval.expressions) = new std::vector<Expression>;
    ((yyval.expressions))->push_back((yyvsp[0].expression));
}
#line 2141 "SysY_parser.tab.c"
    break;

  case 82: /* Exp_list: Exp_list ',' Exp  */
#line 524 "parser/SysY_parser.y"
                 {
    ((yyvsp[-2].expressions))->push_back((yyvsp[0].expression));
    (yyval.expressions) = (yyvsp[-2].expressions);
}
#line 2150 "SysY_parser.tab.c"
    break;

  case 83: /* MulExp: UnaryExp  */
#line 532 "parser/SysY_parser.y"
         {(yyval.expression) = (yyvsp[0].expression);(yyval.expression)->SetLineNumber(line_number);}
#line 2156 "SysY_parser.tab.c"
    break;

  case 84: /* MulExp: MulExp '*' UnaryExp  */
#line 533 "parser/SysY_parser.y"
                    {
    (yyval.expression) = new MulExp_mul((yyvsp[-2].expression),(yyvsp[0].expression)); 
    (yyval.expression)->SetLineNumber(line_number);
    }
#line 2165 "SysY_parser.tab.c"
    break;

  case 85: /* MulExp: MulExp '/' UnaryExp  */
#line 537 "parser/SysY_parser.y"
                    {
    (yyval.expression) = new MulExp_div((yyvsp[-2].expression),(yyvsp[0].expression)); 
    (yyval.expression)->SetLineNumber(line_number);
    }
#line 2174 "SysY_parser.tab.c"
    break;

  case 86: /* MulExp: MulExp '%' UnaryExp  */
#line 541 "parser/SysY_parser.y"
                    {
    (yyval.expression) = new MulExp_mod((yyvsp[-2].expression),(yyvsp[0].expression)); 
    (yyval.expression)->SetLineNumber(line_number);
    }
#line 2183 "SysY_parser.tab.c"
    break;

  case 87: /* AddExp: MulExp  */
#line 551 "parser/SysY_parser.y"
       {
    (yyval.expression) = (yyvsp[0].expression);
    (yyval.expression)->SetLineNumber(line_number);
}
#line 2192 "SysY_parser.tab.c"
    break;

  case 88: /* AddExp: AddExp '+' MulExp  */
#line 555 "parser/SysY_parser.y"
                  {
    (yyval.expression) = new AddExp_plus((yyvsp[-2].expression),(yyvsp[0].expression)); 
    (yyval.expression)->SetLineNumber(line_number);
}
#line 2201 "SysY_parser.tab.c"
    break;

  case 89: /* AddExp: AddExp '-' MulExp  */
#line 559 "parser/SysY_parser.y"
                  {
    (yyval.expression) = new AddExp_sub((yyvsp[-2].expression),(yyvsp[0].expression)); 
    (yyval.expression)->SetLineNumber(line_number);
}
#line 2210 "SysY_parser.tab.c"
    break;

  case 90: /* RelExp: AddExp  */
#line 566 "parser/SysY_parser.y"
       {
    (yyval.expression) = (yyvsp[0].expression);
    (yyval.expression)->SetLineNumber(line_number);
}
#line 2219 "SysY_parser.tab.c"
    break;

  case 91: /* RelExp: RelExp LEQ AddExp  */
#line 570 "parser/SysY_parser.y"
                  {
    (yyval.expression)=new RelExp_leq((yyvsp[-2].expression),(yyvsp[0].expression));
    (yyval.expression)->SetLineNumber(line_number);
}
#line 2228 "SysY_parser.tab.c"
    break;

  case 92: /* RelExp: RelExp '<' AddExp  */
#line 574 "parser/SysY_parser.y"
                  {
    (yyval.expression)=new RelExp_lt((yyvsp[-2].expression),(yyvsp[0].expression));
    (yyval.expression)->SetLineNumber(line_number);
}
#line 2237 "SysY_parser.tab.c"
    break;

  case 93: /* RelExp: RelExp GEQ AddExp  */
#line 578 "parser/SysY_parser.y"
                   {
    (yyval.expression)=new RelExp_geq((yyvsp[-2].expression),(yyvsp[0].expression));
    (yyval.expression)->SetLineNumber(line_number);
}
#line 2246 "SysY_parser.tab.c"
    break;

  case 94: /* RelExp: RelExp '>' AddExp  */
#line 582 "parser/SysY_parser.y"
                  {
    (yyval.expression)=new RelExp_gt((yyvsp[-2].expression),(yyvsp[0].expression));
    (yyval.expression)->SetLineNumber(line_number);
}
#line 2255 "SysY_parser.tab.c"
    break;

  case 95: /* EqExp: RelExp  */
#line 590 "parser/SysY_parser.y"
       {(yyval.expression) = (yyvsp[0].expression);(yyval.expression)->SetLineNumber(line_number);}
#line 2261 "SysY_parser.tab.c"
    break;

  case 96: /* EqExp: EqExp EQ RelExp  */
#line 591 "parser/SysY_parser.y"
                {
    (yyval.expression)=new EqExp_eq((yyvsp[-2].expression),(yyvsp[0].expression));
    (yyval.expression)->SetLineNumber(line_number);
}
#line 2270 "SysY_parser.tab.c"
    break;

  case 97: /* EqExp: EqExp NE RelExp  */
#line 595 "parser/SysY_parser.y"
                {
    (yyval.expression)=new EqExp_neq((yyvsp[-2].expression),(yyvsp[0].expression));
    (yyval.expression)->SetLineNumber(line_number);
}
#line 2279 "SysY_parser.tab.c"
    break;

  case 98: /* LAndExp: EqExp  */
#line 603 "parser/SysY_parser.y"
      {(yyval.expression) = (yyvsp[0].expression);(yyval.expression)->SetLineNumber(line_number);}
#line 2285 "SysY_parser.tab.c"
    break;

  case 99: /* LAndExp: LAndExp AND EqExp  */
#line 604 "parser/SysY_parser.y"
                  {
    (yyval.expression)=new LAndExp_and((yyvsp[-2].expression),(yyvsp[0].expression));
    (yyval.expression)->SetLineNumber(line_number);
}
#line 2294 "SysY_parser.tab.c"
    break;

  case 100: /* LOrExp: LAndExp  */
#line 612 "parser/SysY_parser.y"
        {(yyval.expression) = (yyvsp[0].expression);(yyval.expression)->SetLineNumber(line_number);}
#line 2300 "SysY_parser.tab.c"
    break;

  case 101: /* LOrExp: LOrExp OR LAndExp  */
#line 613 "parser/SysY_parser.y"
                  {(yyval.expression) = new LOrExp_or((yyvsp[-2].expression),(yyvsp[0].expression)); (yyval.expression)->SetLineNumber(line_number);}
#line 2306 "SysY_parser.tab.c"
    break;

  case 102: /* ConstExp: AddExp  */
#line 619 "parser/SysY_parser.y"
       {(yyval.expression) = (yyvsp[0].expression);(yyval.expression)->SetLineNumber(line_number);}
#line 2312 "SysY_parser.tab.c"
    break;

  case 103: /* Array: '[' Exp ']'  */
#line 625 "parser/SysY_parser.y"
            {(yyval.expression) = (yyvsp[-1].expression);(yyval.expression)->SetLineNumber(line_number);}
#line 2318 "SysY_parser.tab.c"
    break;

  case 104: /* Array_list: Array  */
#line 630 "parser/SysY_parser.y"
{
    (yyval.expressions) = new std::vector<Expression>;
    ((yyval.expressions))->push_back((yyvsp[0].expression));
}
#line 2327 "SysY_parser.tab.c"
    break;

  case 105: /* Array_list: Array_list Array  */
#line 635 "parser/SysY_parser.y"
{
    ((yyvsp[-1].expressions))->push_back((yyvsp[0].expression));
    (yyval.expressions) = (yyvsp[-1].expressions);
}
#line 2336 "SysY_parser.tab.c"
    break;

  case 106: /* ConstArray: '[' ConstExp ']'  */
#line 642 "parser/SysY_parser.y"
                 {(yyval.expression) = (yyvsp[-1].expression);(yyval.expression)->SetLineNumber(line_number);}
#line 2342 "SysY_parser.tab.c"
    break;

  case 107: /* ConstArray_list: ConstArray  */
#line 647 "parser/SysY_parser.y"
{
    (yyval.expressions) = new std::vector<Expression>;
    ((yyval.expressions))->push_back((yyvsp[0].expression));
}
#line 2351 "SysY_parser.tab.c"
    break;

  case 108: /* ConstArray_list: ConstArray_list ConstArray  */
#line 652 "parser/SysY_parser.y"
{
    ((yyvsp[-1].expressions))->push_back((yyvsp[0].expression));
    (yyval.expressions) = (yyvsp[-1].expressions);
}
#line 2360 "SysY_parser.tab.c"
    break;


#line 2364 "SysY_parser.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 660 "parser/SysY_parser.y"
 

void yyerror(char* s, ...)
{
    ++error_num;
    fout<<"parser error in line "<<line_number<<"\n";
}

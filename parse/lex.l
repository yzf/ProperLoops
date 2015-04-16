%{
#include "parse.h"

extern "C" {
	int yywrap(void);
	int yylex(void);
}
extern int context_flag;/* 0 - not in any atom, 1 - in an atom*/

%}

blank	  			[ \n\t\r]*
atom                            [a-zA-Z0-9_]+
comment                         %[^\n]*[\n]
%%
{atom}                                  {
                                            yylval.s=strdup(yytext); 
                                            if(strcmp(yytext, "not") == 0)
                                                return NEGA;
                                            return ATOM;
                                        }
":-"                                    {return IMPLY;			}
"("					{return LPAREN;			}
")"					{return RPAREN;			}
","					{return COMMA;			}
"."                                     {return PERIOD;                 }
"|"                                     {return DIS;                    }
{blank}		    	{/* skip blankspace */	}
{comment}               {   }
%%

int yywrap() {
  return 1;
}

%{
#include "parse.h"

extern "C" {
	int yywrap(void);
	int yylex(void);
}

%}

blank	  			[ \n\t\r]*
atom                            [a-zA-Z0-9_]+
comment                         %[^\n]*[\n]
%%
{atom}                                  {
                                            if(strcmp(yytext, "not") == 0)
                                                return NEGA;
                                            yylval.s=strdup(yytext);
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

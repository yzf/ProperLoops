%{
#include <assert.h>
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>
#include "structs.h"
#include "rule.h"
#include "vocabulary.h"

using namespace std;

extern "C" {
    void yyerror(const char *s);
    extern int yylex(void);
}
 
extern vector<Rule> g_rules;
extern Vocabulary g_vocabulary;

int id;

void yyerror(const char* s) {
    printf("Parser error: %s\n", s);
}

%}

%union {
    char* s;
    int i;
    struct _RuleHelper* r;
    struct _HeadHelper* h;
    struct _BodyHelper* b;
}

%token <s> ATOM
%token <s> NEGA
%token <s> IMPLY
%token <s> LPAREN
%token <s> RPAREN
%token <s> COMMA
%token <s> PERIOD
%token <s> DIS

%type <s> term terms
%type <i> literal atom
%type <r> rule
%type <b> conjunction
%type <h> disjunction

%left IMPLY

%%
dlp 
    : rules {
        //printf("dlp\n");
    }
    |
;

rules
    : rules rule {
        Rule rule($2);
        g_rules.push_back(rule);
    }
    | rule {
        Rule rule($1);
        g_rules.push_back(rule);
    }
;

rule 
    : disjunction PERIOD {//事实
        $$ = new RuleHelper();
        $$->type = kFact;
        $$->head_length = $1->length;
        for(int i = 0; i < ($1->length); ++ i) {
            $$->head[i] = $1->atoms[i];
        }
        $$->body_length = 0;
    }
    | IMPLY conjunction PERIOD {//约束
        $$ = new RuleHelper();
        $$->type = kConstrant;
        $$->head_length = 0;
        $$->body_length = $2->length;
        for(int i = 0; i < ($2->length); ++ i) {
            $$->body[i] = $2->atoms[i];
        }
    }
    | disjunction IMPLY conjunction PERIOD {//规则
        $$ = new RuleHelper();
        $$->type = kRule;
        $$->head_length = $1->length;
        for(int i = 0; i < ($1->length); ++ i) {
            $$->head[i] = $1->atoms[i];
        }
        $$->body_length = $3->length;
        for(int i = 0; i < ($3->length); ++ i) {
            $$->body[i] = $3->atoms[i];
        }
    }
;

disjunction
    : disjunction DIS atom {
        $1->atoms[$1->length] = $3;
        $1->length ++;
    }
    | atom {
        $$ = new HeadHelper();
        $$->atoms[0] = $1;
        $$->length = 1;
    }

conjunction
    : conjunction COMMA literal {
        $1->atoms[$1->length] = $3;
        $1->length ++;
    }
    | literal {
        $$ = new BodyHelper();
        $$->atoms[0] = $1;
        $$->length = 1;
    }
;

literal
    : NEGA atom {
        $$ = -1 * $2;
    }
    | atom {
        $$ = $1;
    }
;

atom
    : ATOM LPAREN terms RPAREN {
        char str_buff[512];
        sprintf(str_buff, "%s(%s)", $1, $3);
        string atom_name = str_buff;
        int id = g_vocabulary.GetAtomId(atom_name);
        if(id == 0)
            id = g_vocabulary.AddAtom(atom_name);
        $$ = id;
    } 
    | ATOM {
        string atom_name = $1;
        int id = g_vocabulary.GetAtomId(atom_name);
        if(id == 0)
            id = g_vocabulary.AddAtom(atom_name);
        $$ = id;
    }
;

terms
    : terms COMMA term {
        char str_buff[512];
        sprintf(str_buff, "%s,%s", $1, $3);
        $$ = strdup(str_buff);
    }
    | term {
        $$ = strdup($1);
    }
;

term
    : ATOM {
        $$ = strdup($1);
    }
;
%%

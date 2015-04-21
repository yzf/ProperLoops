/*
 * File:   main.cc
 * Author: yzf
 *
 * Created on April 16, 2015, 5:29 PM
 */
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <iostream>
#include "structs.h"
#include "rule.h"
#include "graph.h"
#include "util.h"
#include "loop.h"
#include "program.h"
#include "algorithm.h"
#include "hash.h"

using namespace std;

extern FILE* yyin;
extern RuleSet g_rules;
extern int yyparse();
/*
 *
 */
int main(int argc, char** argv) {
    yyin = fopen("res/sample.in", "r");
    if (NULL == yyin) {
        printf("open file failed.");
        exit(1);
    }
    yyparse();

    Program program(g_rules);
    LoopSet elementary_loops = ElementaryLoops(program);
    OutputLoops(stdout, elementary_loops);
    FreeLoops(elementary_loops);
    
    LoopSet proper_loops = ProperLoops(program);
    OutputLoops(stdout, proper_loops);
    FreeLoops(proper_loops);

    return 0;
}


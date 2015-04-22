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
    yyin = fopen("res/2_5_1", "r");
    if (NULL == yyin) {
        printf("open file failed.");
        exit(1);
    }
    yyparse();

    Program program(g_rules);
    
    LoopSet elementary_loops = ElementaryLoops(program);
//    OutputLoops(stdout, elementary_loops);
    printf("elementary loop: %lu\n", elementary_loops.size());
    FreeLoops(elementary_loops);
    
    LoopSet proper_loops = ProperLoops(program, program.GetAtoms());
//    OutputLoops(stdout, proper_loops);
    printf("proper loop: %lu\n", proper_loops.size());
    FreeLoops(proper_loops);

    return 0;
}


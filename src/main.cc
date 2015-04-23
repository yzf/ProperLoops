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
    clock_t t_begin, t_end;
    
    t_begin = clock();
    LoopSet loops = AllLoops(program);
    t_end = clock();
    printf("all loops: %lu\ntimes: %f\n\n", loops.size(), (float)(t_end - t_begin)/1000000);
    FreeLoops(loops);
    
    t_begin = clock();
    LoopSet elementary_loops = ElementaryLoops(program);
    t_end = clock();
//    OutputLoops(stdout, elementary_loops);
    printf("elementary loop: %lu\ntimes: %f\n\n", elementary_loops.size(), (float)(t_end - t_begin)/1000000);
    FreeLoops(elementary_loops);

    t_begin = clock();
    LoopSet proper_loops = ProperLoops(program, program.GetAtoms());
    t_end = clock();
//    OutputLoops(stdout, proper_loops);
    printf("proper loop: %lu\ntimes: %f\n\n", proper_loops.size(), (float)(t_end - t_begin)/1000000);
    FreeLoops(proper_loops);

    return 0;
}


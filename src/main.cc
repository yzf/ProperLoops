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
    AtomSet all, s;
    all.insert(1);
    all.insert(2);
    all.insert(3);
    s.insert(1);
    s.insert(2);
    s.insert(3);
    Loop loop(s, &program);
    
    printf("program:\n");
    program.Output(stdout);
    
    printf("loop:");
    loop.Output(stdout);
    
    cout << ElementaryLoopStar(loop, program) << endl;
    
    return 0;
}


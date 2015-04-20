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
    AtomIdSet all;
    all.insert(1);
    all.insert(2);
    all.insert(3);
    cout << ElementaryLoopStar(all, g_rules) << endl;
    return 0;
}


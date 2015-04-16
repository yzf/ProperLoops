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
#include "dlp.h"
#include "graph.h"

using namespace std;

extern FILE* yyin;
extern vector<Rule> g_rules;
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
    DLP dlp = DLP(g_rules);
    dlp.Output(stdout);
    cout << endl;
    Graph graph = GetDependencyGraph(dlp.rules_);
    OutputGraph(stdout, graph);
    cout << endl;
    AtomIdSet s;
    s.insert(3);
    Graph sub1 = GetInducedSubgraph(s, graph);
    OutputGraph(stdout, sub1);
    cout << endl;
    RuleIdSet external_support = GetExternalSupport(s, dlp.rules_);
    OutputExternalSupport(stdout, external_support, dlp.rules_);
    cout << endl;
    AtomIdSet all;
    all.insert(1);
    all.insert(2);
    all.insert(3);
    RuleIdSet external_support_with = GetExternalSupportWithConstrant(s, all, dlp.rules_);
    OutputExternalSupport(stdout, external_support_with, dlp.rules_);
    cout << endl;
    return 0;
}


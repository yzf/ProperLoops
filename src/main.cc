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
#include "hash.h"

using namespace std;
//三个宏分别开启不同的实验的主函数，每次只能打开一个
//#define EL_PL_CMP     //实验一：对比正规逻辑程序的elementary loop和proper loop
#define LOOP_CMP      //实验二：对比析取逻辑程序的各种环
//#define HWEF_STAR       //实验三：检测输入文件是否为HWEF*程序

extern FILE* yyin;
extern RuleSet g_rules;
extern int yyparse();
/*
 *
 */
int main(int argc, char** argv) {
#ifdef EL_PL_CMP
    yyin = fopen("res/2_5_1", "r");
    if (NULL == yyin) {
        printf("open file failed.");
        exit(1);
    }
    yyparse();
    fclose(yyin);

    Program program(g_rules);
    clock_t t_begin, t_end;
    
    t_begin = clock();
    LoopSet elementary_loops = program.GetElementaryLoops();
    t_end = clock();
    printf("elementary loop: %lu\ntimes: %f\n\n", elementary_loops.size(),
            (float)(t_end - t_begin)/1000000);
    FreeLoops(elementary_loops);

    t_begin = clock();
    LoopSet proper_loops = program.GetProperLoops(program.GetAtoms());
    t_end = clock();
    printf("proper loop: %lu\ntimes: %f\n\n", proper_loops.size(),
            (float)(t_end - t_begin)/1000000);
    FreeLoops(proper_loops);
#endif
#ifdef LOOP_CMP
#endif
#ifdef HWEF_STAR
    yyin = fopen("res/hwefstar", "r");
    if (NULL == yyin) {
        printf("open file failed.");
        exit(1);
    }
    yyparse();
    fclose(yyin);

    Program program(g_rules);
    if (program.IsHeadElementaryLoopFreeStar()) {
        printf("HWEF* program\n");
    }
    else {
        printf("Not HWEF* program\n");
    }
#endif
    return 0;
}


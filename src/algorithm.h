/* 
 * File:   algorithm.h
 * Author: yzf
 *
 * Created on April 20, 2015, 5:21 PM
 */

#ifndef ALGORITHM_H
#define	ALGORITHM_H

#include "structs.h"

class Loop;
class Program;
/*
 * 论文相关的算法
 */
bool ElementaryLoopStar(const Loop&, const Program&);
LoopSet ElementaryLoops(const Program&);
bool ProperLoopStar(const Loop&, const Program&);
LoopSet ProperLoops(const Program&);


#endif	/* ALGORITHM_H */


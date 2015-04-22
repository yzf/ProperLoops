/* 
 * File:   util.h
 * Author: yzf
 *
 * Created on April 20, 2015, 6:09 PM
 */

#ifndef UTIL_H
#define	UTIL_H

#include <cstdio>
#include "structs.h"

/*
 * 各种辅助函数
 */
AtomSet HeadOfRules(const RuleSet&);
RelationType RelationBetween(const AtomSet&, const AtomSet&, AtomSet&);
RelationType RelationBetween(const RuleSet&, const RuleSet&, RuleSet&);

void FreeRules(RuleSet rules);
void FreeLoops(LoopSet loops);
/*
 * 输出函数
 */
void OutputRules(FILE*, const RuleSet&);
void OutputAtoms(FILE*, const AtomSet&);
void OutputLoops(FILE*, const LoopSet&);

#endif	/* UTIL_H */


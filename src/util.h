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
AtomIdSet HeadOfRules(const RuleSet&);
/*
 * 输出函数
 */
void OutputGraph(FILE*, const Graph&);
void OutputExternalSupport(FILE*, const RuleSet&);
void OutputAtoms(FILE*, const AtomIdSet&);
void OutputSccs(FILE*, const SccSet&);

#endif	/* UTIL_H */


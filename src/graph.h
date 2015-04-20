/* 
 * File:   graph.h
 * Author: yzf
 *
 * Created on April 16, 2015, 11:11 PM
 */

#ifndef GRAPH_H
#define	GRAPH_H

#include <cstdio>
#include "structs.h"

/*
 * 与图相关的一些函数
 */

Graph GetDependencyGraph(const RuleSet&);
Graph GetInducedSubgraph(const AtomIdSet&, const Graph&);
RuleSet GetExternalSupport(const AtomIdSet&, const RuleSet&);
RuleSet GetExternalSupportWithConstrant(const AtomIdSet&, const AtomIdSet&, const RuleSet&);
void Tarjan(const int&, const Graph&, SccSet&);
SccSet GetSccsOfGraph(const Graph&);


#endif	/* GRAPH_H */


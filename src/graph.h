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
#include "dlp.h"

/*
 * 与图相关的一些函数
 */

Graph GetDependencyGraph(const RuleSet&);
Graph GetInducedSubgraph(const AtomIdSet&, const Graph&);
RuleIdSet GetExternalSupport(const AtomIdSet&, const RuleSet&);
RuleIdSet GetExternalSupportWithConstrant(const AtomIdSet&, const AtomIdSet&, const RuleSet&);
void OutputGraph(FILE*, const Graph&);
void OutputExternalSupport(FILE*, const RuleIdSet&, const RuleSet&);

#endif	/* GRAPH_H */


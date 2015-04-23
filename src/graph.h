/* 
 * File:   graph.h
 * Author: yzf
 *
 * Created on April 16, 2015, 11:11 PM
 */

#ifndef GRAPH_H
#define	GRAPH_H

#include <cstdio>
#include <vector>
#include "structs.h"

class Program;
class Hash;

class Graph {
public:
    explicit Graph(const Program*);
    Graph* GetInducedSubgraph(const AtomSet&) const;
    LoopSet GetSccs() const;
    void ExtendSccsFromInducedSubgraph(const Loop*, Hash&, LoopSet&, LoopSet&) const;
    void ExtendSccsFromInducedSubgraph(const AtomSet&, Hash&, LoopSet&, LoopSet&) const;
    void ExtendSccsFromInducedSubgraph(const Loop*, const AtomSet&, Hash&, LoopSet&, LoopSet&) const;
    void Output(FILE* out) const;
private:
    void Tarjan(const int&, LoopSet&) const;
public:
    EdgeMap edge_map_;
    const Program* program_;
private:
    mutable std::vector<int> pre_, low_, s_;
    mutable int stop_, cnt_;
};


#endif	/* GRAPH_H */


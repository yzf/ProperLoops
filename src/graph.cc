#include <cassert>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include "rule.h"
#include "graph.h"
#include "vocabulary.h"
#include "loop.h"

using namespace std;

extern Vocabulary g_vocabulary;

Graph::Graph(const Program* program) : program_(program) {
}
/*
 * 求graph关于atoms的诱导子图
 */
Graph* Graph::GetInducedSubgraph(const AtomSet& atoms) const {
    Graph* subgraph = new Graph(program_);
    for (EdgeMap::const_iterator it = edge_map_.begin(); it != edge_map_.end(); ++ it) {
        int from = it->first;
        if (atoms.find(from) != atoms.end()) {
            subgraph->edge_map_.insert(make_pair(from, AtomSet()));
            for (AtomSet::const_iterator it_to = it->second.begin();
                    it_to != it->second.end(); ++ it_to) {
                int to = *it_to;
                if (atoms.find(to) != atoms.end())
                    subgraph->edge_map_[from].insert(to);
            }
        }
    }
    return subgraph;
}

/*
 * tarjan算法
 */
void Graph::Tarjan(const int& v, LoopSet& result) const {
    int minc = low_[v] = pre_[v] = cnt_ ++;
    AtomSet::const_iterator pv;
    EdgeMap::const_iterator it = edge_map_.find(v);
    
    s_[stop_ ++] = v;
    assert(it != edge_map_.end());
    for (pv = it->second.begin(); pv != it->second.end(); ++ pv) {
        if (-1 == pre_[*pv])
            Tarjan(*pv, result);
        if (low_[*pv] < minc)
            minc = low_[*pv];
    }
    if (minc < low_[v]) {
        low_[v] = minc;
        return ;
    }
    
    int t;
    Loop new_scc(AtomSet(), program_);
    do {
        t = s_[-- stop_];
        low_[t] = edge_map_.rbegin()->first + 1;
        new_scc.atoms_.insert(t);
    } while (t != v);
    result.push_back(new_scc);
}
/*
 * 求graph的强连通分量
 */
LoopSet Graph::GetSccs() const {
    LoopSet ret;
    if (edge_map_.empty())
        return ret;
    stop_ = cnt_ = 0;
    pre_.clear();
    low_.clear();
    s_.clear();
    for (size_t i = 0; i <= edge_map_.rbegin()->first; ++ i) {
        pre_.push_back(-1);
        low_.push_back(0);
        s_.push_back(0);
    }
    for (EdgeMap::const_iterator it = edge_map_.begin(); it != edge_map_.end(); ++ it) {
        int v = it->first;
        if (-1 == pre_[v])
            Tarjan(v, ret);
    }
    return ret;
}

void Graph::Output(FILE* out) const {
    for (EdgeMap::const_iterator it = edge_map_.begin();
            it != edge_map_.end(); ++ it) {
        int from = it->first;
        for (AtomSet::const_iterator it_to = it->second.begin();
                it_to != it->second.end(); ++ it_to) {
            int to = *it_to;
            fprintf(out, "%s -> %s\n", g_vocabulary.GetAtom(from).c_str(),
                    g_vocabulary.GetAtom(to).c_str());
        }
    }
    fprintf(out, "\n");
}

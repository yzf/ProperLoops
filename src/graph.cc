#include <cassert>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include "rule.h"
#include "graph.h"
#include "hash.h"
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
    AtomSet* scc_atoms = new AtomSet();
    do {
        t = s_[-- stop_];
        low_[t] = edge_map_.rbegin()->first + 1;
        scc_atoms->insert(t);
    } while (t != v);
    Loop* new_scc = new Loop(scc_atoms, program_);
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

void Graph::ExtendSccsFromInducedSubgraph(const Loop* c, Hash& hash, LoopSet& sccs, LoopSet& scc_checked) const {
    for (AtomSet::const_iterator i = c->atoms_->begin();
            i != c->atoms_->end(); ++ i) {
        const int& a = *i;
        AtomSet c_backslash_a = *(c->atoms_);
        c_backslash_a.erase(a);
        Graph* g_star = GetInducedSubgraph(c_backslash_a);//3+
        LoopSet scc_star = g_star->GetSccs();//4+
        while (! scc_star.empty()) {
            Loop* new_c = scc_star.front();
            if (! hash.HasLoop(new_c)) {
                sccs.push_back(new_c);
                hash.AddLoop(new_c);
            }
            else {
                scc_checked.push_back(new_c);
            }
            scc_star.pop_front();
        }
        delete g_star;//3-
    }
}

void Graph::ExtendSccsFromInducedSubgraph(const AtomSet& atoms, Hash& hash, LoopSet& sccs, LoopSet& scc_checked) const {
    Graph* g_c = GetInducedSubgraph(atoms);
    LoopSet scc_c = g_c->GetSccs();
    while (! scc_c.empty()) {
        Loop* new_c = scc_c.front();
        if (! hash.HasLoop(new_c)) {
            hash.AddLoop(new_c);
            sccs.push_back(new_c);
        }
        else {
            scc_checked.push_back(new_c);//该loop已经存在，但内存是新的，所以要放进scc_checked，统一销毁
        }
        scc_c.pop_front();
    }
    delete g_c;
}

void Graph::ExtendSccsFromInducedSubgraph(const Loop* c, const AtomSet& atoms,
        Hash& hash, LoopSet& sccs, LoopSet& scc_checked) const {
    for (AtomSet::const_iterator i = atoms.begin(); i != atoms.end(); ++ i) {
        const int& a = *i;
        AtomSet c_backslash_a = *(c->atoms_);
        c_backslash_a.erase(a);
        Graph* g_c = GetInducedSubgraph(c_backslash_a);//3+
        LoopSet scc_c = g_c->GetSccs();//4+
        while (! scc_c.empty()) {
            Loop* new_c = scc_c.front();
            if (! hash.HasLoop(new_c)) {
                hash.AddLoop(new_c);
                sccs.push_back(new_c);
            }
            else {
                scc_checked.push_back(new_c);
            }
            scc_c.pop_front();
        }
        delete g_c;//3-
    }
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

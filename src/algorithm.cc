#include <algorithm>
#include <map>
#include "algorithm.h"
#include "graph.h"
#include "util.h"
#include "program.h"
#include "loop.h"
#include "hash.h"

using namespace std;

/*
 * 判断loop是否为程序program的elementary loops
 */
bool ElementaryLoopStar(const Loop& loop, const Program& program) {
    Graph* g_p = program.GetDependencyGraph();
    RuleSet r_loop = loop.GetExternalSupport();

    for (AtomSet::const_iterator it_atom = loop.atoms_->begin();
            it_atom != loop.atoms_->end(); ++ it_atom) {
        const int& a = *it_atom;
        AtomSet l_backslash_a = *(loop.atoms_);
        l_backslash_a.erase(a);
        
        Graph* g_star = g_p->GetInducedSubgraph(l_backslash_a);
        LoopSet scc_star = g_star->GetSccs();

        Hash hash;
        hash.AddLoops(scc_star);
        while (! scc_star.empty()) {
            Loop* c = scc_star.front();
            RuleSet r_c_of_loop = c->GetExternalSupportWithConstrant(&loop);
            RuleSet diff;
            set_difference(r_c_of_loop.begin(), r_c_of_loop.end(), r_loop.begin(),
                    r_loop.end(), inserter(diff, diff.begin()));
            
            if (diff.empty()) {
                delete g_p;
                delete g_star;
                return false;
            }
            else {
                AtomSet head = HeadOfRules(diff);
                AtomSet c_backslash_head;
                set_difference(c->atoms_->begin(), c->atoms_->end(), head.begin(), head.end(),
                        inserter(c_backslash_head, c_backslash_head.begin()));
                
                Graph* g_c = g_star->GetInducedSubgraph(c_backslash_head);
                LoopSet scc_c = g_c->GetSccs();
                // append new element from scc_c to scc_star
                for (LoopSet::const_iterator it_scc = scc_c.begin();
                        it_scc != scc_c.end(); ++ it_scc) {
                    if (! hash.HasLoop(**it_scc)) {
                        scc_star.push_back(*it_scc);
                        hash.AddLoop(**it_scc);
                    }
                }
            }
            delete c;
            scc_star.pop_front();
        }
        delete g_star;
    }
    delete g_p;
    return true;
}
/*
 * 求program的所有elementary loops
 */
LoopSet ElementaryLoops(const Program& program) {
    LoopSet loops;
    Graph* g_p = program.GetDependencyGraph();
    LoopSet scc = g_p->GetSccs();
    Hash hash;
    hash.AddLoops(scc);
    while (! scc.empty()) {
        Loop* c = scc.front();
        bool is_elementary_loop = ElementaryLoopStar(*c, program);
        if (is_elementary_loop) {
            loops.push_back(c);
        }
        AtomSet c_backslash_a = *(c->atoms_);
        for (AtomSet::const_iterator i = c->atoms_->begin(); i != c->atoms_->end(); ++ i) {
            c_backslash_a.erase(*i);
            Graph* g_c = g_p->GetInducedSubgraph(c_backslash_a);
            LoopSet scc_c = g_c->GetSccs();
            for (LoopSet::const_iterator j = scc_c.begin(); j != scc_c.end(); ++ j) {
                if (! hash.HasLoop(**j)) {
                    scc.push_back(*j);
                    hash.AddLoop(**j);
                }
            }
            c_backslash_a.insert(*i);
        }
        if (! is_elementary_loop) {
            delete c;
        }
        scc.pop_front();
    }
    delete g_p;
    return loops;
}


bool ProperLoopStar(const Loop& loop, const Program& program) {
    Graph* g_p = program.GetDependencyGraph();
    LoopSet scc = g_p->GetSccs();
    const Loop* l = &loop;
    while (! scc.empty()) {
        Loop* c = scc.front();
        
        scc.pop_front();
    }
    delete g_p;
    return true;
}
LoopSet ProperLoops(const Program& program) {
    LoopSet loops;
    
    return loops;
}
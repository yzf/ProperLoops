#include <algorithm>
#include <map>
#include "algorithm.h"
#include "graph.h"
#include "util.h"
#include "program.h"
#include "loop.h"

using namespace std;

bool ElementaryLoopStar(const Loop& loop, const Program& program) {
    Graph* g_p = program.GetDependencyGraph();
    RuleSet r_loop = loop.GetExternalSupport();

    for (AtomSet::const_iterator it_atom = loop.atoms_.begin();
            it_atom != loop.atoms_.end(); ++ it_atom) {
        const int& a = *it_atom;
        AtomSet l_backslash_a = loop.atoms_;
        l_backslash_a.erase(a);
        
        Graph* g_star = g_p->GetInducedSubgraph(l_backslash_a);
        LoopSet scc_star = g_star->GetSccs();

        map< AtomSet, bool > is_in_queue;
        for (LoopSet::const_iterator it_scc = scc_star.begin(); it_scc != scc_star.end(); ++ it_scc) {
            is_in_queue[it_scc->atoms_] = true;
        }
        while (! scc_star.empty()) {
            const Loop& c = scc_star.front();
            RuleSet r_c_of_loop = c.GetExternalSupportWithConstrant(loop.atoms_);
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
                set_difference(c.atoms_.begin(), c.atoms_.end(), head.begin(), head.end(),
                        inserter(c_backslash_head, c_backslash_head.begin()));
                
                Graph* g_c = g_star->GetInducedSubgraph(c_backslash_head);
                LoopSet scc_c = g_c->GetSccs();
                // append new element from scc_c to scc_star
                for (LoopSet::const_iterator it_scc = scc_c.begin(); it_scc != scc_c.end(); ++ it_scc) {
                    if (is_in_queue[it_scc->atoms_] == false) {
                        scc_star.push_back(*it_scc);
                        is_in_queue[it_scc->atoms_] = true;
                    }
                }
            }
            scc_star.pop_front();
        }
        delete g_star;
    }
    delete g_p;
    return true;
}

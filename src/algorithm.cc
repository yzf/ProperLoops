#include <algorithm>
#include <map>
#include "algorithm.h"
#include "graph.h"
#include "util.h"

using namespace std;

bool ElementaryLoopStar(const Loop& loop, const RuleSet& rules) {
    Graph g_p = GetDependencyGraph(rules);
    RuleSet r_loop = GetExternalSupport(loop, rules);

    for (Loop::const_iterator it_loop = loop.begin(); it_loop != loop.end(); ++ it_loop) {
        const int& a = *it_loop;
        AtomIdSet l_backslash_a = loop;
        l_backslash_a.erase(a);
        
        Graph g_star = GetInducedSubgraph(l_backslash_a, g_p);
        SccSet scc_star = GetSccsOfGraph(g_star);

        map< Loop, bool > is_in_queue;
        for (SccSet::const_iterator it_scc = scc_star.begin(); it_scc != scc_star.end(); ++ it_scc) {
            is_in_queue[*it_scc] = true;
        }
        while (! scc_star.empty()) {
            const Scc& c = scc_star.front();
            RuleSet r_c_of_loop = GetExternalSupportWithConstrant(c, loop, rules);
            RuleSet diff;
            set_difference(r_c_of_loop.begin(), r_c_of_loop.end(), r_loop.begin(),
                    r_loop.end(), inserter(diff, diff.begin()));
            
            if (diff.empty()) {
                return false;
            }
            else {
                AtomIdSet head = HeadOfRules(diff);
                AtomIdSet c_backslash_head;
                set_difference(c.begin(), c.end(), head.begin(), head.end(),
                        inserter(c_backslash_head, c_backslash_head.begin()));
                
                Graph g_c = GetInducedSubgraph(c_backslash_head, g_star);
                SccSet scc_c = GetSccsOfGraph(g_c);
                // append new element from scc_c to scc_star
                for (SccSet::const_iterator it_scc = scc_c.begin(); it_scc != scc_c.end(); ++ it_scc) {
                    if (is_in_queue[*it_scc] == false) {
                        scc_star.push_back(*it_scc);
                        is_in_queue[*it_scc] = true;
                    }
                }
            }
            scc_star.pop_front();
        }           
    }
    return true;
}

#include <cassert>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include "rule.h"
#include "graph.h"
#include "vocabulary.h"

using namespace std;

static vector<int> g_pre, g_low, g_s;
static int g_stop, g_cnt;

extern Vocabulary g_vocabulary;
/*
 * 求程序的正依赖图
 */
Graph GetDependencyGraph(const RuleSet& rules) {
    Graph ret;
    for (size_t i = 1; i <= g_vocabulary.Size(); ++ i) {
        ret.insert(make_pair(i, AtomIdSet()));
    }
    for (size_t i = 0; i < rules.size(); ++ i) {
        const Rule* rule = rules[i];
        if (rule->type_ == kRule) {
            for (AtomIdSet::const_iterator it_head = rule->head_.begin();
                    it_head != rule->head_.end(); ++ it_head) {
                for (AtomIdSet::const_iterator it_body = rule->body_.begin();
                        it_body != rule->body_.end(); ++ it_body) {
                    if (*it_body > 0) {
                        ret[*it_head].insert(*it_body);
                    }
                }
            }
        }
    }
    return ret;
}
/*
 * 求graph关于atoms的诱导子图
 */
Graph GetInducedSubgraph(const AtomIdSet& atoms, const Graph& graph) {
    Graph ret;
    for (Graph::const_iterator it = graph.begin(); it != graph.end(); ++ it) {
        int from = it->first;
        if (atoms.find(from) != atoms.end()) {
            ret.insert(make_pair(from, AtomIdSet()));
            for (AtomIdSet::const_iterator it_to = it->second.begin();
                    it_to != it->second.end(); ++ it_to) {
                int to = *it_to;
                if (atoms.find(to) != atoms.end())
                    ret[from].insert(to);
            }
        }
    }
    return ret;
}
/*
 * R^-(x)
 */
RuleSet GetExternalSupport(const AtomIdSet& x, const RuleSet& rules) {
    RuleSet ret;
    for (size_t i = 0; i < rules.size(); ++ i) {
        Rule* rule = rules[i];
        AtomIdSet result;
        set_intersection(x.begin(), x.end(), rule->head_.begin(),
                rule->head_.end(), inserter(result, result.begin()));
        if (! result.empty()) {
            result.clear();
            set_intersection(x.begin(), x.end(), rule->body_.begin(),
                    rule->body_.end(), inserter(result, result.begin()));
            if (result.empty()) {
                ret.push_back(rule);
            }
        }
    }
    return ret;
}
/*
 * R^-_x(y)
 */
RuleSet GetExternalSupportWithConstrant(const AtomIdSet& y, const AtomIdSet& x, const RuleSet& rules) {
    RuleSet ret = GetExternalSupport(y, rules);
    AtomIdSet x_diff_y;
    set_difference(x.begin(), x.end(), y.begin(), y.end(), inserter(x_diff_y, x_diff_y.end()));
    int i = 0;
    while (i < ret.size()) {
        AtomIdSet& head = ret[i]->head_;
        AtomIdSet result;
        set_intersection(head.begin(), head.end(), x_diff_y.begin(), x_diff_y.end(),
                inserter(result, result.begin()));
        if (! result.empty()) {
            swap(ret[i], ret.back());
            ret.pop_back();
        }
        ++ i;
    }
    return ret;
}
/*
 * tarjan算法
 */
void Tarjan(const int& v, const Graph& graph, SccSet& result) {
    int minc = g_low[v] = g_pre[v] = g_cnt ++;
    AtomIdSet::const_iterator pv;
    Graph::const_iterator it = graph.find(v);
    
    g_s[g_stop ++] = v;
    assert(it != graph.end());
    for (pv = it->second.begin(); pv != it->second.end(); ++ pv) {
        if (-1 == g_pre[*pv])
            Tarjan(*pv, graph, result);
        if (g_low[*pv] < minc)
            minc = g_low[*pv];
    }
    if (minc < g_low[v]) {
        g_low[v] = minc;
        return ;
    }
    
    int t;
    Scc new_scc;
    do {
        t = g_s[-- g_stop];
        g_low[t] = graph.size() + 1;
        new_scc.insert(t);
    } while (t != v);
    result.push_back(new_scc);
}
/*
 * 求graph的强连通分量
 */
SccSet GetSccsOfGraph(const Graph& graph) {
    SccSet ret;
    if (graph.empty())
        return ret;
    g_stop = g_cnt = 0;
    g_pre.clear();
    g_low.clear();
    g_s.clear();
    for (size_t i = 0; i <= graph.rbegin()->first; ++ i) {
        g_pre.push_back(-1);
        g_low.push_back(0);
        g_s.push_back(0);
    }
    for (Graph::const_iterator it = graph.begin(); it != graph.end(); ++ it) {
        int v = it->first;
        if (-1 == g_pre[v])
            Tarjan(v, graph, ret);
    }
    return ret;
}


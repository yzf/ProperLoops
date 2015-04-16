#include <map>
#include <set>
#include <algorithm>
#include "rule.h"
#include "graph.h"
#include "vocabulary.h"

using namespace std;

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
        const Rule& rule = rules[i];
        if (rule.type_ == kRule) {
            for (AtomIdSet::const_iterator it_head = rule.head_.begin();
                    it_head != rule.head_.end(); ++ it_head) {
                for (AtomIdSet::const_iterator it_body = rule.body_.begin();
                        it_body != rule.body_.end(); ++ it_body) {
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
Graph GetInducedSubgraph(const LiteralIdSet& atoms, const Graph& graph) {
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
RuleIdSet GetExternalSupport(const AtomIdSet& x, const RuleSet& rules) {
    RuleIdSet ret;
    for (size_t i = 0; i < rules.size(); ++ i) {
        const Rule& rule = rules[i];
        AtomIdSet result;
        set_intersection(x.begin(), x.end(), rule.head_.begin(),
                rule.head_.end(), inserter(result, result.begin()));
        if (! result.empty()) {
            result.clear();
            set_intersection(x.begin(), x.end(), rule.body_.begin(),
                    rule.body_.end(), inserter(result, result.begin()));
            if (result.empty()) {
                ret.insert(i);
            }
        }
    }
    return ret;
}
/*
 * R^-_y(x)
 */
RuleIdSet GetExternalSupportWithConstrant(const AtomIdSet& y, const AtomIdSet& x, const RuleSet& rules) {
    RuleIdSet ret = GetExternalSupport(x, rules);
    AtomIdSet x_diff_y;
    set_difference(x.begin(), x.end(), y.begin(), y.end(), inserter(x_diff_y, x_diff_y.end()));
    for (RuleIdSet::iterator it = ret.begin(); it != ret.end(); ) {
        const AtomIdSet& head = rules[*it].head_;
        AtomIdSet result;
        set_intersection(head.begin(), head.end(), x_diff_y.begin(), x_diff_y.end(),
                inserter(result, result.begin()));
        if (! result.empty()) {
            ret.erase(it ++);
        }
        else {
            ++ it;
        }
    }
    return ret;
}
/*
 * 输出图的所有边
 */
void OutputGraph(FILE* out, const Graph& graph) {
    for (Graph::const_iterator it = graph.begin();
            it != graph.end(); ++ it) {
        int from = it->first;
        for (AtomIdSet::const_iterator it_to = it->second.begin();
                it_to != it->second.end(); ++ it_to) {
            int to = *it_to;
            fprintf(out, "%s -> %s\n", g_vocabulary.GetAtom(from).c_str(),
                    g_vocabulary.GetAtom(to).c_str());
        }
    }
}

void OutputExternalSupport(FILE* out, const RuleIdSet& rules_id, const RuleSet& rules) {
    for (RuleIdSet::const_iterator it = rules_id.begin(); it != rules_id.end(); ++ it) {
        rules[*it].Output(out);
    }
}
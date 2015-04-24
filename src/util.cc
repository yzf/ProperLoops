#include <algorithm>
#include "vocabulary.h"
#include "util.h"
#include "rule.h"
#include "loop.h"

using namespace std;

extern Vocabulary g_vocabulary;
/*
 * 公式集合的所有头部原子
 */
AtomSet HeadOfRules(const RuleSet& rules) {
    AtomSet ret;
    for (RuleSet::const_iterator it = rules.begin(); it != rules.end(); ++ it) {
        const Rule* rule = *it;
        ret.insert(rule->head_.begin(), rule->head_.end());
    }
    return ret;
}

RelationType RelationBetween(const AtomSet& set_a, const AtomSet& set_b, AtomSet& set_result) {
    set_difference(set_a.begin(), set_a.end(), set_b.begin(), set_b.end(),
            inserter(set_result, set_result.begin()));
    if (! set_result.empty()) {
        return kNSubsetEq;
    }
    else if (set_a == set_b) {
        return kSubsetEq;
    }
    else {
        return kSubset;
    }
}

RelationType RelationBetween(const RuleSet& set_a, const RuleSet& set_b, RuleSet& set_result) {
    set_difference(set_a.begin(), set_a.end(), set_b.begin(), set_b.end(),
            inserter(set_result, set_result.begin()));
    if (! set_result.empty()) {
        return kNSubsetEq;
    }
    else if (set_a == set_b) {
        return kSubsetEq;
    }
    else {
        return kSubset;
    }
}
/*
 * 销毁公式集合的内存
 */
void FreeRules(RuleSet rules) {
    for (RuleSet::iterator i = rules.begin(); i != rules.end(); ++ i) {
        if (NULL != *i) {
            delete *i;
            *i = NULL;
        }
    }
}
/*
 * 销毁环集合的内存
 */
void FreeLoops(LoopSet loops) {
    for (LoopSet::iterator i = loops.begin(); i != loops.end(); ++ i) {
        if (NULL != *i) {
            delete *i;
            *i = NULL;
        }
    }
}

void OutputRules(FILE* out, const RuleSet& rules) {
    for (size_t i = 0; i < rules.size(); ++ i) {
        rules[i]->Output(out);
    }
    fprintf(out, "\n");
}

void OutputAtoms(FILE* out, const AtomSet& atoms) {
    fprintf(out, "{");
    if (! atoms.empty()) {
        AtomSet::const_iterator i = atoms.begin();
        for ( ; i != (-- atoms.end()); ++ i) {
            fprintf(out, "%s,", g_vocabulary.GetAtom(*i).c_str());
        }
        fprintf(out, "%s", g_vocabulary.GetAtom(*i).c_str());
    }
    fprintf(out, "}\n");
}

void OutputLoops(FILE* out, const LoopSet& sccs) {
    for (LoopSet::const_iterator it = sccs.begin(); it != sccs.end(); ++ it) {
        (*it)->Output(out);
    }
    fprintf(out, "\n");
}
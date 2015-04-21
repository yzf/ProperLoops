#include "vocabulary.h"
#include "util.h"
#include "rule.h"
#include "loop.h"

extern Vocabulary g_vocabulary;

AtomSet HeadOfRules(const RuleSet& rules) {
    AtomSet ret;
    for (RuleSet::const_iterator it = rules.begin(); it != rules.end(); ++ it) {
        const Rule* rule = *it;
        ret.insert(rule->head_.begin(), rule->head_.end());
    }
    return ret;
}

void FreeRules(RuleSet rules) {
    for (RuleSet::iterator i = rules.begin(); i != rules.end(); ++ i) {
        if (NULL != *i) {
            delete *i;
        }
    }
}
void FreeLoops(LoopSet loops) {
    for (LoopSet::iterator i = loops.begin(); i != loops.end(); ++ i) {
        if (NULL != *i) {
            delete *i;
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
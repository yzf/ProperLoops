#include "vocabulary.h"
#include "util.h"
#include "rule.h"

extern Vocabulary g_vocabulary;

AtomIdSet HeadOfRules(const RuleSet& rules) {
    AtomIdSet ret;
    for (RuleSet::const_iterator it = rules.begin(); it != rules.end(); ++ it) {
        const Rule* rule = *it;
        ret.insert(rule->head_.begin(), rule->head_.end());
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
    fprintf(out, "\n");
}

void OutputExternalSupport(FILE* out, const RuleSet& rules) {
    for (size_t i = 0; i < rules.size(); ++ i) {
        rules[i]->Output(out);
    }
    fprintf(out, "\n");
}

void OutputAtoms(FILE* out, const AtomIdSet& atoms) {
    fprintf(out, "{");
    for (AtomIdSet::const_iterator i = atoms.begin(); i != atoms.end(); ++ i) {
        fprintf(out, "%s, ", g_vocabulary.GetAtom(*i).c_str());
    }
    fprintf(out, "}\n");
}

void OutputSccs(FILE* out, const SccSet& sccs) {
    for (SccSet::const_iterator it = sccs.begin(); it != sccs.end(); ++ it) {
        OutputAtoms(out, *it);
    }
    fprintf(out, "\n");
}
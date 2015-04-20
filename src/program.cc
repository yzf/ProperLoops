#include "program.h"
#include "vocabulary.h"
#include "graph.h"
#include "util.h"

using namespace std;

extern Vocabulary g_vocabulary;

Program::Program(const RuleSet& rules) : rules_(rules) {
    
}

Program::~Program() {
    for (RuleSet::iterator i = rules_.begin(); i != rules_.end(); ++ i)
        if (NULL != *i)
            delete *i;
}

Graph* Program::GetDependencyGraph() const {
    Graph* graph = new Graph(this);
    for (size_t i = 1; i <= g_vocabulary.Size(); ++ i) {
        graph->edge_map_.insert(make_pair(i, AtomSet()));
    }
    for (size_t i = 0; i < rules_.size(); ++ i) {
        const Rule* rule = rules_[i];
        if (rule->type_ == kRule) {
            for (AtomSet::const_iterator it_head = rule->head_.begin();
                    it_head != rule->head_.end(); ++ it_head) {
                for (AtomSet::const_iterator it_body = rule->body_.begin();
                        it_body != rule->body_.end(); ++ it_body) {
                    if (*it_body > 0) {
                        graph->edge_map_[*it_head].insert(*it_body);
                    }
                }
            }
        }
    }
    return graph;
}

void Program::Output(FILE* out) const {
    OutputRules(out, rules_);
}
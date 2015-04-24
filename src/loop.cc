#include <cassert>
#include <algorithm>
#include "loop.h"
#include "program.h"
#include "util.h"
#include "graph.h"
#include "hash.h"

using namespace std;

Loop::Loop(AtomSet* atoms, const Program* program)
        : atoms_(atoms),
          program_(program),
          hash_code_(-1) {
    const AtomSet* x = atoms_;
    const RuleSet& rules = program_->rules_;
    for (size_t i = 0; i < rules.size(); ++ i) {
        Rule* rule = rules[i];
        AtomSet result;
        set_intersection(x->begin(), x->end(), rule->head_.begin(),
                rule->head_.end(), inserter(result, result.begin()));
        if (! result.empty()) {
            result.clear();
            set_intersection(x->begin(), x->end(), rule->body_.begin(),
                    rule->body_.end(), inserter(result, result.begin()));
            if (result.empty()) {
                external_support_.push_back(rule);
            }
        }
    }
}

Loop::Loop(const Loop& rhs) {
    atoms_ = new AtomSet();
    *atoms_ = *(rhs.atoms_);
    external_support_ = rhs.external_support_;
    program_ = rhs.program_;
    hash_code_ = rhs.hash_code_;
}

Loop::~Loop() {
    if (atoms_ != NULL) {
        delete atoms_;
        atoms_ = NULL;
    }
    hash_code_ = -1;
}
/*
 * R^-_x(y)
 */
RuleSet Loop::ExternalSupportWithConstrant(const Loop* loop) const {
    const AtomSet* y = atoms_;
    const AtomSet* x = loop->atoms_;
    
    RuleSet ret = external_support_;
    AtomSet x_diff_y;
    set_difference(x->begin(), x->end(), y->begin(), y->end(),
            inserter(x_diff_y, x_diff_y.end()));
    int i = 0;
    while (i < ret.size()) {
        const AtomSet& head = ret[i]->head_;
        AtomSet result;
        set_intersection(head.begin(), head.end(), x_diff_y.begin(), x_diff_y.end(),
                inserter(result, result.begin()));
        if (! result.empty()) {
            swap(ret[i], ret.back());
            ret.pop_back();
        }
        else
            ++ i;
    }
    return ret;
}

void Loop::Output(FILE* out) const {
    OutputAtoms(out, *atoms_);
}

bool Loop::IsLoop() const {
    Graph* graph = program_->GetDependencyGraph();
    Graph* subgraph = graph->GetInducedSubgraph(*atoms_);
    LoopSet scc = subgraph->GetSccs();
    bool is_loop = false;
    if (scc.size() == 1 && *(scc.front()->atoms_) == *(atoms_)) {
        is_loop = true;
    }
    delete subgraph;
    delete graph;
    return is_loop;
}

int Loop::hash_code() const {
    if (-1 == hash_code_) {
        hash_code_ = 0;
        for (AtomSet::const_iterator i = atoms_->begin(); i != atoms_->end(); ++ i) {
            hash_code_ += (*i) * (*i);
            hash_code_ %= kHashSize;
        }
    }
    return hash_code_;
}

bool Loop::IsElementaryLoop() const {
    assert(program_->is_nlp());
    return IsWeakElementaryLoop();
}

bool Loop::IsProperLoop(const AtomSet& atoms) const {
    assert(program_->is_nlp());
    Graph* g_p = program_->GetDependencyGraph();
    const RuleSet& rl = external_support_;
    LoopSet scc;
    LoopSet scc_checked;
    Hash hash;
    
    g_p->ExtendSccsFromInducedSubgraph(atoms, hash, scc, scc_checked);

    while (! scc.empty()) {
        Loop *c = scc.front();
        const RuleSet& rc = c->external_support_;
        AtomSet c_diff_l;
        RuleSet rc_diff_rl;
        RelationType atom_relation = RelationBetween(*(c->atoms_), *atoms_, c_diff_l);
        RelationType rule_relation = RelationBetween(rc, rl, rc_diff_rl);
        if (atom_relation == kSubset && rule_relation == kSubsetEq) {
            FreeLoops(scc);//2- 4-
            FreeLoops(scc_checked);//2- 4-
            delete g_p;//1-
            return false;
        }
        else if ((! rc.empty()) && rule_relation == kSubset) {
            FreeLoops(scc);//2- 4-
            FreeLoops(scc_checked);//2- 4-
            delete g_p;//1-
            return false;
        }
        else if (rc.empty() || rc == rl) {
            g_p->ExtendSccsFromInducedSubgraph(c, hash, scc, scc_checked);
        }
        else {
            AtomSet head_diff = HeadOfRules(rc_diff_rl);
            AtomSet c_backslash_head_diff;
            set_difference(c->atoms_->begin(), c->atoms_->end(),
                    head_diff.begin(), head_diff.end(), 
                    inserter(c_backslash_head_diff, c_backslash_head_diff.begin()));
            g_p->ExtendSccsFromInducedSubgraph(c_backslash_head_diff, hash, scc, scc_checked);
        }
        scc.pop_front();
        scc_checked.push_back(c);
    }
    
    FreeLoops(scc_checked);//2- 4-
    delete g_p;//1-
    return true;
}

bool Loop::IsWeakElementaryLoop() const {
    const RuleSet& rl = external_support_;
    Graph* g_p = program_->GetDependencyGraph();//1+

    Hash hash;
    LoopSet scc;
    LoopSet scc_checked;//hash存放的元素是Loop*类型，所以要在hash不再被使用时才统一销毁所有scc
    AtomSet l_backslash_a = *atoms_;
        
    for (AtomSet::const_iterator i = atoms_->begin(); i != atoms_->end(); ++ i) {
        const int& a = *i;
        l_backslash_a.erase(a);
        
        g_p->ExtendSccsFromInducedSubgraph(l_backslash_a, hash, scc, scc_checked);
        l_backslash_a.insert(a);
        
        while (! scc.empty()) {
            Loop* c = scc.front();
            const RuleSet& rc = c->external_support_;
            RuleSet rc_diff_rl;
            RelationType relation_type = RelationBetween(rc, rl, rc_diff_rl);
            if (relation_type == kSubsetEq) {
                FreeLoops(scc);//3-
                FreeLoops(scc_checked);//3-
                delete g_p;//1-
                return false;
            }
            else {
                AtomSet head_diff = HeadOfRules(rc_diff_rl);
                AtomSet c_backslash_head_diff;
                set_difference(c->atoms_->begin(), c->atoms_->end(),
                        head_diff.begin(), head_diff.end(), 
                        inserter(c_backslash_head_diff, c_backslash_head_diff.begin()));
                g_p->ExtendSccsFromInducedSubgraph(c_backslash_head_diff, hash, scc, scc_checked);
            }
            scc.pop_front();
            scc_checked.push_back(c);
        }
    }
    
    FreeLoops(scc_checked);//3-
    delete g_p;//1-
    return true;
}
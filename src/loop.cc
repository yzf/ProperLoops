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
    const Loop* l = this;
    const RuleSet& rl = l->external_support_;
    Graph* g_p = program_->GetDependencyGraph();//1+

    Hash hash;
    LoopSet scc_checked;//hash存放的元素是Loop*类型，所以要在hash不再被使用时才统一销毁所有scc

    for (AtomSet::const_iterator i = l->atoms_->begin(); i != l->atoms_->end(); ++ i) {
        const int& a = *i;
        AtomSet l_backslash_a = *(l->atoms_);
        l_backslash_a.erase(a);
        Graph* g_star = g_p->GetInducedSubgraph(l_backslash_a);//2+
        LoopSet scc_star = g_star->GetSccs();//3+
        hash.AddLoops(scc_star);
        
        while (! scc_star.empty()) {
            Loop* c = scc_star.front();
            const RuleSet& rc = c->external_support_;
            RuleSet rc_diff_rl;
            RelationType relation_type = RelationBetween(rc, rl, rc_diff_rl);
            if (relation_type == kSubsetEq) {
                FreeLoops(scc_star);//3-
                FreeLoops(scc_checked);//3-
                delete g_star;//2-
                delete g_p;//1-
                return false;
            }
            else {
                AtomSet head_diff = HeadOfRules(rc_diff_rl);
                AtomSet c_backslash_head_diff;
                set_difference(c->atoms_->begin(), c->atoms_->end(),
                        head_diff.begin(), head_diff.end(), 
                        inserter(c_backslash_head_diff, c_backslash_head_diff.begin()));
                g_p->ExtendSccsFromInducedSubgraph(c_backslash_head_diff, hash, scc_star, scc_checked);
            }
            scc_star.pop_front();
            scc_checked.push_back(c);
        }
        delete g_star;//2-
    }
    
    FreeLoops(scc_checked);//3-
    delete g_p;//1-
    return true;
}

bool Loop::IsProperLoop(const AtomSet& atoms) const {
    assert(program_->is_nlp());
    Graph* g_p = program_->GetDependencyGraph();
    Graph* g_p_s = g_p->GetInducedSubgraph(atoms);
    delete g_p;
    
    const Loop* l = this;
    const RuleSet& rl = l->external_support_;
    LoopSet scc = g_p_s->GetSccs();//2+
    LoopSet scc_checked;
    Hash hash;
    hash.AddLoops(scc);

    while (! scc.empty()) {
        Loop *c = scc.front();
        const RuleSet& rc = c->external_support_;
        AtomSet c_diff_l;
        RuleSet rc_diff_rl;
        RelationType atom_relation = RelationBetween(*(c->atoms_), *(l->atoms_), c_diff_l);
        RelationType rule_relation = RelationBetween(rc, rl, rc_diff_rl);
        if (atom_relation == kSubset && rule_relation == kSubsetEq) {
            FreeLoops(scc);//2- 4-
            FreeLoops(scc_checked);//2- 4-
            delete g_p_s;//1-
            return false;
        }
        else if ((! rc.empty()) && rule_relation == kSubset) {
            FreeLoops(scc);//2- 4-
            FreeLoops(scc_checked);//2- 4-
            delete g_p_s;//1-
            return false;
        }
        else if (rc.empty() || rc == rl) {
            g_p_s->ExtendSccsFromInducedSubgraph(c, hash, scc, scc_checked);
        }
        else {
            AtomSet head_diff = HeadOfRules(rc_diff_rl);
            AtomSet c_backslash_head_diff;
            set_difference(c->atoms_->begin(), c->atoms_->end(),
                    head_diff.begin(), head_diff.end(), 
                    inserter(c_backslash_head_diff, c_backslash_head_diff.begin()));
            g_p_s->ExtendSccsFromInducedSubgraph(c_backslash_head_diff, hash, scc, scc_checked);
        }
        scc.pop_front();
        scc_checked.push_back(c);
    }
    
    FreeLoops(scc_checked);//2- 4-
    delete g_p_s;//1-
    return true;
}
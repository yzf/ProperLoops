#include <algorithm>
#include "loop.h"
#include "program.h"
#include "util.h"

using namespace std;

Loop::Loop(AtomSet* atoms, const Program* program)
        : atoms_(atoms),
          program_(program),
          hash_code_(-1) {
    
}

Loop::Loop(const Loop& rhs) {
    atoms_ = new AtomSet();
    *atoms_ = *(rhs.atoms_);
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
 * R^-(x)
 */
RuleSet Loop::GetExternalSupport() const {
    const AtomSet* x = atoms_;
    const RuleSet& rules = program_->rules_;
    
    RuleSet ret;
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
                ret.push_back(rule);
            }
        }
    }
    return ret;
}
/*
 * R^-_x(y)
 */
RuleSet Loop::GetExternalSupportWithConstrant(const Loop* loop) const {
    const AtomSet* y = atoms_;
    const AtomSet* x = loop->atoms_;
    
    RuleSet ret = GetExternalSupport();
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

//bool Loop::operator < (const Loop& rhs) const {
//    return *atoms_ < *(rhs.atoms_);
//}
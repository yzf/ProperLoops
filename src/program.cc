#include <cassert>
#include <cmath>
#include <algorithm>
#include <vector>
#include "program.h"
#include "vocabulary.h"
#include "graph.h"
#include "util.h"
#include "loop.h"
#include "hash.h"

using namespace std;

extern Vocabulary g_vocabulary;

Program::Program(const RuleSet& rules) : rules_(rules) {
    is_nlp_ = true;
    for (RuleSet::const_iterator i = rules_.begin(); i != rules_.end(); ++ i) {
        if ((*i)->head_.size() > 1) {
            is_nlp_ = false;
            break;
        }
    }
}

Program::~Program() {
    FreeRules(rules_);
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

AtomSet Program::GetAtoms() const {
    AtomSet atoms;
    for (RuleSet::const_iterator i = rules_.begin(); i != rules_.end(); ++ i) {
        atoms.insert((*i)->head_.begin(), (*i)->head_.end());
        for (LiteralSet::const_iterator j = (*i)->body_.begin();
                j != (*i)->body_.end(); ++j) {
            atoms.insert(abs(*j));
        }
    }
    return atoms;
}

bool Program::is_nlp() const {
    return is_nlp_;
}

void Program::Output(FILE* out) const {
    OutputRules(out, rules_);
}

LoopSet Program::GetElementaryLoops() const {
    assert(is_nlp());
    LoopSet loops;
    Graph* g_p = GetDependencyGraph();//1+
    LoopSet scc = g_p->GetSccs();//2+
    LoopSet scc_checked;
    Hash hash;
    hash.AddLoops(scc);
    
    while(! scc.empty()) {
        Loop* c = scc.front();
        bool is_elementary_loop = c->IsElementaryLoop();

        if (is_elementary_loop) {
            if (c->atoms_.size() > 1)
                loops.push_back(c);
        }
        else {
            scc_checked.push_back(c);//非elementary loop才需要销毁
        }
        g_p->ExtendSccsFromInducedSubgraph(c, hash, scc, scc_checked);   
        scc.pop_front();
    }
    
    FreeLoops(scc_checked);//2-
    delete g_p;//1-
    return loops;
}

LoopSet Program::GetProperLoops(const AtomSet& atoms) const {
    assert(is_nlp());
    LoopSet loops;
    Graph* g_p = GetDependencyGraph();//1+
    LoopSet scc = g_p->GetSccs();//2+
    LoopSet scc_checked;
    Hash hash;
    hash.AddLoops(scc);

    while (! scc.empty()) {
        Loop* c = scc.front(); 
        bool is_proper_loop = c->IsProperLoop(atoms);

        if (is_proper_loop) {
            loops.push_back(c);
            const RuleSet& rc = c->external_support_;
            AtomSet head_rc = HeadOfRules(rc);
            g_p->ExtendSccsFromInducedSubgraph(c, head_rc, hash, scc, scc_checked);
        }
        else {
            scc_checked.push_back(c);
            g_p->ExtendSccsFromInducedSubgraph(c, hash, scc, scc_checked);
        }
        scc.pop_front();
    }
    
    FreeLoops(scc_checked);
    delete g_p;//1-
    return loops;
}

bool Program::ExistWeakElementaryLoop(const Loop& loop) const {
    const Loop* e = &loop;
    Graph* g_p = GetDependencyGraph();
    LoopSet scc = g_p->GetSccs();
    Loop* c = NULL;
    for (LoopSet::const_iterator i = scc.begin(); i != scc.end(); ++ i) {
        if (includes((*i)->atoms_.begin(), (*i)->atoms_.end(),
                e->atoms_.begin(), e->atoms_.end())) {
            c = *i;
            break;
        }
    }
    if (NULL == c) {
        FreeLoops(scc);
        delete g_p;
        return false;
    }
    for (RuleSet::const_iterator i = rules_.begin(); i != rules_.end(); ++ i) {
        const Rule* r = *i;
        AtomSet body_r_diff_e;
        set_difference(r->body_.begin(), r->body_.end(), e->atoms_.begin(),
                e->atoms_.end(), inserter(body_r_diff_e, body_r_diff_e.begin()));
        // body^(r)\cap E =\emptyset   =>    body^+(r)\cap E =\emptyset
        if (body_r_diff_e.size() == r->body_.size()) {
            continue;
        }
        // Get body^+(r)\backslash E
        for (AtomSet::iterator j = body_r_diff_e.begin(); j != body_r_diff_e.end(); ) {
            if (*j > 0) {
                ++ j;
            }
            else {
                body_r_diff_e.erase(j ++);
            }
        }
        AtomSet c_backshlash_body_r_diff_e;
        set_difference(c->atoms_.begin(), c->atoms_.end(), body_r_diff_e.begin(),
                body_r_diff_e.end(), inserter(c_backshlash_body_r_diff_e,
                c_backshlash_body_r_diff_e.begin()));
        Graph* g_r = g_p->GetInducedSubgraph(c_backshlash_body_r_diff_e);
        LoopSet scc_r = g_r->GetSccs();
        for (LoopSet::const_iterator j = scc_r.begin(); j != scc_r.end(); ++ j) {
            Loop* c_r = *j;
            if (includes(c_r->atoms_.begin(), c_r->atoms_.end(), e->atoms_.begin(),
                    e->atoms_.end())) {
                AtomSet c_r_diff_e;
                set_difference(c_r->atoms_.begin(), c_r->atoms_.end(), e->atoms_.begin(),
                        e->atoms_.end(), inserter(c_r_diff_e, c_r_diff_e.begin()));
                AtomSet head_r_cap_c_r_diff_e;
                set_intersection(r->head_.begin(), r->head_.end(), c_r_diff_e.begin(),
                        c_r_diff_e.end(), inserter(head_r_cap_c_r_diff_e, head_r_cap_c_r_diff_e.begin()));
                if (! head_r_cap_c_r_diff_e.empty()) {
                    FreeLoops(scc_r);
                    delete g_r;
                    FreeLoops(scc);
                    delete g_p;
                    return true;
                }
            }
        }
        FreeLoops(scc_r);
        delete g_r;
    }
    
    FreeLoops(scc);
    delete g_p;
    return false;
}
/*
 * 
 */
bool Program::IsHeadElementaryLoopFreeStar() const {
    AtomSet::const_iterator j;
    AtomSet::const_iterator k;
    for (RuleSet::const_iterator i = rules_.begin(); i != rules_.end(); ++ i) {
        const Rule* r = *i;
        if (r->head_.size() < 2)
            continue;
        for (j= r->head_.begin(); j != (-- r->head_.end()); ++ j) {
            k = j;
            ++ k;
            for ( ; k != r->head_.end(); ++ k) {
                AtomSet s;
                s.insert(*j);
                s.insert(*k);
                Loop e(s, this);
                if (e.IsLoop() && e.IsElementaryLoop()) {
                    return false;
                }
                if (ExistWeakElementaryLoop(e)) {
                    return false;
                }
            }
        }
    }
    return true;
}
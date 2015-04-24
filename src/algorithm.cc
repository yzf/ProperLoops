#include <cassert>
#include <algorithm>
#include <map>
#include "algorithm.h"
#include "graph.h"
#include "util.h"
#include "program.h"
#include "loop.h"
#include "hash.h"

using namespace std;

LoopSet AllLoops(const Program& program) {
    LoopSet loops;
    Hash hash;
    Graph* g_p = program.GetDependencyGraph();
    LoopSet sccs = g_p->GetSccs();
    LoopSet scc_checked;
    hash.AddLoops(sccs);
    while (! sccs.empty()) {
        Loop* c = sccs.front();
        if (c->atoms_->size() > 1)
            loops.push_back(c);
        g_p->ExtendSccsFromInducedSubgraph(c, hash, sccs, scc_checked);
        sccs.pop_front();
    }
    delete g_p;
    return loops;
}

////////////////////////////// nlp /////////////////////////////////
/*
 * 判断loop是否为nlp的elementary loop
 * 算法细节请参阅《回答集逻辑程序特征环的研究》中的算法１
 */
bool ElementaryLoop(const Loop& loop, const Program& program) {
    assert(program.is_nlp());
    const Loop* l = &loop;
    const RuleSet& rl = l->external_support_;
    Graph* g_p = program.GetDependencyGraph();//1+
    
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
/*
 * 求nlp的所有elementary loops
 */
LoopSet ElementaryLoops(const Program& program) {
    assert(program.is_nlp());
    LoopSet loops;
    Graph* g_p = program.GetDependencyGraph();//1+
    LoopSet scc = g_p->GetSccs();//2+
    LoopSet scc_checked;
    Hash hash;
    hash.AddLoops(scc);
    
    while(! scc.empty()) {
        Loop* c = scc.front();
        bool is_elementary_loop = ElementaryLoop(*c, program);

        if (is_elementary_loop) {
            if (c->atoms_->size() > 1)
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
/*
 * 判断loop是否为nlp的proper loop
 * 算法细节请参阅《回答集逻辑程序特征环的研究》中的算法３
 */
bool ProperLoop(const Loop& loop, const Program& program, const AtomSet& atoms) {
    assert(program.is_nlp());
    Graph* g_p = program.GetDependencyGraph();
    Graph* g_p_s = g_p->GetInducedSubgraph(atoms);
    delete g_p;
    
    const Loop* l = &loop;
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
/*
 * 求nlp的所有proper loop，不包括单原子
 */
LoopSet ProperLoops(const Program& program, const AtomSet& atoms) {
    assert(program.is_nlp());
    LoopSet loops;
    Graph* g_p = program.GetDependencyGraph();//1+
    LoopSet scc = g_p->GetSccs();//2+
    LoopSet scc_checked;
    Hash hash;
    hash.AddLoops(scc);

    while (! scc.empty()) {
        Loop* c = scc.front(); 
        bool is_proper_loop = ProperLoop(*c, program, atoms);

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



////////////////////////////// dlp /////////////////////////////////

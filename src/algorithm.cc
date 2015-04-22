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

static int g_cnt;
////////////////////////////// nlp /////////////////////////////////
/*
 * 判断loop是否为nlp的elementary loop
 * 算法细节请参阅《回答集逻辑程序特征环的研究》中的算法１
 */
bool ElementaryLoop(const Loop& loop, const Program& program) {
    assert(! program.is_dlp());
    const Loop* l = &loop;
    RuleSet rl = l->GetExternalSupport();
    Graph* g_p = program.GetDependencyGraph();//1+
    
    for (AtomSet::const_iterator i = l->atoms_->begin(); i != l->atoms_->end(); ++ i) {
        const int& a = *i;
        AtomSet l_backslash_a = *(l->atoms_);
        l_backslash_a.erase(a);
        Graph* g_star = g_p->GetInducedSubgraph(l_backslash_a);//2+
        LoopSet scc_star = g_star->GetSccs();//3+
        LoopSet scc_checked;//hash存放的元素是Loop*类型，所以要在hash不再被使用时才统一销毁所有scc
        Hash hash;
        hash.AddLoops(scc_star);
        
        while (! scc_star.empty()) {
            Loop* c = scc_star.front();
            RuleSet rc = c->GetExternalSupport();
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
                Graph* g_c = g_p->GetInducedSubgraph(c_backslash_head_diff);//4+
                LoopSet scc_c = g_c->GetSccs();//5+
                while (! scc_c.empty()) {
                    Loop* new_c = scc_c.front();
                    if (! hash.HasLoop(new_c)) {
                        hash.AddLoop(new_c);
                        scc_star.push_back(new_c);
                    }
                    else {
                        scc_checked.push_back(new_c);//该loop已经存在，但内存是新的，所以要放进scc_checked，统一销毁
                    }
                    scc_c.pop_front();
                }
                delete g_c;//4-
            }
            scc_checked.push_back(c);
            scc_star.pop_front();
        }
        
        g_cnt += hash.Size();
        
        FreeLoops(scc_checked);//3-
        delete g_star;//2-
    }
    
    delete g_p;//1-
    return true;
}
/*
 * 求nlp的所有elementary loops
 */
LoopSet ElementaryLoops(const Program& program) {
    assert(! program.is_dlp());
    LoopSet loops;
    Graph* g_p = program.GetDependencyGraph();//1+
    LoopSet scc = g_p->GetSccs();//2+
    LoopSet scc_checked;
    Hash hash;
    hash.AddLoops(scc);
    
    g_cnt = 0;
    
    while(! scc.empty()) {
        Loop* c = scc.front();
        bool is_elementary_loop = ElementaryLoop(*c, program);
        if (is_elementary_loop) {
            loops.push_back(c);
        }
        else {
            scc_checked.push_back(c);//非elementary loop才需要销毁
        }
        for (AtomSet::const_iterator i = c->atoms_->begin();
                i != c->atoms_->end(); ++ i) {
            const int& a = *i;
            AtomSet c_backslash_a = *(c->atoms_);
            c_backslash_a.erase(a);
            Graph* g_c = g_p->GetInducedSubgraph(c_backslash_a);//3+
            LoopSet scc_c = g_c->GetSccs();//4+
            while (! scc_c.empty()) {
                Loop* new_c = scc_c.front();
                if (! hash.HasLoop(new_c)) {
                    hash.AddLoop(new_c);
                    scc.push_back(new_c);
                }
                else {
                    scc_checked.push_back(new_c);
                }
                scc_c.pop_front();
            }
            delete g_c;//3-
        }
        scc.pop_front();
    }
    
    g_cnt += hash.Size();
    printf("g_cnt: %d\n", g_cnt);
    
    FreeLoops(scc_checked);//2-
    delete g_p;//1-
    return loops;
}
/*
 * 判断loop是否为nlp的proper loop
 * 算法细节请参阅《回答集逻辑程序特征环的研究》中的算法３，其中S=Atoms(P)
 */
bool ProperLoop(const Loop& loop, const Program& program, const AtomSet& atoms) {
    assert(! program.is_dlp());
    Graph* g_p = program.GetDependencyGraph();
    Graph* g_p_s = g_p->GetInducedSubgraph(atoms);
    delete g_p;
    const Loop* l = &loop;
    RuleSet rl = l->GetExternalSupport();
    LoopSet scc = g_p_s->GetSccs();//2+
    LoopSet scc_checked;
    Hash hash;
    hash.AddLoops(scc);

    while (! scc.empty()) {
        Loop *c = scc.front();
        RuleSet rc = c->GetExternalSupport();
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
            for (AtomSet::const_iterator i = c->atoms_->begin();
                    i != c->atoms_->end(); ++ i) {
                const int& a = *i;
                AtomSet c_backslash_a = *(c->atoms_);
                c_backslash_a.erase(a);
                Graph* g_star = g_p_s->GetInducedSubgraph(c_backslash_a);//3+
                LoopSet scc_star = g_star->GetSccs();//4+
                while (! scc_star.empty()) {
                    Loop* new_c = scc_star.front();
                    if (! hash.HasLoop(new_c)) {
                        scc.push_back(new_c);
                        hash.AddLoop(new_c);
                    }
                    else {
                        scc_checked.push_back(new_c);
                    }
                    scc_star.pop_front();
                }
                delete g_star;//3-
            }
        }
        scc.pop_front();
    }
    
    g_cnt += hash.Size();
    
    FreeLoops(scc_checked);//2- 4-
    delete g_p_s;//1-
    return true;
}
LoopSet ProperLoops(const Program& program, const AtomSet& atoms) {
    LoopSet loops;
    Graph* g_p = program.GetDependencyGraph();//1+
    LoopSet scc = g_p->GetSccs();//2+
    LoopSet scc_checked;
    Hash hash;
    hash.AddLoops(scc);
    
    g_cnt = 0;
    
    while (! scc.empty()) {
        Loop* c = scc.front();
        bool is_proper_loop = ProperLoop(*c, program, atoms);
        if (is_proper_loop) {
            loops.push_back(c);
            RuleSet rc = c->GetExternalSupport();
            AtomSet head_rc = HeadOfRules(rc);
            for (AtomSet::const_iterator i = head_rc.begin(); i != head_rc.end(); ++ i) {
                const int& a = *i;
                AtomSet c_backslash_a = *(c->atoms_);
                c_backslash_a.erase(a);
                Graph* g_c = g_p->GetInducedSubgraph(c_backslash_a);//3+
                LoopSet scc_c = g_c->GetSccs();//4+
                while (! scc_c.empty()) {
                    Loop* new_c = scc_c.front();
                    if (! hash.HasLoop(new_c)) {
                        hash.AddLoop(new_c);
                        scc.push_back(new_c);
                    }
                    else {
                        scc_checked.push_back(new_c);
                    }
                    scc_c.pop_front();
                }
                delete g_c;//3-
            }
        }
        else {
            scc_checked.push_back(c);
            for (AtomSet::const_iterator i = c->atoms_->begin();
                    i != c->atoms_->end(); ++ i) {
                const int& a = *i;
                AtomSet c_backslash_a = *(c->atoms_);
                c_backslash_a.erase(a);
                Graph* g_c = g_p->GetInducedSubgraph(c_backslash_a);//3+
                LoopSet scc_c = g_c->GetSccs();//4+
                while (! scc_c.empty()) {
                    Loop* new_c = scc_c.front();
                    if (! hash.HasLoop(new_c)) {
                        hash.AddLoop(new_c);
                        scc.push_back(new_c);
                    }
                    else {
                        scc_checked.push_back(new_c);
                    }
                    scc_c.pop_front();
                }
                delete g_c;//3-
            }
        }
        scc.pop_front();
    }
    
    g_cnt += hash.Size();
    printf("gnt: %d\n", g_cnt);
    
    FreeLoops(scc_checked);
    delete g_p;//1-
    return loops;
}



////////////////////////////// dlp /////////////////////////////////
/*
 * 判断loop是否为程序program的EL*(P)
 */
bool ElementaryLoopStar(const Loop& loop, const Program& program) {
    return true;
}
/*
 * 判断loop是否为program的proper loop
 */
bool ProperLoopStar(const Loop& loop, const Program& program) {
    return true;
}

#include <cstdio>
#include "rule.h"
#include "vocabulary.h"

using namespace std;

extern Vocabulary g_vocabulary;

Rule::Rule() : type_(kFact) {
}

Rule::Rule(const RuleHelper* rule_helper) {
    head_.insert(rule_helper->head, rule_helper->head + rule_helper->head_length);
    body_.insert(rule_helper->body, rule_helper->body + rule_helper->body_length);
    type_ = rule_helper->type;
}
/*
 * 根据字符表输出规则
 */
void Rule::Output(FILE* out) const {
    LiteralIdSet::const_iterator it;
    string atom;
    if (type_ != kConstrant) {
        for (it = head_.begin(); it != (-- head_.end()); ++ it) {
            atom = g_vocabulary.GetAtom(*it);
            fprintf(out, "%s | ", atom.c_str());
        }
        atom = g_vocabulary.GetAtom(*it);
        fprintf(out, "%s", atom.c_str());
    }
    if (type_ != kFact) {
        fprintf(out, " :- ");
        for (it = body_.begin(); it != (-- body_.end()); ++ it) {
            if (*it > 0) {
                atom = g_vocabulary.GetAtom(*it);
                fprintf(out, "%s, ", atom.c_str());
            }
            else {
                atom = g_vocabulary.GetAtom(-(*it));
                fprintf(out, "not %s, ", atom.c_str());
            }
        }
        if (*it > 0) {
                atom = g_vocabulary.GetAtom(*it);
            fprintf(out, "%s", atom.c_str());
        }
        else {
            atom = g_vocabulary.GetAtom(-(*it));
            fprintf(out, "not %s", atom.c_str());
        }
    }
    fprintf(out, ".\n");
}
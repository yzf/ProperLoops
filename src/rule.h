/* 
 * File:   rule.h
 * Author: yzf
 *
 * Created on April 16, 2015, 3:54 PM
 */

#ifndef RULE_H
#define	RULE_H

#include "structs.h"

class Rule {
public:
    LiteralSet head_;//规则头部
    LiteralSet body_;//规则体部
    RuleType type_;//规则类型
public:
    Rule();
    explicit Rule(const RuleHelper*);
    void Output(FILE*) const;
};


#endif	/* RULE_H */


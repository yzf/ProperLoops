/* 
 * File:   dlp.h
 * Author: yzf
 *
 * Created on April 16, 2015, 7:07 PM
 */

#ifndef DLP_H
#define	DLP_H

#include "structs.h"

/*
 * 析取逻辑程序
 */
class DLP {
public:
    RuleSet rules_;//所有规则
public:
    DLP(const RuleSet&);
    void Output(FILE *out) const;
    
};

#endif	/* DLP_H */


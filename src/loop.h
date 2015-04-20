/* 
 * File:   loop.h
 * Author: yzf
 *
 * Created on April 20, 2015, 9:33 PM
 */

#ifndef LOOP_H
#define	LOOP_H

#include <cstdio>
#include "structs.h"

class Program;

class Loop {
public:
    AtomSet atoms_;
    const Program* program_;
public:
    Loop(const AtomSet&, const Program*);
    RuleSet GetExternalSupport() const;
    RuleSet GetExternalSupportWithConstrant(const AtomSet&) const;
    void Output(FILE* out) const;
};

#endif	/* LOOP_H */


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
    Loop(AtomSet*, const Program*);
    Loop(const Loop&);
    ~Loop();
    RuleSet GetExternalSupport() const;
    RuleSet GetExternalSupportWithConstrant(const Loop*) const;
    void Output(FILE* out) const;
    int hash_code() const;
    //bool operator < (const Loop& rhs) const;
public:
    AtomSet* atoms_;
    const Program* program_;
private:
    mutable int hash_code_;
};

#endif	/* LOOP_H */


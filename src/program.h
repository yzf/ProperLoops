/* 
 * File:   program.h
 * Author: yzf
 *
 * Created on April 20, 2015, 9:24 PM
 */

#ifndef PROGRAM_H
#define	PROGRAM_H

#include <cstdio>
#include "rule.h"

class Graph;

class Program {
public:
    Program(const RuleSet&);
    ~Program();
    Graph* GetDependencyGraph() const;
    AtomSet GetAtoms() const;
    bool is_dlp() const;
    void Output(FILE* out) const;
public:
    RuleSet rules_;
private:
    bool is_dlp_;
};

#endif	/* PROGRAM_H */


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
class Loop;

class Program {
public:
    Program(const RuleSet&);
    ~Program();
    Graph* GetDependencyGraph() const;
    AtomSet GetAtoms() const;
    bool is_nlp() const;
    void Output(FILE* out) const;
    LoopSet GetElementaryLoops() const;
    LoopSet GetProperLoops(const AtomSet&) const;
    bool ExistWeakElementaryLoop(const Loop&) const;
    bool IsHeadElementaryLoopFreeStar() const;
public:
    RuleSet rules_;
private:
    bool is_nlp_;
};

#endif	/* PROGRAM_H */


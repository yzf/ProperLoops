/* 
 * File:   hash.h
 * Author: yzf
 *
 * Created on April 21, 2015, 2:43 PM
 */

#ifndef HASH_H
#define	HASH_H

#include <cstdio>
#include <set>
#include "structs.h"

class Loop;

class Hash {
public:
    bool HasLoop(const Loop&) const;
    void AddLoop(const Loop&);
    void AddLoops(const LoopSet& loops);
    size_t Size() const;
    void Dump(FILE*) const;
private:
    std::set<Loop> hash_[kHashSize];
};

#endif	/* HASH_H */


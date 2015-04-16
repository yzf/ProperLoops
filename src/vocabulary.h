/* 
 * File:   vocabulary.h
 * Author: yzf
 *
 * Created on April 16, 2015, 4:23 PM
 */

#ifndef VOCABULARY_H
#define	VOCABULARY_H

#include "structs.h"

class Vocabulary {
private:
    AtomSet atoms_;//原子，atoms_[i-1]保存的是id为i的原子的名字
public:
    int AddAtom(const std::string&);
    std::string GetAtom(const int&) const;
    int GetAtomId(const std::string&) const;
    void Dump(FILE* out) const;
    size_t Size() const;
};

#endif	/* VOCABULARY_H */


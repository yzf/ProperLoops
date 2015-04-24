#include "hash.h"
#include "loop.h"

using namespace std;

bool LoopCmp::operator ()(const Loop* lhs, const Loop* rhs) const {
    return lhs->atoms_ < rhs->atoms_;
}

bool Hash::HasLoop(const Loop* loop) const {
    int hash_code = loop->hash_code();
    return hash_[hash_code].find(const_cast<Loop*>(loop)) != hash_[hash_code].end();
}

void Hash::AddLoop(const Loop* loop) {
    hash_[loop->hash_code()].insert(const_cast<Loop*>(loop));
}

void Hash::AddLoops(const LoopSet& loops) {
    for (LoopSet::const_iterator i = loops.begin(); i != loops.end(); ++ i) {
        hash_[(*i)->hash_code()].insert(*i);
    }
}

size_t Hash::Size() const {
    size_t ret = 0;
    for (int i = 0; i < kHashSize; ++ i) {
        ret += hash_[i].size();
    }
    return ret;
}

void Hash::Dump(FILE* out) const {
    for (int i = 0; i < kHashSize; ++ i) {
        for (set<Loop*>::const_iterator j = hash_[i].begin();
                j != hash_[i].end(); ++ j) {
            (*j)->Output(out);
        }
    }
    fprintf(out, "\n\n");
}
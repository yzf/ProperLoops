#include <assert.h>
#include <cstdio>
#include "vocabulary.h"

using namespace std;
/*
 * 添加原子到字符表，函数不做判重处理，调用前请用GetAtomId检测
 */
int Vocabulary::AddAtom(const string& atom) {
    static int atom_id = 1;
    atoms_.push_back(atom);
    return atom_id ++;
}

string Vocabulary::GetAtom(const int& id) const {
    assert(id > 0 && id <= atoms_.size());
    return atoms_[id - 1];
}

int Vocabulary::GetAtomId(const std::string& atom) const {
    for (size_t i = 0; i < atoms_.size(); ++ i) {
        if (atoms_[i] == atom) {
            return i + 1;
        }
    }
    return 0;
}

void Vocabulary::Dump(FILE* out) const {
    for (size_t i = 0; i < atoms_.size(); ++ i) {
        fprintf(out, "%lu: %s\n", i + 1, atoms_[i].c_str());
    }
}

size_t Vocabulary::Size() const {
    return atoms_.size();
}

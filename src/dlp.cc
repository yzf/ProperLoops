#include "dlp.h"
#include "rule.h"

using namespace std;

DLP::DLP(const RuleSet& rules) : rules_(rules) {
}

void DLP::Output(FILE* out) const {
    for (size_t i = 0; i < rules_.size(); ++ i) {
        rules_[i].Output(out);
    }
}
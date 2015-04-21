/* 
 * File:   structs.h
 * Author: yzf
 *
 * Created on April 16, 2015, 3:58 PM
 */

#ifndef STRUCTS_H
#define	STRUCTS_H

#include <string>
#include <set>
#include <map>
#include <vector>
#include <deque>

class Rule;
class Loop;

const int kMaxHeadLength = 512;//规则头部的最大原子个数
const int kMaxBodyLength = 512;//规则体部的最大文字个数
const int kHashSize = 95;

//规则类型
enum RuleType {
    kFact = 0,//事实
    kConstrant,//约束
    kRule,//规则
};
/*
 * parse时用到的辅助数据结构
 */
typedef struct _RuleHelper {
    int head[kMaxHeadLength];
    int head_length;
    int body[kMaxBodyLength];
    int body_length;
    RuleType type;
} RuleHelper;

typedef struct _HeadHelper {
    int atoms[kMaxHeadLength];
    int length;
} HeadHelper;

typedef struct _BodyHelper {
    int atoms[kMaxBodyLength];
    int length;
} BodyHelper;

typedef std::vector<Rule*> RuleSet;
typedef std::set<int> LiteralSet;
typedef LiteralSet AtomSet;
typedef std::vector<std::string> AtomNameSet;
typedef std::map<int, AtomSet> EdgeMap;
typedef std::deque<Loop*> LoopSet;

#endif	/* STRUCTS_H */


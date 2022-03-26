//
// Created by Kobe De Broeck on 19-2-2022.
//

#ifndef TA_PRAC_1_DFA_H
#define TA_PRAC_1_DFA_H

#include <string>
#include <map>
#include <set>

#include "json.hpp"

class DFA {
    std::string startState {};
    std::set<std::string> finalStates {};
    std::map<std::pair<std::string, std::string>, std::string> transitionTable {};
    std::set<std::string> alphabet {};

public:
    DFA(const std::string& fileName);
    DFA(const nlohmann::json& jsonDesc, int);
private:
    void initDFA(const nlohmann::json& jsonDesc);
public:

    bool accepts(const std::string& s) const;

    void print() const;

};


#endif //TA_PRAC_1_DFA_H

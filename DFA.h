//
// Created by Kobe De Broeck on 19-2-2022.
//

#ifndef TA_PRAC_1_DFA_H
#define TA_PRAC_1_DFA_H

#include <string>
#include <iostream>
#include <map>
#include <set>

#include "json.hpp"

class DFA {

    struct State {
        std::string name;
        bool starting;
        bool accepting;

        bool operator==(const State &other) const {
            return name == other.name && starting == other.starting && accepting == other.accepting;
        }

        bool operator<(const State &other) const {
            return name < other.name;
        }
    };

    friend std::ostream& operator<<(std::ostream& os, const std::pair<State,State>& p) {
        os << p.first.name << ", " << p.second.name;
        return os;
    }

    std::string startState {};
    std::set<std::string> finalStates {};
    std::map<std::pair<std::string, std::string>, std::string> transitionTable {};
    std::set<std::string> alphabet {};

    std::set<State> states;

    // A sorted map of pairs (sorted by name). bool tells if pair is marked/distinguishable.
    std::map<std::pair<State,State>, bool> statePairs;

public:
    DFA(const std::string& fileName);
    DFA(const nlohmann::json& jsonDesc, int);
private:
    void initDFA(const nlohmann::json& jsonDesc);
public:

    bool accepts(const std::string& s) const;

    void print() const;

    void printTable() const;
};


#endif //TA_PRAC_1_DFA_H

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

struct State {
    std::string name;
    bool starting;
    bool accepting;

    bool operator==(const State &other) const {
        //return name == other.name && starting == other.starting && accepting == other.accepting;
        return name == other.name;
    }

    bool operator<(const State &other) const {
        return name < other.name;
    }
};

class DFA {

    friend std::ostream& operator<<(std::ostream& os, const std::pair<State,State>& p) {
        os << p.first.name << ", " << p.second.name;
        return os;
    }

    State startState {};
    std::map<std::pair<State, std::string>, State> transitionTable {};
    std::set<std::string> alphabet {};

    std::map<std::string, State> states; // Maps name of state on State object itself

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

std::map<std::pair<State,State>, bool> /*statePairs*/
        fillTable(
                const std::set<std::string>& alphabet,
                const std::map<std::string, State>& states,
                const std::map<std::pair<State, std::string>, State>& transitionTable);



#endif //TA_PRAC_1_DFA_H

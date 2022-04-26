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

struct ENFAState {
    std::string name;
    bool starting;
    bool accepting;

    bool operator==(const ENFAState &other) const {
        return name == other.name && starting == other.starting && accepting == other.accepting;
    }

    bool operator!=(const ENFAState& other) const {
        return !(*this==other);
    }

    bool operator<(const ENFAState &other) const {
        return name < other.name;
    }
};

class DFA {

    friend std::ostream& operator<<(std::ostream& os, const std::pair<ENFAState,ENFAState>& p) {
        os << p.first.name << ", " << p.second.name;
        return os;
    }

    ENFAState startState {};
    std::map<std::pair<ENFAState, std::string>, ENFAState> transitionTable {};
    std::set<std::string> alphabet {};

    std::map<std::string, ENFAState> states; // Maps name of state on ENFAState object itself

    // A sorted map of pairs (sorted by name). bool tells if pair is marked/distinguishable.
    std::map<std::pair<ENFAState,ENFAState>, bool> statePairs;

public:
    DFA(const std::string& fileName);
    DFA(const nlohmann::json& jsonDesc, int);
private:
    void initDFA(const nlohmann::json& jsonDesc);
public:

    DFA minimize() const;

    bool operator==(const DFA& other) const;

    bool accepts(const std::string& s) const;

    void print() const;

    void printTable() const;
};

std::map<std::pair<ENFAState,ENFAState>, bool> /*statePairs*/
        fillTable(
                const std::set<std::string>& alphabet,
                const std::map<std::string, ENFAState>& states,
                const std::map<std::pair<ENFAState, std::string>, ENFAState>& transitionTable);

void printTable(const std::map<std::string, ENFAState>& states, const std::map<std::pair<ENFAState,ENFAState>, bool>& statePairs );


#endif //TA_PRAC_1_DFA_H

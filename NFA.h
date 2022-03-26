//
// Created by Kobe De Broeck on 23-3-2022.
//

#ifndef TA_PRAC_2_NFA_H
#define TA_PRAC_2_NFA_H

#include <iostream>
#include <set>
#include <map>
#include <optional>

#include "DFA.h"

class NFA {

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

    State startState{};
    std::map<std::string, State> states{};
    std::map<std::pair<State, std::string>, std::set<State>> transitionTable{};
    std::set<std::string> alphabet{};
public:
    NFA(const std::string &nfaDescFile);

    DFA toDFA() const;

private:
    std::set<State> getNFADestinations(const NFA::State &s, const std::string& input) const;

    friend std::string toDFAString(const std::set<State> &s);

    friend nlohmann::json toDFAJsonTransition(const std::set<NFA::State>& from, const std::string& input, const std::set<NFA::State>& to);

    bool containsStartingState(std::set<State> s) const;

    bool containsAcceptingState(const std::set<State>&  s) const;
};

#endif //TA_PRAC_2_NFA_H

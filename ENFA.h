//
// Created by kobedb on 26.04.22.
//

#ifndef TA_PRAC_2_ENFA_H
#define TA_PRAC_2_ENFA_H

#include <memory>
#include <vector>
#include <map>

class ENFAState;

class ENFA {

    ENFAState* startState;

    std::vector<std::unique_ptr<ENFAState>> states;

public:
    ENFA(ENFAState* startState);

    std::vector<ENFAState*> getFinalStates();

    ENFAState* createState(bool accepting);

};

class ENFAState {
    bool accepting;
    std::map<char, ENFAState*> transitions;
    explicit ENFAState(bool accepting) : accepting{accepting} {}

    friend class ENFA;

public:
    void addTransition(char c, ENFAState *to);
};

#endif //TA_PRAC_2_ENFA_H

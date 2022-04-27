//
// Created by kobedb on 26.04.22.
//

#ifndef TA_PRAC_2_ENFA_H
#define TA_PRAC_2_ENFA_H

#include <memory>
#include <vector>
#include <map>
#include <set>

class ENFAState {
    bool accepting;
    std::map<char, std::set<ENFAState*>> transitions;
    explicit ENFAState(bool accepting) : accepting{accepting} {}

    friend class ENFA;

public:
    void addTransition(char c, ENFAState *to);
    void setAccepting(bool newAccepting) { accepting = newAccepting; }
};

class ENFA {
    static const char eps{'~'};

    ENFAState* startState;

    std::vector<std::unique_ptr<ENFAState>> states;

public:
    explicit ENFA(bool startIsAccepting) : startState{}, states{} {
        startState = createState(startIsAccepting);
    }

    ENFAState* getStartState() {return startState;}
    std::vector<ENFAState*> getFinalStates();

    ENFAState* createState(bool accepting);

    friend ENFA operator+(ENFA&& enfa1, ENFA&& enfa2);

};



#endif //TA_PRAC_2_ENFA_H

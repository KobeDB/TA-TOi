//
// Created by kobedb on 26.04.22.
//

#include "ENFA.h"

using namespace std;

ENFA::ENFA(ENFAState* startState) : startState{startState}, states{}
{
    states.push_back(std::unique_ptr<ENFAState>{startState});
}

std::vector<ENFAState*> ENFA::getFinalStates() {
    vector<ENFAState*> finalStates;
    for(auto& s : states) {
        if(s->accepting)
            finalStates.push_back(s.get());
    }
    return finalStates;
}

ENFAState *ENFA::createState(bool accepting) {
    ENFAState* newState = new ENFAState{accepting};
    states.push_back(std::unique_ptr<ENFAState>{newState});
    return newState;
}

void ENFAState::addTransition(char c, ENFAState *to)
{
    transitions[c] = to;
}

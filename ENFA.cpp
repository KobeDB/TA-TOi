//
// Created by kobedb on 26.04.22.
//

#include "ENFA.h"

#include <iostream>

using namespace std;

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

ENFA operator+(ENFA &&enfa1, ENFA &&enfa2)
{
    ENFA sumEnfa{false};

    // We use ~ for epsilon
    sumEnfa.getStartState()->addTransition(ENFA::eps, enfa1.getStartState());
    sumEnfa.getStartState()->addTransition(ENFA::eps, enfa2.getStartState());

    auto enfa1Finals = enfa1.getFinalStates();
    if(enfa1Finals.size() != 1) cerr << "ERROR: enfa1 doesnt have exactly one final state!\n";
    auto enfa1Final = enfa1Finals[0];

    auto enfa2Finals = enfa2.getFinalStates();
    if(enfa2Finals.size() != 1) cerr << "ERROR: enfa2 doesnt have exactly one final state!\n";
    auto enfa2Final = enfa2Finals[0];

    auto final = sumEnfa.createState(true);
    enfa1Final->addTransition(ENFA::eps, final);
    enfa2Final->addTransition(ENFA::eps, final);

    enfa1Final->setAccepting(false);
    enfa2Final->setAccepting(false);

    // Finally, move State-ownership from the 2 ENFAs to the new sumEnfa
    std::move(enfa1.states.begin(), enfa1.states.end(), std::back_inserter(sumEnfa.states));
    std::move(enfa2.states.begin(), enfa2.states.end(), std::back_inserter(sumEnfa.states));

    return sumEnfa;
}

void ENFAState::addTransition(char c, ENFAState *to)
{
    transitions[c].insert(to);
}

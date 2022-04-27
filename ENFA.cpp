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

ENFA add(ENFA &&enfa1, ENFA &&enfa2)
{
    ENFA sumEnfa{false};

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

    // Finally, move ENFAState-ownership from the 2 ENFAs to the new sumEnfa
    std::move(enfa1.states.begin(), enfa1.states.end(), std::back_inserter(sumEnfa.states));
    std::move(enfa2.states.begin(), enfa2.states.end(), std::back_inserter(sumEnfa.states));

    return sumEnfa;
}

ENFA concat(ENFA &&enfa1, ENFA &&enfa2) {
    ENFA concatEnfa{};
    concatEnfa.startState = enfa1.getStartState();

    auto enfa1Finals = enfa1.getFinalStates();
    if(enfa1Finals.size() != 1) cerr << "ERROR: enfa1 doesnt have exactly one final state!\n";
    auto enfa1Final = enfa1Finals[0];

    enfa1Final->addTransition(ENFA::eps, enfa2.getStartState());
    enfa1Final->setAccepting(false);

    // Finally, move ENFAState-ownership from the 2 ENFAs to the new concatEnfa
    std::move(enfa1.states.begin(), enfa1.states.end(), std::back_inserter(concatEnfa.states));
    std::move(enfa2.states.begin(), enfa2.states.end(), std::back_inserter(concatEnfa.states));

    return concatEnfa;
}

ENFA kleenestar(ENFA &&enfa) {
    ENFA starNFA{false};

    starNFA.getStartState()->addTransition(ENFA::eps, enfa.getStartState());




    return ENFA(false);
}

void ENFAState::addTransition(char c, ENFAState *to)
{
    transitions[c].insert(to);
}

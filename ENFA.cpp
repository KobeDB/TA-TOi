//
// Created by kobedb on 26.04.22.
//

#include "ENFA.h"

#include <iostream>
#include <vector>

using namespace std;

int uid;
int getUid() { uid++; return uid; }

std::vector<ENFAState*> ENFA::getFinalStates() {
    vector<ENFAState*> finalStates;
    for(auto& s : states) {
        if(s->accepting)
            finalStates.push_back(s.get());
    }
    return finalStates;
}

ENFAState *ENFA::createState(bool accepting) {
    ENFAState* newState = new ENFAState{accepting, to_string(getUid())};
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

    // Eps-transition from starNFA's startState to the final state
    auto final = starNFA.createState(true);
    starNFA.getStartState()->addTransition(ENFA::eps, final);

    // Eps-transition from enfa's final state to the new final state
    auto enfaFinals = enfa.getFinalStates();
    if(enfaFinals.size() != 1) cerr << "ERROR: enfa doesnt have exactly one final state!\n";
    auto enfaFinal = enfaFinals[0];
    enfaFinal->addTransition(ENFA::eps, final);

    // Eps-transition from enfa's final state to its start state
    enfaFinal->addTransition(ENFA::eps, enfa.getStartState());

    enfaFinal->setAccepting(false);

    // Finally, move ENFAState-ownership from the ENFA to the new starNFA
    std::move(enfa.states.begin(), enfa.states.end(), std::back_inserter(starNFA.states));

    return starNFA;
}

void ENFAState::addTransition(char c, ENFAState *to)
{
    transitions[c].insert(to);
}

nlohmann::json ENFA::toJson() const
{
    using json = nlohmann::json;
    json enfaDesc;

    enfaDesc["type"] = "ENFA";

    set<string> alphabet;
    vector<json> statesDesc;
    vector<json> transitionsDesc;
    for(const auto& s : states) {
        for(const auto& t : s->transitions) {
            for(const auto& to : t.second) {
                transitionsDesc.push_back({{"from",s->name},
                                           {"to", to->name},
                                           {"input", string{t.first}}});
            }
            if(t.first == eps) continue;
            alphabet.insert(string{t.first});
        }
        statesDesc.push_back({
                {"name", s->name},
                {"starting", s.get() == startState},
                {"accepting", s->accepting}}
                );

    }
    enfaDesc["alphabet"] = vector<string>{alphabet.begin(),alphabet.end()};
    enfaDesc["states"] = statesDesc;
    enfaDesc["transitions"] = transitionsDesc;

    return enfaDesc;
}
//
// Created by Kobe De Broeck on 19-2-2022.
//

#include "DFA.h"

#include <sstream>
#include <fstream>
#include <iostream>

#include <iomanip>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

void DFA::initDFA(const json &dfaDesc) {
    if(dfaDesc["type"] != "DFA") cerr << "File does not describe a DFA\n";

    auto alphabetVec = dfaDesc["alphabet"];
    alphabet = {alphabetVec.begin(), alphabetVec.end()};

    auto statesVec = dfaDesc["states"];
    for(auto state : statesVec) {
        State newState = {state["name"], state["starting"], state["accepting"]};
        states[newState.name] = newState;
        if(state["starting"]) {
            this->startState = newState;
        }
    }

    auto transitions = dfaDesc["transitions"];
    for(auto transition : transitions) {
        State from = states[transition["from"]];
        State to = states[transition["to"]];

        string input = transition["input"];
        if(alphabet.find(input) == alphabet.end())
            cerr << "Inconsistency in DFA description file: input symbol not in alphabet\n";

        this->transitionTable[{from, input}] = to;
    }

    // TF-algorithm

    statePairs = fillTable(alphabet, states, transitionTable);

    printTable();
}

// The TF-algorithm
map<pair<State,State>, bool> /*statePairs*/ fillTable (
        const set<string>& alphabet,
        const map<string, State>& states,
        const map<pair<State, string>, State>& transitionTable
                )
{
    map<pair<State,State>, bool> statePairs;

    // Filling the statePairs map
    for(const auto& p1 : states) {
        State s1 = p1.second;
        for(const auto& p2 : states) {
            State s2 = p2.second;
            if(s1.name < s2.name ) {
                statePairs[{s2,s1}] = false; // Initially not marked
            }
        }
    }

    bool madeProgress = true; // true if previous pass marked at least one state
    while(madeProgress) {
        madeProgress = false;
        for (auto &p: statePairs) {
            if (p.second) continue; // If already marked continue

            auto statePair = p.first;

            // Basis
            if (statePair.first.accepting != statePair.second.accepting) {
                p.second = true; // Mark pair
                madeProgress = true;
                continue;
            }

            //Inductive
            for (const auto &symbol: alphabet) {
                State firstNext = transitionTable.at({statePair.first, symbol});
                State secondNext = transitionTable.at({statePair.second, symbol});
                if (firstNext == secondNext) continue;
                // Our statePairs table requires that the first state of the pair
                // must come later in the alphabet as the second
                if (firstNext < secondNext) {
                    State temp = secondNext;
                    secondNext = firstNext;
                    firstNext = temp;
                }
                if (statePairs.at({firstNext, secondNext})) {
                    // Mark this pair of states
                    p.second = true;
                    madeProgress = true;
                    break;
                }
            }
        }
    }

    return statePairs;
}

DFA::DFA(const std::string &fileName)
{
    ifstream ifs{fileName};
    if(!ifs) cerr << "Could not open DFA file\n";
    json dfaDesc;
    ifs >> dfaDesc;

    initDFA(dfaDesc);
}

DFA::DFA(const nlohmann::json& dfaDesc, int) {
    initDFA(dfaDesc);
}

bool DFA::accepts(const std::string& s) const
{
    State currentState = startState;

    istringstream iss{s};
    for(char c; iss >> c; ) {
        if(alphabet.find(string{c}) == alphabet.end())
            cerr << "Unknown symbol in input string\n";
        currentState = transitionTable.at({currentState, string{c}});
    }

    return currentState.accepting;
}

void DFA::printTable() const
{
    string prevRowName;
    for(const auto& p : statePairs) {
        const auto& statePair = p.first;
        if(prevRowName.empty()) {
            prevRowName = statePair.first.name;
            cout << statePair.first.name << "\t";
        }
        if(prevRowName < statePair.first.name) {
            cout << "\n";
            cout << statePair.first.name << "\t";
        }
        //cout << p.first << " | " << std::boolalpha << p.second << "\n";
        cout << (p.second? 'X' : '-') << "\t";

        prevRowName = statePair.first.name;
    }
    // Print the bottom row
    cout << "\n \t";
    for(auto it = states.begin(); it != states.end();) {
        auto s = (*it).second;
        if(++it == states.end()) break;
        cout << s.name << "\t";
    }
    cout << "\n";
}

void DFA::print() const
{
    json dfaDesc;

    dfaDesc["type"] = "DFA";

    dfaDesc["alphabet"] = alphabet;

    set<json> statesDesc;
    vector<json> transitionsDesc;
    for(const auto& t : transitionTable) {
        json stateDesc;
        State from = t.first.first;
        string fromName = from.name;
        stateDesc["name"] = fromName;
        stateDesc["starting"] = from.starting;
        stateDesc["accepting"] = from.accepting;
        statesDesc.insert(stateDesc);

        json transitionDesc;
        transitionDesc["from"] = fromName;
        transitionDesc["to"] = t.second.name;
        transitionDesc["input"] = t.first.second;
        transitionsDesc.push_back(transitionDesc);
    }
    dfaDesc["states"] = statesDesc;
    dfaDesc["transitions"] = transitionsDesc;


    cout << setw(4) << dfaDesc << endl;
}



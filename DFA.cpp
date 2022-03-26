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
        if(state["starting"]) {
            this->startState = state["name"];
        }
        if(state["accepting"]) {
            this->finalStates.insert((string)state["name"]);
        }
    }

    auto transitions = dfaDesc["transitions"];
    for(auto transition : transitions) {
        string from = transition["from"];
        string to = transition["to"];
        string input = transition["input"];
        if(alphabet.find(input) == alphabet.end())
            cerr << "Inconsistency in DFA description file: input symbol not in alphabet\n";
        this->transitionTable[{from, input}] = to;
    }
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
    string currentState = startState;

    istringstream iss{s};
    for(char c; iss >> c; ) {
        if(alphabet.find(string{c}) == alphabet.end())
            cerr << "Unknown symbol in input string\n";
        currentState = transitionTable.at({currentState, string{c}});
    }

    return finalStates.find(currentState) != finalStates.end();
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
        string fromName = t.first.first;
        stateDesc["name"] = fromName;
        stateDesc["starting"] = (fromName == startState);
        stateDesc["accepting"] = (finalStates.find(fromName) != finalStates.end());
        statesDesc.insert(stateDesc);

        json transitionDesc;
        transitionDesc["from"] = fromName;
        transitionDesc["to"] = t.second;
        transitionDesc["input"] = t.first.second;
        transitionsDesc.push_back(transitionDesc);
    }
    dfaDesc["states"] = statesDesc;
    dfaDesc["transitions"] = transitionsDesc;


    cout << setw(4) << dfaDesc << endl;
}



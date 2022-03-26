//
// Created by Kobe De Broeck on 23-3-2022.
//

#include "NFA.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <unordered_set>

#include <iomanip>
#include "json.hpp"

using namespace std;

using json = nlohmann::json;


NFA::NFA(const std::string& nfaDescFile) {
    ifstream ifs{nfaDescFile};
    if(!ifs) cerr << "Could not open NFA desc file\n";
    json nfaDesc;
    ifs >> nfaDesc;

    if(nfaDesc["type"] != "NFA") cerr << "File does not describe an NFA\n";

    auto alphabetVec = nfaDesc["alphabet"];
    alphabet = {alphabetVec.begin(), alphabetVec.end()};

    bool startingAlreadyInitialized = false;
    auto statesVec = nfaDesc["states"];
    for(const auto& state : statesVec) {
        State s {state["name"], state["starting"], state["accepting"]};
        states[s.name] = s;
        if(s.starting) {
            if(startingAlreadyInitialized) {
                cerr << "NFA ERROR: Two start states in description file!";
            }
            startState = s;
            startingAlreadyInitialized = true;
        }
    }

    if(!startingAlreadyInitialized) {
        cerr << "No start state given in NFA desc file!";
    }

    auto transitions = nfaDesc["transitions"];
    for(auto transition : transitions) {
        string from = transition["from"];
        string to = transition["to"];
        string input = transition["input"];
        if(alphabet.find(input) == alphabet.end())
            cerr << "Inconsistency in NFA description file: input symbol not in alphabet\n";

        transitionTable[{states.at(from), input}].insert(states.at(to));
    }
}

set<string> getNFADestinations(const std::map<std::pair<std::string, std::string>, std::set<std::string>>& table,
                               const string& dfaFromName, const string& inputSymbol)
{
    try{
        return table.at({dfaFromName, inputSymbol});
    }
    catch(...) {
        cerr <<"no worries\n";
    }
    return {};
}

std::string toDFAString(const set<NFA::State> &s)
{
    string result = "{";
    int i = 0;
    for(const auto& state : s) {
        if(i != 0) {
            result += ",";
        }
        result += state.name;
        i++;
    }
    result += "}";
    return result;

}

json toDFAJsonTransition(const std::set<NFA::State>& from, const std::string& input, const std::set<NFA::State>& to)
{
    json transition;
    transition["from"] = toDFAString(from);
    transition["input"] = input;
    transition["to"] = toDFAString(to);
    return transition;
}

set<NFA::State> NFA::getNFADestinations(const NFA::State &s, const std::string& input) const
{
    if(transitionTable.find({s, input}) != transitionTable.end()) {
        return transitionTable.at({s,input});
    }
    return {};
}

bool NFA::containsStartingState(set<State> s) const
{
    if(s.size() != 1) return false;
    return (s.find(startState) != s.end());
}

bool NFA::containsAcceptingState(const set<State>& s) const
{
    for(const auto& state : s) {
        if(state.accepting) {
            return true;
        }
    }
    return false;
}

DFA NFA::toDFA() const {
    json dfaDesc;

    dfaDesc["type"] = "DFA";
    dfaDesc["alphabet"] = alphabet;

    vector<json> transitions;
    vector<set<State>> dfaStates;
    dfaStates.push_back({startState});

    for (int i = 0; i < dfaStates.size(); i++) {
        if (dfaStates[i].empty()) {
            for (const string &inputSymbol: alphabet) {
                transitions.push_back(toDFAJsonTransition(dfaStates[i], inputSymbol, dfaStates[i]));
            }
            continue;
        }

        for (const string &inputSymbol: alphabet) {
            set<State> unionOfDestinations; // the to-state
            for (const auto &memberState: dfaStates[i]) {
                set<State> dests = getNFADestinations(memberState, inputSymbol);
                unionOfDestinations.insert(dests.begin(), dests.end());
            }
            json t = toDFAJsonTransition(dfaStates[i], inputSymbol, unionOfDestinations);
//            setbuf(stdout, 0);
//            cout << setw(4) << t << "Hello\n";
            transitions.push_back(t);

            // Add the to-state if it was not yet present in the dfaStates vector,
            // so its DFA transitions will be calculated in a later pass of the dfaStates-for-loop
            if (std::find(dfaStates.begin(), dfaStates.end(), unionOfDestinations) == dfaStates.end()) {
                dfaStates.push_back(unionOfDestinations);
            }
        }
    }

    dfaDesc["transitions"] = transitions;
    dfaDesc["states"] = vector<json>{};
    for (const auto &state: dfaStates) {
        json jsonDFAState;
        jsonDFAState["name"] = toDFAString(state);
        jsonDFAState["starting"] = containsStartingState(state);
        jsonDFAState["accepting"] = containsAcceptingState(state);
        dfaDesc["states"].push_back(jsonDFAState);
    }

    int dummy{};
    return {dfaDesc, dummy};
}
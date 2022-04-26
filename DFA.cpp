//
// Created by Kobe De Broeck on 19-2-2022.
//

#include "DFA.h"

#include <sstream>
#include <fstream>
#include <iostream>
#include <unordered_set>

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
        ENFAState newState = {state["name"], state["starting"], state["accepting"]};
        states[newState.name] = newState;
        if(state["starting"]) {
            this->startState = newState;
        }
    }

    auto transitions = dfaDesc["transitions"];
    for(auto transition : transitions) {
        ENFAState from = states[transition["from"]];
        ENFAState to = states[transition["to"]];

        string input = transition["input"];
        if(alphabet.find(input) == alphabet.end())
            cerr << "Inconsistency in DFA description file: input symbol not in alphabet\n";

        this->transitionTable[{from, input}] = to;
    }

    // TF-algorithm

    statePairs = fillTable(alphabet, states, transitionTable);

    //printTable();
}

// The TF-algorithm
map<pair<ENFAState,ENFAState>, bool> /*statePairs*/ fillTable (
        const set<string>& alphabet,
        const map<string, ENFAState>& states,
        const map<pair<ENFAState, string>, ENFAState>& transitionTable
                )
{
    map<pair<ENFAState,ENFAState>, bool> statePairs;

    // Filling the statePairs map
    for(const auto& p1 : states) {
        ENFAState s1 = p1.second;
        for(const auto& p2 : states) {
            ENFAState s2 = p2.second;
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
                ENFAState firstNext = transitionTable.at({statePair.first, symbol});
                ENFAState secondNext = transitionTable.at({statePair.second, symbol});
                if (firstNext == secondNext) continue;
                // Our statePairs table requires that the first state of the pair
                // must come later in the alphabet as the second
                if (firstNext < secondNext) {
                    ENFAState temp = secondNext;
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
    ENFAState currentState = startState;

    istringstream iss{s};
    for(char c; iss >> c; ) {
        if(alphabet.find(string{c}) == alphabet.end())
            cerr << "Unknown symbol in input string\n";
        currentState = transitionTable.at({currentState, string{c}});
    }

    return currentState.accepting;
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
        ENFAState from = t.first.first;
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

struct EquiClass {
    set<ENFAState> equivalents;
    ENFAState representative;
    EquiClass(const ENFAState& representative) : representative{representative}, equivalents{representative} {}

    string toString() const {
        stringstream ss;
        ss << "{";
        bool first = true;
        for(const auto& state : equivalents) {
            if(!first) ss << ", ";
            first = false;
            ss << state.name;
        }
        ss << "}";
        string result = ss.str();
        return result;
    }

    bool isStarting() const {
        if(equivalents.empty()) cerr << "ERROR: equivalents is empty!\n";
        return equivalents.begin()->starting;
    }
    bool isAccepting() const {
        if(equivalents.empty()) cerr << "ERROR: equivalents is empty!\n";
        return equivalents.begin()->accepting;
    }

    bool operator<(const EquiClass& other) const {return equivalents < other.equivalents;}
};

EquiClass getEquiClass(const ENFAState& s, const set<EquiClass>& equiClasses)
{
    for(const auto& equiClass : equiClasses) {
        if(find(equiClass.equivalents.begin(), equiClass.equivalents.end(), s) == equiClass.equivalents.end()) continue;
        return equiClass;
    }
    cerr << "No getEquiClass for state s\n";
    return {State{"ERR"}};
}

DFA DFA::minimize() const {
    json minDFADesc;
    minDFADesc["type"] = "DFA";
    minDFADesc["alphabet"] = alphabet;

    set<EquiClass> equivalenceClasses;
    for(const auto& s : states) {
        const auto& state = s.second;
        EquiClass equivalenceClass{state};
        for(const auto& p : statePairs) {
            if(p.second) continue; // If pair is marked continue
            const auto& statePair = p.first;
            if(statePair.first.name == state.name) {
                equivalenceClass.equivalents.insert(statePair.second);
            }
            else if(statePair.second.name == state.name) {
                equivalenceClass.equivalents.insert(statePair.first);
            }
            else {
                continue;
            }
        }
        equivalenceClasses.insert(equivalenceClass);
    }

    json minimizedTransitions = vector<json>{};
    for(const auto& t : transitionTable) {
        const ENFAState& from = t.first.first;
        const ENFAState& to = t.second;
        EquiClass fromEquiClass = getEquiClass(from, equivalenceClasses);
        if(from != fromEquiClass.representative) continue;
        json minTransition;
        minTransition["from"] = fromEquiClass.toString();
        minTransition["to"] = getEquiClass(to, equivalenceClasses).toString();
        minTransition["input"] = t.first.second;
        minimizedTransitions.push_back(minTransition);
    }
    minDFADesc["transitions"] = minimizedTransitions;
//    setbuf(stdout, 0);
//    cout << setw(4) << minimizedTransitions;

    json minimizedStates = vector<json>{};
    for(const auto& equiClass : equivalenceClasses) {
        json minState;
        minState["name"] = equiClass.toString();
        minState["starting"] = equiClass.isStarting();
        minState["accepting"] = equiClass.isAccepting();
        minimizedStates.push_back(minState);
    }
    minDFADesc["states"] = minimizedStates;
//    setbuf(stdout, 0);
//    cout << setw(4) << minimizedStates;

    int dummy{};
    return {minDFADesc, dummy};
}

bool DFA::operator==(const DFA &other) const
{
    if(alphabet != other.alphabet) return false;

    map<string, ENFAState> unionOfStates{states};
    map<pair<ENFAState, string>, ENFAState> unionOfTransitions{transitionTable};
    unionOfStates.insert(other.states.begin(), other.states.end());
    unionOfTransitions.insert(other.transitionTable.begin(), other.transitionTable.end());

    auto TF_table = fillTable(alphabet, unionOfStates, unionOfTransitions);
    ENFAState firstStartState = startState;
    ENFAState secondStartState = other.startState;
    if(firstStartState < secondStartState) {
        swap(firstStartState, secondStartState);
    }
    ::printTable(unionOfStates, TF_table);
    return !TF_table.at({firstStartState, secondStartState});
}



void printTable(const std::map<std::string, ENFAState>& states, const std::map<std::pair<ENFAState,ENFAState>, bool>& statePairs )
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

void DFA::printTable() const {
    ::printTable(states, statePairs);
}



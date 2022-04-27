#include <iostream>
#include <fstream>
#include <iomanip>
#include "json.hpp"

#include "DFA.h"
#include "NFA.h"
#include "RE.h"

using namespace std;

using json = nlohmann::json;

void jsonToDot(ostream& os, json faDesc)
{
    os << "digraph finite_state_machine {";
    os << "rankdir=LR;";

    os << "node [shape = doublecircle];";
    vector<json> states = faDesc["states"];
    string startState;
    for(const auto& s : states) {
        if(s["starting"])
            startState = string{s["name"]};
        if(s["accepting"])
            os << string{s["name"]} << ";";
    }

    os << "node [shape = point ]; _qi ;";

    os << "node [shape = circle];";
    vector<json> transitions = faDesc["transitions"];
    os << "_qi -> " << startState << ";";
    for(const auto& t : transitions) {
        os << string{t["from"]} << " -> " << string{t["to"]} << "[label = \"" << string{t["input"]} <<"\"] ;";
    }

    os << "}";
}

int main(){
    RE re{".*+01.10", 'e'};
    ENFA enfa = re.toENFA();
    json enfaDesc = enfa.toJson();
    cout << setw(4) << enfaDesc;

    ofstream ofs{"enfa.dot"};
    jsonToDot(ofs, enfaDesc);

    cout << "Hello there\n";
    return 0;
}
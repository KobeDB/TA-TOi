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

struct REDesc { string name; string regex; char eps; };

int main(){

    vector<REDesc> regexes = {{"regex0", ".*+01.10", 'e'},
                              {"regex1", "e", 'e'},
                              {"regex2", "+ea", 'e'},
                              {"regex3", ".ee", 'e'},
                              {"vis", "+..vis ......vislijn", 'e'}
                              };

    ofstream dotCompilationScript{"regex_outputs/dot_script.sh"};
    for(const auto& reDesc : regexes) {
        RE re{reDesc.regex, reDesc.eps};
        ENFA enfa = re.toENFA();
        json enfaDesc = enfa.toJson();
        string outFilename = reDesc.regex;
        ofstream ofs{"regex_outputs/" + reDesc.name};
        if(!ofs) cerr << "rip\n";
        jsonToDot(ofs, enfaDesc);

        string compilationScriptFilename = string{"\\\""} + outFilename + string{"\\\""};
        dotCompilationScript << "dot ./" + reDesc.name + " -Tsvg > ./" << reDesc.name << ".svg \n";
    }

    cout << "Hello there\n";
    return 0;
}
#include <iostream>
#include <fstream>
#include <iomanip>
#include "json.hpp"

#include "DFA.h"
#include "NFA.h"

using namespace std;

using json = nlohmann::json;

int main(){
    DFA dfa("DFA.json");
    cout << boolalpha << dfa.accepts("0010110100") << endl << dfa.accepts("0001") << endl;
    //dfa.print();


    NFA nfa("input-ssc2.json");
    nfa.toDFA().print();

    return 0;
}
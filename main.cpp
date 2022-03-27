#include <iostream>
#include <fstream>
#include <iomanip>
#include "json.hpp"

#include "DFA.h"
#include "NFA.h"

using namespace std;

using json = nlohmann::json;

int main(){
    DFA dfa("DFA-TF.json");
    cout << boolalpha << dfa.accepts("aaeeeeaa") << endl << dfa.accepts("aaeeaaea") << endl;
    //dfa.print();


    //NFA nfa("input-ssc2.json");
    //nfa.toDFA().print();

    return 0;
}
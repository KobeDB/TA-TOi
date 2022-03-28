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
    DFA mindfa = dfa.minimize();
    dfa.printTable();
    //mindfa.print();
    cout << boolalpha << (dfa == mindfa) << endl;

    return 0;
}
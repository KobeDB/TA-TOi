#include <iostream>
#include <fstream>
#include <iomanip>
#include "json.hpp"

#include "DFA.h"
#include "NFA.h"
#include "RE.h"

using namespace std;

using json = nlohmann::json;

int main(){
    RE re{"+ab", 'e'};
    ENFA enfa = re.toENFA();
    json enfaDesc = enfa.toJson();
    cout << setw(4) << enfaDesc;
    cout << "Hello there\n";
    return 0;
}
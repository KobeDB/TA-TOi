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
    RE re{".ab", 'e'};
    ENFA enfa = re.toENFA();
    cout << "Hello there\n";
    return 0;
}
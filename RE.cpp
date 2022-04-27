//
// Created by kobedb on 27.04.22.
//

#include "RE.h"

#include <iostream>

ENFA RE::toENFA()
{
    char ch;
    ss >> ch;
    switch(ch) {
        case '+': return toENFA() + toENFA();
        //case '-': return toENFA() - toENFA();
        //case '*': return toENFA().closure();
        default: {
            ENFA enfa{false};
            auto final = enfa.createState(true);
            enfa.getStartState()->addTransition(ch, final);
            return enfa;
        }
    }
    std::cerr << "How did we even get here?!\n";
    return ENFA(false);
}

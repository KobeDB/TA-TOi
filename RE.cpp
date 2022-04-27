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
        case '+': {
            ENFA enfa1 = toENFA();
            ENFA enfa2 = toENFA();
            return add(std::move(enfa1), std::move(enfa2)); // Union
        }
        case '.': {
            ENFA enfa1 = toENFA();
            ENFA enfa2 = toENFA();
            return concat(std::move(enfa1), std::move(enfa2)); //Concatenation
        }
        case '*': return kleenestar(toENFA()); // Kleene Closure
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

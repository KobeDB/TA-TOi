//
// Created by kobedb on 27.04.22.
//

#ifndef TA_PRAC_2_RE_H
#define TA_PRAC_2_RE_H

#include <sstream>

#include "ENFA.h"

class RE {
    const std::string expr;
    char eps;
public:
    RE(const std::string& expr, char e);

    ENFA toENFA();

};


#endif //TA_PRAC_2_RE_H

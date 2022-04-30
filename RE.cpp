//
// Created by kobedb on 27.04.22.
//

#include "RE.h"

#include <iostream>
#include <stack>

using namespace std;

ENFA RE::toENFA()
{
    stack<ENFA> operands;

    istringstream ss{expr};
    char c;
    while(ss >> c) {
        if(isalnum(c)) {
            ENFA enfa{false};
            auto final = enfa.createState(true);
            enfa.getStartState()->addTransition(c == eps? ENFA::eps : c, final);
            operands.push(std::move(enfa));
            continue;
        }
        if(c == '+') {
            ENFA enfa2 = std::move(operands.top()); operands.pop(); // Top of stack has 2nd operand
            ENFA enfa1 = std::move(operands.top()); operands.pop(); //
            operands.push(add(std::move(enfa1), std::move(enfa2))); // Union
        }
        if(c == '.') {
            ENFA enfa2 = std::move(operands.top()); operands.pop(); // Top of stack has 2nd operand: Concatentation is not Commutative!!
            ENFA enfa1 = std::move(operands.top()); operands.pop(); //
            operands.push(concat(std::move(enfa1), std::move(enfa2))); // Concatenation
        }
        if(c == '*') {
            ENFA oper = std::move(operands.top()); operands.pop();
            operands.push(kleenestar(std::move(oper))); // Kleene Closure
        }
    }
    return std::move(operands.top());
}

string makeConcatsExplicit( const string& in)
{
    string result;

    istringstream iss{in};
    char c;
    while(iss >> c) {
        result += c;
        if(c == '(' || c == '+') continue;

        char next = (char)iss.peek();
        if( next == EOF ) break;
        if(isalnum(next) || next == '(') {
            result += ".";
            continue;
        }
    }

    return result;
}

int precedence(char c)
{
    switch(c) {
        case '+': return 0;
        case '.': return 1;
        case '*': return 2;
        default: {
            cerr << "int precedence(char): unknown operator: " << c << "\n";
            return -1;
        }
    }
}

string infixToPostfix( const string& infixExpr)
{
    string postfixExpr;
    stack<char> operatorStack;
    for(char c : infixExpr) {
        if(isalnum(c)) {
            postfixExpr += c;
            continue;
        }
        if(c == '(') {
            operatorStack.push(c);
            continue;
        }
        if(c == ')') {
            while(operatorStack.top() != '(') {
                postfixExpr += operatorStack.top();
                operatorStack.pop();
            }
            operatorStack.pop(); // pop '('
            continue;
        }
        if(c == '+' || c == '.' || c == '*') {
            while(!operatorStack.empty() && operatorStack.top() != '(' && precedence(c) <= precedence(operatorStack.top())) {
                postfixExpr += operatorStack.top();
                operatorStack.pop();
            }
            operatorStack.push(c);
            continue;
        }
    }

    while(!operatorStack.empty()) {
        postfixExpr += operatorStack.top();
        operatorStack.pop();
    }

    return postfixExpr;
}

RE::RE(const std::string& expr, char e) : expr{infixToPostfix(makeConcatsExplicit(expr))}, eps{e} {}



//ENFA RE::toENFA()
//{
//    char ch;
//    ss >> ch;
//    switch(ch) {
//        case '+': {
//            ENFA enfa1 = toENFA();
//            ENFA enfa2 = toENFA();
//            return add(std::move(enfa1), std::move(enfa2)); // Union
//        }
//        case '.': {
//            ENFA enfa1 = toENFA();
//            ENFA enfa2 = toENFA();
//            return concat(std::move(enfa1), std::move(enfa2)); //Concatenation
//        }
//        case '*': return kleenestar(toENFA()); // Kleene Closure
//        default: {
//            ENFA enfa{false};
//            auto final = enfa.createState(true);
//            if(ch == eps) {
//                enfa.getStartState()->addTransition(ENFA::eps, final);
//            }
//            else {
//                enfa.getStartState()->addTransition(ch, final);
//            }
//            return enfa;
//        }
//    }
//    std::cerr << "How did we even get here?!\n";
//    return ENFA(false);
//}


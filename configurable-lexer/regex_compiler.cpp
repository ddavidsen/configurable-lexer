#include <iostream>
#include <stack>
#include <string>

#include "regex_compiler.h"
#include "NFA.h"

using namespace std;


// define precedence and associativity of operators
int Precedence(char op) 
{
    if (op == '*') return 3;  // highest precedence for Kleene star (*)
    if (op == '.') return 2;  // concatenation (.) has lower precedence than *
    if (op == '|') return 1;  // alternation (|) has the lowest precedence
    return 0;
}


// check if a character is an operand (alpha)
bool IsOperand(char c) 
{
    return isalpha(c);
}


// convert an infix regular expression to postfix
string InfixToPostfix(const string& infix) 
{
    stack<char> ops;        // stack for operators
    string postfix = "";    // resulting postfix expression 'queue'
    
    for (int i = 0; i < infix.size(); i++) 
    {
        char c = infix[i];
        
        if (IsOperand(c)) 
        {
            // if c is an operand, put it on the output queue
            postfix += c;
        }
        else if (c == '(') 
        {
            // if it's an opening parenthesis, push it onto the stack
            ops.push(c);
        }
        else if (c == ')') 
        {
            // pop off the stack until matching '(' is found
            while (!ops.empty() && ops.top() != '(') 
            {
                postfix += ops.top();
                ops.pop();
            }
            ops.pop();  // pop the '('
        }
        else if (c == '|') 
        {
            // if it's an alternation operator, pop operators of higher precedence
            while (!ops.empty() && ops.top() != '(' && Precedence(ops.top()) >= Precedence(c)) 
            {
                postfix += ops.top();
                ops.pop();
            }
            ops.push(c);  // push the alternation operator onto the stack
        }
        else if (c == '.') 
        {
            // do same for concatenation (.)
            while (!ops.empty() && ops.top() != '(' && Precedence(ops.top()) >= Precedence(c)) 
            {
                postfix += ops.top();
                ops.pop();
            }
            ops.push(c);  // push the . operator onto the stack
        }
        else if (c == '*') 
        {
            // and for (*)
            ops.push(c);  // push the * operator onto the stack
        }
    }
    
    // pop all remaining operators off the stack
    while (!ops.empty()) 
    {
        postfix += ops.top();
        ops.pop();
    }
    
    return postfix;
}

// this creates the sort of "mini nfas" for each character and then combines them using
// the already defined union, concat, and kleene functions
NFA PostfixtoNFA(const string& postfix) {
    stack<NFA> stack;

    // iterate though each character in the postfix string
    for (char c : postfix) {
    // for each character, determine what type it is
        // regular char
        if (isalpha(c)) {
            // need to build the single cjar nfa, which has 2 states and the initial
            // has an a transition to accepting given the single char
            NFA mininfa;
            mininfa.SetInitialState(0);
            mininfa.SetFinalState(1);
            mininfa.SetMaxLabel(1);
            mininfa.SetAlphabet({ c });
            mininfa.AddTransition(0, { 1 }, c);
            stack.push(mininfa);
        }

        else if (c == '*') {
            // perform kleene on the current nfa
            NFA nfaAtTop = stack.top();
            stack.pop();
            nfaAtTop.Kleene();
            stack.push(nfaAtTop);
        }

        else if (c == '.') {
            // perform concat on the top 2 nfas
            // nfa1 should come before nfa2 in the combined nfa,
            // so the nfa on top will be nfa2
            NFA nfa2 = stack.top();
            stack.pop();
            NFA nfa1 = stack.top();
            stack.pop();

            nfa1.Concat(nfa2);
            stack.push(nfa1);
        }

        else if (c == '|') {
            // perform union on the top 2 nfas
            NFA nfa2 = stack.top();
            stack.pop();
            NFA nfa1 = stack.top();
            stack.pop();

            nfa1.Union(nfa2);
            stack.push(nfa1);
        }
    }

    return stack.top();
}


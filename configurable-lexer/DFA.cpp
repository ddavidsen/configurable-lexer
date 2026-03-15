#include <iostream>
#include <set>
#include <map>
#include <queue>
#include <stack>
#include <string>

#include "DFA.h"


// DFA constructor
DFA::DFA(set<char> A, int I, set<int> F) : alphabet(A), init_state(I), fin_states(F)
{
    Reset();
}

// reset the DFA to the start state and clear any accepted lexeme
void DFA::Reset() 
{
    status = START; 
    current_state = init_state; 
    accepted=false;
    lexeme.clear();
    accepted_lexeme.clear(); 
}

// move one time on the input character c
// update the current state and accepted lexeme if necessary
void DFA::Move(char c)
{
    // If already failed, remain failed.
    if (status == FAIL) {
        return;
    }

    // Check transition from current state on c.
    auto stateIt = dfa_transitionFunction.find(current_state);
    if (stateIt == dfa_transitionFunction.end()) {
        status = FAIL;
        accepted = false;
        return;
    }

    auto transitionIt = stateIt->second.find(c);
    if (transitionIt == stateIt->second.end()) {
        status = FAIL;
        accepted = false;
        return;
    }

    current_state = transitionIt->second;
    lexeme += c;

    // accepted represents "current state is final"
    accepted = (fin_states.count(current_state) > 0);
    if (accepted) {
        status = ACCEPT;
        accepted_lexeme = lexeme;
    }
    else {
        status = POTENTIAL;
    }
}


// print the DFA
void DFA::Print() 
{
    cout << "DFA Transitions:\n";
    for (const auto& dfa_row : dfa_transitionFunction) 
    {
        cout << dfa_row.first << ":\t";
        for (const auto& transition : dfa_row.second) 
            cout << transition.first << ": " << transition.second << " "; 
        cout << endl;
    }
}

//---------------------------------------------------------------------------------
// simulate the DFA
//
// s = so
// c = nextChar(x) 
// while (c != eof)
//    s = move(s,c)
//    c = nextChar(x) 
// return (s in F)
//---------------------------------------------------------------------------------
bool DFA::Simulate(string input) 
{
    // map< int, map<char, int> > Dtran;
    bool first = true;
    cout << "{";
    for(auto &[k,v] : dfa_transitionFunction)
    {
        if (!first) cout << ",";
        cout << k << ":{";
        bool ffirst = true;
        for(auto &[kk,vv] : v)  
        {
            if (!ffirst) cout << ",";
            cout << kk << ":" << vv; 
            ffirst = false;
        }
        cout << "}";
        first = false;
    }
    cout << "}" << endl;

    int s = init_state;
    int pos = 0;
    while (pos < input.length())
    {
        char c = input[pos++];
        Move( c );
        //cout << "state: " << s << endl;
    }
    return fin_states.count(current_state) > 0;
}



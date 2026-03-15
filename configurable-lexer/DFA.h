#ifndef DFA_H
#define DFA_H

#include <set>
#include <map>

using namespace std;

enum DFAstatus {START, FAIL, POTENTIAL, ACCEPT};

//---------------------------------------------------------------------------------
// class DFA
// (S, Σ, δ, s0, F)
//---------------------------------------------------------------------------------
class DFA 
{
public:
    DFA() {}
    DFA(set<char> A, int I, set<int> F);
    void Reset();

    void AddTransition( int src, int dst, char sym) {dfa_transitionFunction[src][sym] = dst;}
    void Move( char c);
    bool Simulate(string);
    DFAstatus GetStatus() {return status;}
    string GetAcceptedLexeme() {return accepted_lexeme;}
    bool GetAccepted() {return accepted;}
    void Print();
    void setAlphabet(set<char> a) { alphabet = a; }
    void SetInitialState(int istate) { init_state = istate; }
    void AddFinalState(int fstate) { fin_states.insert(fstate); }

private:
    map< int, map<char, int> > dfa_transitionFunction;
    DFAstatus status;
    int current_state;
    bool accepted;
    string lexeme;
    string accepted_lexeme;
    
    set<char> alphabet;      // set of input symbols in the alphabet
    int init_state; // initial state of the DFA
    set<int> fin_states;  // final states of the DFA
};

#endif

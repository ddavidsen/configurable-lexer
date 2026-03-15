
#ifndef NFA_H
#define NFA_H

using namespace std;

#include "DFA.h"


//---------------------------------------------------------------------------------
// class NFA
// (S, Σ, δ, s0, F)
//---------------------------------------------------------------------------------
class NFA 
{
public:
    NFA() {}
    NFA(set<char> A, int I, set<int> F) : alphabet(A), init_state(I), fin_states(F), max_node_label(0) {}
    void SetInitialState(int istate) {init_state = istate;}
    void SetFinalState(int fstate) {fin_states.clear(); fin_states.insert(fstate);}
    int GetMaxLabel() {return max_node_label;}
    void AddTransition(int source_state, const set<int>& destination_states, char symbol) {
        set<int>& destinations = transitionFunction[source_state][symbol];
        destinations.insert(destination_states.begin(), destination_states.end());
    }
    void Union(const NFA&);
    void Concat(const NFA&);
    void Kleene();
    set<int> EpsilonClosure(const set<int>& states); 
    DFA NFA2DFA(); 
    void Print() const;
    void SetMaxLabel(int m) { max_node_label = m; }
    void SetAlphabet(set<char> a) { alphabet = a; }

private:
    map< int, map<char, set<int>> > transitionFunction;

    set<char> alphabet;   // set of input symbols in the alphabet
    char epsilon = '_';   // epsilon is represented by '_'
    int init_state;       // initial state of the NFA
    set<int> fin_states;  // final states of the NFA
    int max_node_label;
};

#endif

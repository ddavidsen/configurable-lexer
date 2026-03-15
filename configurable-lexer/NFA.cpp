#include <iostream>
#include <set>
#include <map>
#include <queue>
#include <stack>
#include <string>

#include "NFA.h"


// print the NFA
void NFA::Print() const
{
    cout << "NFA Transitions:\n";
    for (const auto& nfa_row : transitionFunction)
    {
        cout << nfa_row.first << ":\t";
        for (const auto& transition : nfa_row.second)
        {
            cout << transition.first << ": { ";
            for (int state : transition.second)
                cout << state << " ";
            cout << "} ";
        }
        cout << endl;
    }
    cout << "Initial state: " << init_state << endl;
}


// epsilon closure of a set of states in the NFA
set<int> NFA::EpsilonClosure(const set<int>& states)
{
    stack<int> dfs_stack;
    set<int> epsilon_closure_set = states;
    for (int state : states)
        dfs_stack.push(state);
    while (!dfs_stack.empty())
    {
        int state = dfs_stack.top(); dfs_stack.pop();
        // Check all epsilon transitions from this state
        if (transitionFunction[state].find('_') != transitionFunction[state].end())
            for (int next_state : transitionFunction[state]['_'])
                if (epsilon_closure_set.find(next_state) ==
                    epsilon_closure_set.end())
                {
                    epsilon_closure_set.insert(next_state);
                    dfs_stack.push(next_state);
                }
    }
    return epsilon_closure_set;
}

void NFA::Union(const NFA& nfa2) {
    // offset to adjust nfa2 states (to avoid state number conflicts)
	int offset = max_node_label + 1;

    //adjustments based on offset
    for (auto& [source_state, inner_map] : nfa2.transitionFunction) {
        for (auto& [symbol, destination_states] : inner_map) {
            set<int> adjusted_dest_states;
            for (int state : destination_states) {
                adjusted_dest_states.insert(state + offset);
            }
            AddTransition(source_state + offset, adjusted_dest_states, symbol);
        }
    }

    // more adjustments from offset (these are referred to in the rest of the function)
    int nfa2_new_init_state = nfa2.init_state + offset;
    set<int> nfa2_new_final_states;
    for (int i : nfa2.fin_states)
        nfa2_new_final_states.insert(i + offset);

    // new initial state + e transitions
	int new_init_state = max_node_label + 1;
    AddTransition(new_init_state, { init_state }, epsilon);
	AddTransition(new_init_state, { nfa2_new_init_state}, epsilon);

	// new final state + e transitions
	int new_final_state = new_init_state + 1;
    for (int i : fin_states) {
		AddTransition(i, {new_final_state}, epsilon);
    }
    for (int i : nfa2_new_final_states) {
        AddTransition(i, {new_final_state}, epsilon);
    }

    // combine alphabets if not the same
	alphabet.insert(nfa2.alphabet.begin(), nfa2.alphabet.end());

    // update nfa1
    init_state = new_init_state;
	fin_states = {new_final_state};
    max_node_label = max(max_node_label, nfa2.max_node_label + offset);
	max_node_label = max(max_node_label, new_final_state);
}

void NFA::Concat(const NFA& nfa2) {
    // offset to adjust nfa2 states (to avoid state number conflicts)
    int offset = max_node_label + 1;

    //adjustments based on offset
    for (auto& [source_state, inner_map] : nfa2.transitionFunction) {
        for (auto& [symbol, destination_states] : inner_map) {
            set<int> adjusted_dest_states;
            for (int state : destination_states) {
                adjusted_dest_states.insert(state + offset);
            }
            AddTransition(source_state + offset, adjusted_dest_states, symbol);
        }
    }

    // more adjustments from offset (these are referred to in the rest of the function)
    int nfa2_new_init_state = nfa2.init_state + offset;
    set<int> nfa2_new_final_states;
    for (int i : nfa2.fin_states)
        nfa2_new_final_states.insert(i + offset);

    // add e transitions from final states of nfa1 to initial state of nfa2
    for (int i : fin_states) {
        AddTransition(i, { nfa2_new_init_state }, epsilon);
    }

    // update nfa1 
    fin_states = nfa2_new_final_states;

    // combine alphabets if not the same
    alphabet.insert(nfa2.alphabet.begin(), nfa2.alphabet.end());

    // update nfa1
    max_node_label = max(max_node_label, nfa2.max_node_label + offset);
}

void NFA::Kleene() {
    // new initial state, final state + e transitions
    int new_init_state = max_node_label + 1;
    int new_final_state = new_init_state + 1;
    AddTransition(new_init_state, { init_state }, '_');
    AddTransition(new_init_state, { new_final_state }, '_');
    for (int i : fin_states) {
        AddTransition(i, { init_state }, '_');
        AddTransition(i, { new_final_state }, '_');
    }

    //update nfa1
    init_state = new_init_state;
    fin_states = { new_final_state };
    max_node_label = max_node_label + 2;
}

DFA NFA::NFA2DFA() {
    DFA dfa;
    dfa.setAlphabet(alphabet);

    // get dfa's initial state
    set<int> dfa_init_state = EpsilonClosure({ init_state });
    dfa.SetInitialState(0);

    // make a queue to go through each discovered state
    queue<set<int>> dfa_states_queue;
    dfa_states_queue.push(dfa_init_state);

    // give a new int label to each state (which is a set of nfa states)?
    map< set<int>, int > state_conversion;
    state_conversion[dfa_init_state] = 0;
    int next_state_label = 1;

    // this loop gets all transitions and states in the new dfa
    // 1. it gets the state which has been found first
    // 2. it gets a symbol from the alphabet
    // 3. it goes through each nfa state in the dfa state (which is a set of states)
    //    and gets the transition for the current input symbol
    // 4. it takes that set of states and takes the epsilion closure
    // this will be the final transition for that state (from step 1) on that input symbol
    while (!dfa_states_queue.empty()) {
        set<int> currState = dfa_states_queue.front();
        dfa_states_queue.pop();

        // find transitions from current state in worklist
        int currStatesLabel = state_conversion[currState];

        // compute for each symbol in alphabet
        for (char symbol : alphabet) {
            set<int> possibleNextStates;

            // compute transition for this symbol, for each state in state set
            for (int nfaState : currState) {
                if (transitionFunction[nfaState].count(symbol)) {
                    set<int>& destinations = transitionFunction[nfaState][symbol];
                    possibleNextStates.insert(destinations.begin(), destinations.end());
                }
            }

            // now add epsilon closures for each of those states
            set<int> nextState = EpsilonClosure(possibleNextStates);

            // if there is no possible transition, dont add any new state
            if (nextState.empty()) {
                continue;
            }

            // if this state has not been found yet, 
            if (state_conversion.find(nextState) == state_conversion.end()) {
                // add to the dfa's states
                state_conversion[nextState] = next_state_label;
                next_state_label++;

                // and add to the state worklist queue
                dfa_states_queue.push(nextState);
            }

            // finalize this transition for the current input symbol
            int destinationState = state_conversion[nextState];
            dfa.AddTransition(currStatesLabel, destinationState, symbol);
        }
    }

    // all transitions and states have been found at this point
    // now determine which shoudl be accepting states
    for (auto& oneStatePair : state_conversion) {
        set<int> stateSet = oneStatePair.first;  // state set from nfa
        int dfaStateLabel = oneStatePair.second;

        // checks if any nfa states in the dfa state (or state set) are accepting
        // if any are accepting (from the nfa) this dfa state will also be accepting
        for (int i : stateSet) {
            if (fin_states.count(i)) {
                dfa.AddFinalState(dfaStateLabel);
                break;
            }
        }
    }

    return dfa;
}

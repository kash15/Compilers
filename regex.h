#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <algorithm>
#include <queue>
#include<numeric>
#include <sstream>

using namespace std;

#define CONCATENATE '.'
#define eps '`'

class dfa;

/*

An NFA A consists of:

        A finite set I of input symbols
        A finite set S of states
        A next-state function f from S x I into P(S)
        A subset Q of S of accepting states
        An initial state s0 from S 

        denoted as A(I, S, f, Q, s0)
*/

class nfa{

	public:
		int n_states;					//number of states of the NFA
		set<char> input_symbols;
		vector< vector <set<int> > > trans_table;	// Transition table where each entry is set of states for (curent_state, input_symbol)

		nfa(string regex);				//constructor for nfa
		void union_nfa(nfa &n);
		void kleene_star();
		void concatenate_nfa(nfa &n);

		void create_nfa(string regex);
		string convert_to_basic_regex_format(string r);
		string add_concatenate(string input);
		string infix2postfix(string input);
		
		void print_transition_table();

		//set<int> epsilon_closure(int state,set<int> closr);
		set<int> epsilon_closure(int state);
		
		void print_epsilon_closure(int state);

		set<int> epsilon_closure(set<int> states);
		set<int> move(set<int> states,char input_symbol);

		dfa convert_nfa_to_dfa();
};

class dfa{
	
	public:

		string type;						// DFA for what ? (keywords, operators etc.)

		int num_states;
		set<char> alphabet;
		
		vector<bool> final;
		vector<bool> rejecting;	
		
		vector< vector<int> > transitions;

		vector< set<int> > Dstates;				// Storing the states of DFA formed by the states of NFA

		dfa();

		dfa(int num_states,set<char> alphabet,vector<vector<int> > transitions,vector<bool> final);
		
		void calculate_rejecting_states();

		void print_dfa_table();
		void print_final_states();
		void print_rejecting_states();

		bool is_match(string input);
		string max_match(string input);
};

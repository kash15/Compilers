#include<iostream>
#include<stdio.h>
#include<string>
#include<stack>
#include<string.h>
#include<limits.h>
#include <fstream>

#include "regex.h"

#define MAX_SIZE 256

using namespace std;


//Constructor
nfa::nfa(string regex)
{
	input_symbols.insert(eps);								//insert epsilon in the input symbols set
	n_states=2;

	//vector <set<int> > v(MAX_SIZE);
	
	trans_table.resize(2);
	trans_table[0].resize(MAX_SIZE);
	trans_table[1].resize(MAX_SIZE);

	create_nfa(regex);

}

//To convert strings of the form [a-d] to (a|b|c|d)
string nfa::convert_to_basic_regex_format(string r)
{
	string ret="";
	for(int i=0;i<r.length();i++)
	{
		if(r[i]=='[')
		{
			int pos = r.find_first_of(']',i+1);			//Find the ']' bracket
			//cout << pos << endl; 
			if(pos!= string::npos)
			{
				char start = r[++i];
				if(r[++i]=='-')
				{
					
					char end = r[++i];
					if(r[++i]!=']')
					{
						cout << "Invalid Format. Correct Format -> [a-z]" << endl;
						exit(1);
					}
					else
					{
						
						ret+="(";
						for(int j=0;j<=end-start;j++)
						{
							//cout << ret << endl;
							ret+= (char)(start+j);
							//cout << ret << endl;
							if((start+j)!=end)
								ret+="|";
						}
						ret+=")";
						//cout << ret << endl;
						
					}				
				}
				else
				{
					cout << "Invalid Format. Correct Format -> [a-z]" << endl;
					exit(1);
				}
			}		
		}
		else
			ret+=r[i];
	}
	return ret;

}

//To do-> Add Concatenate operator (.) at appropriate places
string nfa:: add_concatenate(string r)
{
		string ret="";
		string ct = ".";
		char p=r[0];
		ret+=r[0];
		int i=1;
		
		for(;i<r.length();i++)
		{		
		if ((p=='*' && r[i]=='(') ||
		    (p=='*' && (r[i] != '*' && r[i]!='|' && r[i]!=')' && r[i]!='(')) ||
		    (p==')' && (r[i] != '*' && r[i]!='|' && r[i]!=')' && r[i]!='(')) ||
		    (r[i]=='(' && (p != '*' && p!='|' && p!=')' && p!='(')) ||
		    ((r[i] != '*' && r[i]!='|' && r[i]!=')' && r[i]!='(') 
			&& (p != '*' && p!='|' && p!=')' && p!='('))) ret += ct + r[i];
		else ret += r[i];

		p=r[i];
		}

	return ret;
}

//Convert from Infix to postfix for parsing (according to precedence)
//Precedence Order -> (A), A* , AB , A|B

//Working fine
string nfa::infix2postfix(string input)
{
	stack<char> s;											//stack for storing the operators (  ( , ) , * , +, | , . )
	
	string out="";
	int count=-1;
	for(int i=0;i<input.length();i++)
	{
		switch(input[i])
		{
			case '(':
				s.push('(');
				break;

			case ')':
				while(s.top()!='(')
				{
					out+=s.top();
					s.pop();
					if(s.empty())
					{
						cout << "Error..No matching '(' " << endl;
					}
				}
				if(!s.empty())
					s.pop();
				break;

			case '*':
				while(!s.empty() && (s.top()=='*' || s.top()=='+' || s.top()=='?') )
				{
					out+=s.top();
					s.pop();
				}
				s.push('*');
				break;

			//case '+':
		
			//case '?':
			
			case '.' :									// Concatenation
				while(!s.empty() && (s.top()=='*' || s.top()=='+' || s.top()== '.') )
				{
					out+=s.top();
					s.pop();
				}
				s.push('.');
				break;

			case '|':
				while(!s.empty() && (s.top()=='*' || s.top()=='+' || s.top()=='.' || s.top()== '|') )
				{
					out+=s.top();
					s.pop();
				}
				s.push('|');
				break;
		
			default:
				out+=input[i];
				break;
		
		}
	}
	while(!s.empty())
	{
		if(s.top()=='(')
			cout << "Error" << endl;
		else
		{
			out+=s.top();
			s.pop();
		}
	}

	return out;
	
}


/*

How do we go from something like (a|b)*ab to the graph or table..!! If we consider what we really need to do, we can see that evaluating regular expressions is similar to evaluating arithmetic expressions. For example, if we would like to evaluate R=A+B*C-D, we could do it like:

    PUSH A

    PUSH B

    PUSH C

    MUL

    ADD

    PUSH D

    SUB

    POP R

Here PUSH and POP are stacks and MUL, ADD and SUB take 2 operands from the stack and do the corresponding operation. We could use this knowledge for constructing an NFA from a regular expression. Let's look at the sequence of operations that need to be performed in order to construct an NFA from a regular expression (a|b)*cd:

    PUSH a

    PUSH b

    UNION

    STAR

    PUSH c

    CONCAT

    PUSH d

    CONCAT

    POP R

As we can see, it is very similar to the evaluation of arithmetic expressions. The difference is that in regular expressions the star operation pops only one element from the stack and evaluates the star operator. Additionally, the concatenation operation is not denoted by any symbol, so we would have to detect it. The code provided with the article simplifies the problem by pre-processing the regular expression and inserting a character ASCII code 0x8 whenever a concatenation is detected. Obviously it is possible to do this "on the fly", during the evaluation of the regular expression, but I wanted to simplify the evaluation as much as possible. The pre-processing does nothing else but detects a combination of symbols that would result in concatenation, like for example: ab, a(, )a,*a,*(, )(.

PUSH and POP operations actually work with a stack of simple NFA objects. If we would PUSH symbol a on the stack, the operation would create two state objects on the heap and create a transition object on symbol a from state 1 to state 2.

*/



void nfa::create_nfa(string regex)
{
	//Base Case ( Length is 1 but check it should be a valid character or digit only )
	if(regex.length()==1)
	{
		// if the only character is not a valid character
		//if(regex[0]=='*' || regex[0]=='|' || regex[0]=='(' || regex[0]==')' || regex[0]=='+' || regex[0]=='.' || regex[0]=='?' )
		if(regex[0]=='*' || regex[0]=='|' || regex[0]=='(' || regex[0]==')' || regex[0]=='?' )
		{
			cout << "Invalid Regular Expression..!!" ;
		}
		else	//the only character is a valid one (then create a the 2 states )
		{
			if(regex[0]=='$')
			{
				regex="";
				regex+=" ";
			}
			input_symbols.insert(regex[0]);
			trans_table[0][regex[0]].insert(1);		// Eg when the regular expression is only of the form 'a'
		}
		return;
	}
	
	string after_concate,simple_format;	
	simple_format = convert_to_basic_regex_format(regex);
	after_concate = add_concatenate(simple_format);
	cout << "After Concatenation " << after_concate << endl;
	string postfix= infix2postfix(after_concate);
	//string postfix= infix2postfix(regex);
	cout << "The postfix_expression for " << regex << " is " << postfix << endl;
	
	stack <nfa> thomson;								//stack for converting RE to NFA using Thomson construction

	for(int i=0;i<postfix.length();i++)
	{
		switch(postfix[i])
		{
			//Kleene Star
			case '*':
				if(!thomson.empty())
				{
					nfa n = thomson.top();				//get the nfa whose kleene star has to be taken
					thomson.pop();
					n.kleene_star();
					//cout << "Taking Kleene Star" << endl;
					//n.print_transition_table();
					thomson.push(n);				//After taking the Kleene star push the new NFA back
				}
				else
				{
					cout << "Error for Kleene Star";		
					exit(EXIT_FAILURE);				
				}
				break;		
			
			//Concatenation
			case '.':
				//cout << "Doing Concatenation" << endl;
				if(!thomson.empty())
				{
					//nfa n2,n1;
					nfa n2 = thomson.top();				//Taking B for 'AB.' i.e (A.B in original expression)
					thomson.pop();
					//cout << "B";	

					if(!thomson.empty())
					{
						nfa n1=thomson.top();				// Taking A for 'A.B' (AB. in postfix)
						thomson.pop();
						//cout << "A" << endl;
						n1.concatenate_nfa(n2);				// Doing A.B					
						thomson.push(n1);					
					}
					else
					{
						cout << "Error for Concatenation..!!\n Missing NFA for concatenation" << endl;
						exit(EXIT_FAILURE);
					}
				}
				else
				{
					cout << "Error for Concatenation..!!\n Missing NFA for concatenation" << endl;
					exit(EXIT_FAILURE);
				}
				break;


			//Union
			case '|':
				//cout << "Doing Union..!!" << endl;
				if(!thomson.empty())
				{
					nfa n2=thomson.top();			//Taking B for A|B (AB| in postfix expression)
					thomson.pop();
					//cout << "B for A|B " << endl;

					if(!thomson.empty())
					{
						nfa n1=thomson.top();
						thomson.pop();
						//cout << "A for A|B " << endl;
						n1.union_nfa(n2);
						thomson.push(n1);		// push the NFA created by taking union on the statck		
					}
					else
					{
						cout << "Error for Union..!!\nMissing NFA for union" << endl;
						exit(EXIT_FAILURE);
					}
				}
				else
				{
					cout << "Error for Union..!!\nMissing NFA for union" << endl;
						exit(EXIT_FAILURE);
				}
				break;
					

			default:							//when it is an input symbol and not an operator
				//cout << "Hello " << postfix[i] << endl;
				string in_symbol="";				
				if(postfix[i]=='$')									
					in_symbol+=" ";
				else				
					in_symbol+=postfix[i];
				nfa n(in_symbol);
				thomson.push(n);				
		}
	}

	//this = thomson.top();
	//thomson.pop();
	
	nfa a = thomson.top(); thomson.pop();							// the final nfa
	*this = a;
	
}


//Taking the Kleene star of the NFA
void nfa::kleene_star()
{
	vector< set<int> > new_start(MAX_SIZE), new_final(MAX_SIZE);


	int start_prev =n_states-2;							//the 2nd last state is the start state and last is the final state
	int final_prev = n_states-1;
	int start_new=n_states;
	int final_new=n_states+1;

	//Add the Epsilon transitions required for Kleene Star
	new_start[eps].insert(start_prev);						// epsilon transition from new start -> old start	
	new_start[eps].insert(final_new);						// new start -> new final
	trans_table[final_prev][eps].insert(start_prev);				// prev final -> prev_start
	trans_table[final_prev][eps].insert(final_new);					// prev final -> new final	

	trans_table.push_back(new_start);
	trans_table.push_back(new_final);						// Insert these two states new states into the transition table

	n_states=n_states+2;								//The number of states has increased by 2
}


// Taking the concatenation A.B
void nfa::concatenate_nfa(nfa &n2)
{
	/* For concatenation no new states need to be added
	   THe states of the two nfa needs to be merged and the appropriate epsilon transitions need to be applied
		FINAL STATE of N1 -> START STATE OF N2
		NEW_FINAL STATE = FINAL_STATE OF N2
		NEW START STATE = OLD START STATE	
	*/
	int n_states_n2 = n2.trans_table.size();

	input_symbols.insert(n2.input_symbols.begin(),n2.input_symbols.end());		//Adding the input symbols of n2 to n1 for concatenation of n1.n2
	int new_n_states = n_states + n2.n_states;


	//Add the new states by changing the state numbers
	for (int i=0; i<n_states_n2; i++) {
		for (int j=0; j<n2.trans_table[i].size (); j++) {
			set<int>::iterator it;
			set<int> n_set;
			for (it=n2.trans_table[i][j].begin (); it != n2.trans_table[i][j].end (); it++) {
				n_set.insert (*it + n_states);
				//                cout<< i <<" "<<j<<" "<<*it+num_states<<endl;
			}
			n2.trans_table[i][j] = n_set;
		}
		//Merging the states of n2 with n1
		trans_table.push_back (n2.trans_table[i]);		
	}

	//The start and final states of n1 and n2 previously
	int prev_start_n1=n_states-2;
	int prev_end_n1 = n_states-1;
	int prev_start_n2= new_n_states-2;
	int prev_end_n2= new_n_states-1;	

	// The new start will be the same as the previous start ( But we represent the start states by 2nd last state )
	vector <set<int> > start_row;
	start_row = trans_table[prev_start_n1];
	
	//Add the epsilon transition from FINAL STATE OF N1 -> START STATE OF N2
	trans_table[prev_end_n1][eps].insert(prev_start_n2);	
	
	//To change the position of start state..Need to erase it and insert it as proper place 
	trans_table.erase(trans_table.begin()+prev_start_n1);					//Remove and insert at correct position for start state (i.e. 2nd last position) 
	trans_table.insert(trans_table.begin()+new_n_states-2,start_row);					//This is the correct position to insert

	
 

	//Correct the indices that might have changed due to changing the position of start state
	// All states with index >= number_states_of_n1 and index <= number_of_states_of_n2 will be decremented by 1 
	for (int i=0; i<trans_table.size(); i++) {
		for (int j=0; j<trans_table[i].size (); j++) {
			set<int>::iterator it;
			set<int> n_set;
			for (it=trans_table[i][j].begin (); it != trans_table[i][j].end (); it++) {
				if(*it >= (n_states-1) && *it <= (new_n_states - 2)){
					//*it -= 1;	// decrement the state index with 1 if its index was changed during erase and insert process.
					n_set.insert(*it-1);
				}
				else 
					n_set.insert(*it);
			}
			trans_table[i][j] = n_set;
		}
	}

	n_states = new_n_states;							//the number of states after 'concatenation'

}

//Taking the union
void nfa::union_nfa(nfa &n2)
{
	/*
	For Union two new states need to be added A new START STATE and a NEW END STATE
	 The states of the two NFA's need to be merged together
	 The epsilon transitions are as follows
		NEW START -> START OF N1
		NEW START -> START OF N2
		FINAL OF N1 -> NEW FINAL
		FINAL OF N2 -> NEW FINAL 
	*/	
	
	int n_states_n2 = n2.trans_table.size();

	input_symbols.insert(n2.input_symbols.begin(),n2.input_symbols.end());		//Adding the input symbols of n2 to n1 for concatenation of n1.n2

	int new_n_states = n_states + n2.n_states +2;					// New number of states

	//Add the new states by changing the state numbers
	for (int i=0; i<n_states_n2; i++) {
		for (int j=0; j<n2.trans_table[i].size (); j++) {
			set<int>::iterator it;
			set<int> n_set;
			for (it=n2.trans_table[i][j].begin (); it != n2.trans_table[i][j].end (); it++) {
				n_set.insert (*it + n_states);
				//                cout<< i <<" "<<j<<" "<<*it+num_states<<endl;
			}
			n2.trans_table[i][j] = n_set;
		}
		//Merging the states of n2 with n1
		trans_table.push_back (n2.trans_table[i]);		
	}

	vector <set<int> > new_start(MAX_SIZE),new_final(MAX_SIZE);

	int start_prev_n1=n_states-2;							//Start state of NFA 1
	int final_prev_n1=n_states-1;							// End state OF NFA 1
	int start_prev_n2=n_states + n2.n_states-2;					// Start state of N2 after merging
	int final_prev_n2=n_states + n2.n_states-1;					// End state of N2 after merging 

	new_start[eps].insert(start_prev_n1);						//NEW START -> START OF NFA 1
	new_start[eps].insert(start_prev_n2);						//NEW START -> START OF NFA 2
	trans_table[final_prev_n1][eps].insert(new_n_states-1);				// FINAL OF NFA 1 -> NEW FINAL
	trans_table[final_prev_n2][eps].insert(new_n_states-1);				// FINAL OF NFA 2 -> NEW FINAL
	
	trans_table.push_back(new_start);
	trans_table.push_back(new_final);						// Add the new final and start states at 2nd last and last positions respectively	
	
	n_states=new_n_states;
}

// To print the NFA transition Table
void nfa::print_transition_table()
{
	//vector< vector<set<int > > > :: iterator it;
	
	vector< set<int> > :: iterator set_it;

	set<char> :: iterator symbols;
	set<int> :: iterator states_it;

	for(int i=0;i<trans_table.size();i++)
	{
		cout << " Transitions for state " << i << " are " ;
		for(symbols=input_symbols.begin();symbols!=input_symbols.end();symbols++)
		{
			cout << *symbols << " : " ;
			set<int> states = trans_table[i][*symbols];
			for(states_it=states.begin();states_it!=states.end();states_it++)
			{
				cout << " " << *states_it ;
			}
			cout << " | ";
		}
		cout << endl;
	}

}

void nfa::print_epsilon_closure(int state)
{
	set<int> closr;
	set<int> closure = epsilon_closure(state,closr);
	set<int> :: iterator it;	
	
	cout << "Epsilon closure for state " << state << endl;
	for(it=closure.begin();it!=closure.end();it++)
	{
		cout << *it << " "; 
	}
	cout << endl;
}

set<int> nfa::epsilon_closure(int state)
{
	set<int> closr;
	set<int> closure = epsilon_closure(state,closr);

	return closure;
}

set<int> nfa::epsilon_closure(set<int> states)
{
	stack<int> state_stack;

	set<int> closure;
	set<int> :: iterator it;
	for(it=states.begin();it!=states.end();it++)
	{
		state_stack.push(*it);
		while(!state_stack.empty())
		{
			int state_cur = state_stack.top();
			state_stack.pop();
			set<int> cur_state_closure = epsilon_closure(state_cur);
			closure.insert(cur_state_closure.begin(),cur_state_closure.end());
		}
		
	}
	return closure;	
}


// STACK can also be used 
set<int> nfa::epsilon_closure(int state,set<int> closr)
{
	set<int> closure(closr);				//Set to store states in epsilon closure for "state"
	closure.insert(state);				// the 'state' is in the epsilon closure of state
	
	set<int> :: iterator states_it;
	set<int> :: iterator states_it1;
	set<int> states = trans_table[state][eps];	
		

	for(states_it=states.begin();states_it!=states.end();states_it++)
	{
		//cout << *states_it << " " << endl;
		
		if(closure.count(*states_it)==0)					//If the current state is not there already
		{
			closure.insert(*states_it);
			//if((*states_it)!=state)
			{	
				set<int> state_cur=epsilon_closure(*states_it,closure);
				for(states_it1=state_cur.begin();states_it1!=state_cur.end();states_it1++)
				{
					closure.insert(*states_it1);
				}
			}		
		}
	}
	return closure;
}

set<int> nfa::move(set<int> states,char input_symbol)
{
	set<int>::iterator it;
	set<int> move_set;
	for(it=states.begin();it != states.end();it++){
		set<int> to_move = trans_table[*it][input_symbol];
		move_set.insert(to_move.begin(),to_move.end());
	}
	
	return move_set;
}

/*
ALGORITHM

Initially
D-States = EpsilonClosure(NFA Start State) and it is unmarked
while there are any unmarked states in D-States
{
    mark T
    for each input symbol a
    {
        U = EpsilonClosure(Move(T, a));
        if U is not in D-States
        {
            add U as an unmarked state to D-States
        }
        DTran[T,a] = U
    }
}
*/

dfa nfa::convert_nfa_to_dfa()
{

	vector< vector<int> > dfa_table;								//Transition table for DFA 

	vector< set<int> > dfa_states;									// States of the DFA

	vector<bool> marked_states;									//To mark which of the states have been marked
	vector<bool> final;

	int start_state = n_states-2;									//Start state of NFA
	int final_state = n_states-1;

	dfa_states.push_back(epsilon_closure(start_state));
	vector<int> transitions(MAX_SIZE,-1);
	dfa_table.push_back(transitions);	
										//Initially D-States = EpsilonClosure(NFA Start State) and it is unmarked		
	marked_states.push_back(false);				
	
	
	while(1)
	{

		if(accumulate(marked_states.begin(),marked_states.end(),0)==marked_states.size())
			break;
		
		int i;
		for(i=0;i<marked_states.size();i++)
		{
			if(marked_states[i]==false)
			{
				marked_states[i]=true;
				break;			
			}
			
		}

		set<char> :: iterator symbols;
		
		//Now for all input symbols find the next transition state for DFA
		for(symbols=input_symbols.begin();symbols!=input_symbols.end();symbols++)
		{
			if(*symbols==eps)
				continue;
			
			set<int> U = epsilon_closure(move(dfa_states[i],*symbols));
			
			vector<set<int> > :: iterator it;
			it = find(dfa_states.begin(),dfa_states.end(),U);
			if(it!=dfa_states.end())
			{
				//dfa_states.push_back(U);
				//marked_states.push_back(false);
				int index = it - dfa_states.begin();
				dfa_table[i][*symbols]=index;			
			}
			else
			{
				vector<int> transitions(MAX_SIZE,-1);
				//int index = it - dfa_states.begin();	

				dfa_states.push_back(U);			
				marked_states.push_back(false);
				dfa_table[i][*symbols]=dfa_states.size()-1;				
				//transitions[*symbols]=dfa_states.size()-1;
				
				dfa_table.push_back(transitions);			
			}
		}	
	}
	
	//Marking the final states
	for(int i=0;i<dfa_states.size();i++){
		set<int> nfa_states = dfa_states[i];
		if(find(nfa_states.begin(),nfa_states.end(),final_state) == nfa_states.end())
			final.push_back(false);
		else
			final.push_back(true);

	}

	set<char> alp_without_epsilon = input_symbols;
	alp_without_epsilon.erase(eps);
	dfa converted(dfa_states.size(),alp_without_epsilon,dfa_table,final);
	return converted;

}

//Constructor
dfa::dfa(int num_states,set<char> alphabet,vector<vector<int> > transitions,vector<bool> final)
{
	
	this->num_states = num_states;
	this->alphabet = alphabet;
	this->final = final;
	this->transitions = transitions;

	calculate_rejecting_states();
}

//Printing the DFA table
void dfa::print_dfa_table()
{
	//vector< vector<set<int > > > :: iterator it;
	
	vector<int> :: iterator set_it;

	set<char> :: iterator symbols;
	//set<int> :: iterator states_it;

	for(int i=0;i<transitions.size();i++)
	{
		cout << " Transitions for state " << i << " --> " ;
		for(symbols=alphabet.begin();symbols!=alphabet.end();symbols++)
		{
			cout << *symbols << " : " ;
			int state = transitions[i][*symbols];
			
			cout << " " << state ;
		
			cout << " | ";
		}
		cout << endl;
	}

}

//Function to print final states of DFA
void dfa::print_final_states()
{
	for (int i=0;i<final.size();i++)
	{
		if( final[i]==true)
			cout<< i<<":\t";
	}
	cout<<endl;
}

//Function to print rejecting states
void dfa::print_rejecting_states()
{
	for (int i=0;i<rejecting.size();i++)
	{
		if( rejecting[i]==true)
			cout<< i<<":\t";
	}
	cout<<endl;
}

void dfa::calculate_rejecting_states()
{
	rejecting.resize(num_states);
	for(int i=0;i<rejecting.size();i++)
	{
		rejecting[i]=true;
	}

	bool graph[num_states][num_states];
	memset(graph,0,sizeof(graph));

	//queue<int> q;						//Queue to be used for BFS

	cout << endl;
	for(int i=0;i<transitions.size();i++)
	{
		for(int j=0;j<transitions[i].size();j++)
		{
			if((transitions[i][j])!=-1)
			{
				graph[transitions[i][j]][i]=true;
			}
			
		}
		
	}
/*
	for(int i=0;i<num_states;i++)
	{
		for(int j=0;j<num_states;j++)
			if(graph[i][j]==true)
				cout << "i " << i << " j " << j << endl;
	}
*/
	vector<bool> marked(num_states,0);
	
	for(int i=0;i<num_states;i++)
	{
		if(final[i]==true)
		{
			//dfs(i)
			stack<int> dfs;
			dfs.push(i);
			marked[i]=true;
			rejecting[i]=false;
			while(!dfs.empty())
			{
				int cur_state=dfs.top();
				marked[cur_state]=true;
				//cout << cur_state << endl;
				dfs.pop();
				for(int j=0;j<num_states;j++)
				{
					if(graph[cur_state][j]==true && marked[j]==false)
					{
						
						rejecting[j]=false;						
						dfs.push(j);
					}
				}
			}						
		}
	}
	
}

bool dfa::is_match(string input)
{
	int start_state=0;
	int cur_state=0,next_state;	
	for(int i=0;i<input.length();i++)
	{
		next_state=transitions[cur_state][input[i]];
		cur_state=next_state;
	}
	if(final[cur_state]==true)
	{
		cout << "Matched" << endl;
		return true;
	}	
	else
	{
		cout << "Not Matched..!" << endl;
		return false;
	}
}

string dfa::max_match(string input)
{
	//cout << "INPUT for max_match " << input << endl;
	int start_state=0,i=0;
	int cur_state=0,next_state;
	int prev_max_length=0,prev_final_state=-1;
	
	for(i=0;i<input.length();i++)
	{
		//cout << input[i] << endl;
		if(alphabet.count(input[i])==0)
		{
			//cout << input[i] << " is not in input symbols of the DFA" << endl;
			break;		
		}			
		next_state=transitions[cur_state][input[i]];
		
		if(rejecting[next_state]==true)
			break;
		
		cur_state=next_state;
		if(final[cur_state]==true)
		{	
			prev_final_state=cur_state;
			prev_max_length=i;
		}
	}
	if(prev_final_state==-1)
	{	
		//cout << "None of the substrings match" << endl;
		return "";
	}
	string out = input.substr(0,prev_max_length+1);
	return out;
}


int main()
{
/*
	//string regex="(a*.b)|c.d";
	string regex;
	cin >> regex;
	string postfix;
	//postfix=infix2postfix(regex);
	nfa n1(regex);
	cout << postfix << endl;	
	n1.print_transition_table() ;
	n1.print_epsilon_closure(0);
	

	dfa d=n1.convert_nfa_to_dfa();	

	d.print_dfa_table();
	d.print_final_states();
	d.print_rejecting_states();

	string input_string;
	cin >> input_string;
	//cout << d.is_match(input_string) << endl;
	cout << d.max_match(input_string) << endl;
*/
	//Now for all the DFA's check the maximum substring that matches

/**************************************************/
	cout<<"Input filename: "<<endl;
	string filename = "test";
	//   cin>>filename;

	ifstream fpi(filename.c_str ());
	if(!fpi)
	{
		cout<<"Error in Openeing File: "<< filename<<endl;
		exit(EXIT_FAILURE);
	}

	vector<string> token_class;
	vector<string> regex;
	string token_class_buf,regex_buf;

	while(!fpi.eof() && fpi>>token_class_buf && fpi>>regex_buf) 
	{
	/*           cout<< "class: " << token_class_buf
		<< " regex: " << regex_buf ;
	   cout <<" BEFORE" << endl; */
	   token_class.push_back(token_class_buf);
	   //regex_buf = sanitize(regex_buf);
	   regex.push_back(regex_buf);

	   cout<< "class: " << token_class_buf
		<< " regex: " << regex_buf ;
	   cout <<" DONE" << endl;
	}

/*
    vector<dfa> dfas;

    for(int i=0 ; i< regex.size(); i++){
        nfa n(regex[i]);
        dfas.push_back(n.to_dfa());    
        dfas[i].print_transitions ();
	    dfas[i].print_final();
        cout<<endl;
    }
*/
/**************************************/

	int n;
	//string regex;
	string type_regex;
	/*cout << "Enter Number of Regular Expressions " ;
	cin >> n;*/
	int no_of_dfa=regex.size();
	vector<dfa> dfa_vector;
    	for(int j=0 ; j< regex.size(); j++)
	{
		/*cin >> type_regex;
		cin >> regex;*/
		nfa n2(regex[j]);

		dfa d1=n2.convert_nfa_to_dfa();
		//d1.print_final_states();
		//d1.print_rejecting_states();
		d1.type= token_class[j];
		dfa_vector.push_back(d1);
		//d1.print_dfa_table();
		cout<<"end of dfa "<<j<<endl;
	}
	
	string input_string;
	//cin >> input_string;		
	cout << "Enter String " << endl ;
	/*cin.clear();
	cin.ignore(INT_MAX,'\n');*/
	
	char input[1024];
	//cin.getline(input,1024);
	cin.getline(input,1024);						
	input_string = input;
 	//cout<<input<<"bb\n";
	//cout << input_string ;
	//cin.getline(input,1024);
	//cin >> input_string;	

	int i= no_of_dfa;
	string out="";
	string max_matched="";
	string cur=input_string;
	
	int dfa_matched=-1;	
	while(cur!="")
	{
		dfa_matched=-1;
		max_matched="";
		i=no_of_dfa;
		while(i--)
		{
			//cout << "Max Matched " << max_matched << endl;
			out=dfa_vector[i].max_match(cur);
			if (max_matched.length()<out.length())
			{
				max_matched=out;
				dfa_matched=i;			
			}		
		}
		if(max_matched=="")
		{
			cout << "Matched in None..!!" << endl;
			break;
		}
		//cout << max_matched << endl;
		cout << "< " << dfa_vector[dfa_matched].type << ",\t" << max_matched << " >" << endl;
		cur=cur.substr(max_matched.length());
		//cout << "cur " << cur << endl;	
	}
	//cout << max_matched << endl;
	return 0;

}

// ================================================================================================ Beginning of file "main.cpp"
// Copyright (c) 2020-2021, Eric Jacopin, ejacopin@ymail.com
// ------------------------------------------------------------------------------------------------
// Main for travel example, blocksworld example and IPC 2011 blocksworld problems
//
///////////////////////////////////////////////////////////////////////////////////////////////////
//
//                           S T D      &      S T L      F I L E S
//
///////////////////////////////////////////////////////////////////////////////////////////////////
//
#include <algorithm>		// Visibility for std::find_if
#include <iomanip>			// Visibility for std::setw, std::left, std::right
#include <iostream>			// Visibility for stc::cout, std::endl
#include <map>				// Visibility for std::map
#include <string>			// Visibility for std::string
#include <type_traits>		// Visibility for std::is_same
#include <vector>			// Visibility for std::vector
//
///////////////////////////////////////////////////////////////////////////////////////////////////
//
//                           P R O J E C T      F I L E S
//
///////////////////////////////////////////////////////////////////////////////////////////////////
//
#include "EnumAgent.h"			// Application Domain: Travel
#include "EnumBlock.h"			// Application Domain: Blocksworld
#include "EnumBlockStatus.h"	// Application Domain: Blocksworld
#include "EnumLocation.h"		// Application Domain: Travel
#include "EnumReturnedValue.h"	// Visibility for None, False, and True
//
///////////////////////////////////////////////////////////////////////////////////////////////////
//
typedef std::string MethodId;
typedef std::string OperatorId;
typedef std::string TaskId;

class State {
	private:
		std::string name;

	public:
		// Travel info
		std::map<Agent, float> cash;
		std::map<std::pair<Location, Location>, float> dist;
		std::map<Agent, Location> loc;
		std::map<Agent, float> owe;

		// Blocksworld info
		std::map<Block, Block> pos;
		std::map<Block, bool> clear;
		Block holding;

	public:
		State() : name(""), holding(Block::none) {}
		State(std::string a_name) : name(a_name), holding(Block::none) {}

		~State() {}

		inline std::string get_name() const { return name; }
		inline void set_name(const std::string a_name) { name = a_name; }
		void Clear() {
			name = "";
			cash.clear();
			dist.clear();
			loc.clear();
			owe.clear();
			pos.clear();
			clear.clear();
			Block holding = Block::none;
		}
};

State empty("Empty");
typedef State Goal;

struct Parameters {
	public:
		// Travel
		Agent a;
		Location x;
		Location y;

		// Blocksworld
		Block b;
		Block c;
		Goal goal;

	public:
		Parameters() : a(Agent::none), x(Location::none), y(Location::none), b(Block::none), c(Block::none), goal(empty) {}
		Parameters(Agent a_a) : a(a_a), x(Location::none), y(Location::none), b(Block::none), c(Block::none), goal(empty) {}
		Parameters(Agent a_a, Location a_x) : a(a_a), x(a_x), y(Location::none), b(Block::none), c(Block::none), goal(empty) {}
		Parameters(Agent a_a, Location a_x, Location a_y) : a(a_a), x(a_x), y(a_y), b(Block::none), c(Block::none), goal(empty) {}

		Parameters(Block b_b) : b(b_b), c(Block::none), a(Agent::none), x(Location::none), y(Location::none), goal(empty) {}
		Parameters(Block b_b, Block c_c) : b(b_b), c(c_c), a(Agent::none), x(Location::none), y(Location::none), goal(empty) {}
		Parameters(Goal g_goal) : a(Agent::none), x(Location::none), y(Location::none), b(Block::none), c(Block::none), goal(g_goal) {}
		Parameters(Block b_b, Goal g_goal) : b(b_b), c(Block::none), a(Agent::none), x(Location::none), y(Location::none), goal(g_goal) {}
		Parameters(Block b_b, Block c_c, Goal g_goal) : b(b_b), c(c_c), a(Agent::none), x(Location::none), y(Location::none), goal(g_goal) {}

		void print()
		{
			std::string p("");
			if (Agent::none != a)
				p += (", " + std::string(GetStringAgent(a)));
			if (Location::none != x)
				p += (", " + std::string(GetStringLocation(x)));
			if (Location::none != y)
				p += (", " + std::string(GetStringLocation(y)));
			if (Block::none != b)
				p += (", " + std::string(GetStringBlock(b)));
			if (Block::none != c)
				p += (", " + std::string(GetStringBlock(c)));
			std::cout << p;
		}
};

// Helpers
bool is_done(Block b1, State& state, Goal& goal, Block done_state)
{
	if (b1 == done_state)
		return true;

	std::map<Block, Block>::iterator it_b1 = goal.pos.find(b1);
	if ((goal.pos.end() != it_b1) && (goal.pos[b1] != state.pos[b1]))
		return false;
	
	if (state.pos[b1] == done_state)
		return true;

	return is_done(state.pos[b1], state, goal, done_state);
}

// Declare Operators
typedef std::pair<ReturnedValue, State> bState;
using Ptr2Operator = bState(*)(State, Parameters);
typedef std::map<OperatorId, Ptr2Operator> Operators;
Operators operators;
void declare_operators(OperatorId a_OperatorId, Ptr2Operator a_Ptr2Operator)
{
	operators[a_OperatorId] = a_Ptr2Operator;
}

// Print out the names of the operators
void print_operators(Operators operators)
{
	std::cout << "OPERATORS: ";
	for (std::map<OperatorId, Ptr2Operator>::iterator o = operators.begin(); o != operators.end();)
	{
		std::cout << (o->first);
		if (++o != operators.end())
			std::cout << ", ";
	}
	std::cout << "." << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Travel Operators 
float taxi_rate(float dist)
{
	return (1.5f + 0.5f * dist);
}

bState walk(State state, Parameters p)
{
	if (state.loc[p.a] == p.x)
	{
		State result = state;
		result.loc[p.a] = p.y;
		return { ReturnedValue::True, result };
	}
	else
		return { ReturnedValue::False, empty };
}

bState call_taxi(State state, Parameters p)
{
	State result = state;
	result.loc[Agent::taxi] = p.x;
	return { ReturnedValue::True, result };
}

bState ride_taxi(State state, Parameters p)
{
	if (state.loc[Agent::taxi] == p.x && state.loc[p.a] == p.x)
	{
		State result = state;
		result.loc[Agent::taxi] = p.y;
		result.loc[p.a] = p.y;
		result.owe[p.a] = taxi_rate(result.dist[{p.x, p.y}]);
		return { ReturnedValue::True, result };
	}
	else
		return { ReturnedValue::False, empty };
}

bState pay_driver(State state, Parameters p)
{
	if (state.cash[p.a] >= state.owe[p.a])
	{
		State result = state;
		result.cash[p.a] = result.cash[p.a] - result.owe[p.a];
		result.owe[p.a] = 0;
		return { ReturnedValue::True, result };
	}
	else
		return { ReturnedValue::False, empty };
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Blocksworld Operators 
bState pickup(State state, Parameters p)
{
	if (state.pos[p.b] == Block::table && state.clear[p.b] == true && state.holding == Block::none)
	{
		State result = state;
		result.pos[p.b] = Block::hand;
		result.clear[p.b] = false;
		result.holding = p.b;
		return { ReturnedValue::True, result };
	}
	else
		return { ReturnedValue::False, empty };
}

bState unstack(State state, Parameters p)
{
	if (state.pos[p.b] == p.c && p.c != Block::table && state.clear[p.b] == true && state.holding == Block::none)
	{
		State result = state;
		result.pos[p.b] = Block::hand;
		result.clear[p.b] = false;
		result.holding = p.b;
		result.clear[p.c] = true;
		return { ReturnedValue::True, result };
	}
	else
		return { ReturnedValue::False, empty };

}

bState putdown(State state, Parameters p)
{
	if (state.pos[p.b] == Block::hand)
	{
		State result = state;
		result.pos[p.b] = Block::table;
		result.clear[p.b] = true;
		result.holding = Block::none;
		return { ReturnedValue::True, result };
	}
	else
		return { ReturnedValue::False, empty };
}

bState stack(State state, Parameters p)
{
	if (state.pos[p.b] == Block::hand && state.clear[p.c] == true)
	{
		State result = state;
		result.pos[p.b] = p.c;
		result.clear[p.b] = true;
		result.holding = Block::none;
		result.clear[p.c] = false;
		return { ReturnedValue::True, result };
	}
	else
		return { ReturnedValue::False, empty };
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Tasks 
typedef std::pair<TaskId, Parameters> Task;
typedef std::vector<Task> Tasks;
typedef std::pair<ReturnedValue, Tasks> bTasks;

///////////////////////////////////////////////////////////////////////////////////////////////////
// Travel Tasks 
bTasks travel_by_foot(State state, Parameters p)
{
	if (state.dist[{p.x, p.y}] <= 2)
		return { ReturnedValue::True, { Task(OperatorId("walk"),p) } };
	else
		return { ReturnedValue::False, {} };
}

bTasks travel_by_taxi(State state, Parameters p)
{
	if (state.cash[p.a] >= taxi_rate(state.dist[{p.x, p.y}]))
		return { ReturnedValue::True, { Task(OperatorId("call_taxi"), p), Task(OperatorId("ride_taxi"), p), Task(OperatorId("pay_driver"), p) } };
	else
		return { ReturnedValue::False, {} };
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Blocksworld Tasks 

//	A helper function used in the methods' preconditions.
BlockStatus status(Block b1, State state, Goal goal, Block done_state)
{
	if (is_done(b1, state, goal, done_state))
		return BlockStatus::done;
	else if (state.clear[b1] == false)
		return BlockStatus::inaccessible;
	else if ((goal.pos.end() == goal.pos.find(b1)) || (goal.pos[b1] == done_state))
		return BlockStatus::move_to_table;
	else if (is_done(goal.pos[b1], state, goal, done_state) && state.clear[goal.pos[b1]])
		return BlockStatus::move_to_block;
	else
		return BlockStatus::waiting;
}

bTasks moveb_m(State state, Parameters p)
{
	//	This method implements the following block - stacking algorithm :
	//	If there's a block that can be moved to its final position, then
	//	do soand call move_blocks recursively. Otherwise, if there's a
	//	block that needs to be moved and can be moved to the table, then
	//	do so and call move_blocks recursively. Otherwise, no blocks need
	//	to be moved.
	for (std::map<Block, bool>::iterator it_clear = state.clear.begin(); it_clear != state.clear.end(); ++it_clear)
	{
		Block b1 = it_clear->first;
		BlockStatus s = status(b1, state, p.goal, Block::table);
		if (s == BlockStatus::move_to_table)
			return { ReturnedValue::True, { Task(MethodId("move_one"), Parameters(b1, Block::table, p.goal)), Task(MethodId("move_blocks"), p) } };
		else if (s == BlockStatus::move_to_block)
			return { ReturnedValue::True, { Task(MethodId("move_one"), Parameters(b1, p.goal.pos[b1], p.goal)), Task(MethodId("move_blocks"), p) } };
		else
			continue;
	}

	//
	//	if we get here, no blocks can be moved to their final locations
	std::map<Block, bool>::iterator it_b1 = std::find_if(state.clear.begin(), state.clear.end(), [&state, &p](std::pair<Block, bool> c) { return ((BlockStatus::waiting == status(c.first, state, p.goal, Block::table)) && (Block::table != state.pos[c.first])); });
	if (it_b1 != state.clear.end())
		return { ReturnedValue::True, { Task(MethodId("move_one"), Parameters(it_b1->first, Block::table)), Task(MethodId("move_blocks"), Parameters(p.goal)) } };

	//
	// if we get here, there are no blocks that need moving
	return { ReturnedValue::None, { } };
}

bTasks move1(State state, Parameters p)
{
	return { ReturnedValue::True, { Task(OperatorId("get"), Parameters(p.b, p.goal) ), Task(OperatorId("put"), Parameters(p.b, p.c, p.goal)) } };
}

bTasks get_m(State state, Parameters p)
{
	if (state.clear[p.b] == true)
	{
		if (state.pos[p.b] == Block::table)
			return { ReturnedValue::True, { Task(OperatorId("pickup"), Parameters(p.b, p.goal)) } };
		else
			return { ReturnedValue::True, { Task(OperatorId("unstack"), Parameters(p.b, state.pos[p.b], p.goal)) } };
	}
	else
		return { ReturnedValue::False, {} };
}

bTasks put_m(State state, Parameters p)
{
	if (state.holding == p.b)
		if (p.c == Block::table)
			return { ReturnedValue::True, { Task(OperatorId("putdown"), Parameters(p.b, p.goal)) } };
		else
		{
			return { ReturnedValue::True, { Task(OperatorId("stack"), Parameters(p.b, p.c, p.goal)) } };
		}
	else
		return { ReturnedValue::False, {} };
}

// Declare methods
using Ptr2Method = bTasks(*)(State, Parameters);
typedef std::map<TaskId, std::vector<Ptr2Method>> Methods;
Methods methods;
template<typename T> void declare_methods(TaskId a_TaskId, T a_method)
{
	static_assert(std::is_same<Ptr2Method, T>(), "declare_methods/2::2nd parameter is not a pointer to a method.");
	methods[a_TaskId].push_back(a_method);
}
template<typename T, typename... Args> void declare_methods(TaskId a_TaskId, T a_method, Args... more_methods)
{
	static_assert(std::is_same<Ptr2Method, T>(), "declare_methods/n::2nd parameter is not a pointer to a method.");
	methods[a_TaskId].push_back(a_method);
	return declare_methods(a_TaskId, more_methods...);
}

// Print out a table of what the methods are for each task
void print_methods(Methods mlist)
{
	std::cout << std::setw(14) << std::left << "TASK:" << std::right << "METHODS:" << std::endl;
	for (auto &task : mlist)
	{
		std::cout << std::setw(14) << std::left << task.first;
		for (std::vector<Ptr2Method>::iterator m = task.second.begin(); m != task.second.end();)
		{
			std::cout << (*m);
			if (++m != task.second.end())
				std::cout << ", ";
		}
		std::cout << "." << std::endl;
	}
}

// Print each variable in state, indented by indent spaces.
void print_state(bState state, unsigned short indent = 4)
{
	if (ReturnedValue::True == state.first)
	{
		for (const auto &o : state.second.cash)
			std::cout << std::setw(indent) << "" << state.second.get_name() + "::cash::" + GetStringAgent(o.first) + " = " << o.second /* float */ << std::endl;
		for (const auto &o : state.second.dist)
			std::cout << std::setw(indent) << "" << state.second.get_name() + "::dist::{" + GetStringLocation(o.first.first) + "," + GetStringLocation(o.first.second) + "} = " << o.second /* float */ << std::endl;
		for (const auto &o : state.second.loc)
			std::cout << std::setw(indent) << "" << state.second.get_name() + "::loc::" + GetStringAgent(o.first) + " = " + GetStringLocation(o.second) << std::endl;
		for (const auto &o : state.second.owe)
			std::cout << std::setw(indent) << "" << state.second.get_name() + "::owe::" + GetStringAgent(o.first) + " = " << o.second /* float */ << std::endl;
		for (const auto &o : state.second.clear)
			std::cout << std::setw(indent) << "" << state.second.get_name() + "::clear::" + GetStringBlock(o.first) + " = " << o.second /* bool */ << std::endl;
		for (const auto &o : state.second.pos)
			std::cout << std::setw(indent) << "" << state.second.get_name() + "::pos::" + GetStringBlock(o.first) + " = " << GetStringBlock(o.second) << std::endl;
		if (state.second.clear.size() > 0 && state.second.pos.size() > 0)
			std::cout << std::setw(indent) << "" << state.second.get_name() + "::holding::" + GetStringBlock(state.second.holding) << std::endl;
	}
	else if (ReturnedValue::False == state.first)
		std::cout << state.second.get_name() + "::False" << std::endl;
	else
		std::cout << state.second.get_name() + "::None" << std::endl;
}

void print_goal(Goal goal, unsigned short indent = 4)
{
	for (const auto &o : goal.clear)
		std::cout << std::setw(indent) << "" << goal.get_name() + "::clear::" + GetStringBlock(o.first) + " = " << o.second /* bool */ << std::endl;
	for (const auto &o : goal.pos)
		std::cout << std::setw(indent) << "" << goal.get_name() + "::pos::" + GetStringBlock(o.first) + " = " << GetStringBlock(o.second) << std::endl;

	std::cout << std::setw(indent) << "" << goal.get_name() + "::holding::" + GetStringBlock(goal.holding) << std::endl;
}

// ############################################################
// The actual planner

bTasks seek_plan(State& state, Tasks tasks, Operators& operators, Methods& methods, bTasks plan, unsigned int depth, unsigned short verbose);

bTasks search_operators(State& state, Tasks tasks, Operators& operators, Methods& methods, bTasks plan, Task task, unsigned int& depth, unsigned short verbose = 0)
{
	if (verbose > 2)
		std::cout << "depth = " << depth << " action = " << task.first << std::endl;
	bState newstate = (operators[task.first])(state, task.second);
	if (verbose > 2)
	{
		std::cout << "depth = " << depth << " new state: " << std::endl;
		print_state(newstate, 14);
	}
	if (ReturnedValue::True == newstate.first)
	{
		tasks.pop_back();
		plan.second.push_back(task);
		bTasks solution = seek_plan(newstate.second, tasks, operators, methods, plan, depth + 1, verbose);
		if (ReturnedValue::False != solution.first)
			return solution;
	}

	return { newstate.first, {} };
}

bTasks search_methods(State& state, Tasks tasks, Operators& operators, Methods& methods, bTasks plan, Task task, unsigned int& depth, unsigned short verbose = 0)
{
	if (verbose > 2)
		std::cout << "depth = " << depth << " method instance = " << task.first << std::endl;
	std::vector<Ptr2Method> relevant = methods[task.first];
	if (verbose > 2)
	{
		std::cout << "relevant: {";
		for (std::vector<Ptr2Method>::iterator r = relevant.begin(); r != relevant.end(); )
		{
			std::cout << (*r);
			if (++r != relevant.end())
				std::cout << ", ";
		}
		std::cout << "}" << std::endl;
	}

	for (std::vector<Ptr2Method>::iterator r = relevant.begin(); r != relevant.end(); r++)
	{
		bTasks SubTasks = (*r)(state, task.second);
		// Can't just say "if subtasks:", because that's wrong if subtasks == []
		if (verbose > 2)
		{
			std::cout << "method = " << (*r) << std::endl;
			std::cout << "depth = " << depth << " new tasks: {";
			for (Tasks::iterator r = SubTasks.second.begin(); r != SubTasks.second.end(); )
			{
				std::cout << r->first;
				if (++r != SubTasks.second.end())
					std::cout << ", ";
			}
			std::cout << "}" << std::endl;
		}
		if (ReturnedValue::False != SubTasks.first)
		{
			tasks.pop_back();
			for (Tasks::iterator i = SubTasks.second.end(); i != SubTasks.second.begin(); )
				tasks.push_back(*(--i));
			bTasks solution = seek_plan(state, tasks, operators, methods, plan, depth + 1, verbose);
			if (ReturnedValue::False != solution.first)
				return solution;
		}
	}

	return { ReturnedValue::None, {} };
}

// Workhorse for pyhop. state, tasks, operators, and methods are as in the plan function.
// - plan is the current partial plan.
// - depth is the recursion depth, for use in debugging
// - verbose is whether to print debugging messages
bTasks seek_plan(State& state, Tasks tasks, Operators& operators, Methods& methods, bTasks plan, unsigned int depth, unsigned short verbose = 0)
{
	if (verbose > 1)
	{
		std::cout << "depth = " << depth << " tasks = [";
		for (Tasks::iterator t = tasks.begin(); t != tasks.end(); )
		{
			std::cout << t->first;
			if (++t != tasks.end())
				std::cout << ", ";
		}
		std::cout << "]" << std::endl;
	}
	if (tasks.empty())
	{
		if (verbose > 2)
			std::cout << "depth = " << depth << " returns plan = " << std::endl;

		return bTasks(ReturnedValue::True, plan.second);
	}
	Task task = tasks.back();

	if (operators.end() != operators.find(task.first))
		return search_operators(state, tasks, operators, methods, plan, task, depth, verbose);

	if (methods.end() != methods.find(task.first))
		return search_methods(state, tasks, operators, methods, plan, task, depth, verbose);

	if (verbose > 2)
		std::cout << "depth = " << depth << " returns failure." << std::endl;

	return { ReturnedValue::False, {} };
}

// Try to find a plan that accomplishes tasks in state.
// If successful, return the plan. Otherwise return False.
bTasks plan(State& state, Tasks tasks, Operators& operators, Methods& methods, unsigned short verbose = 0)
{
	if (verbose > 0)
	{
		std::cout << "** hop++, verbose = " << verbose << ": **" << std::endl;
		std::cout << "   state = " << state.get_name() << std::endl;
	}
	bTasks result = seek_plan(state, tasks, operators, methods, { ReturnedValue::True, {} }, 0, verbose);
	if (verbose > 0)
	{
		if (ReturnedValue::True == result.first)
		{
			std::cout << "** result = [";
			for (Tasks::iterator it_t = result.second.begin(); it_t != result.second.end(); )
			{
				std::cout << "(" << it_t->first;
				it_t->second.print();
				std::cout << ")";
				if (++it_t != result.second.end())
					std::cout << ", ";
			}
			std::cout << "]" << std::endl;
		}
		else
			std::cout << "** result = " << std::string(GetStringReturnedValue(result.first)) << std::endl;
	}

	return result;
}

// ############################################################

int main()
{
	///////////////////////////////////////////////////////////////////////////////////////////////
	//
	//   FIRST TESTS
	//
	///////////////////////////////////////////////////////////////////////////////////////////////
	print_state({ ReturnedValue::False, empty });
	print_state({ ReturnedValue::True, empty });

	State state1("State1");
	state1.loc = { {Agent::me, Location::home} };
	state1.cash = { {Agent::me, 20.0f} };
	state1.owe = { {Agent::me, 0.0f} };
	state1.dist = { {{Location::home, Location::park}, 8.0f}, {{Location::park, Location::home}, 8.0f} };

	print_state({ ReturnedValue::True, state1 });
	print_state({ ReturnedValue::False, state1 });

	///////////////////////////////////////////////////////////////////////////////////////////////
	//
	//   TRAVEL
	//
	///////////////////////////////////////////////////////////////////////////////////////////////
	// Declare Travel operators
	declare_operators(OperatorId("walk"), walk);
	declare_operators(OperatorId("call_taxi"), call_taxi);
	declare_operators(OperatorId("ride_taxi"), ride_taxi);
	declare_operators(OperatorId("pay_driver"), pay_driver);

	print_operators(operators);
	std::cout << std::endl;

	bState ort2 = (operators[OperatorId("call_taxi")])(state1, Parameters(Agent::me, Location::home));

	// Declare Travel Methods
	declare_methods(TaskId("travel"), travel_by_foot, travel_by_taxi);

	print_methods(methods);
	std::cout << std::endl;

	std::cout << std::endl 
			  << "*************************************************************************************" << std::endl
			  << "Call plan(state1, [('travel', 'me', 'home', 'park')]) with different verbosity levels" << std::endl
			  << "*************************************************************************************" << std::endl
			  << std::endl;

	std::cout << "- If verbose=0 (the default), hop++ returns the solution but prints nothing." << std::endl;
	plan(state1, { { TaskId("travel"), Parameters(Agent::me, Location::home, Location::park) } }, operators, methods);

	std::cout << "- If verbose=1, hop++ prints the problem and solution, and returns the solution:" << std::endl;
	plan(state1, { { TaskId("travel"), Parameters(Agent::me, Location::home, Location::park) } }, operators, methods, 1);

	std::cout << "- If verbose=2, hop++ also prints a note at each recursive call:" << std::endl;
	plan(state1, { { TaskId("travel"), Parameters(Agent::me, Location::home, Location::park) } }, operators, methods, 2);

	std::cout << "- If verbose=3, hop++ also prints the intermediate states:" << std::endl;
	plan(state1, { { TaskId("travel"), Parameters(Agent::me, Location::home, Location::park) } }, operators, methods, 3);

	std::cout << std::endl;

	///////////////////////////////////////////////////////////////////////////////////////////////
	//
	//   BLOCKSWORLD
	//
	///////////////////////////////////////////////////////////////////////////////////////////////
	// Declare Blocksworld operators
	operators.clear();
	operators[OperatorId("pickup")] = pickup;
	operators[OperatorId("unstack")] = unstack;
	operators[OperatorId("putdown")] = putdown;
	operators[OperatorId("stack")] = stack;

	// Declare Blocksworld methods
	methods.clear();
	declare_methods(MethodId("move_blocks"), moveb_m);
	declare_methods(MethodId("move_one"), move1);
	declare_methods(MethodId("get"), get_m);
	declare_methods(MethodId("put"), put_m);

	print_operators(operators);
	std::cout << std::endl;

	// #############     beginning of blocksworld tests     ################

	std::cout << std::endl 
			  << "************************************************************" << std::endl
			  << "First, test pyhop on some of the operators and smaller tasks" << std::endl
			  << "************************************************************" << std::endl
			  << std::endl;

	std::cout << "- Define state1: a on b, b on tale, c on table" << std::endl;

	state1.Clear();
	state1.set_name("State1");
	state1.pos = { { Block::a, Block::b }, { Block::b, Block::table}, { Block::c, Block::table} };
	state1.clear = { { Block::c, true }, { Block::b, false}, { Block::a, true } };
	state1.holding = Block::none;

	print_state({ ReturnedValue::True, state1 });
	std::cout << std::endl;

	std::cout << "- these should fail:" << std::endl;
	plan(state1, { { TaskId("pickup"), Parameters(Block::a) } }, operators, methods, 3);	// a is clear but is not on the table -- pickup requires a block to be both clear and on the table
	plan(state1, { { TaskId("pickup"), Parameters(Block::b) } }, operators, methods, 3);	// b is on the table but is not clear -- pickup requires a block to be both clear and on the table

	std::cout << "- these should succeed:" << std::endl;
	plan(state1, { { TaskId("pickup"), Parameters(Block::c) } }, operators, methods, 3);			// a is both clear and on the table
	plan(state1, { { TaskId("unstack"), Parameters(Block::a, Block::b) } }, operators, methods, 1);	// a is both clear and atop of b
	plan(state1, { { TaskId("get"), Parameters(Block::a) } }, operators, methods, 1);
	std::cout << "- this should fail:" << std::endl;
	plan(state1, { { TaskId("get"), Parameters(Block::b) } }, operators, methods, 1);
	std::cout << "- this should succeed:" << std::endl;
	plan(state1, { { TaskId("get"), Parameters(Block::c) } }, operators, methods, 1);

	std::cout << std::endl
			  << "**************************************************************************" << std::endl
			  << "Run pyhop on two block - stacking problems, both of which start in state1." << std::endl
			  << "The goal for the 2nd problem omits some of the conditions in the goal"	  << std::endl
			  << "of the 1st problem, but those conditions will need to be achieved"		  << std::endl
			  << "anyway, so both goals should produce the same plan."						  << std::endl
			  << "**************************************************************************" << std::endl
			  << std::endl;

	std::cout << "- Define goal1a:"
			  << std::endl;

	//	A goal is a collection of some(but not necessarily all) of the state variables
	//	and their desired values. Below, both goal1aand goal1b specify c on b, and b
	//	on a. The difference is that goal1a also specifies that a is on table and the
	//	hand is empty.

	Goal goal1a = Goal("goal1a");
	goal1a.pos = { { Block::c, Block::b }, { Block::b, Block::a }, { Block::a, Block::table} };
	goal1a.clear = { { Block::c, true}, { Block::b, false }, { Block::a, false} };
	goal1a.holding = Block::none;

	print_goal(goal1a);

	std::cout << std::endl
			  << "- Define goal1b:"
			  << std::endl;

	Goal goal1b = Goal("goal1b");
	goal1b.pos = { { Block::c, Block::b }, { Block::b, Block::a } };

	print_goal(goal1b);

	std::cout << std::endl;

	// goal1b omits some of the conditions of goal1a,
	// but those conditions will need to be achieved anyway

	plan(state1, { { TaskId("move_blocks"), Parameters(goal1a) } }, operators, methods, 1);
	plan(state1, { { TaskId("move_blocks"), Parameters(goal1b) } }, operators, methods, 1);

	std::cout << std::endl
			  << "**********************************************************************" << std::endl
			  << "Run pyhop on two more planning problems. As before, the 2nd goal omits" << std::endl
			  << "some of the conditions in the 1st goal, but both goals should produce"  << std::endl
			  << "the same plan."														  << std::endl
			  << "**********************************************************************" << std::endl
			  << std::endl;

	std::cout << "- Define state 2:"
			  << std::endl;

	State state2("state2");
	state2.pos = { { Block::a, Block::c }, { Block::b, Block::d }, { Block::c, Block::table }, { Block::d, Block::table} };
	state2.clear = { { Block::a, true }, { Block::c, false }, { Block::b, true }, { Block::d, false} };
	state2.holding = Block::none;

	print_state({ ReturnedValue::True, state2 });

	std::cout << std::endl
			  << "- Define goal2a:"
			  << std::endl;

	Goal goal2a("goal2a");
	goal2a.pos = { { Block::b, Block::c }, { Block::a, Block::d }, { Block::c, Block::table }, { Block::d, Block::table } };
	goal2a.clear = { { Block::a, true }, { Block::c, false }, { Block::b, true }, { Block::d, false } };
	goal2a.holding = Block::none;

	print_goal(goal2a);

	std::cout << std::endl
			  << "- Define goal2b:"
			  << std::endl;

	Goal goal2b("goal2b");
	goal2b.pos = { { Block::b, Block::c }, { Block::a, Block::d } };

	print_goal(goal2b);
	std::cout << std::endl;

	// goal2b omits some of the conditions of goal2a,
	// but those conditions will need to be achieved anyway.

	plan(state2, { { TaskId("move_blocks"), Parameters(goal2a) } }, operators, methods, 1);
	plan(state2, { { TaskId("move_blocks"), Parameters(goal2b) } }, operators, methods, 1);

	std::cout << std::endl 
			  << "*********************************************************************" << std::endl
			  << "Test pyhop on planning problem bw_large_d from the SHOP distribution." << std::endl
			  << "*********************************************************************" << std::endl
		      << std::endl;

	std::cout << "- Define state3:"
			  << std::endl;

	State state3("state3");
	state3.pos = { { Block::a, Block::l }, { Block::l, Block::m }, { Block::m, Block::table },
				   { Block::k, Block::j }, { Block::j, Block::e }, { Block::e, Block::d }, { Block::d, Block::n }, { Block::n, Block::o }, { Block::o, Block::table },
				   { Block::i, Block::h }, { Block::h, Block::g }, { Block::g, Block::f }, { Block::f, Block::table },
				   { Block::s, Block::r }, { Block::r, Block::q }, { Block::q, Block::p }, { Block::p, Block::c }, { Block::c, Block::b }, { Block::b, Block::table } };
	state3.clear = { { Block::a, true}, { Block::b, false}, { Block::c, false}, { Block::d, false}, { Block::e, false}, { Block::f, false}, { Block::g, false}, { Block::h, false},
					 { Block::i, true}, { Block::j, false},
					 { Block::k, true}, { Block::l, false}, { Block::m, false}, { Block::n, false}, { Block::o, false}, { Block::p, false}, { Block::q, false}, { Block::r, false},
					 { Block::s, true}};
	state3.holding = Block::none;

	print_state({ ReturnedValue::True, state3 });

	std::cout << std::endl
			  << "- Define goal3:"
			  << std::endl;

	Goal goal3("goal3");
	goal3.pos = { { Block::o, Block::m }, { Block::m, Block::h }, { Block::h, Block::i }, { Block::i, Block::d }, { Block::d, Block::table },
				  { Block::l, Block::b }, { Block::b, Block::c }, { Block::c, Block::p }, { Block::p, Block::k }, { Block::k, Block::g }, { Block::g, Block::f }, { Block::f, Block::table } };
	goal3.clear = { { Block::q, true },
					{ Block::o, true },
					{ Block::l, true} };

	print_goal(goal3);
	std::cout << std::endl;

	plan(state3, { { TaskId("move_blocks"), Parameters(goal3) } }, operators, methods, 1);

	std::cout << std::endl << "That's all folks!" << std::endl;
	
	return 0;
}
// ================================================================================================ End of file "main.cpp"


#include <iomanip>			// Visibility for std::setw, std::left, std::right
#include <iostream>			// Visibility for stc::cout, std::endl
#include <map>				// Visibility for std::map
#include <string>			// Visibility for std::string
#include <vector>			// Visibility for std::vector

#include "EnumAgent.h"			// Application Domain: Travel
#include "EnumLocation.h"		// Application Domain: Travel
#include "EnumReturnedValue.h"	// Visibility for None, False, and True

typedef std::string MethodId;
typedef std::string OperatorId;
typedef std::string TaskId;

class State {
	private:
		std::string name;

	public:
		typedef std::pair<ReturnedValue, State> bState;

		std::map<Agent, float> cash;
		std::map<std::pair<Location, Location>, float> dist;
		std::map<Agent, Location> loc;
		std::map<Agent, float> owe;

	public:
		State() : name("") {}
		State(std::string a_name) : name(a_name) {}

		~State() {}

		inline std::string get_name() const { return name; }
		void clear() {
			name = "";
			cash.clear();
			dist.clear();
			loc.clear();
			owe.clear();
		}
};

State empty;

struct Parameters {
	public:
		Agent a;
		Location x;
		Location y;

	public:
		Parameters() : a(Agent::none), x(Location::none), y(Location::none) {}
		Parameters(Agent a_a) : a(a_a), x(Location::none), y(Location::none) {}
		Parameters(Agent a_a, Location a_x) : a(a_a), x(a_x), y(Location::none) {}
		Parameters(Agent a_a, Location a_x, Location a_y) : a(a_a), x(a_x), y(a_y) {}

		void print()
		{
			std::string p("");
			if (Agent::none != a)
				p += (", " + std::string(GetStringAgent(a)));
			if (Location::none != x)
				p += (", " + std::string(GetStringLocation(x)));
			if (Location::none != y)
				p += (", " + std::string(GetStringLocation(y)));
			std::cout << p;
		}
};

// Declare Operators
using Ptr2Operator = State::bState(*)(State, Parameters);
typedef std::map<OperatorId, Ptr2Operator> Operators;

float taxi_rate(float dist)
{
	return (1.5f + 0.5f * dist);
}

State::bState walk(State state, Parameters p)
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

State::bState call_taxi(State state, Parameters p)
{
	State result = state;
	result.loc[Agent::taxi] = p.x;
	return { ReturnedValue::True, result };
}

State::bState ride_taxi(State state, Parameters p)
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

State::bState pay_driver(State state, Parameters p)
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

typedef std::pair<TaskId, Parameters> Task;
typedef std::vector<Task> Tasks;
typedef std::pair<ReturnedValue, Tasks> bTasks;

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

// Print each variable in state, indented by indent spaces.
void print_state(State::bState state, unsigned short indent = 4)
{
	if (ReturnedValue::True == state.first)
	{
		for (auto o : state.second.cash)
			std::cout << std::setw(indent) << "" << state.second.get_name() + "::cash::" + GetStringAgent(o.first) + " = " << o.second /* float */ << std::endl;
		for (auto o : state.second.dist)
			std::cout << std::setw(indent) << "" << state.second.get_name() + "::dist::{" + GetStringLocation(o.first.first) + "," + GetStringLocation(o.first.second) + "} = " << o.second /* float */ << std::endl;
		for (auto o : state.second.loc)
			std::cout << std::setw(indent) << "" << state.second.get_name() + "::loc::" + GetStringAgent(o.first) + " = " + GetStringLocation(o.second) << std::endl;
		for (auto o : state.second.owe)
			std::cout << std::setw(indent) << "" << state.second.get_name() + "::owe::" + GetStringAgent(o.first) + " = " << o.second /* float */ << std::endl;
	}
	else
		std::cout << "False" << std::endl;
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

// Declare methods
using Ptr2Method = bTasks(*)(State, Parameters);
typedef std::map<MethodId, Ptr2Method> SubTasks;

// Declare HTNs
typedef std::map<TaskId, std::vector<TaskId>> Methods;
Methods travel_methods = { {TaskId("travel"), {MethodId("travel_by_foot"), MethodId("travel_by_taxi")}} };

// Print out a table of what the methods are for each task
void print_methods(Methods mlist)
{
	std::cout << std::setw(14) << std::left << "TASK:" << std::right << "METHODS:" << std::endl;
	for (auto task : mlist)
	{
		std::cout << std::setw(14) << std::left << task.first;
		for (std::vector<MethodId>::iterator m = task.second.begin(); m != task.second.end();)
		{
			std::cout << (*m);
			if (++m != task.second.end())
				std::cout << ", ";
		}
		std::cout << "." << std::endl;
	}
}

// ############################################################
// The actual planner

bTasks seek_plan(State state, Tasks tasks, Operators operators, Methods methods, SubTasks subtasks, bTasks plan, unsigned int depth, unsigned short verbose);

bTasks search_operators(State state, Tasks tasks, Operators operators, Methods methods, SubTasks subtasks, bTasks plan, Task task, unsigned int& depth, unsigned short verbose = 0)
{
	if (verbose > 2)
		std::cout << "depth = " << depth << " action = " << task.first << std::endl;
	State::bState newstate = (operators[task.first])(state, task.second);
	if (verbose > 2)
	{
		std::cout << "depth = " << depth << " new state: " << std::endl;
		print_state(newstate, 14);
	}
	if (ReturnedValue::True == newstate.first)
	{
		bTasks newplan = plan;
		tasks.pop_back();
		newplan.second.push_back(task);
		bTasks solution = seek_plan(newstate.second, tasks, operators, methods, subtasks, newplan, depth + 1, verbose);
		if (ReturnedValue::False != solution.first)
			return solution;
	}

	return { newstate.first, {} };
}

bTasks search_methods(State state, Tasks tasks, Operators operators, Methods methods, SubTasks subtasks, bTasks plan, Task task, unsigned int& depth, unsigned short verbose = 0)
{
	if (verbose > 2)
		std::cout << "depth = " << depth << " method instance = " << task.first << std::endl;
	std::vector<TaskId> relevant = methods[task.first];
	if (verbose > 2)
	{
		std::cout << "relevant: {";
		for (std::vector<TaskId>::iterator r = relevant.begin(); r != relevant.end(); )
		{
			std::cout << (*r);
			if (++r != relevant.end())
				std::cout << ", ";
		}
		std::cout << "}" << std::endl;
	}

	for (std::vector<TaskId>::iterator r = relevant.begin(); r != relevant.end(); r++)
	{
		const SubTasks::iterator it_st = subtasks.find(*r);
		if (it_st != subtasks.end())
		{
			bTasks SubTasks = (subtasks[(*r)])(state, task.second);
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
				bTasks solution = seek_plan(state, tasks, operators, methods, subtasks, plan, depth + 1, verbose);
				if (ReturnedValue::False != solution.first)
					return solution;
			}
		}
		else
			std::cout << "method >>" << (*r) << "<< is unknown; it may be mispelled." << std::endl;
	}
	return { ReturnedValue::None, {} };
}

// Workhorse for pyhop.state, tasks, operators, and methods are as in the plan function.
// - plan is the current partial plan.
// - depth is the recursion depth, for use in debugging
// - verbose is whether to print debugging messages
bTasks seek_plan(State state, Tasks tasks, Operators operators, Methods methods, SubTasks subtasks, bTasks plan, unsigned int depth, unsigned short verbose = 0)
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
	const Operators::iterator it_op = operators.find(task.first);
	if (it_op != operators.end())
		return search_operators(state, tasks, operators, methods, subtasks, plan, task, depth, verbose);

	const Methods::iterator it_me = methods.find(task.first);
	if (it_me != methods.end())
		return search_methods(state, tasks, operators, methods, subtasks, plan, task, depth, verbose);

	if (verbose > 2)
		std::cout << "depth = " << depth << " returns failure." << std::endl;

	return { ReturnedValue::False, {} };
}

// Try to find a plan that accomplishes tasks in state.
// If successful, return the plan. Otherwise return False.
bTasks plan(State state, Tasks tasks, Operators operators, Methods methods, SubTasks subtasks, unsigned short verbose = 0)
{
	if (verbose > 0)
	{
		std::cout << "** hop++, verbose = " << verbose << ": **" << std::endl;
		std::cout << "   state = " << state.get_name() << std::endl;
	}
	bTasks result = seek_plan(state, tasks, operators, methods, subtasks, {}, 0, verbose);
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
	print_state({ ReturnedValue::False, empty });
	print_state({ ReturnedValue::True, empty });

	State state1("State1");
	state1.loc = { {Agent::me, Location::home} };
	state1.cash = { {Agent::me, 20.0f} };
	state1.owe = { {Agent::me, 0.0f} };
	state1.dist = { {{Location::home, Location::park}, 8.0f}, {{Location::park, Location::home}, 8.0f} };

	print_state({ ReturnedValue::True, state1 });
	print_state({ ReturnedValue::False, state1 });

	// Declare operators
	Operators operators;
	operators[OperatorId("walk")] = walk;
	operators[OperatorId("call_taxi")] = call_taxi;
	operators[OperatorId("ride_taxi")] = ride_taxi;
	operators[OperatorId("pay_driver")] = pay_driver;

	print_operators(operators);
	
	State::bState ort2 = (operators[OperatorId("call_taxi")])(state1, Parameters(Agent::me, Location::home));

	// print('')
	std::cout << std::endl;

	// Declare Methods
	SubTasks subtasks;
	subtasks[MethodId("travel_by_foot")] = travel_by_foot;
	subtasks[MethodId("travel_by_taxi")] = travel_by_taxi;

	print_methods(travel_methods);

	Task Initial = { TaskId("travel"), Parameters(Agent::me, Location::home, Location::park) };

	std::cout << "*************************************************************************************" << std::endl
			  << "Call plan(state1, [('travel', 'me', 'home', 'park')]) with different verbosity levels" << std::endl
			  << "*************************************************************************************" << std::endl;

	std::cout << "- If verbose=0 (the default), hop++ returns the solution but prints nothing." << std::endl;
	plan(state1, { Initial }, operators, travel_methods, subtasks);

	std::cout << "- If verbose=1, hop++ prints the problem and solution, and returns the solution:" << std::endl;
	plan(state1, { Initial }, operators, travel_methods, subtasks, 1);

	std::cout << "- If verbose=2, hop++ also prints a note at each recursive call:" << std::endl;
	plan(state1, { Initial }, operators, travel_methods, subtasks, 2);

	std::cout << "- If verbose=3, hop++ also prints the intermediate states:" << std::endl;
	plan(state1, { Initial }, operators, travel_methods, subtasks, 3);
	return 0;
}
from __future__ import print_function
import copy
import sys

############################################################

def forall(seq,cond):
    """True if cond(x) holds for all x in seq, otherwise False."""
    for x in seq:
        if not cond(x): return False
    return True

def find_if(cond,seq):
    """
    Return the first x in seq such that cond(x) holds, if there is one.
    Otherwise return None.
    """
    for x in seq:
        if cond(x): return x
    return None

def is_done(b1,state,goal, done_state):
    if b1 == done_state: return True
    if b1 in goal.pos and goal.pos[b1] != state.pos[b1]:
        return False
    if state.pos[b1] == done_state: return True
    return is_done(state.pos[b1],state,goal,done_state)

def all(state):
    return state.clear.keys()

def print_state(state,indent=4):
    """Print each variable in state, indented by indent spaces."""
    if state != False:
        for (name,val) in vars(state).items():
            if name != '__name__':
                for x in range(indent): sys.stdout.write(' ')
                sys.stdout.write(state.__name__ + '.' + name)
                print(' =', val)
    else: print('False')

print_goal = print_state

def print_operators(olist):
    """Print out the names of the operators"""
    print('OPERATORS:', ', '.join(olist))

def print_methods(mlist):
    """Print out a table of what the methods are for each task"""
    print('{:<14}{}'.format('TASK:','METHODS:'))
    for task in mlist:
        print('{:<14}'.format(task) + ', '.join(
            [f.__name__ for f in mlist[task]]))

############################################################
# States and goals

class State():
    """A state is just a collection of variable bindings."""
    def __init__(self,name):
        self.__name__ = name

class Goal():
    """A goal is just a collection of variable bindings."""
    def __init__(self,name):
        self.__name__ = name

############################################################
# Commands to tell Pyhop what the operators and methods are

operators = {}
methods = {}

def declare_operators(*op_list):
    """
    Call this after defining the operators, to tell Pyhop what they are.
    op_list must be a list of functions, not strings.
    """
    operators.update({op.__name__:op for op in op_list})
    return operators

def declare_methods(task_name,*method_list):
    """
    Call this once for each task, to tell Pyhop what the methods are.
    task_name must be a string.
    method_list must be a list of functions, not strings.
    """
    methods.update({task_name:list(method_list)})
    return methods[task_name]

def get_operators():
    return operators

def get_methods():
    return methods

############################################################
# The actual planner

def plan(state,tasks,operators,methods,verbose=0):
    """
    Try to find a plan that accomplishes tasks in state.
    If successful, return the plan. Otherwise return False.
    """
    if verbose>0: print(
        '** hop, verbose={}: **\n   state = {}\n   tasks = {}'.format(
            verbose, state.__name__, tasks))
    result = seek_plan(state,tasks,operators,methods,[],0,verbose)
    if verbose>0: print('** result =',result,'\n')
    return result

def search_operators(state,tasks,operators,methods,plan,task,depth,verbose):
    if verbose>2:
        print('depth {} action {}'.format(depth,task))
    operator = operators[task[0]]
    newstate = operator(copy.deepcopy(state),*task[1:])
    if verbose>2:
        print('depth {} new state:'.format(depth))
        print_state(newstate)
    if newstate:
        solution = seek_plan(
            newstate,tasks[1:],operators,methods,plan+[task],depth+1,verbose)
        if solution != False:
            return solution

def search_methods(state,tasks,operators,methods,plan,task,depth,verbose):
    if verbose>2:
        print('depth {} method instance {}'.format(depth,task))
    relevant = methods[task[0]]
    print('relevant: {}'.format(relevant))
    for method in relevant:
        print('method: {}'.format(method))
        print('*task[1:]: {},{},{}.'.format(*task[1:]))
        subtasks = method(state,*task[1:])
        # Can't just say "if subtasks:", because that's wrong if
        # subtasks == []
        if verbose>2:
            print('depth {} new tasks: {}'.format(depth,subtasks))
        if subtasks != False:
            solution = seek_plan(
                state,subtasks+tasks[1:],operators,methods,plan,depth+1,verbose)
            if solution != False:
                return solution

def seek_plan(state,tasks,operators,methods,plan,depth,verbose=0):
    """
    Workhorse for pyhop. state, tasks, operators, and methods are as in the
    plam function.
    - plan is the current partial plan.
    - depth is the recursion depth, for use in debugging
    - verbose is whether to print debugging messages
    """
    if verbose>1:
        print('depth {} tasks {}'.format(depth,tasks))
    if tasks == []:
        if verbose>2:
            print('depth {} returns plan {}'.format(depth,plan))
        return plan
    task = tasks[0]
    if task[0] in operators:
        return search_operators(
            state,tasks,operators,methods,plan,task,depth,verbose)
    if task[0] in methods:
        return search_methods(
            state,tasks,operators,methods,plan,task,depth,verbose)
    if verbose>2:
        print('depth {} returns failure'.format(depth))
    return False

############################################################

def taxi_rate(dist):
    return (1.5 + 0.5 * dist)

def walk(state,a,x,y):
    if state.loc[a] == x:
        state.loc[a] = y
        return state
    else: return False

def call_taxi(state,a,x):
    state.loc['taxi'] = x
    return state

def ride_taxi(state,a,x,y):
    if state.loc['taxi']==x and state.loc[a]==x:
        state.loc['taxi'] = y
        state.loc[a] = y
        state.owe[a] = taxi_rate(state.dist[x][y])
        return state
    else: return False

def pay_driver(state,a):
    if state.cash[a] >= state.owe[a]:
        state.cash[a] = state.cash[a] - state.owe[a]
        state.owe[a] = 0
        return state
    else: return False

declare_operators(walk, call_taxi, ride_taxi, pay_driver)
print('')
print_operators(get_operators())

def travel_by_foot(state,a,x,y):
    if state.dist[x][y] <= 2:
        return [('walk',a,x,y)]
    return False

def travel_by_taxi(state,a,x,y):
    if state.cash[a] >= taxi_rate(state.dist[x][y]):
        return [('call_taxi',a,x), ('ride_taxi',a,x,y), ('pay_driver',a)]
    return False

declare_methods('travel')
print('')
print_methods(get_methods())

state1 = State('state1')
state1.loc = {'me':'home'}
state1.cash = {'me':20}
state1.owe = {'me':0}
state1.dist = {'home':{'park':8}, 'park':{'home':8}}

print("""
********************************************************************************
Call hop.plan(state1,[('travel','me','home','park')]) with different verbosity levels
********************************************************************************
""")

print("- If verbose=0 (the default), Pyhop returns the solution but prints nothing.\n")
plan(state1,
         [('travel','me','home','park')],
         get_operators(),
         get_methods())

print('- If verbose=1, Pyhop prints the problem and solution, and returns the solution:')
plan(state1,
         [('travel','me','home','park')],
         get_operators(),
         get_methods(),
         verbose=1)

print('- If verbose=2, Pyhop also prints a note at each recursive call:')
plan(state1,
         [('travel','me','home','park')],
         get_operators(),
         get_methods(),
         verbose=2)

print('- If verbose=3, Pyhop also prints the intermediate states:')
plan(state1,
         [('travel','me','home','park')],
         get_operators(),
         get_methods(),
         verbose=3)

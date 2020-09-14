from __future__ import print_function
import copy
import sys

###############################################################################
###
###  helpers.py
###

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
    if b1 in goal.pos and goal.pos[b1] != state.pos[b1]: return False
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

###############################################################################
###
###  hop.py
###

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
    for method in relevant:
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
    Workhorse for py state, tasks, operators, and methods are as in the
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

###############################################################################
###
###  operators.py
###

"""Each Pyhop planning operator is a Python function. The 1st argument is
the current state, and the others are the planning operator's usual arguments.
This is analogous to how methods are defined for Python classes (where
the first argument is always the name of the class instance). For example,
the function pickup(state,b) implements the planning operator for the task
('pickup', b).

The blocks-world operators use three state variables:
- pos[b] = block b's position, which may be 'table', 'hand', or another block.
- clear[b] = False if a block is on b or the hand is holding b, else True.
- holding = name of the block being held, or False if the hand is empty.
"""

def pickup(state,b):
    if (state.pos[b] == 'table'
        and state.clear[b] == True
        and state.holding == False):
        state.pos[b] = 'hand'
        state.clear[b] = False
        state.holding = b
        return state
    else: return False

def unstack(state,b,c):
    if (state.pos[b] == c
        and c != 'table'
        and state.clear[b] == True
        and state.holding == False):
        state.pos[b] = 'hand'
        state.clear[b] = False
        state.holding = b
        state.clear[c] = True
        return state
    else: return False

def putdown(state,b):
    if state.pos[b] == 'hand':
        state.pos[b] = 'table'
        state.clear[b] = True
        state.holding = False
        return state
    else: return False

def stack(state,b,c):
    if state.pos[b] == 'hand' and state.clear[c] == True:
        state.pos[b] = c
        state.clear[b] = True
        state.holding = False
        state.clear[c] = False
        return state
    else: return False

"""
Below, 'declare_operators(pickup, unstack, putdown, stack)' tells Pyhop
what the operators are. Note that the operator names are *not* quoted.
"""

declare_operators(pickup, unstack, putdown, stack)

###############################################################################
###
###  method1.py
###

"""
Blocks World methods for Pyhop 1.1.
Author: Dana Nau <nau@cs.umd.edu>, November 15, 2012
This file should work correctly in both Python 2.7 and Python 3.2.
"""

def status(b1,state,goal,done_state):
    """
    A helper function used in the methods' preconditions.
    """
    if is_done(b1,state,goal,done_state):
        return 'done'
    elif not state.clear[b1]:
        return 'inaccessible'
    elif not (b1 in goal.pos) or goal.pos[b1] == done_state:
        return 'move-to-table'
    elif (is_done(goal.pos[b1],state,goal,done_state) and
          state.clear[goal.pos[b1]]):
        return 'move-to-block'
    else:
        return 'waiting'


"""
In each Pyhop planning method, the first argument is the current state (this
is analogous to Python methods, in which the first argument is the class
instance). The rest of the arguments must match the arguments of the task
that the method is for. For example, ('pickup', b1) has a method
get_m(state,b1), as shown below.
"""

### methods for "move_blocks"

def moveb_m(state,goal):
    """
    This method implements the following block-stacking algorithm:
    If there's a block that can be moved to its final position, then
    do so and call move_blocks recursively. Otherwise, if there's a
    block that needs to be moved and can be moved to the table, then
    do so and call move_blocks recursively. Otherwise, no blocks need
    to be moved.
    """
    for b1 in all(state):
        s = status(b1,state,goal,'table')
        if s == 'move-to-table':
            return [('move_one',b1,'table'),('move_blocks',goal)]
        elif s == 'move-to-block':
            return [('move_one',b1,goal.pos[b1]), ('move_blocks',goal)]
        else:
            continue
    #
    # if we get here, no blocks can be moved to their final locations
    b1 = find_if(
        lambda x: status(x,state,goal,'table') == 'waiting',
        all(state))
    if b1 != None:
        return [('move_one',b1,'table'), ('move_blocks',goal)]
    #
    # if we get here, there are no blocks that need moving
    return []

"""
declare_methods must be called once for each taskname. Below,
'declare_methods('get',get_m)' tells Pyhop that 'get' has one method, get_m.
Notice that 'get' is a quoted string, and get_m is the actual function.
"""
declare_methods('move_blocks',moveb_m)


### methods for "move_one"
def move1(state,b1,dest):
    """
    Generate subtasks to get b1 and put it at dest.
    """
    return [('get', b1), ('put', b1,dest)]
declare_methods('move_one',move1)


### methods for "get"
def get_m(state,b1):
    """
    Generate either a pickup or an unstack subtask for b1.
    """
    if state.clear[b1]:
        if state.pos[b1] == 'table':
                return [('pickup',b1)]
        else:
                return [('unstack',b1,state.pos[b1])]
    else:
        return False
declare_methods('get',get_m)


### methods for "put"
def put_m(state,b1,b2):
    """
    Generate either a putdown or a stack subtask for b1.
    b2 is b1's destination: either the table or another block.
    """
    if state.holding == b1:
        if b2 == 'table':
                return [('putdown',b1)]
        else:
                return [('stack',b1,b2)]
    else:
        return False
declare_methods('put',put_m)

###############################################################################
###
###  run1.py
###
"""
Blocks-world test data for Pyhop 1.1.
Author: Dana Nau <nau@cs.umd.edu>, November 15, 2012
This file should work correctly in both Python 2.7 and Python 3.2.
"""

print('')
print_operators(get_operators())
print('')
print_methods(get_methods())

#############     beginning of tests     ################

print("""
****************************************
First, test pyhop on some of the operators and smaller tasks
****************************************
""")

print("- Define state1: a on b, b on tale, c on table")

"""
A state is a collection of all of the state variables and their values. Every
state variable in the domain should have a value.
"""

state1 = State('state1')
state1.pos={'a':'b', 'b':'table', 'c':'table'}
state1.clear={'c':True, 'b':False,'a':True}
state1.holding=False

print_state(state1)
print('')

print('- these should fail:')
plan(state1,[('pickup','a')], get_operators(), get_methods(), verbose=3)
plan(state1,[('pickup','b')], get_operators(), get_methods(), verbose=3)
print('- these should succeed:')
plan(state1,[('pickup','c')], get_operators(), get_methods(), verbose=3)
plan(state1,[('unstack','a','b')], get_operators(), get_methods(), verbose=1)
plan(state1,[('get','a')], get_operators(), get_methods(), verbose=1)
print('- this should fail:')
plan(state1,[('get','b')], get_operators(), get_methods(), verbose=1)
print('- this should succeed:')
plan(state1,[('get','c')], get_operators(), get_methods(), verbose=1)

print("""
****************************************
Run pyhop on two block-stacking problems, both of which start in state1.
The goal for the 2nd problem omits some of the conditions in the goal
of the 1st problem, but those conditions will need to be achieved
anyway, so both goals should produce the same plan.
****************************************
""")

print("- Define goal1a:")

"""
A goal is a collection of some (but not necessarily all) of the state variables
and their desired values. Below, both goal1a and goal1b specify c on b, and b
on a. The difference is that goal1a also specifies that a is on table and the
hand is empty.
"""

goal1a = Goal('goal1a')
goal1a.pos={'c':'b', 'b':'a', 'a':'table'}
goal1a.clear={'c':True, 'b':False, 'a':False}
goal1a.holding=False

print_goal(goal1a)
print('')

print("- Define goal1b:")

goal1b = Goal('goal1b')
goal1b.pos={'c':'b', 'b':'a'}

print_goal(goal1b)

### goal1b omits some of the conditions of goal1a,
### but those conditions will need to be achieved anyway


plan(state1,[('move_blocks', goal1a)], get_operators(), get_methods(), verbose=1)
plan(state1,[('move_blocks', goal1b)], get_operators(), get_methods(), verbose=1)

print("""
****************************************
Run pyhop on two more planning problems. As before, the 2nd goal omits
some of the conditions in the 1st goal, but both goals should produce
the same plan.
****************************************
""")

print("- Define state 2:")

state2 = State('state2')
state2.pos={'a':'c', 'b':'d', 'c':'table', 'd':'table'}
state2.clear={'a':True, 'c':False,'b':True, 'd':False}
state2.holding=False

print_state(state2)
print('')

print("- Define goal2a:")

goal2a = Goal('goal2a')
goal2a.pos={'b':'c', 'a':'d', 'c':'table', 'd':'table'}
goal2a.clear={'a':True, 'c':False,'b':True, 'd':False}
goal2a.holding=False

print_goal(goal2a)
print('')

print("- Define goal2b:")

goal2b = Goal('goal2b')
goal2b.pos={'b':'c', 'a':'d'}

print_goal(goal2b)
print('')


### goal2b omits some of the conditions of goal2a,
### but those conditions will need to be achieved anyway.

plan(state2,[('move_blocks', goal2a)], get_operators(), get_methods(), verbose=1)
plan(state2,[('move_blocks', goal2b)], get_operators(), get_methods(), verbose=1)


print("""
****************************************
Test pyhop on planning problem bw_large_d from the SHOP distribution.
****************************************
""")

print("- Define state3:")

state3 = State('state3')
state3.pos = {1:12, 12:13, 13:'table',
              11:10, 10:5, 5:4, 4:14, 14:15, 15:'table',
              9:8, 8:7, 7:6, 6:'table',
              19:18, 18:17, 17:16, 16:3, 3:2, 2:'table'}
state3.clear = {x:False for x in range(1,20)}
state3.clear.update({1:True, 11:True, 9:True, 19:True})
state3.holding = False

print_state(state3)
print('')

print("- Define goal3:")

goal3 = Goal('goal3')
goal3.pos = {15:13, 13:8, 8:9, 9:4, 4:'table', 12:2, 2:3, 3:16, 16:11, 11:7,
             7:6, 6:'table'}
goal3.clear = {17:True, 15:True, 12:True}

print_goal(goal3)
print('')

plan(state3,[('move_blocks', goal3)], get_operators(), get_methods(), verbose=1)

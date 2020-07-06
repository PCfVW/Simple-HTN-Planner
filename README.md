# Simple-HTN-Planner
PyHop is a simple Hierarchical Task Network (HTN) planner written in Python by Dana Nau:

[PyHop 2.0 Github repo](https://github.com/oubiwann/pyhop)

[Dana Nau's slides on HTN Planning -- PyHop and the "Travel" example are detailed](https://www.cs.umd.edu/users/nau/apa/slides/htn-planning.pdf)

Here is a trivial port of PyHop 2.0 in Visual Studio (2019) C++.

The file "MyPyHop.py" (master branch) contains the python code to run the PyHop "Travel" example; the file "BlocksWorldPyHop.py" (Blocksworld branch) contains the python code to run both the "Travel" and the "Blocksworld" examples from PyHop.

Both "MyPyHop.py" (master branch) and "BlocksWorldPyHop.py" (Blocksworld branch) have been tested with VS 2019 Python; I've uploaded both files so that you can compare the C++ code to the Python code.

I did my best to (i) use PyHop identifiers in the C++ code, and (ii) print exactly what and when PyHop does print something, so you should be able to follow the C++ code from the Python code.

Currently:

1. C++ code of the master branch only runs the "Travel" example from PyHop.

2. C++ code of the BlocksWorld branch runs both the "Travel" and the "Blocksworld" examples from PyHop.

3. In each branch, C++ code of both planner and examples is in one file (main.cpp).

4. Get the master branch to get a stable version which (only) runs the "Travel" example.

5. Get the BlocksWorld branch to get a stable version which runs all examples (both "Travel" and "Blocksworld"); this branch is closer to PyHop's Python code than the master branch.

6. I intend to merge the 2 branches and create a new branch where I'll split the C++ code as is done for the PyHop Python code.

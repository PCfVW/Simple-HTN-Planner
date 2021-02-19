Simple-HTN-Planner
==================

PyHop is a simple Hierarchical Task Network (HTN) planner written in Python by
Dana Nau:

[PyHop 2.0 Github repo](https://github.com/oubiwann/pyhop)

[Dana Nau's slides on HTN Planning -- PyHop and the "Travel" example are
detailed](https://www.cs.umd.edu/users/nau/apa/slides/htn-planning.pdf)

Here is a trivial port of PyHop 2.0 in Visual Studio (2019) C++17; g++ 9.3.0
will also compile and run this port (thanks to Johannes Haindl for reporting
about this).

The file "MyPyHop.py" contains the python code to run the PyHop "Travel"
example; the file "BlocksWorldPyHop.py" contains the python code to run both the
"Travel" and the "Blocksworld" examples from PyHop.

Both "MyPyHop.py" and "BlocksWorldPyHop.py" have been tested with VS 2019
Python; I've uploaded both files so that you can compare the C++ code to the
Python code.

I did my best to (i) use PyHop identifiers in the C++ code, and (ii) print
exactly what and when PyHop does print something, so you should be able to
follow the C++ code from the Python code.

I fixed a bug in PyHop 2.0’s blocksworld HTN which prevented me from solving IPC
2011 blocksworld problems; C++ code has been updated accordingly.

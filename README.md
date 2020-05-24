# Simple-HTN-Planner
PyHop is a simple Hierarchical Task Network (HTN) planner written in Python by [Dana Nau](https://www.cs.umd.edu/users/nau/); here is a trivial port of PyHop 2.0 in Visual Studio (2019) C++.

[PyHop 2.0 Github repo](https://github.com/oubiwann/pyhop)

[Dana Nau's slides on HTN Planning -- PyHop and the "Travel" example are detailed](https://www.cs.umd.edu/users/nau/apa/slides/htn-planning.pdf)

The file "MyHop.py" contains the python code to run the PyHop "Travel" example; tested with VS 2019 Python. I've uploaded this file so that you can compare the C++ code to the Python code.

I did my best to keep PyHop identificators in the C++ code, and to print exactly what and when PyHop does print, so you should be able to follow the C++ code from the Python code.

Currently:
1. C++ code only runs the travel example from PyHop; other examples will follow.

2. Planner and (travel) example are in one file (main.cpp); further commits will split planner and examples (planning domains), and I'll try to keep the C++ code as simple as possible.

I've branched this project to produce a version able to run PyHop's blocks-world examples.

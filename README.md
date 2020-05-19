# Simple-HTN-Planner
PyHop is a simple hierarchical task network (HTN) planner written in Python by [Dana Nau](https://www.cs.umd.edu/users/nau/); here is a trivial port of PyHop 2.0 in Visual Studio (2019) C++.

[PyHop 2.0 Github repo](https://github.com/oubiwann/pyhop)

[Dana Nau's Slides on HTN Planning -- PyHop and the travel example are detailed](https://www.cs.umd.edu/users/nau/apa/slides/htn-planning.pdf)

I did my best to keep PyHop identificators in the C++ code, so you should be able to follow the C++ code from the Python code.

C++ code only runs the travel example from PyHop; other examples will follow.

Currently, planner and (travel) example are in one file (main.cpp). Later commits will split planner and examples (planning domains), I'll try to keep the C++ code as simple as possible.

The file "MyHop.py" contains the python code to run the PyHop "Travel" example; tested with VS 2019 Python. I've uploaded this file so that you can compare the C++ code to the Python code.

# C++ and Robotics Interview Prep

A complete, self-contained study path for C++ coding rounds and robotics system design
conversations at robotics and AI companies. Written for someone who knows Python and is
learning C++ from scratch.

Every idea starts with a picture or a story, then a small example worked by hand, then the
Python you would have written, then the C++ with every new symbol explained. Every problem is
a compilable, self-testing C++ file.

## What is here

| Folder | Contents |
|---|---|
| [tutorial/](tutorial/README.md) | Seven chapters: C++ basics, memory and the STL, grids and BFS/DFS, graphs and Dijkstra, heaps/trees/design, robotics-specific builds, interview day. Plus a list of UK robotics employers. |
| [code/](code/) | One standalone `.cpp` per problem with `assert`-based tests, a Makefile that builds and runs everything, and an A* grid planner as a CMake project with GoogleTest. |
| [system-design/](system-design/README.md) | Five levels of robotics system design, from "what is a system" to full design questions worked end to end. |
| [pdf/](pdf/) | Every chapter and level rendered as a PDF, plus two combined books. |
| [cpp-interview-tracker.html](cpp-interview-tracker.html) | A progress tracker for every topic and problem. Open it in a browser. |

## Quick start

Requirements: `g++` with C++17, `cmake`, Eigen 3 and GoogleTest. On Ubuntu:

```bash
sudo apt install build-essential cmake libeigen3-dev libgtest-dev
```

Build and run every example, including the A* test suite:

```bash
cd code
make
```

Compile one file by hand:

```bash
g++ -std=c++17 -Wall -Wextra -O2 code/01-cpp-basics/01_two_sum.cpp -o two_sum && ./two_sum
```

Rebuild the PDFs after editing any markdown file (needs `python3-markdown` and WeasyPrint):

```bash
python3 pdf/build_pdfs.py
```

## How to study

1. Start with [tutorial/README.md](tutorial/README.md); it has the six-week plan.
2. Read a chapter's concept part and do every hand trace on paper.
3. For each problem, read only the statement, close the file, and solve it in C++ in 25 minutes
   with `-Wall -Wextra`. Then compare with the solution.
4. Mark progress in the tracker. Only mark a concept done when you can explain it to someone else.
5. Start the system design levels after chapter 03, one level per sitting.

## Layout

```
tutorial/          chapters 01–08 and the index
code/
  Makefile         make | make 03 | make astar
  01-cpp-basics/   ... 06-robotics/
  03-grids-bfs-dfs/astar/   CMake + GoogleTest project
system-design/     levels 1–5 and the index
pdf/               rendered PDFs and build_pdfs.py
```

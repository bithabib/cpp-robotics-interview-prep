# Builds and runs every standalone example in this tutorial.
#
#   make            build + run everything (stops at the first failure)
#   make 03         build + run only chapter 03
#   make astar      build the A* project and run its GoogleTest suite
#   make clean
#
# Every .cpp is standalone: it has its own main() with assert-based tests and
# prints "OK <name>" on success.

CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
EIGEN    := -isystem /usr/include/eigen3
BUILD    := build

CHAPTERS := 01-cpp-basics 02-cpp-interviewers-test 03-grids-bfs-dfs \
            04-graphs-dijkstra 05-heaps-trees-design 06-robotics

SRCS := $(foreach c,$(CHAPTERS),$(wildcard $(c)/*.cpp))
BINS := $(patsubst %.cpp,$(BUILD)/%,$(SRCS))
RUNS := $(patsubst %,%.ok,$(BINS))

.PHONY: all clean astar 01 02 03 04 05 06

all: $(RUNS) astar
	@echo
	@echo "ALL EXAMPLES PASSED"

$(BUILD)/%: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(EIGEN) $< -o $@

$(BUILD)/%.ok: $(BUILD)/%
	@./$< && touch $@

01: $(filter $(BUILD)/01-%,$(RUNS))
02: $(filter $(BUILD)/02-%,$(RUNS))
03: $(filter $(BUILD)/03-%,$(RUNS))
04: $(filter $(BUILD)/04-%,$(RUNS))
05: $(filter $(BUILD)/05-%,$(RUNS))
06: $(filter $(BUILD)/06-%,$(RUNS))

astar:
	cmake -S 03-grids-bfs-dfs/astar -B $(BUILD)/astar -DCMAKE_BUILD_TYPE=Release > /dev/null
	+cmake --build $(BUILD)/astar > /dev/null
	ctest --test-dir $(BUILD)/astar --output-on-failure

clean:
	rm -rf $(BUILD)

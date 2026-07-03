SRCS := $(wildcard src/*.cpp)
OBJS := $(SRCS:src/%.cpp=build/%.o)

LLVM_LIBS  := `llvm-config --libs core native executionengine orcjit x86codegen --system-libs`
LLVM_FLAGS := `llvm-config --cxxflags --ldflags`

build/flint: $(OBJS)
	g++ $(OBJS) $(LLVM_FLAGS) $(LLVM_LIBS) -o build/flint


build/%.o: src/%.cpp
	@mkdir -p build
	g++ -std=c++17 $(shell llvm-config --cxxflags) -c $< -o $@

clean:
	rm -rf build

.PHONY: clean
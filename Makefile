CXX = g++
CXXFLAGS = -std=c++20 -O3 -march=native -mbmi2 -g -Isource

TARGET = tempo.exe

SRCS := $(wildcard source/*.cpp) \
        $(wildcard source/*/*.cpp) \
        $(wildcard source/*/*/*.cpp)

OBJS := $(patsubst source/%.cpp, build/%.o, $(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

build/%.o: source/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf build $(TARGET)

.PHONY: all clean
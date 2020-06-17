CXXFLAGS=-Wall -Wextra -Wconversion -Werror -std=c++17 -pedantic -O3
LIBS=-lm

sort: main.cpp
	$(CXX) $(CXXFLAGS) -o sort main.cpp $(LIBS)

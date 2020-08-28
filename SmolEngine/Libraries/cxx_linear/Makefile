CXX = /home/ed/bin/bin/g++ -std=c++17 -g -Wall -Wextra

all: test_lerp test_midpoint

test_lerp: test_lerp.cpp lerp.h
	$(CXX) -o test_lerp test_lerp.cpp

test_midpoint: test_midpoint.cpp midpoint.h
	$(CXX) -o test_midpoint test_midpoint.cpp

clean:
	rm -f a.out test_lerp test_midpoint

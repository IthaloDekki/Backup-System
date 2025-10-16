CXX = g++
CXXFLAGS = -std=c++14 -Wall

SRC = velha.cpp catch_amalgamated.cpp
TEST = testa_velha.cpp
OBJ = $(SRC:.cpp=.o)

all: testa_velha
	./testa_velha
	#use comentario se necessario

compile: $(OBJ) $(TEST)
	$(CXX) $(CXXFLAGS) -o testa_velha $(OBJ) $(TEST)

velha.o : velha.cpp velha.hpp
	$(CXX) $(CXXFLAGS) -c velha.cpp

catch_amalgamated.o: catch_amalgamated.cpp catch_amalgamated.hpp
	$(CXX) $(CXXFLAGS) -c catch_amalgamated.cpp

testa_velha: $(OBJ) $(TEST)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ) $(TEST)

test: testa_velha
	./testa_velha

cpplint:
	cpplint velha.cpp velha.hpp testa_velha.cpp

gcov: $(SRC) $(TEST)
	$(CXX) $(CXXFLAGS) -fprofile-arcs -ftest-coverage -c velha.cpp
	$(CXX) $(CXXFLAGS) -fprofile-arcs -ftest-coverage -c catch_amalgamated.cpp
	$(CXX) $(CXXFLAGS) -fprofile-arcs -ftest-coverage velha.o catch_amalgamated.o $(TEST) -o testa_velha
	./testa_velha
	gcov velha.cpp

debug: $(SRC) $(TEST)
	$(CXX) $(CXXFLAGS) -g -c velha.cpp
	$(CXX) $(CXXFLAGS) -g -c catch_amalgamated.cpp
	$(CXX) $(CXXFLAGS) -g velha.o catch_amalgamated.o $(TEST) -o testa_velha
	gdb testa_velha

cppcheck:
	cppcheck --enable=warning --std=c++14 \
		--check-level=exhaustive \
		--suppress=missingIncludeSystem \
		--suppress=uninitMemberVar \
		--suppress=duplInheritedMember \
		--suppress=syntaxError:catch_amalgamated.hpp \
		--suppress=syntaxError:testa_velha.cpp \
		--force \
		velha.cpp velha.hpp testa_velha.cpp


valgrind: testa_velha
	valgrind --leak-check=yes --log-file=valgrind.rpt ./testa_velha

clean:
	rm -rf *.o *.exe *.gc* testa_velha
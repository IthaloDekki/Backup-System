# ============================================
# Makefile para Sistema de Backup com Catch2
# ============================================

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude
LDFLAGS = -lstdc++fs           # <- Necessário para <filesystem> (GCC até 11)

SRCDIR = src
INCDIR = include
TESTDIR = tests

SRC = $(SRCDIR)/backup.cpp $(SRCDIR)/catch_amalgamated.cpp
TEST = $(TESTDIR)/testa_backup.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = testa_backup

.PHONY: all compile test cpplint cppcheck gcov debug valgrind docs clean

# ============================================
# Regras principais
# ============================================

all: $(TARGET)
	./$(TARGET)

# Linka executável de testes
$(TARGET): $(OBJ) $(TEST)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ) $(TEST) $(LDFLAGS)

# Compila .cpp → .o
$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

compile: $(TARGET)

test: $(TARGET)
	./$(TARGET)

# ============================================
# Análise de código
# ============================================

cpplint:
	python3 -m cpplint $(SRCDIR)/backup.cpp $(INCDIR)/backup.hpp $(TEST)

cppcheck:
	cppcheck --enable=warning --std=c++17 \
		--check-level=exhaustive \
		--suppress=missingIncludeSystem \
		--suppress=uninitMemberVar \
		--suppress=duplInheritedMember \
		--suppress=syntaxError:$(INCDIR)/catch_amalgamated.hpp \
		--suppress=syntaxError:$(TEST) \
		--force \
		$(SRCDIR) $(INCDIR) $(TESTDIR)

# ============================================
# Cobertura (gcov)
# ============================================

gcov: clean
	$(CXX) $(CXXFLAGS) -fprofile-arcs -ftest-coverage -c $(SRCDIR)/backup.cpp
	$(CXX) $(CXXFLAGS) -fprofile-arcs -ftest-coverage -c $(SRCDIR)/catch_amalgamated.cpp
	$(CXX) $(CXXFLAGS) -fprofile-arcs -ftest-coverage $(SRCDIR)/backup.o $(SRCDIR)/catch_amalgamated.o $(TEST) -o $(TARGET) $(LDFLAGS)
	./$(TARGET)
	gcov -o . $(SRCDIR)/backup.cpp

# ============================================
# Debug e Valgrind
# ============================================

debug:
	$(CXX) $(CXXFLAGS) -g -c $(SRCDIR)/backup.cpp
	$(CXX) $(CXXFLAGS) -g -c $(SRCDIR)/catch_amalgamated.cpp
	$(CXX) $(CXXFLAGS) -g $(SRCDIR)/backup.o $(SRCDIR)/catch_amalgamated.o $(TEST) -o $(TARGET) $(LDFLAGS)
	gdb $(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=yes --show-leak-kinds=all --log-file=valgrind.rpt ./$(TARGET)

# ============================================
# Geração de documentação (Doxygen)
# ============================================

docs: Doxyfile
	doxygen Doxyfile
	@echo "📘 Documentação gerada em ./docs/html/index.html"

Doxyfile:
	doxygen -g Doxyfile
	sed -i 's|^INPUT .*|INPUT = src include|' Doxyfile
	sed -i 's|^RECURSIVE .*|RECURSIVE = NO|' Doxyfile
	sed -i 's|^OUTPUT_DIRECTORY .*|OUTPUT_DIRECTORY = docs|' Doxyfile
	sed -i 's|^EXCLUDE .*|EXCLUDE = src/catch_amalgamated.cpp include/catch_amalgamated.hpp|' Doxyfile
	sed -i 's|^GENERATE_LATEX .*|GENERATE_LATEX = NO|' Doxyfile
	sed -i 's|^GENERATE_HTML .*|GENERATE_HTML = YES|' Doxyfile
	sed -i 's|^EXTRACT_ALL .*|EXTRACT_ALL = YES|' Doxyfile

# ============================================
# Limpeza
# ============================================

clean:
	rm -rf $(SRCDIR)/*.o *.o *.gc* $(TARGET) valgrind.rpt docs Doxyfile *.gcov *.info
	rm -rf backup-destino/*

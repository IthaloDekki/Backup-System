# ============================================
# Makefile para Sistema de Backup (TP2 - TDD)
# ============================================

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude
LDFLAGS = -lstdc++fs          # Necessário para <filesystem> em GCC < 12

SRCDIR = src
INCDIR = include
TESTDIR = tests

SRC = $(SRCDIR)/backup.cpp $(SRCDIR)/catch_amalgamated.cpp
TEST = $(TESTDIR)/testa_backup.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = testa_backup

.PHONY: all compile test cpplint cppcheck gcov debug valgrind docs clean

# ============================================
# Compilação e execução
# ============================================

all: $(TARGET)
	./$(TARGET)
	# use comentário se necessário

compile: $(OBJ) $(TEST)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(TEST) $(LDFLAGS)

$(SRCDIR)/%.o: $(SRCDIR)/%.cpp $(INCDIR)/%.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJ) $(TEST)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ) $(TEST) $(LDFLAGS)

test: $(TARGET)
	./$(TARGET)

# ============================================
# Verificadores de estilo e análise estática
# ============================================

cpplint:
	cpplint $(SRCDIR)/backup.cpp $(INCDIR)/backup.hpp $(TEST)

cppcheck:
	cppcheck --enable=warning --std=c++17 \
		--check-level=exhaustive \
		--suppress=missingIncludeSystem \
		--suppress=uninitMemberVar \
		--suppress=duplInheritedMember \
		--suppress=syntaxError:tests/testa_backup.cpp \
		--suppress=syntaxError:include/catch_amalgamated.hpp \
		--force \
		src/backup.cpp include/backup.hpp tests/testa_backup.cpp


# ============================================
# Teste de cobertura (GCOV)
# ============================================

gcov: clean
	$(CXX) $(CXXFLAGS) -fprofile-arcs -ftest-coverage -c $(SRCDIR)/backup.cpp -o backup.o
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/catch_amalgamated.cpp -o catch_amalgamated.o

	$(CXX) $(CXXFLAGS) -fprofile-arcs -ftest-coverage \
		backup.o catch_amalgamated.o $(TEST) -o $(TARGET) $(LDFLAGS)

	./$(TARGET)

	gcov -o . $(SRCDIR)/backup.cpp



# ============================================
# Depuração (GDB) e Análise Dinâmica (Valgrind)
# ============================================

debug:
	$(CXX) $(CXXFLAGS) -g -c $(SRCDIR)/backup.cpp -o backup.o
	$(CXX) $(CXXFLAGS) -g -c $(SRCDIR)/catch_amalgamated.cpp -o catch_amalgamated.o
	$(CXX) $(CXXFLAGS) -g backup.o catch_amalgamated.o $(TEST) -o $(TARGET) $(LDFLAGS)

	gdb $(TARGET)


valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --log-file=valgrind.rpt ./$(TARGET)

# ============================================
# Geração da Documentação (Doxygen)
# ============================================

docs: Doxyfile
	doxygen Doxyfile
	@echo "Documentação gerada em ./docs/html/index.html"

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

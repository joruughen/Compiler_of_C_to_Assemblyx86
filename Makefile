CXX = g++
CXXFLAGS = -std=c++14 -Wall -g
TARGET = compiler
SOURCES = exp.cpp parser.cpp scanner.cpp token.cpp visitor.cpp main.cpp \
          imp_type.cpp gencode.cpp imp_value.cpp type_visitor.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Directorios de tests
TEST_DIR = tests
TEST_SUBDIRS = variables expressions control functions unsigned_int long_int

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
	rm -f tests/*/*.s

# Crear estructura de directorios para tests
test-dirs:
	@mkdir -p $(addprefix $(TEST_DIR)/, $(TEST_SUBDIRS))

# Ejecutar todos los tests
test: $(TARGET) test-dirs
	@echo "Ejecutando tests de variables..."
	@for test in $(TEST_DIR)/variables/*.c; do \
		echo "Test: $$test"; \
		./$(TARGET) $$test 2>/dev/null | sed -n '/^\.data/,$$p' > $$test.s; \
	done
	@echo "Ejecutando tests de expresiones..."
	@for test in $(TEST_DIR)/expressions/*.c; do \
		echo "Test: $$test"; \
		./$(TARGET) $$test 2>/dev/null | sed -n '/^\.data/,$$p' > $$test.s; \
	done
	@echo "Ejecutando tests de control..."
	@for test in $(TEST_DIR)/control/*.c; do \
		echo "Test: $$test"; \
		./$(TARGET) $$test 2>/dev/null | sed -n '/^\.data/,$$p' > $$test.s; \
	done
	@echo "Ejecutando tests de funciones..."
	@for test in $(TEST_DIR)/functions/*.c; do \
		echo "Test: $$test"; \
		./$(TARGET) $$test 2>/dev/null | sed -n '/^\.data/,$$p' > $$test.s; \
	done
	@echo "Ejecutando tests de unsigned int..."
	@for test in $(TEST_DIR)/unsigned_int/*.c; do \
		echo "Test: $$test"; \
		./$(TARGET) $$test 2>/dev/null | sed -n '/^\.data/,$$p' > $$test.s; \
	done
	@echo "Ejecutando tests de long int..."
	@for test in $(TEST_DIR)/long_int/*.c; do \
		echo "Test: $$test"; \
		./$(TARGET) $$test 2>/dev/null | sed -n '/^\.data/,$$p' > $$test.s; \
	done

.PHONY: all clean test test-dirs
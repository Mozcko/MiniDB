# Compilador
CXX = g++

# Banderas de compilación
CXXFLAGS = -std=c++17 -Wall -Iinclude

# Directorios
SRCDIR = src
BUILDDIR = build
BINDIR = bin

# Archivos fuente y objeto
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SOURCES))
EXECUTABLE = $(BINDIR)/MiniDB

# Regla principal
all: $(EXECUTABLE)

# Regla para crear el ejecutable
$(EXECUTABLE): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Regla para compilar archivos objeto
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Regla para limpiar
clean:
	rm -rf $(BUILDDIR)/* $(BINDIR)/*

.PHONY: all clean

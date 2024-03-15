# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Ofast

# Directories
SRCDIR = ./src
HEADERDIR = ./header
BOOSTDIR = /path/to/boost # Change this to your boost installation directory

# Boost libraries
BOOST_LIBS = -lboost_system -lboost_filesystem

# Source and header files
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
HEADERS = $(wildcard $(HEADERDIR)/*.hpp)

# Object files
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=%.o)

# Executable name
EXECUTABLE = noise_sim

# Target for building the executable
$(EXECUTABLE): $(OBJECTS)
    $(CXX) $(CXXFLAGS) -I$(HEADERDIR) -L$(BOOSTDIR)/lib $(OBJECTS) -o $@ $(BOOST_LIBS)

# Target for building object files
%.o: $(SRCDIR)/%.cpp $(HEADERS)
    $(CXX) $(CXXFLAGS) -I$(HEADERDIR) -c $< -o $@

# Target for cleaning generated files
clean:
    rm -f $(OBJECTS) $(EXECUTABLE)

# Compiler
CC := g++

# Directories
IDIR := include
SRCDIR := src
BUILDDIR := build

# Target
TARGET := mixnmatch

# Source Extention
SRCEXT := cpp

# Sources
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))

# Objects
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

# Flags
CPPFLAGS := -std=c++11 -Wall -w -I $(IDIR) 

# Libraries
LIB := -lGL -lGLU -lglut -lglui -lCGAL -lboost_system -lboost_filesystem -lGLEW -lOSMesa `pkg-config --cflags --libs opencv` -lgmp -lmpfr

# Build Executable
$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@ $(LIB)

# Build Object Files
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CPPFLAGS) -c -o $@ $< 

# Clean
clean:
	$(RM) -r $(BUILDDIR) $(TARGET)

.PHONY: clean

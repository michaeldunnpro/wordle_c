# ==============================================================================
# Summary
# ==============================================================================
# A simple C Makefile that builds from a source folder and outputs to a bin 
# folder. Assumes the GNU Compiler (gcc) is installed, and terminal commands
# are UNIX-like.
# ==============================================================================
# Variables & Configuration
# ==============================================================================
CC = gcc
CFLAGS = -Wall -Werror -g -fsanitize=address
TARGET = wordle_c
SRCDIR = src
BINDIR = bin
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(BINDIR)/%.o)
# ==============================================================================
# Build Rules
# ==============================================================================
all: $(BINDIR)/$(TARGET)
$(BINDIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^
$(BINDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -c -o $@ $<
# ==============================================================================
# Clean Rule
# ==============================================================================
.PHONY: clean # Mark 'clean' as a phony target to avoid conflicts with files
clean:
	rm -rf $(BINDIR)/*
# ==============================================================================
# Run Rule
# ==============================================================================
.PHONY: run # Mark 'run' as a phony target to avoid conflicts with files
run: all
	./$(BINDIR)/$(TARGET)
# ==============================================================================
# Help Rule
# ==============================================================================
.PHONY: help # Mark 'help' as a phony target to avoid conflicts with files
help:
	@echo "Usage: make [target]"
	@echo "Targets:"
	@echo "  all     - Build the project (default)"
	@echo "  clean   - Remove all build artifacts"
	@echo "  run     - Build and run the project"
	@echo "  help    - Show this help message"
# ==============================================================================
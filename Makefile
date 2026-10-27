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
CFLAGS = -Wall -Werror -g
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
	@mkdir -p "$(BINDIR)"
	$(CC) $(CFLAGS) -o "$@" $^
$(BINDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p "$(BINDIR)"
	$(CC) $(CFLAGS) -c -o "$@" "$<"
# ==============================================================================
# Launcher Install Rule (system symlink so the game starts from anywhere)
# ==============================================================================
# Override for a user-local directory (pass the same INSTALL_DIR to clean):
#   make install INSTALL_DIR=$HOME/.local/bin && make clean INSTALL_DIR=$HOME/.local/bin
INSTALL_DIR ?= /usr/local/bin
LAUNCH_LINK = $(INSTALL_DIR)/$(TARGET)
.PHONY: install # Mark 'install' as a phony target to avoid conflicts with files
install: all
	mkdir -p "$(INSTALL_DIR)"
	ln -sfn "$(abspath $(BINDIR)/$(TARGET))" "$(LAUNCH_LINK)"
	@printf '[install] %s -> %s\n[install] Install successful. Type "%s" to play.\n' \
		"$(LAUNCH_LINK)" "$(abspath $(BINDIR)/$(TARGET))" "$(TARGET)"
# ==============================================================================
# Clean Rule
# ==============================================================================
.PHONY: clean # Mark 'clean' as a phony target to avoid conflicts with files
clean:
	rm -rf "$(BINDIR)"/*
	@if [ -L "$(LAUNCH_LINK)" ]; then rm "$(LAUNCH_LINK)" && echo "[clean] Removed launcher link $(LAUNCH_LINK)" || echo "[clean] $(LAUNCH_LINK) is protected; remove it with sudo"; fi
# ==============================================================================
# Run Rule
# ==============================================================================
.PHONY: run # Mark 'run' as a phony target to avoid conflicts with files
run: all
	"./$(BINDIR)/$(TARGET)"
# ==============================================================================
# Memory Safety Rules (Address + Undefined Behaviour Sanitizers)
# ==============================================================================
SANFLAGS = -fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer
SANBIN = $(BINDIR)/$(TARGET)-san
.PHONY: sanitize check
sanitize:
	@mkdir -p "$(BINDIR)"
	$(CC) $(CFLAGS) $(SANFLAGS) -o "$(SANBIN)" $(SOURCES)
check: sanitize
	@echo "[check] Valid guesses, then replay, then quit..."
	@printf 'crane\nr\nq\n' | "./$(SANBIN)" > /dev/null
	@echo "[check] Deterministic win with forced target 'slate'..."
	@printf 'slate\nr\nq\n' | "./$(SANBIN)" slate > /dev/null
	@echo "[check] Invalid inputs (symbols, wrong length, off-list), loss game, then quit..."
	@printf 'slate\nbarn!\ncrane\nnotaword\npride\nabout\nshiny\nerase\nr\n' | "./$(SANBIN)" > /dev/null
	@echo "[check] Over-long line, bytes, then end-of-file mid-game and at replay prompt..."
	@printf 'abcdefghij\nhello1!\nqwertyuiopasdfghjklzxcvbnmqwertyuiopasdfghjklzxcvbnmqwertyuiopasdfghjklzxcvbnmqwertyuiopasdfghjkl\nqwe\n' | "./$(SANBIN)" > /dev/null
	@echo "[check] All sanitized runs finished with no sanitizer reports."
	@$(MAKE) --no-print-directory valgrind
# ==============================================================================
# Memory Check Rule (Valgrind)
# ==============================================================================
VGFLAGS = --leak-check=full --show-leak-kinds=definite,possible \
          --errors-for-leak-kinds=definite,possible --error-exitcode=1 --quiet
.PHONY: valgrind # Mark 'valgrind' as a phony target to avoid file conflicts
valgrind: all
	@echo "[valgrind] Valid guesses, then replay, then quit..."
	@printf 'crane\nr\nq\n' | valgrind $(VGFLAGS) "./$(BINDIR)/$(TARGET)" > /dev/null
	@echo "[valgrind] Deterministic win with forced target 'slate'..."
	@printf 'slate\nr\nq\n' | valgrind $(VGFLAGS) "./$(BINDIR)/$(TARGET)" slate > /dev/null
	@echo "[valgrind] Invalid inputs (symbols, wrong length, off-list), loss game, then replay..."
	@printf 'slate\nbarn!\ncrane\nnotaword\npride\nabout\nshiny\nerase\nr\nq\n' | valgrind $(VGFLAGS) "./$(BINDIR)/$(TARGET)" > /dev/null
	@echo "[valgrind] Over-long line, bytes, then end-of-file mid-game and at replay prompt..."
	@printf 'abcdefghij\nhello1!\nqwertyuiopasdfghjklzxcvbnmqwertyuiopasdfghjklzxcvbnmqwertyuiopasdfghjklzxcvbnmqwertyuiopasdfghjkl\nqwe\n' | valgrind $(VGFLAGS) "./$(BINDIR)/$(TARGET)" > /dev/null
	@echo "[valgrind] All runs finished with no leaks or memory errors."
# =============================================================================
# Formatting and Style Rule (Requires clang-format)
# =============================================================================
.PHONY: format # Mark 'format' as a phony target to avoid conflicts with files
format:
	@echo "Formatting source files..."
	@clang-format -i "$(SRCDIR)"/*.c "$(SRCDIR)"/*.h
# ==============================================================================
# Help Rule
# ==============================================================================
.PHONY: help # Mark 'help' as a phony target to avoid conflicts with files
help:
	@echo "Usage: make [target]"
	@echo "Targets:"
	@echo "  all     - Build the project (default)"
	@echo "  clean   - Remove all build artifacts and the launcher link"
	@echo "  run     - Build and run the project"
	@echo "  install - Symlink $(TARGET) into $(INSTALL_DIR) to play from anywhere (try: sudo make install)"
	@echo "  sanitize - Build with ASan+UBSan sanitizers into bin/$(TARGET)-san"
	@echo "  valgrind - Run scripted input scenarios under valgrind"
	@echo "  check   - Run the scripted scenarios under sanitizers and valgrind"
	@echo "  format  - Format source files using clang-format (if installed)"
	@echo "  help    - Show this help message"
# ==============================================================================

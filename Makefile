# Compiler specific
CC := clang
CFLAGS := -O3 -Wall

# Source and out files specific
SRC := main.c drop.c
OBJ := $(SRC:.c=.o)
TARGET := murrls

OBJDIR := build
BINDIR := build/bin

# OS specific
UNAME_S := $(shell uname -s)

LINUX_LIBS := -lraylib -lm -ldl -lpthread -lGL -lrt -lX11
MAC_LIBS := -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -lraylib -lm
WINDOWS_LIBS := -lwinmm -lgdi32 -lcomdlg32 -lole32 -luuid -loleaut32 -lshell32 -lraylib -lopengl32 -lgdi32

# Commands
.PHONY: all clean run

all: $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): $(OBJDIR)/$(OBJ)
	@mkdir -p $(BINDIR) 
ifeq ($(UNAME_S),Linux)
	$(CC) $(CFLAGS) -o $@ $^ $(LINUX_LIBS)
endif
ifeq ($(UNAME_S),Darwin)
	$(CC) $(CFLAGS) -o $@ $^ $(MAC_LIBS)
endif
ifeq ($(OS),Windows_NT)
	$(CC) $(CFLAGS) -o $@ $^ $(WINDOWS_LIBS)
endif

$(OBJDIR)/%.o: %.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

run: $(BINDIR)/$(TARGET)
	$^

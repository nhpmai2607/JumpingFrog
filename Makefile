
BIN := s3558475

CC := gcc
LD := gcc

SRC_EXT := c

SRC_DIR := src
OBJ_DIR := obj


CFLAGS := -Wall -Wextra -std=c11 -g -I$(SRC_DIR) -I inc
LDFLAGS = 

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	# Linux
	LDFLAGS += -lm -lGL -lGLU -lglut -D_LINUX ./lib/libSOIL.a
	CFLAGS += 
endif
ifeq ($(UNAME_S),Darwin)
	# OSX
	LDFLAGS += -F/Library/Frameworks -framework Carbon -framework OpenGL -framework GLUT -lSOIL
	CFLAGS += -D__APPLE__ -I/usr/local/include -Wno-deprecated-declarations -I/opt/local/include
endif

SOURCES := $(shell find $(SRC_DIR)/ -name '*.$(SRC_EXT)')
OBJECTS := $(SOURCES:$(SRC_DIR)/%.$(SRC_EXT)=%.o)
OBJECTS := $(OBJECTS:%.o=$(OBJ_DIR)/%.o)
DEPS := $(OBJECTS:.o=.d)

.PHONY: all
all: $(BIN)

# link objects together to make executable
$(BIN) : $(OBJECTS)
	$(LD) -o $(BIN) $(OBJECTS) $(LDFLAGS)

# make objects and dependencies
$(OBJECTS): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.$(SRC_EXT)
#	if the obj_dir exists -p suppresses the error
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -MP -MMD $< -o $@

# handle dependencies
-include $(DEPS)

# remove the compiled objects and the binary to clean up
.PHONY: clean
clean:
	rm -f $(OBJECTS) $(BIN) $(DEPS)

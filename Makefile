CC=gcc
CFLAGS=-Wall -Wextra -O3
DFLAGS=-Wall -Wextra -O0 -g -fsanitize=address

LD=gcc

BINARY=mmvm

ROOT_TARGET=./bin
TARGET_DIR=$(ROOT_TARGET)/Release
TARGET=$(TARGET_DIR)/$(BINARY)

DEBUG_TARGET_DIR=$(ROOT_TARGET)/Debug
DEBUG_TARGET=$(DEBUG_TARGET_DIR)/$(BINARY).debug

SRC_DIR=./src
SRC=utils/binary_stream.c main.c instructions/arithmetic.c instructions/logic.c \
	instructions/data_transfer.c instructions/utils.c utils/format.c \
	instructions/control_transfer.c instructions/processor_control.c
HEADERS=utils/binary_stream.h instructions/arithmetic.h instructions/logic.h \
		instructions/data_transfer.h instructions/utils.h utils/format.h \
		instructions/instructions.h instructions/control_transfer.h \
		instructions/processor_control.h

DEPS=$(addprefix $(SRC_DIR)/, $(HEADERS))

OBJ_ROOT=./obj
OBJ_DIR=$(OBJ_ROOT)/Release
OBJS=$(addprefix $(OBJ_DIR)/, $(SRC:.c=.o))

DEBUG_OBJ_DIR=$(OBJ_ROOT)/Debug
DEBUG_OBJS=$(addprefix $(DEBUG_OBJ_DIR)/, $(SRC:.c=.o))

.PHONY: all debug build link dbuild clean

all: prod

prod: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(TARGET_DIR)
	$(LD) -o $@ $(LDFLAGS) $^

debug: $(DEBUG_TARGET)

$(DEBUG_TARGET): $(DEBUG_OBJS)
	@mkdir -p $(DEBUG_TARGET_DIR)
	$(LD) $^ $(LDLIBS) -o $@ $(LDFLAGS)  -fsanitize=address

dbuild: $(DEBUG_OBJS)

build: $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	@mkdir -p $(shell echo $@ | grep -Eo "(\w+/)+")
	@# vim color scheme debug "
	$(CC) $(CFLAGS) -c $< -o $@

$(DEBUG_OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	@mkdir -p $(shell echo $@ | grep -Eo "(\w+/)+")
	@# vim color scheme debug "
	$(CC) $(DFLAGS) -c $< -o $@

clean:
	rm -rf ./bin
	rm -rf ./obj

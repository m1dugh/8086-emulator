CC=gcc
COMMON_FLAGS=-Wall -Wextra -Wno-format-security
CFLAGS=$(COMMON_FLAGS) -O3
DFLAGS=$(COMMON_FLAGS) -O0 -g -fsanitize=address

LD=gcc

BINARY=mmvm

ROOT_TARGET=./bin
TARGET_DIR=$(ROOT_TARGET)/Release
TARGET=$(TARGET_DIR)/$(BINARY)

DEBUG_TARGET_DIR=$(ROOT_TARGET)/Debug
DEBUG_TARGET=$(DEBUG_TARGET_DIR)/$(BINARY).debug

SRC_DIR=./src

INSTRUCTION_IMPLEMENTATION=xor mov utils lea add cmp jump test push call \
						   processor_control dec shift pop sub interrupts \
						   or inc convert and neg
_INSTRUCTION_IMPL_FULL_PATH=$(addprefix instructions/implementation/, $(INSTRUCTION_IMPLEMENTATION))

INSTRUCTION_IMPL_SECTIONS=data_transfer logic arithmetic control_transfer
_INSTRUCTION_IMPL_SECTIONS_FULL_PATH=$(addprefix instructions/implementation/, $(INSTRUCTION_IMPL_SECTIONS))

SRC=utils/binary_stream.c main.c instructions/arithmetic.c instructions/logic.c \
	utils/trie.c utils/vector.c utils/byte_vector.c \
	instructions/data_transfer.c instructions/utils.c utils/format.c \
	instructions/control_transfer.c instructions/processor_control.c \
	instructions/string_manipulation.c \
	models/emulator.c models/emulator_instructions.c models/instruction.c \
	models/memory_segment.c models/processor.c minix/syscalls.c
SRC := $(SRC) $(addsuffix .c, $(_INSTRUCTION_IMPL_FULL_PATH))

HEADERS=utils/binary_stream.h instructions/arithmetic.h instructions/logic.h \
		utils/trie.h utils/vector.h env.h \
		instructions/data_transfer.h instructions/utils.h utils/format.h \
		instructions/instructions.h instructions/control_transfer.h \
		instructions/processor_control.h instructions/string_manipulation.h \
		models/emulator.h models/instruction.h models/memory_segment.h models/processor.h \
		minix/syscalls.h minix/env.h

HEADERS := $(HEADERS) $(addsuffix .h, $(_INSTRUCTION_IMPL_FULL_PATH)) \
		   $(addsuffix .h, $(_INSTRUCTION_IMPL_SECTIONS_FULL_PATH))

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

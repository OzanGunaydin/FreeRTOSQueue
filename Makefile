CC                    := gcc
BIN                   := queue_demo

BUILD_DIR             := ./build
BUILD_DIR_ABS         := $(abspath $(BUILD_DIR))

#This (FREERTOS_DIR_REL) is the only variable needs adjustment
#Update this path to your FreeRTOS root directory
FREERTOS_DIR_REL      := ../../FreeRTOS/FreeRTOS/FreeRTOS
FREERTOS_DIR          := $(abspath $(FREERTOS_DIR_REL))

KERNEL_DIR            := ${FREERTOS_DIR}/Source

INCLUDE_DIRS          := -I./src
INCLUDE_DIRS          += -I${KERNEL_DIR}/include
INCLUDE_DIRS          += -I${KERNEL_DIR}/portable/ThirdParty/GCC/Posix
INCLUDE_DIRS          += -I${KERNEL_DIR}/portable/ThirdParty/GCC/Posix/utils

SOURCE_FILES          := $(wildcard ./src/*.c)
SOURCE_FILES          += $(wildcard ${FREERTOS_DIR}/Source/*.c)
# Memory manager (use malloc() / free() )
SOURCE_FILES          += ${KERNEL_DIR}/portable/MemMang/heap_3.c
# posix port
SOURCE_FILES          += ${KERNEL_DIR}/portable/ThirdParty/GCC/Posix/utils/wait_for_event.c
SOURCE_FILES          += ${KERNEL_DIR}/portable/ThirdParty/GCC/Posix/port.c


CFLAGS                :=    -ggdb3
LDFLAGS               :=    -ggdb3 -pthread
CPPFLAGS              :=    $(INCLUDE_DIRS) -DBUILD_DIR=\"$(BUILD_DIR_ABS)\"
CPPFLAGS              +=    -D_WINDOWS_


OBJ_FILES = $(SOURCE_FILES:%.c=$(BUILD_DIR)/%.o)

DEP_FILE = $(OBJ_FILES:%.o=%.d)

${BIN} : $(BUILD_DIR)/$(BIN)

${BUILD_DIR}/${BIN} : ${OBJ_FILES}
	-mkdir -p ${@D}
	$(CC) $^ ${LDFLAGS} -o $@

-include ${DEP_FILE}

${BUILD_DIR}/%.o : %.c Makefile
	-mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -c $< -o $@

.PHONY: clean

clean:
	-rm -rf $(BUILD_DIR)
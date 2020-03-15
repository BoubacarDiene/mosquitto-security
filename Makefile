##
#
# \file Makefile
#
# \author Boubacar DIENE <boubacar.diene@gmail.com>
# \date   2020
#
# \brief  Makefile to build provided sample codes
#
##

LOCAL := $(shell pwd)

#################################################################
#                            Variables                          #
#################################################################

# Paths
EXAMPLES_SRC_DIR := $(LOCAL)/src
EXAMPLES_OUT_DIR := $(LOCAL)/out
EXAMPLES_LIST    := $(patsubst $(EXAMPLES_SRC_DIR)/%/.,%,$(wildcard $(EXAMPLES_SRC_DIR)/*/.))

# Build options
#   - BUILD_TYPE = "',' separated between 'gdb', 'asan' and 'secu'" (Not set => release build)
#
#   Examples:
#   - make all
#   - make all BUILD_TYPE=gdb
#   - make all BUILD_TYPE=gdb,asan,secu
BUILD_TYPE ?= release

# https://linux.die.net/man/1/gcc
# https://security.stackexchange.com/questions/24444/what-is-the-most-hardened-set-of-options-for-gcc-compiling-c-c
LDFLAGS_OPTIONS := -pthread -lmosquitto
CFLAGS_OPTIONS  := -std=c99 -D_GNU_SOURCE \
                   -Wall -Wextra -Werror -Wconversion -Wsign-conversion \
                   -Wuninitialized -Wparentheses -Winit-self -Wcomment \
                   -Wstrict-prototypes -Wmissing-prototypes -Wshadow \
                   -Wno-unused-result

CFLAGS_OPTIONS += $(if $(findstring release,$(BUILD_TYPE)),-O3 -s -DNDEBUG,)
CFLAGS_OPTIONS += $(if $(findstring gdb,$(BUILD_TYPE)),-ggdb,)
CFLAGS_OPTIONS += $(if $(findstring asan,$(BUILD_TYPE)),-fsanitize=address -fno-omit-frame-pointer,)

ifneq (,$(findstring secu,$(BUILD_TYPE)))
LDFLAGS_OPTIONS += -Wl,-z,now -Wl,-z,relro -Wl,-z,noexecstack
CFLAGS_OPTIONS  += -D_FORTIFY_SOURCE=2 -Wformat -Wformat-security \
                   -fstack-protector-all -Wstack-protector --param ssp-buffer-size=4 \
                   -pie -fPIE -ftrapv
endif

CC      := gcc
LDFLAGS := $(LDFLAGS_OPTIONS)
CFLAGS  := $(CFLAGS_OPTIONS) \
           -I$(EXAMPLES_SRC_DIR)

# Shell commands
CP    := cp -rf
RM    := rm -rf
MKDIR := mkdir -p

#################################################################
#                             Build                             #
#################################################################

.PHONY: all clean
.DEFAULT_GOAL := all
.NOTPARALLEL:

#
# Function to build and install the provided examples
# $1: A list of directories containing sample codes and mosquitto.conf file
#
define make-examples
    @- $(foreach example,$1, \
        @$(MKDIR) $(EXAMPLES_OUT_DIR)/$(example) ; \
        $(CC) $(CFLAGS) $(EXAMPLES_SRC_DIR)/$(example)/subscriber.c \
                        -o $(EXAMPLES_OUT_DIR)/$(example)/subscriber \
              $(LDFLAGS) ; \
        $(CC) $(CFLAGS) $(EXAMPLES_SRC_DIR)/$(example)/publisher.c \
                        -o $(EXAMPLES_OUT_DIR)/$(example)/publisher \
              $(LDFLAGS) ; \
        $(CP) $(EXAMPLES_SRC_DIR)/$(example)/mosquitto.conf \
              $(EXAMPLES_OUT_DIR)/$(example)/
    )
endef

# Build all examples
all:
	$(call make-examples,$(EXAMPLES_LIST))
	@./scripts/generate-certificates.sh $(EXAMPLES_OUT_DIR)

# Clean output directory
clean:
	$(RM) $(EXAMPLES_OUT_DIR) ||:

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
EXAMPLES_ROOT_DIRECTORY := $(LOCAL)/examples
EXAMPLES_OUT_DIRECTORY  := $(LOCAL)/out

# Build options
#   - BUILD_TYPE = "',' separated between 'gdb', 'asan' and 'secu'" (Not set => release build)
#
#   Examples:
#   - make all install
#   - make all install BUILD_TYPE=gdb
#   - make all install BUILD_TYPE=gdb,asan,secu
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
           -I$(EXAMPLES_ROOT_DIRECTORY)

# Shell commands
CP    := cp -rf
RM    := rm -rf
MKDIR := mkdir -p

#################################################################
#                             Build                             #
#################################################################

.PHONY: all
.DEFAULT_GOAL := all
.NOTPARALLEL:

#
# Function to build and install the provided examples
# $1: A list of directories containing sample codes and mosquitto.conf file
#
define make-examples
    @- $(foreach example,$1, \
        $(MKDIR) $(EXAMPLES_OUT_DIRECTORY)/$(example) ; \
        $(CC) $(CFLAGS) $(EXAMPLES_ROOT_DIRECTORY)/$(example)/subscriber.c \
                        -o $(EXAMPLES_OUT_DIRECTORY)/$(example)/subscriber \
              $(LDFLAGS) ; \
        $(CC) $(CFLAGS) $(EXAMPLES_ROOT_DIRECTORY)/$(example)/publisher.c \
                        -o $(EXAMPLES_OUT_DIRECTORY)/$(example)/publisher \
              $(LDFLAGS) ; \
        $(CP) $(EXAMPLES_ROOT_DIRECTORY)/$(example)/mosquitto.conf \
              $(EXAMPLES_OUT_DIRECTORY)/$(example)/
    )
endef

# Build all examples
# make or make all
all: EXAMPLES := 1-client-id-prefix
all:
	$(call make-examples,$(EXAMPLES))

# Build a single example
# E.g.: make 1-client-id-prefix
%:
	$(call make-examples,$@)

# Clean examples' output directory
clean:
	$(RM) $(EXAMPLES_OUT_DIRECTORY) ||:

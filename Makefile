CC := g++
CCFLAGS := -Wall -O2 -g
STD := -std=c++0x
CCSHAREDFLAGS := -fPIC -c -shared

TARGET_NAME := Quadtree
TARGET := $(addprefix lib,$(basename $(TARGET_NAME)))
TARGET := $(addsuffix .so,$(basename $(TARGET)))

SRC_DIRS ?= ./src
BUILD_DIR ?= ./build

SRCS := $(shell find $(SRC_DIRS) -name \*.cpp -type f)

$(TARGET): $(BUILD_DIR)
	$(CC) $(STD) $(CCFLAGS) $(CCSHAREDFLAGS) $(SRCS) -o $(BUILD_DIR)/$@
	cp $(SRC_DIRS)/$(TARGET_NAME).hpp $(BUILD_DIR)/$(TARGET_NAME).hpp

$(BUILD_DIR):
	mkdir -p $@

.PHONY: clean

clean:
	rm -rf build
## Compilation options
CXX=g++
CXXFLAGS=-Wall -Wextra -std=gnu++11
LDFLAGS=
RELEASE_FLAGS=-O3
DEBUG_FLAGS=-DDEBUG -g

## Directories setup
SOURCE_DIR=src
INCLUDE_DIR=include
BUILD_DIR=build
BIN_DIR=bin

## Products setup
PRODUCT=test

## Colors definitions
red=`tput setaf 1`
green=`tput setaf 2`
yellow=`tput setaf 3`
blue=`tput setaf 4`
rcol=`tput sgr0`

## Build-specific commands
CXXFLAGS+=-I$(INCLUDE_DIR) -DPRODUCT_NAME=\"$(PRODUCT)\"
release?=0
ifeq ($(release),1)
	CXXFLAGS+=$(RELEASE_FLAGS)
else
	CXXFLAGS+=$(DEBUG_FLAGS)
endif
BINARY=$(BIN_DIR)/$(PRODUCT)


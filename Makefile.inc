# This file is part of libconf.
#
# libconf is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# libconf is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with libconf.  If not, see <http://www.gnu.org/licenses/>.

## Compilation options
CXX=g++
CXXFLAGS=-fPIC -Wall -Wextra -std=gnu++11
LDFLAGS=
RELEASE_FLAGS=-O3
DEBUG_FLAGS=-DDEBUG -g

## Directories setup
SOURCE_DIR=src
INCLUDE_DIR=include
BUILD_DIR=build
BIN_DIR=bin
TEST_DIR=test

## Products setup
PRODUCT=conf

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
BINARY=$(BIN_DIR)/lib$(PRODUCT).a
BINARY_LINK_FLAGS=-L$(BIN_DIR) -l$(PRODUCT)

SHELL     := /bin/sh
CXX       ?= g++
AR?       := ar
INCLUDES  += -I. -I../..
TARGET    ?= Release
SOURCEDIR := ../src/
OBJDIR    := ../build/
LANGDIR   := ../lang/
BINDIR    ?= ../../bin/
GENDIR    := ../gen/
SEDDIR    ?= ..\/build\/
DEPENDENCYDIR ?= ../../../Externals/bin/Linux/$(shell uname -m)/$(TARGET)/


ifeq ($(CXX), g++)
	GCC_MAJOR := $(shell $(CXX) -v 2>&1 | grep " version " | cut -d' ' -f3  | cut -d'.' -f1)
	GCC_MINOR := $(shell $(CXX) -v 2>&1 | grep " version " | cut -d' ' -f3  | cut -d'.' -f2)
endif


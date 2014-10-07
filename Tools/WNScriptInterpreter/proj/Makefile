include ../../PreToolsHelper.mk

LLVM_CONFIG    ?= $(DEPENDENCYDIR)bin/llvm-config
LLVM_EXISTS   = $(shell which $(LLVM_CONFIG) > /dev/null && echo asdf)

ifneq ("$(LLVM_EXISTS)", "asdf")
    $(error Invalid llvm config $(LLVM_EXISTS))
endif



TOOL      := WNScriptInterpreter
SOURCES   := Main.cpp
WNLIBS    := WNEntryPoint WNScripting WNFileSystem WNUtils
LINKFLAGS := $(shell $(LLVM_CONFIG) --ldflags)
CXXFLAGS += -fno-rtti

TOOL_ADDITIONAL_LIBS := $(shell $(LLVM_CONFIG) --libs)

include ../../ToolsHelper.mk


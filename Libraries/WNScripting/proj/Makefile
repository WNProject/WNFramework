include ../../PreLibrariesHelper.mk

LLVM_CONFIG  ?= $(DEPENDENCYDIR)bin/llvm-config
LLVM_EXISTS   = $(shell which $(LLVM_CONFIG) > /dev/null && echo exists)

ifneq ("$(LLVM_EXISTS)", "exists")
    $(error Invalid llvm config $(LLVM_CONFIG))
endif


LIB            := WNScripting
DEPLIBS        := WNFileSystem
SOURCES        := WNArgList.cpp WNArrayAccessExpression.cpp WNArrayAllocation.cpp WNAssignment.cpp WNBinExpression.cpp \
				  WNCodeModule.cpp WNCondExpression.cpp WNConstantExpression.cpp WNDeclaration.cpp WNDeclList.cpp \
				  WNDoInstruction.cpp WNExpression.cpp WNForInstruction.cpp WNFunctionCallExpression.cpp WNFunction.cpp \
				  WNIDExpression.cpp WNIfInstruction.cpp WNInstruction.cpp WNInstructionList.cpp WNLValue.cpp \
				  WNMemberAccessExpression.cpp WNNullAllocation.cpp WNPostExpression.cpp WNPostUNExpression.cpp WNReturn.cpp \
				  WNScopedVariableListImpl.cpp WNScriptASTLexer.cpp WNScriptASTParser.cpp WNScriptCastingInitialization.cpp \
				  WNScriptFile.cpp WNScriptFloatTypeInitialiization.cpp WNScriptFunctionInitialization.cpp\
				  WNScriptingEngineFactory.cpp WNScriptingEngineImpl.cpp WNScriptingFactoryInternal.cpp WNScriptIntTypeInitialization.cpp \
				  WNSSExpression.cpp WNStringType.cpp WNStructAllocation.cpp WNStruct.cpp WNTypeManagerImp.cpp \
				  WNTypeNode.cpp WNUnExpression.cpp WNWhileInstruction.cpp
CXXFLAGS       += $(shell $(LLVM_CONFIG) --cppflags)
ADDITIONALLIBS += $(addsuffix .a, $(addprefix ../../$(BINDIR)$(TARGET)/lib,$(DEPLIBS))) $(shell $(LLVM_CONFIG) --libs)

include ../../LibrariesHelper.mk

SHELL := /bin/sh
SOURCES := WNEntryPoint WNFileSystem WNLogging WNGraphics WNUtils WNScripting WNNetworking WNPlatform WNMath WNCore WNConcurrency WNMemory WNStrings WNContainers

.PHONY: tags all $(SOURCES)

all: tags $(SOURCES)

WNScripting: WNFileSystem WNEntryPoint
WNGraphics: WNPlatform WNEntryPoint
WNMath: WNEntryPoint
WNLogging: WNEntryPoint
WNFileSystem: WNEntryPoint
WNGraphics: WNEntryPoint
WNNetworking: WNEntryPoint
WNPlatform: WNEntryPoint
WNCore: WNEntryPoint
WNConcurrency: WNEntryPoint
WNMemory: WNEntryPoint
WNStrings: WNEntryPoint
WNContainers: WNEntryPoint
WNEntryPoint: WNUtils

$(SOURCES):
	@if [ -f $@/proj/Makefile ]; then \
		$(MAKE) -C $@/proj; \
		if ! [ "$$?" -eq 0 ]; then \
			exit 1; \
		fi \
	fi
	@if [ -f $@/test/proj//Makefile ]; then \
		$(MAKE) -C $@/test/proj; \
		if ! [ "$$?" -eq 0 ]; then \
			exit 1; \
		fi \
	fi

clean:
	rm -rf cscope.*
	rm -rf tags 
	rm -rf ncscope.*
	rm -rf bin/
	@for n in $(SOURCES); do \
		if [ -f $$n/proj/Makefile ]; then \
			$(MAKE) -C $$n/proj clean; \
		fi ;\
		if [ -f $$n/test/proj/Makefile ]; then \
			$(MAKE) -C $$n/test/proj clean; \
		fi ;\
	done

tags:
	find -name "*.cpp" -o -name "*.h" > cscope.files
	ctags --recurse --langmap=C++:.C.h.c.cpp.hpp --languages=C++ --extra=+q -L cscope.files
	cscope -ub -i cscope.files

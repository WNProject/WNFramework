SHELL := /bin/sh

SOURCES :=  WNScriptInterpreter

.PHONY: tags externalLibs all $(SOURCES)
all: tags externalLibs $(SOURCES)

externalLibs:
	@${MAKE} -C ../Libraries/	

$(SOURCES): externalLibs
	@$(MAKE) -C $@/proj;

clean:
	@for n in $(SOURCES); do\
		${MAKE} -C $$n/proj clean; \
	done
	rm -rf bin/
	rm -rf cscope.*
	rm -rf tags 
	rm -rf ncscope.*

tags:
	find -name "*.cpp" -o -name "*.h" > cscope.files
	ctags --recurse --langmap=C++:.C.h.c.cpp.hpp --languages=C++ --extra=+q -L cscope.files
	cscope -ub -i cscope.files

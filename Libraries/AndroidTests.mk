
SOURCES := WNEntryPoint WNFileSystem WNLogging WNUtils WNGraphics WNScripting WNNetworking WNPlatform WNMath WNCore WNConcurrency WNMemory WNStrings WNContainers

.PHONY: all $(SOURCES)

all: $(SOURCES) 
	for source in $(SOURCES) ; do\
		if [ -f $$source/test/proj/jni/Makefile ]; then \
			$(MAKE) -C $$source/test/proj/jni/ install;\
			./$$source/test/proj/jni/runner.sh;\
		fi; \
	done


	
$(SOURCES):
	if [ -f $@/test/proj/jni/Makefile ]; then \
		$(MAKE) -C $@/test/proj/jni/; \
		if ! [ "$$?" -eq 0 ]; then \
			exit 1; \
		fi \
	fi

clean:
	for source in $(SOURCES) ; do\
		if [ -f $$source/test/proj/jni/Makefile ]; then \
			$(MAKE) -C $$source/test/proj/jni/ clean;\
		fi; \
	done



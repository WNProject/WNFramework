ARFLAGS += rcs
CXXFLAGS += -I$(DEPENDENCYDIR)include $(INCLUDES) -fno-exceptions -std=c++0x -fno-rtti -Werror
LDFLAGS += -L$(DEPENDENCYDIR)lib

UNAME_P := $(shell uname -p)
	
ifdef ADDITIONALCXXFLAGS
	CXXFLAGS += $(ADDITIONALCXXFLAGS)
endif

ifdef ADDITIONALARFLAGS
	ARFLAGS += $(ADDITIONALARFLAGS)
endif

ifeq ($(TARGET), Release)
	CXXFLAGS += -O3
endif

CXXFLAGS += -ggdb

ifeq ($(CXX), clang++)
	ifneq ($(filter arm%, $(UNAME_P)),)
		ifneq ($(filter armv7%, $(UNAME_P)),)
			CXXFLAGS += -mfloat-abi=hard
		endif
		
		ifneq ($(filter armv6%, $(UNAME_P)),)
			CXXFLAGS += -mfloat-abi=hard
		endif	
	endif
endif

OBJECTS=$(addsuffix .o, $(addprefix $(OBJDIR)$(TARGET)/, $(subst ..,__,$(basename  $(SOURCES)))))

define makecppgoal 
$1.d:$2
	@echo Make Depend $2 \>\> $1.d
	@test -d $(dir $1.d) || mkdir -p $(dir $1.d)
	@$(CXX) -MM $(CXXFLAGS) -c $2 -o $1.d.t
	@sed 's/$(notdir $1.o)/$(subst /,\/,$1.o $1.d)/' < $1.d.t > $1.d
	@rm -rf $1.d.t

$1.o:$1.d
	$(CXX) $(CXXFLAGS) -c $2 -o $1.o
endef

.PHONY: all

all: $(BINDIR)$(TARGET)/lib$(LIB).a

ifneq "$(MAKECMDGOALS)" "clean"
-include $(SOURCES:%.cpp=$(OBJDIR)$(TARGET)/%.d)
endif

$(BINDIR)$(TARGET)/lib$(LIB).a:$(OBJECTS)
	@test -d $(BINDIR)$(TARGET) || mkdir -p $(BINDIR)$(TARGET)
	@rm -rf $@
	ar cq $@ $(OBJECTS) # $(EXTRAOBJS) 

$(foreach source, $(SOURCES), $(eval $(call makecppgoal,$(OBJDIR)$(TARGET)/$(basename $(subst ..,__,$(source))),$(SOURCEDIR)$(source))))

clean:
	rm -rf $(OBJDIR)	
	if test -d $(GENDIR); then rm -rf $(GENDIR)*; fi

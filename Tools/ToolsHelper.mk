LDFLAGS += -L$(DEPENDENCYDIR)lib
INCLUDES += -I$(DEPENDENCYDIR)include

CXXFLAGS += $(INCLUDES) -fno-exceptions -std=c++0x -fno-rtti
UNAME_P := $(shell uname -p)

ifdef ADDITIONALCXXFLAGS
	CXXFLAGS += $(ADDITIONALCXXFLAGS)
endif

ifeq ($(TARGET), Release)
	CXXFLAGS += -O3
endif

ifeq ($(TARGET), Debug)
	CXXFLAGS += -ggdb
endif

ifeq ($(CXX), clang++)
	ifneq ($(filter arm%, $(UNAME_P)),)
		CXXFLAGS += -mfloat-abi=hard
	endif
endif

MAKEDEPEND = $(CXX) -MM $(CXXFLAGS) $(ADDITIONALCXXFLAGS) -o $(OBJDIR)$(TARGET)/$*.d $(SOURCEDIR)$*.cpp

DEPLIBS:=$(addprefix -l, $(WNLIBS))
DEPLIBFILES:=$(addsuffix .a, $(addprefix $(WNLIBDIR)lib, $(WNLIBS)))
OBJECTS=$(addsuffix .o, $(addprefix $(OBJDIR)$(TARGET)/, $(subst ..,__,$(basename  $(SOURCES)))))
	
.PHONY: all

all: $(BINDIR)$(TARGET)/$(TOOL)

ifneq "$(MAKECMDGOALS)" "clean"
-include $(SOURCES:%.cpp=$(OBJDIR)$(TARGET)/%.d)
endif


define makecppgoal
$1.d:$2
	$(info $1.d:$2)
	@echo Make Depend $2 \>\> $1.d
	@test -d $(dir $1.d) || mkdir -p $(dir $1.d)
	@$(CXX) -MM $(CXXFLAGS) -c $2 -o $1.d.t
	@sed 's/$(notdir $1.o)/$(subst /,\/,$1.o $1.d)/' < $1.d.t > $1.d
	@rm -rf $1.d.t

$1.o:$1.d
	$(CXX) $(CXXFLAGS) -c $2 -o $1.o
endef 

$(BINDIR)$(TARGET)/$(TOOL):$(OBJECTS) $(DEPLIBFILES)
	test -d $(BINDIR) || mkdir -p $(BINDIR)
	test -d $(BINDIR)$(TARGET) || mkdir -p $(BINDIR)$(TARGET)
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS)  $(ADDITIONALCXXFLAGS) -L $(WNLIBDIR) $(DEPLIBS) $(ADDITIONALLIBS) $(ADDITIONALLINKFLAGS) $(TOOL_ADDITIONAL_LIBS) $(LINKFLAGS) 

clean:
	rm -rf $(OBJDIR)

$(foreach source, $(SOURCES), $(eval $(call makecppgoal,$(OBJDIR)$(TARGET)/$(basename $(subst ..,__,$(source))),$(SOURCEDIR)$(source))))


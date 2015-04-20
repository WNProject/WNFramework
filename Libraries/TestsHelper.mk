CXXFLAGS += -I$(DEPENDENCYDIR)include $(INCLUDES) -fno-exceptions -std=c++11 -fno-rtti -Werror
LINKFLAGS += -L$(DEPENDENCYDIR)lib -lgtest -lpthread
UNAME_P := $(shell uname -p)
TOOL := $(PROJECT)Test

ifdef ADDITIONALCXXFLAGS
	CXXFLAGS += $(ADDITIONALCXXFLAGS)
endif

ifeq ($(TARGET), Release)
	CXXFLAGS += -O3
endif

CXXFLAGS += -ggdb

ifeq ($(CXX), clang++)
	ifneq ($(filter arm%, $(UNAME_P)),)
		CXXFLAGS += -mfloat-abi=hard
	endif
	INCLUDEPCH:=$(addprefix -include-pch $(PCHDIR)$(TARGET)/$(PROJECT)/test/inc/,$(addsuffix .pch,$(PCHFILES)))
	PCHEXT:=.pch
else
	PCHEXT:=.gch
endif

MAKEDEPEND = $(CXX) -MM $(CXXFLAGS) $(ADDITIONALCXXFLAGS) -o $(OBJDIR)$(TARGET)/$*.d $(SOURCEDIR)$*.cpp

DEPLIBS:=$(addprefix -l, $(WNLIBS))
DEPLIBFILES:=$(addsuffix .a, $(addprefix $(WNLIBDIR)lib, $(WNLIBS)))
OBJECTS=$(addsuffix .o, $(addprefix $(OBJDIR)$(TARGET)/, $(subst ..,__,$(basename  $(SOURCES)))))
ALLPCHFILES:=$(addsuffix .h$(PCHEXT), $(addprefix $(PCHDIR)$(TARGET)/$(PROJECT)/test/inc/, $(basename $(PCHFILES))))
ifneq ($(strip $(PCHFILES)),)
	PCHDEPFLAGS:= $(CXXFLAGS) -D__WN_USE_PRECOMPILED_HEADER
	CXXFLAGS:= -I$(PCHDIR)$(TARGET) $(CXXFLAGS) $(INCLUDEPCH) -D__WN_USE_PRECOMPILED_HEADER
endif
	
.PHONY: all

all: $(BINDIR)$(TARGET)/$(TOOL)

ifneq "$(MAKECMDGOALS)" "clean"
-include $(SOURCES:%.cpp=$(OBJDIR)$(TARGET)/%.d)
endif


define makecppgoal
ifneq "$(MAKECMDGOALS)" "clean"
-include $1.d
endif

$1.d:$2 $(ALLPCHFILES)
	@echo Make Depend $2 \>\> $1.d
	@test -d $(dir $1.d) || mkdir -p $(dir $1.d)
	@$(CXX) -MM $(CXXFLAGS) -c $2 -o $1.d.t
	@sed 's/$(notdir $1.o)/$(subst /,\/,$1.o $1.d)/' < $1.d.t > $1.d
	@rm -rf $1.d.t

$1.o:$1.d $(ALLPCHFILES)
	$(CXX) $(CXXFLAGS) -c $2 -o $1.o
endef 

define makepchgoal
ifneq "$(MAKECMDGOALS)" "clean"
-include $1.d
endif

$1.d:$2
	@echo Make PCH Depend $2 \>\> $1.d
	@test -d $(dir $1.d) || mkdir -p $(dir $1.d)
	@$(CXX) -MM $(PCHDEPFLAGS) -x c++-header -c $2 $(EXTRAPCH) -o $1.d.t
	@sed 's/$(notdir $1.o)/$(subst /,\/,$1.o $1.d)/' < $1.d.t > $1.d
	@rm -rf $1.d.t

$1.h$(PCHEXT):$1.d
	#$(CXX) $(CXXFLAGS) -c $2 -o $1.o
	@echo Making PCH $2 \>\> $1.h$(PCHEXT)
	$(CXX) $(PCHDEPFLAGS) -x c++-header -c $2 $(EXTRAPCH) -o $1.h$(PCHEXT)
endef

$(BINDIR)$(TARGET)/$(TOOL):$(OBJECTS) $(DEPLIBFILES) $(TOOL_ADDITIONAL_LIB_FILES)
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	@test -d $(BINDIR)$(TARGET) || mkdir -p $(BINDIR)$(TARGET)
	$(CXX) -o $@ $(OBJECTS) $(ADDITIONALCXXFLAGS) -L $(WNLIBDIR) $(DEPLIBS) $(ADDITIONALLIBS) $(ADDITIONALLINKFLAGS) $(TOOL_ADDITIONAL_LIBS) $(LINKFLAGS) 
ifeq ($(TARGET), Release)
	@test -d $(BINDIR)$(TARGET)/Symbols || mkdir -p $(BINDIR)$(TARGET)/Symbols
	@objcopy --only-keep-debug $(BINDIR)$(TARGET)/$(TOOL) $(BINDIR)$(TARGET)/Symbols/$(TOOL).debug
	@strip -s $(BINDIR)$(TARGET)/$(TOOL)
endif
	(xset q > /dev/null && $(BINDIR)$(TARGET)/$(TOOL)) || xvfb-run --auto-servernum --server-num=1 --server-args="-screen 0 1024x768x24 +extension GLX +render -noreset" $(BINDIR)$(TARGET)/$(TOOL)

clean:
	rm -rf $(OBJDIR)

$(foreach source, $(SOURCES), $(eval $(call makecppgoal,$(OBJDIR)$(TARGET)/$(basename $(subst ..,__,$(source))),$(SOURCEDIR)$(source))))
$(foreach pch, $(PCHFILES), $(eval $(call makepchgoal,$(PCHDIR)$(TARGET)/$(PROJECT)/test/inc/$(basename $(pch)),$(INCDIR)$(pch))))


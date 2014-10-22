BUILDDIR?=.
SOURCEDIR?=.
PACKAGE_NAME?=org.example.myactivity
ACTIVITY_NAME?=MyNativeActivity
LIB_NAME?=MyLib
PROGRAM_NAME?=MyProgram
PROJECT_NAME?=NativeProject
WN_NDK_BUILD_PATH?=ndk-build
WN_PROJECT_PATH?=
WN_LIBRARIES?=
WN_SOURCES?=
WN_ADDITIONAL_ANDROID_LIBS?=
WN_ADDITIONAL_ANDROID_MODULES?=
WN_ADDITIONAL_CFLAGS?=
WN_ANDROID_MODULES:=
OB=(
CB=)
CM=,
ANDROID_BUILD_SOURCES:= AndroidManifest.xml*AndroidManifest.xml\
			   			build.xml*build.xml\
						runner.sh*runner.sh\
						stacktool.sh*stacktool.sh\
						project.properties*project.properties\
			   			Android.mk*jni/Android.mk\
			   			Application.mk*jni/Application.mk\
			   			MyNativeActivity.java*src/$(subst .,/,$(PACKAGE_NAME))/$(ACTIVITY_NAME).java\
			   			strings.xml*res/values/strings.xml

WN_FORCED_LIBRARIES := $(filter WNEntryPoint,$(WN_LIBRARIES))
WN_LIBRARIES := $(filter-out WNEntryPoint,$(WN_LIBRARIES))

ANDROID_BUILD_SOURCES:= $(addprefix $(WN_BUILD_HELPER_ROOT), $(ANDROID_BUILD_SOURCES))
WN_ANDROID_MODULES:=$(addprefix $$$$$$$(OB)OB$(CB)call import-module$$$(OB)CM$(CB),$(addsuffix $$$(OB)CB$(CB),$(strip $(WN_ADDITIONAL_ANDROID_MODULES))))

WN_SOURCE_TARGETS:=$(subst ..,__,$(WN_SOURCES))
TESTFILES:=
BUILDFILES:=
CLEANFILES:=
all2:all
	
define makecpgoal
TESTFILES+=$1
CLEANFILES+=clean$1
$1:$2 Makefile
	@echo Creating new build file $(1) \<\< $(2)
	@test -d $(dir $1) || mkdir -p $(dir $1)
	@sed -e's*##WN_ADDITIONAL_ANDROID_LIBS##*$(WN_ADDITIONAL_ANDROID_LIBS)*g' -e's*##WN_ADDITIONAL_ANDROID_MODULES##*$(WN_ANDROID_MODULES)*g' -e's*##ANDROID_LLVM_CONFIG##*$(WN_BUILD_HELPER_ROOT)/../../Externals/Android/arm/Build/Release/BuildTools/Release+Asserts/bin/llvm-config*g' -e's*##PACKAGE_NAME##*$(PACKAGE_NAME)*g' -e's*##WN_FORCED_LIBRARIES##*$(WN_FORCED_LIBRARIES)*g' -e 's*##ACTIVITY_NAME##*$(ACTIVITY_NAME)*g' -e 's*##LIB_NAME##*$(LIB_NAME)*g' -e's*##PROGRAM_NAME##*$(PROGRAM_NAME)*g' -e's*##PROJECT_NAME##*$(PROJECT_NAME)*g' -e's*##WN_PROJECT_PATH##*$(WN_PROJECT_PATH)*g' -e's*##WN_LIBRARIES##*$(WN_LIBRARIES)*g' -e's*##WN_SOURCES##*$(WN_SOURCE_TARGETS)*g' -e's*##WN_ADDITIONAL_ANDROID_LIBS##*$(WN_ADDITIONAL_ANDROID_LIBS)*g' -e's*##WN_ADDITIONAL_CFLAGS##*$(WN_ADDITIONAL_CFLAGS)*g' < $2 > $1
.PHONY: clean$1
clean$1: UNBUILD
	rm -rf $1
endef

define makefilecopy
TESTFILES:=$1 $(TESTFILES)
CLEANFILES+=clean$1
$1:$2 Makefile
	@echo Copying Source $1 \<\< $2
	@test -d $(dir $1) || mkdir -p $(dir $1)
	@cp -f $2 $1
.PHONY: clean$1
clean$1: UNBUILD
	rm -rf $1
endef


$(foreach file, $(WN_SOURCES), $(eval $(call makefilecopy,$(BUILDDIR)/jni/$(subst ..,__,$(file)), $(file))))
$(foreach pair, $(ANDROID_BUILD_SOURCES), $(eval $(call makecpgoal,$(BUILDDIR)/$(word 2,$(subst *, ,$(pair))),$(SOURCEDIR)/$(word 1,$(subst *, ,$(pair))))))


.PHONY: build

$(BUILDDIR)/bin/$(PROGRAM_NAME)-debug.apk:$(BUILDDIR)/obj/local/armeabi/lib$(LIB_NAME).so
	@ant debug
	
$(BUILDDIR)/obj/local/armeabi/lib$(LIB_NAME).so:build

build:$(TESTFILES)
	@ + $(WN_NDK_BUILD_PATH) NDK_APPLICATION_MK=$(BUILDDIR)/jni/Application.mk
	chmod +x runner.sh
	chmod +x stacktool.sh
	
all:build $(BUILDDIR)/bin/$(PROGRAM_NAME)-debug.apk

install:
	@adb shell am force-stop $(PACKAGE_NAME)
	@ant installd

run:
	adb logcat -c
	adb shell am force-stop $(PACKAGE_NAME)
	adb shell am start $(PACKAGE_NAME)/.$(ACTIVITY_NAME)
	adb logcat -s $(PACKAGE_NAME):V | while read x ; do echo $x ; done
	adb pull /sdcard/stdout.txt .
	adb shell rm /sdcard/stdout.txt
	cat stdout.txt
	rm stdout.txt

.PHONY: UNBUILD
UNBUILD:
	@ + $(WN_NDK_BUILD_PATH) clean

clean:UNBUILD $(CLEANFILES)
	rm -rf $(BUILDDIR)/jni
	rm -rf $(BUILDDIR)/res
	rm -rf $(BUILDDIR)/src
	rm -rf $(BUILDDIR)/libs
	rm -rf $(BUILDDIR)/obj
	rm -rf $(BUILDDIR)/bin
	rm -rf $(BUILDDIR)/gen

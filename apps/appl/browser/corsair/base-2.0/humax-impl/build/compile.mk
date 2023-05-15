###################################
# Included from Makefile
###################################

###################################
# Silent commands in targets
##
SILENT ?= @


###################################
# Toolchain
##
# If a toolchain's set, let's use that one
##
ifneq ($(TOOLCHAIN),)
  PATH := $(TOOLCHAIN)/bin:${PATH}
endif


###################################
# Debug
##
# If DEBUGSYMBOLS=YES, let's build with debugsymbols & without
# optimization.
##
ifeq ($(DEBUGSYMBOLS),YES)
  GLOBAL_CXXFLAGS := $(GLOBAL_CXXFLAGS) -O0 -ggdb
endif

###################################
# Debug
##
# If DEBUGCODE=YES, let's build with DEBUGCODE defined
##
ifeq ($(DEBUGCODE),YES)
  GLOBAL_CXXFLAGS := $(GLOBAL_CXXFLAGS) -DDEBUGCODE -O0
else
  GLOBAL_CXXFLAGS := $(GLOBAL_CXXFLAGS) -DNDEBUG -O2
endif

###################################
# Logging
##
# If DEBUGCODE=YES, let's build with DEBUGCODE defined
##
ifeq ($(CONFIG_DEBUG),y)
ifeq ($(CONFIG_BUILD_DEBUG),YES)
  GLOBAL_CXXFLAGS := $(GLOBAL_CXXFLAGS) -O0 -ggdb -DLOGGING_ENABLED
else
  GLOBAL_CXXFLAGS := $(GLOBAL_CXXFLAGS) -O2 -DLOGGING_ENABLED
endif
endif

###################################
# Assertions
##
# If ASSERTIONS=NO, let's enable the NDEBUG define.
##
ifeq ($(ASSERTIONS),NO)
  GLOBAL_CXXFLAGS := $(GLOBAL_CXXFLAGS) -DNDEBUG
endif

GLOBAL_CXXFLAGS += $(HUMAX_EXTRA_CFLAG)



###################################
# Endian
##
ifneq ($(ENDIAN),BIG)
  GLOBAL_CXXFLAGS := $(GLOBAL_CXXFLAGS) -DHBBTV_LITTLE_ENDIAN
endif



###################################
# Alignment
##
# If ALIGNED=YES, we can't do unaligned data access.
##
ifeq ($(ALIGNED),YES)
  GLOBAL_CXXFLAGS := $(GLOBAL_CXXFLAGS) -DHBBTV_NEEDS_ALIGNMENT
endif

CC  = $(CROSS)gcc$(COMPILER_SUFFIX)
CXX = $(CROSS)g++$(COMPILER_SUFFIX)
AR = $(CROSS)ar$(COMPILER_SUFFIX)

INCPATH = -I. -I.. -I../include -I../components -I$(NATIVE_PORT_PATH)/include -I$(NATIVE_APP_PATH)/device/include/directfb
LPATH ?= -L../lib -Lout -Wl,-rpath-link=out

ifneq ($(TOOLCHAIN),)
  INCPATH += -I$(TOOLCHAIN)/include
  LPATH += -L$(TOOLCHAIN)/lib
endif

BUILD_COMPONENT_SHARED_LIBRARY = $(SILENT) $(CXX) $(LPATH) -Wall -fPIC -Wl,-soname,$(OUTPUT) -shared -o
POST_BUILD_COMPONENT_SHARED_LIBRARY = $(LFLAGS) $(GLOBAL_LFLAGS)
BUILD_COMPONENT_STATIC_LIBRARY = $(SILENT) $(AR) r
POST_BUILD_COMPONENT_STATIC_LIBRARY =
BUILD_COMPONENT_EXECUTABLE = $(SILENT) $(CXX) $(LPATH) -o
POST_BUILD_COMPONENT_EXECUTABLE = $(LFLAGS) $(GLOBAL_LFLAGS)

%.o: %.cpp
	@echo "Compiling:" $<...
	$(SILENT) $(CXX) -c $($(COMPONENT)_CXXFLAGS) $($(SUBCOMPONENT)_CXXFLAGS) $($(SUBCOMPONENT)_INCPATH) $(GLOBAL_CXXFLAGS) $(INCPATH) -MMD -MF $*.d $*.cpp -o $*.o

%.d: %.cpp
	@echo "Generating dependency:" $@...
	@$(CXX) $($(COMPONENT)_CXXFLAGS) $($(SUBCOMPONENT)_CXXFLAGS) $($(SUBCOMPONENT)_INCPATH) $(INCPATH) $(GLOBAL_CXXFLAGS) -MM -MT $*.o -MF $@ $<

%.o: %.c
	@echo "Compiling:" $<...
	$(SILENT) $(CC) -c $($(COMPONENT)_CXXFLAGS) $(GLOBAL_CXXFLAGS) $(INCPATH) -MMD -MF $*.d $*.c -o $*.o

%.d: %.c
	@echo "Generating dependency:" $@...
	@$(CC) $($(COMPONENT)_CXXFLAGS) $(INCPATH) $(GLOBAL_CXXFLAGS) -MM -MT $*.o -MF $@ $<

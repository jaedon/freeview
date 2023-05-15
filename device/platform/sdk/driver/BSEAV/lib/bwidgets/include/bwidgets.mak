# Bwidgets Makefile include
# Assumes BSEAV has been defined

ifeq ($(DEBUG),)
DEBUG = y
endif

BWIDGETS_DIR = $(BSEAV)/lib/bwidgets
ifeq ($(DEBUG),y)
BWIDGETS_LIBDIR = $(BWIDGETS_DIR)/lib/$(ARCH).debug
else
BWIDGETS_LIBDIR = $(BWIDGETS_DIR)/lib/$(ARCH).release
endif

BWIDGETS_CFLAGS = -I$(BWIDGETS_DIR)/include
BWIDGETS_LDFLAGS = -L$(BWIDGETS_LIBDIR) -lbwidgets

include $(BSEAV)/lib/bwin/include/bwin.mak
BWIDGETS_CFLAGS += $(BWIN_CFLAGS)
BWIDGETS_LDFLAGS += $(BWIN_LDFLAGS)


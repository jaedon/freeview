CUR_DIR			= $(CURDIR)
HUMAX_TOP_DIR		= $(CUR_DIR)/../../../../../..
HUMAX_MAKE_DIR		= $(HUMAX_TOP_DIR)/make
GCC			= gcc
GCC_OPTION		= -g


#include $(HUMAX_MAKE_DIR)/include/globalconfig.h

hostprogs-y			:= strgen
strgen-c			:=
STRGENDEFINE		+= -DLOCAL_COMPILE -DOS_LINUX -Wno-pointer-to-int-cast -Wno-format -Wno-int-to-pointer-cast
# -Wno-unused-value -Wno-type-limits -Wno-format -Wno-pointer-sign -Wno-strict-prototypes
STRGENINCLUDE		+= -I$(CUR_DIR)/StrGen -I$(CUR_DIR)/StrGen/include -I$(HUMAX_MAKE_DIR)


strgen-c 			+= StrGen/reslib/rxvector.c
strgen-c 			+= StrGen/reslib/rxstdlib.c
strgen-c 			+= StrGen/reslib/rxlist.c
strgen-c 			+= StrGen/reslib/rxhash.c
strgen-c 			+= StrGen/reslib/rxfile.c
strgen-c 			+= StrGen/reslib/rxdir.c
strgen-c 			+= StrGen/reslib/rxcodepage.c
strgen-c 			+= StrGen/reslib/rx_cp_utf8.c
strgen-c 			+= StrGen/reslib/rx_cp_ucs2.c
strgen-c			+= StrGen/reslib/rxlang639a.c
strgen-c 			+= StrGen/StrGenTool.c

all:
	$(GCC) $(GCC_OPTION) $(STRGENDEFINE)  $(strgen-c) -o StrGen/strgen $(STRGENINCLUDE)

clean:

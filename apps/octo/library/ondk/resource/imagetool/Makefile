CUR_DIR			= $(CURDIR)
HUMAX_TOP_DIR		= $(CUR_DIR)/../../../../../..
HUMAX_MAKE_DIR		= $(HUMAX_TOP_DIR)/make
GCC			= gcc
GCC_OPTION		= -g


#include $(HUMAX_MAKE_DIR)/include/globalconfig.h

hostprogs-y			:= imggen
imggen-c			:=
STRGENDEFINE		+= -DLOCAL_COMPILE -DOS_LINUX -Wno-pointer-to-int-cast -Wno-format -Wno-int-to-pointer-cast
STRGENINCLUDE		+= -I$(CUR_DIR)/ImgGen -I$(CUR_DIR)/ImgGen/include -I$(HUMAX_MAKE_DIR)


imggen-c 			+= ImgGen/reslib/rxvector.c
imggen-c 			+= ImgGen/reslib/rxstdlib.c
imggen-c 			+= ImgGen/reslib/rxlist.c
imggen-c 			+= ImgGen/reslib/rxhash.c
imggen-c 			+= ImgGen/reslib/rxfile.c
imggen-c 			+= ImgGen/reslib/rxdir.c
imggen-c 			+= ImgGen/reslib/rxcodepage.c
imggen-c 			+= ImgGen/reslib/rx_cp_utf8.c
imggen-c 			+= ImgGen/reslib/rx_cp_ucs2.c
imggen-c			+= ImgGen/reslib/rxlang639a.c
imggen-c 			+= ImgGen/ImgGenTool.c

all:
	$(GCC) $(GCC_OPTION) $(STRGENDEFINE)  $(imggen-c) -o ImgGen/imggen $(STRGENINCLUDE)

clean:

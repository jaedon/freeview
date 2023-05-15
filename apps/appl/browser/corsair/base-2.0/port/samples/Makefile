#
#	Makefile for BPL examples
#
#	Written by Jongwon Kim
#		2011. 8. 19.
#

TOPDIR		:= ../
include $(TOPDIR)makehead.env

C_SRC		:= exbroadcast.c
CXX_SRC 	:= 
BPLLIB		:= $(TOPDIR)src/libbpl.a
LIBS		:= $(BPLLIB)
TARGETS		:= $(CXX_SRC:.cpp=) $(C_SRC:.c=)

CFLAGS		:= -I../include $(CFLAGS) -D_REENTRANT 


%: %.c
	$(CC) $(CFLAGS) $< -o $@ $(LIBS)

%: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIBS)
	

### targets ----------
all: $(BPLLIB) $(TARGETS)

include $(TOPDIR)maketail.env

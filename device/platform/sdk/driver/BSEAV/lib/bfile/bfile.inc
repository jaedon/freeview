############################################################
#     Copyright (c) 2007 Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bfile.inc $
# $brcm_Revision: 3 $
# $brcm_Date: 6/21/10 1:37p $
#
# Module Description:
#   FLV media library module descriptor
#
# Revision History:
#
# $brcm_Log: /BSEAV/lib/bfile/bfile.inc $
# 
# 3   6/21/10 1:37p vsilyaev
# SW3548-2975: Added file utilities
# 
# 2   9/18/08 3:27p vsilyaev
# PR 47105: Added support for layered buffer cache
# 
# 1   7/20/07 1:33p vsilyaev
# PR 32813: Module descriptor file
# 
# 
############################################################

BFILE_INCLUDES := $(BSEAV)/lib/bfile

BFILE_SOURCES :=  \
	${BSEAV}/lib/bfile/bfile_io.c 

BFILE_MEDIA_SOURCES := \
	${BSEAV}/lib/bfile/bfile_cache.c \
	${BSEAV}/lib/bfile/bfile_buffer.c \
	${BSEAV}/lib/bfile/bfile_buffered.c \
	${BSEAV}/lib/bfile/bfile_util.c  

BFILE_MEDIA_INCLUDES := ${BFILE_INCLUDES}

# modified by junsy (for VFIO)
ifeq ($(HUMAX_PLATFORM_BASE),y) # Using VFIO
ifeq ($(CONFIG_MEDIA20),y) # Using VFIO20
        BFILE_STDIO_SOURCES := \
		${BSEAV}/lib/bfile/bfile_stdio.c \
		${BSEAV}/lib/bfile/vfio20.c
else
        BFILE_STDIO_SOURCES := \
		${BSEAV}/lib/bfile/bfile_stdio.c \
		${BSEAV}/lib/bfile/vfio.c
endif
        BFILE_STDIO_INCLUDES := ${BFILE_INCLUDES}
else
BFILE_STDIO_SOURCES := \
	${BSEAV}/lib/bfile/bfile_stdio.c 
BFILE_STDIO_INCLUDES := ${BFILE_INCLUDES}
endif
#END junsy

BFILE_ASYNC_SOURCES := \
	${BSEAV}/lib/bfile/bfile_async.c
BFILE_ASYNC_INCLUDES := ${BFILE_INCLUDES}



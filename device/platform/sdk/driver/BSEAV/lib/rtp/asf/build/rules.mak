############################################################################
#     Copyright (c) 2003-2006, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: rules.mak $
# $brcm_Revision: 1 $
# $brcm_Date: 10/16/06 12:58p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/lib/rtp/asf/build/rules.mak $
# 
# 1   10/16/06 12:58p ptimariu
# PR24026: adding asf ip support
# 
# 24   8/28/06 11:22a mphillip
# PR22821: Issue was the ODIR_FLAG wasn't being made.  This will ignore
# that dependency if the output is the current directory (as directory
# creation serialization is not key if you're using pwd).
# 
# 23   8/25/06 12:13p rjlewis
# PR20604: vxworks doesn't like ?=
# 
# 22   8/2/06 7:01p rjlewis
# PR19725: don't error if there are no files to delete.
# 
# 21   8/2/06 2:08p mphillip
# PR19544: More build support
# 
# 20   7/14/06 11:31a jgarrett
# PR 19909: Merging to latest baseline
# 
# PR19909/2   7/6/06 4:43p jgarrett
# PR 19909: Merging to latest baseline
# 
# 19   6/13/06 11:56a mphillip
# PR21883: Experimental aggressive optimization for p3d, opengles, vdc,
# and grc enabled for 7038.
# 
# 18   5/9/06 4:30p vsilyaev
# PR 21171: Added hook that allows to execute arbitrary command when
# library and map file were built
# 
# 16   4/24/06 3:00p vsilyaev
# PR 21171: Added rule to build map file
# 
# 15   3/28/06 11:16a vsilyaev
# PR 20308: Fixed application build rules
# 
# PR19909/1   7/6/06 10:51a jgarrett
# PR 19909: Reducing brutus build output
# 
# 14   1/24/06 1:34p btan
# PR 19086: Added LIBS during APP_IMAGE build.
# 
# 12   10/10/05 11:37a vsilyaev
# PR9497: Make Q_ options overwritable
# 
# 11   8/8/05 10:29a erickson
# PR15577: removed ODIR_FLAG functionality for vxworks
# 
# 10   7/20/05 5:17p vsilyaev
# PR 15577: Added rule to guarantee that we start compiling only after
# target directory was created.
# 
# 8   5/5/05 2:31p erickson
# PR15159: added double quotes for DOS builds
# 
# 7   5/2/05 12:29p erickson
# PR15145: added OTHER_MAKES which allows option of extending build rules
# 
# 6   4/27/05 4:29p erickson
# PR9497: DOS mkdir doesn't do multiple subdirs (inside makefile at
# least), so be more explicit about it
# 
# 5   3/24/05 12:53p vsilyaev
# PR 14593: 93560 support
# 
# Refsw_93560_Bringup/1   3/21/05 9:09p vsilyaev
# Use C compiler (instead of C++) to build applications. It removes from
# link some libraries.
# 
# 4   2/23/05 2:40p erickson
# PR14180: removed unnecessary vxworks code
# 
# 3   2/17/05 1:51p erickson
# PR9497: make vxworks-friendly
# 
# 2   2/16/05 4:44p erickson
# PR9497: allow dos mkdir to work
# 
# 1   2/7/05 6:57p dlwin
# Merge down for release 2005_REFSW_MERGETOMAIN:
# 
# Irvine_BSEAVSW_Devel/14   1/25/05 5:12p erickson
# PR13908: Settop API makefile rework
# 
# Irvine_BSEAVSW_Devel/13   12/9/04 9:37a erickson
# PR12610: allow png to be cleaned from bwin
# 
# Irvine_BSEAVSW_Devel/12   11/16/04 3:27p erickson
# PR13272: allow Q_= makefile option to have verbose output
# 
# Irvine_BSEAVSW_Devel/11   8/13/04 10:04a liangl
# PR12279: Built fails if DEBUG=n in vxworks.bat
# 
# Irvine_BSEAVSW_Devel/11   8/13/04 10:03a liangl
# PR12279: Built fails if DEBUG=n in vxworks.bat
# 
# Irvine_BSEAVSW_Devel/10   4/7/04 1:18a vsilyaev
# PR 9497: Removed bogus comment.
# 
# Irvine_BSEAVSW_Devel/9   3/29/04 10:14a jjordan
# PR10164: fix build dependencies (*.h) issue
# 
# Irvine_BSEAVSW_Devel/8   1/29/04 8:11p vsilyaev
# PR 9497: Cleanup, compile vxworks to binaries with 'out' suffix.
# 
# Irvine_BSEAVSW_Devel/7   1/29/04 6:29p vsilyaev
# PR 9497: Some hackory to make WindRiver's incornations of Gnu Make and
# Gnu C happier.
# 
# Irvine_BSEAVSW_Devel/6   1/27/04 5:56p vsilyaev
# PR 9497: Use different suffix and rules to build VxWorks shared library
# 
# Irvine_BSEAVSW_Devel/5   1/15/04 10:06a erickson
# PR9320: Switched -MMD to -MD in order to pick up Magnum dependencies.
# 
# Irvine_BSEAVSW_Devel/4   12/19/03 4:54p vsilyaev
# PR 8850: In clean also remove libraries
# 
# Irvine_BSEAVSW_Devel/3   7/28/03 3:48p erickson
# veryclean is dangerous from utils without this check
# 
# Irvine_BSEAVSW_Devel/2   7/1/03 12:38p vsilyaev
# Changes order between destdir and library.
# 
# 
###########################################################################

ifeq ($(Q_),)
Q_=@
endif

CDEP_FLAG = -MD

ifeq ($(ODIR),)
ODIR = .
endif


# We need ODIR_FLAG to serialize between directory creation and building object files into this directory
# ODIR itself can't be used because it would change with every object file written into the directory
ifneq ($(SYSTEM),vxworks)
ifeq ($(ODIR),.)
ODIR_FLAG=
else
ODIR_FLAG = ${ODIR}/flag
endif
endif

# COPT_FLAGS is being set to allow objects in *_OPT_OBJS to be built with specialized flags.  As of the time 
# of implementation, it is restricted to the p3d, opengles, grc, and vdc objects.  Not all areas of settopapi 
# can safely be optimized to this level.

# Implicit C Compile Rule
ifeq ($(SYSTEM),linux)
ifeq ($(BCHP_CHIP),7038)
$(ODIR)/%.o : COPT_FLAGS=$(if $(findstring $@,$(ST_OPT_OBJS)),-O3 -mips32,)
endif
endif
$(ODIR)/%.o : %.c ${ODIR_FLAG}
		@echo [Compile... $(notdir $<)]
		${Q_}$(CC) ${CDEP_FLAG} ${CFLAGS_STATIC} $(CFLAGS) $(COPT_FLAGS) -c $< -o $@
ifeq ($(SYSTEM),vxworks)
ifneq ($(vxWorksVersion),6)
		-${Q_}$(MV) $(patsubst %.c,%.d,$(notdir $< )) ${ODIR}
endif
endif

ifeq ($(SYSTEM),linux)
ifeq ($(BCHP_CHIP),7038)
$(ODIR)/%.so : COPT_FLAGS=$(if $(findstring $@,$(SH_OPT_OBJS)),-O3 -mips32,)
endif
endif
$(ODIR)/%.so : %.c ${ODIR_FLAG}
		@echo [Compile... $(notdir $<)]
		${Q_}$(CC) ${CDEP_FLAG} ${CFLAGS_SHARED} $(CFLAGS) $(COPT_FLAGS) -c $< -o $@
ifeq ($(SYSTEM),vxworks)
ifneq ($(vxWorksVersion),6)
		-${Q_}$(MV) $(patsubst %.c,%.d,$(notdir $< )) ${ODIR}
endif
endif


# Implicit C++ Compile Rule
$(ODIR)/%.o : %.cpp ${ODIR_FLAG}
	@echo [Compile... $(notdir $<)]
	${Q_}$(CC) ${CDEP_FLAG} ${CPPFLAGS_STATIC} $(CPPFLAGS) -c $< -o $@

# Implicit C++ Compile Rule
$(ODIR)/%.so : %.cpp ${ODIR_FLAG}
	@echo [Compile... $(notdir $<)]
	${Q_}$(CC) ${CDEP_FLAG} ${CPPFLAGS_SHARED} $(CPPFLAGS) -c $< -o $@

# Determine whether or not to print rm operations
ifneq ($(VERBOSE),)
RMOPTS := -v
else
RMOPTS :=
endif

ifeq ($(SYSTEM),vxworks)
#ifneq ($(vxWorksVersion),6)
# For tests and utils we need a rule that doesn't include the ODIR (the objects build in the currect working directory).
# This is mainly for VxWorks 5.5 compiler (defaults to putting the .o and the .d in the current working directory.
# Implicit C Compile Rule
%.o : %.c
		@echo === Compiling $<
		${Q_}$(CC) ${CDEP_FLAG} ${CFLAGS_STATIC} $(CFLAGS) -c $< -o $@
#endif
endif

ifdef APP
APP_OBJS = $(SRCS:%.c=${ODIR}/%.o)
APP_OPT_OBJS = $(OPTIMIZE_SRCS:%.c=${ODIR}/%.o)

ifeq ($(SYSTEM),vxworks)
APP_IMAGE = ${ODIR}/${APP}.out
else
APP_IMAGE = ${ODIR}/${APP}
endif

application: $(OTHER_MAKES) ${ODIR} ${APP_IMAGE}

$(APP_IMAGE): ${LIBS} $(OBJS) ${APP_OBJS}
	@echo [Linking... $(notdir $@)]
	${Q_}$(CC) $(OBJS) ${APP_OBJS} $(LDFLAGS) ${LIBS} -o $@

target-clean:
	${Q_}${RM} ${RMOPTS} ${APP_IMAGE}

endif

ifdef LIB
ifeq (${SYSTEM},vxworks)
SH_LIB = ${ODIR}/lib$(LIB).out
else
SH_LIB = ${ODIR}/lib$(LIB).so
endif
SH_MAP = ${ODIR}/lib$(LIB).map
ST_LIB = ${ODIR}/lib$(LIB).a

.PHONY: shared static
shared: $(OTHER_MAKES) ${ODIR} ${SH_LIB} 
static: $(OTHER_MAKES) ${ODIR} ${ST_LIB} 

SH_OBJS = $(SRCS:%.cpp=${ODIR}/%.so)
ST_OBJS = $(SRCS:%.cpp=${ODIR}/%.o)
SH_OPT_OBJS = $(OPTIMIZE_SRCS:%.cpp=${ODIR}/%.so)
ST_OPT_OBJS = $(OPTIMIZE_SRCS:%.cpp=${ODIR}/%.o)

${SH_LIB}: ${SH_OBJS} $(LIBS)
	@echo [Linking shared... $(notdir $@)]
ifeq (${SYSTEM},vxworks)
	${Q_}$(CC) ${LDFLAGS} -nostdlib -r -Wl,-EB -Wl,-X -o $@ -Wl,-soname,lib${LIB}.so ${SH_OBJS}
else
	${Q_}$(CC) ${LDFLAGS} -shared -Wl,-Map,${SH_MAP} -Wl,--cref -Wl,-x -o $@ -Wl,-soname,lib${LIB}.so ${SH_OBJS} ${LIBS} ${CRYPTO_LDFLAGS}
ifneq (${CMD_BUILD_DONE},)
	${Q_}${CMD_BUILD_DONE} ${SH_LIB} ${SH_MAP}
endif
endif

${ST_LIB}: ${ST_OBJS} 
	@echo [Linking static... $(notdir $@)]
	${Q_}$(RM) $@
	${Q_}$(AR) cq $@ $(ST_OBJS) ${CRYPTO_LDFLAGS}
	${Q_}$(RANLIB) $@

target-clean:
	${Q_}${RM} ${RMOPTS} ${SH_LIB} ${ST_LIB}

endif

# Dependency file checking (created with gcc -M command)
-include $(ODIR)/*.d

# Clean Rules
# Use OTHER_CLEANS to extend the clean rule for app-specific stuff.
# It's ok to leave it undefined.

.PHONY: clean target-clean veryclean $(OTHER_CLEANS)
clean: target-clean $(OTHER_CLEANS)
ifdef OBJS
	${Q_}$(RM) ${RMOPTS} $(OBJS)
	${Q_}$(RM) ${RMOPTS} $(OBJS:%.o=%.d)
endif
ifdef APP_OBJS
	${Q_}$(RM) ${RMOPTS} $(APP_OBJS)
	${Q_}$(RM) ${RMOPTS} $(APP_OBJS:%.o=%.d)
endif
ifdef SH_OBJS
	${Q_}$(RM) ${RMOPTS} $(SH_OBJS)
	${Q_}$(RM) ${RMOPTS} $(SH_OBJS:%.so=%.d)
endif
ifdef ST_OBJS
	${Q_}$(RM) ${RMOPTS} $(ST_OBJS)
	${Q_}$(RM) ${RMOPTS} $(ST_OBJS:%.o=%.d)
endif

veryclean:
ifdef ODIR
ifeq ($(vxWorksVersion),6)
# VxWorks 6 command shell can't handle long commands (sic) -- need to do this in steps.
# By doing these deletes first, the wildcards below will have less files to delete and thus shorter command line.
	-@$(RM) ${RMOPTS} $(ODIR)/*.o
	-@$(RM) ${RMOPTS} $(ODIR)/*.d
	-@$(RM) ${RMOPTS} $(ODIR)/*.so
endif
	-@$(RM) ${RMOPTS} $(ODIR)/*
	-@$(RM) -r ${RMOPTS} $(ODIR)
endif

ifneq ($(SYSTEM),vxworks)
ifneq ($(ODIR),.)
${ODIR_FLAG}: ${ODIR}
endif
endif

$(ODIR):
	${Q_}$(MKDIR) "$(ODIR)"
ifneq ($(SYSTEM),vxworks)
	${Q_}${TOUCH} "${ODIR_FLAG}"
endif


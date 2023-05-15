#**@@@+++@@@@*********************************************************
#*
#* Microsoft PlayReady
#* Copyright (c) Microsoft Corporation. All rights reserved.
#*
#**@@@---@@@@********************************************************/

#------------------------------------------------------------------------------
# MS internal builds only:
# The Microsoft internal build tools natively process project.mk and sources.
# If the PK makefile system hasn't been brought into the loop, do so here.
#------------------------------------------------------------------------------

!if "$(_DRM_BUILD_TOOL)" == ""
!    if "$(DRM_ROOT_PATH)" == "" && defined(BASEDIR)
DRM_ROOT_PATH=$(BASEDIR)\source\pk\WMDRM-MD^\
!    endif
!include $(DRM_ROOT_PATH)DrmMakefile.def
!endif


#------------------------------------------------------------------------------
# Define custom settings and then include everything from the project.mk above
# us.
#------------------------------------------------------------------------------

_DRM_BUILD_TEST=1


#------------------------------------------------------------------------------
# Force PK profile in certain situations.
#
# The build profile affects linking, not compilation. As such, source libs are
# somewhat agnostic to the profile. But exes, especially in test, are not.
#
# In the PC profile (for MS internal use), we turn off many features. However,
# we use the test binaries from the build to validate PK profile, which just
# involves linking the test code with different libs.
#------------------------------------------------------------------------------
!if ("$(DRM_BUILD_PROFILE)" == "PC" || "$(DRM_BUILD_PROFILE)" == "WP8")
USE_PK_NAMESPACES=1
DRM_BUILD_PROFILE=PK
!endif

!include $(DRM_ROOT_PATH)project.mk

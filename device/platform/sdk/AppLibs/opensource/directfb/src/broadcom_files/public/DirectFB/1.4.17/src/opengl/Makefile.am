## Makefile.am for DirectFB/src/opengl

INCLUDES = \
	-I$(top_builddir)/include	\
	-I$(top_builddir)/lib		\
	-I$(top_srcdir)/include		\
	-I$(top_srcdir)/lib		\
	-I$(top_srcdir)/src

AM_CPPFLAGS = \
	-DDATADIR=\"${RUNTIME_SYSROOT}@DATADIR@\"		\
	-DSOPATH=\"@SOPATH@\"		\
	-DMODULEDIR=\"${RUNTIME_SYSROOT}@MODULEDIR@\"	\
	"-DBUILDTIME=\"`date -u "+%Y-%m-%d %H:%M"`\""


internalincludedir = $(INTERNALINCLUDEDIR)/opengl

internalinclude_HEADERS = \
	opengl_api.h


noinst_LTLIBRARIES = libdirectfb_opengl.la


libdirectfb_opengl_la_SOURCES = \
	egl_dfb.c

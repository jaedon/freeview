## Makefile.am for DirectFB-examples/src

fontsdatadir = @DATADIR@/fonts

AM_CFLAGS = @DIRECTFB_CFLAGS@ -D_GNU_SOURCE

LIBADDS = \
	@DIRECTFB_LIBS@ -lz

AM_CPPFLAGS = \
	-DDATADIR=\"@DATADIR@\" \
	-DFONT=\"$(fontsdatadir)/decker.ttf\"

bin_PROGRAMS = \
	df_andi \
	df_brcmTest \
	df_dok \
	df_input \
	df_input_keysender \
	dfbtest_blit \
	dfbtest_layer

df_andi_SOURCES = df_andi.c 
df_andi_LDADD  = $(LIBADDS)

df_brcmTest_SOURCES = df_brcmTest.c 
df_brcmTest_LDADD  = $(LIBADDS)

df_dok_SOURCES = df_dok.c pngtest3.h
df_dok_LDADD  = $(LIBADDS)

df_input_SOURCES = df_input.c
df_input_LDADD = $(LIBADDS)

df_input_keysender_SOURCES = df_input_keysender.c

dfbtest_blit_SOURCES = dfbtest_blit.c 
dfbtest_blit_LDADD  = $(LIBADDS)

dfbtest_layer_SOURCES = dfbtest_layer.c 
dfbtest_layer_LDADD  = $(LIBADDS)

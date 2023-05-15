#--------------------------------------------------------------------------------------
# 기능단위로 나눠지는 부분들
#--------------------------------------------------------------------------------------
obj-y += svc_si_lib_basic.o
obj-y += svc_si_lib_es.o
obj-y += svc_si_lib_delivery.o
obj-y += svc_si_lib_conv.o

# Basic include path
include $(OCTO_MAIN_INCLUDE_DIR)/Makefile.include.mw
# Additional include path
ccflags-y += -I$(OCTO_TVSERVICE_DIR)/svc/si/local_include

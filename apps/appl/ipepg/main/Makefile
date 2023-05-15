obj-y += ipepg.o
obj-y += ipepg_svclistupload.o
obj-y += rp_download.o
obj-y += rp_parser.o
obj-y += rp_upload.o
obj-y += ipepg_auth.o

ifeq ($(CONFIG_OP_UK_DTT),y)
obj-$(CONFIG_OP_UK_DTT) += ipepg_downloader_fvc.o
obj-$(CONFIG_OP_UK_DTT) += ipepg_parser_fvc.o
obj-$(CONFIG_OP_UK_DTT) += ipepg_manager_fvc.o
obj-$(CONFIG_OP_UK_DTT) += ipepg_fvc.o
else
obj-y += ipepg_downloader.o
obj-y += ipepg_parser.o
obj-y += ipepg_manager.o
endif

# Additional include path
ccflags-y += -I../../make/include

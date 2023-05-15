obj-y += component/
obj-y += installwizard/
obj-y += guide/

obj-$(CONFIG_APUI_SET_PRISM_HOMEMENU) += home/
obj-y += product/
obj-y += keyboard/
obj-y += lifecycle/
obj-y += live/
obj-$(CONFIG_MW_MULTIMEDIA) += media/
obj-y += menu/
obj-y += popup/
obj-y += prodetail/
obj-$(CONFIG_MW_MM_PVR) += recordings/
obj-y += schedule/
obj-y += subtitle/
obj-y += system/
obj-y += teletext/
obj-$(CONFIG_APUI_SET_SMARTSEARCH_UI_DISPLAY) += smartsearch/
ifeq ($(filter $(CONFIG_MW_MM_PVR) $(CONFIG_MW_MULTIMEDIA),y),y)
obj-y += player/
endif
obj-y += keywordsearch/

# Additional include path
ccflags-y += -I../../../../../../make/include

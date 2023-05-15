obj-y += prism.o
obj-y += volume/
obj-y += keyboard/
obj-y += hdmi/
obj-y += startlogo/
obj-y += screensaver/
obj-y += radiostill/
obj-y += teletext/
obj-y += subtitle/
obj-y += front/
obj-y += msgbox/
obj-$(CONFIG_APWEB_NATIVE_MEM_INFO)		+= meminfo/
obj-$(CONFIG_CAS_CONAX_FINGERPRINT)		+= fingerprint/

# Additional include path
ccflags-y += -I../../../../make/include
ifeq ($(CONFIG_NETFLIX_APP),y)
ccflags-y += -I$(PWD)/../appl/netflix/netflixmanager
endif


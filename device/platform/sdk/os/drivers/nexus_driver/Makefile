CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld

MOD_NAME = nexus_driver
TARGET_LIB_PATH  = $(TARGET_DIR)/lib/modules/

obj-m := nexus_driver.o

clean:
	rm -f ./*.o
	rm -f ./.*.{cmd,flags,d}
	rm -f ./*.{o,ko,mod.{o,c}}

install:
	test -d $(TARGET_LIB_PATH) || mkdir $(TARGET_LIB_PATH)
	sudo install -m 644 $(MOD_NAME).ko $(TARGET_LIB_PATH)

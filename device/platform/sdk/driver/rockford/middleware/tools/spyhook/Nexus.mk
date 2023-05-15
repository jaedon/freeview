
$(info *** Making GPU Monitor Hook ***)

all: copy_to_bin

ifndef B_REFSW_ARCH
B_REFSW_ARCH = mipsel-linux-uclibc
endif

ifeq ($(filter ${B_REFSW_ARCH}, mips-linux mips-uclibc mips-linux-uclibc), ${B_REFSW_ARCH})
# BIG ENDIAN
CFLAGS += -DBSTD_CPU_ENDIAN=BSTD_ENDIAN_BIG -DBIG_ENDIAN_CPU
else
# LITTLE ENDIAN
CFLAGS += -DBSTD_CPU_ENDIAN=BSTD_ENDIAN_LITTLE -DLITTLE_ENDIAN_CPU
endif

NEXUS_TOP ?= ../../../../nexus
ROCKFORD_TOP ?= $(NEXUS_TOP)/../rockford

V3D_DIR ?= $(ROCKFORD_TOP)/middleware/v3d
V3D_PLATFORM_DIR ?= $(ROCKFORD_TOP)/middleware/platform

CFLAGS += -I. -I$(V3D_DIR)/interface/khronos/include -I$(V3D_DIR)
## CAUTION: Using higher optimsation levels causes a SEGV when getting state
#CFLAGS += -O0 -fPIC -DPIC -fvisibility=hidden
CFLAGS += -O0 -fPIC -DPIC

LDFLAGS = -shared --export-dynamic

.PHONY: copy_to_bin
copy_to_bin : lib/libgpumonitor.so
	$(info Copying libgpumonitor.so to $(NEXUS_TOP)/bin)
	@cp lib/libgpumonitor.so $(NEXUS_TOP)/bin

lib/libgpumonitor.so : obj/spyhook.o obj/remote.o obj/archive.o obj/packet.o obj/platform.o
	@mkdir -p lib
	$(info Linking $@)
	@$(B_REFSW_ARCH)-g++ $(LDFLAGS) -o $@ $^

obj/%.o: %.cpp
	@mkdir -p obj
	$(info Compiling $<)
	@$(B_REFSW_ARCH)-g++ -c $(CFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -rf lib obj

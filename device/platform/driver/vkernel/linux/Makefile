#
so-y := vk_clock.o
so-y += vk_dbgmem.o
so-y += vk_event.o
so-y += vk_mem.o
so-y += vk_msg.o
so-y += vk_sem.o
so-y += vk_system.o
so-y += vk_task.o
so-y += vk_test.o
so-y += vk_timer.o
so-y += vk_mutex.o
so-y += vk_ipc.o
so-y += vk_socket.o
so-y += vk_rpc.o
so-y += vk_intr.o
so-y += vk_sig.o
so-y += vkernel.o
so-y += vk_p_common_api.o
so-y += vk_ftp.o

ifeq ($(CONFIG_PRODUCT_DI_DEBUG_LOG),y)
ifeq ($(CONFIG_ARM),y)
HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/driver/vkernel/linux/unwind_arm_$(shell echo $(CONFIG_TOOLCHAIN_PATH) | cut -f 2 -d "-")/include
extra-lib-y := unwind_arm_$(shell echo $(CONFIG_TOOLCHAIN_PATH) | cut -f 2 -d "-")/lib/libunwind.lib
ifeq ($(CONFIG_BCM_DOT_CONFIG),y)
extra-lib-y += unwind_arm_$(shell echo $(CONFIG_TOOLCHAIN_PATH) | cut -f 2 -d "-")/lib/libunwind-arm.lib
endif
else
HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/driver/vkernel/linux/unwind/include
extra-lib-y := unwind/lib/libunwind.lib
ifeq ($(CONFIG_BCM_DOT_CONFIG),y)
extra-lib-y += unwind/lib/libunwind-mips.lib
endif
endif
endif
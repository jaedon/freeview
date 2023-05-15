# Copyright (C) 2008 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

REFSW_PATH :=vendor/broadcom/bcm${BCHP_CHIP}/brcm_nexus

LOCAL_PATH := $(call my-dir)

include $(REFSW_PATH)/bin/include/platform_app.inc

$(info === Building V3D driver ===)

GLES_CFLAGS = \
	-I. \
	-DKHAPI="__attribute__((visibility(\"default\")))" \
	-DSPAPI="__attribute__((visibility(\"default\")))" \
	-DFASTMEM_USE_MALLOC \
	-DASSERT_ON_ALLOC_FAIL \
	-DV3D_LEAN \
	-DMUST_SET_ALPHA \
	-DREMOTE_API_LOGGING \
	-DTIMELINE_EVENT_LOGGING \
	-DBCG_VC4_FAST_ATOMICS \
	-DBCG_MULTI_THREADED \
	-UHAVE_SYS_UIO_H

# Add any customer specific cflags from the command line
GLES_CFLAGS += $(V3D_EXTRA_CFLAGS)

#   Add this to GLES_CFLAGS to compile memory defragmentation into code.
#   There is a performance penalty, but this can be useful for code that runs
#   for a very long time.
#
#   -DBCG_VC4_DEFRAG

include $(CLEAR_VARS)
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)

LOCAL_SHARED_LIBRARIES := liblog libcutils libhardware libnexus libdl libnexuseglclient

LOCAL_C_INCLUDES += $(LOCAL_PATH)/interface/vcos/pthreads/ \
		$(LOCAL_PATH)/interface/vcos/ \
		$(LOCAL_PATH)/interface/khronos/include \
		$(REFSW_PATH)/bin/include \
		vendor/broadcom/bcm${BCHP_CHIP}/libgralloc \
		$(LOCAL_PATH)/default_android_pl \
		frameworks/base/include/ui \
		hardware/libhardware/include

LOCAL_SRC_FILES := interface/khronos/common/khrn_int_util.c \
	interface/khronos/common/khrn_int_parallel.c \
	interface/khronos/common/khrn_int_image.c \
	interface/khronos/common/khrn_int_hash.c \
	interface/khronos/common/khrn_client_vector.c \
	interface/khronos/common/khrn_client_pointermap.c \
	interface/khronos/common/khrn_client_global_image_map.c \
	interface/khronos/common/khrn_client_cache.c \
	interface/khronos/common/khrn_client.c \
	interface/khronos/common/khrn_options.c \
	interface/khronos/common/khrn_api_interposer.c \
	interface/khronos/common/abstract/khrn_client_platform_abstract.c \
	interface/khronos/egl/egl_client_surface.c \
	interface/khronos/egl/egl_client_context.c \
	interface/khronos/egl/egl_client_config.c \
	interface/khronos/egl/egl_client.c \
	interface/khronos/egl/egl_client_get_proc.c \
	interface/khronos/ext/egl_android_ext.c \
	interface/khronos/ext/gl_oes_framebuffer_object.c \
	interface/khronos/ext/gl_oes_egl_image_client.c \
	interface/khronos/ext/egl_khr_sync_client.c \
	interface/khronos/ext/egl_khr_lock_surface_client.c \
	interface/khronos/ext/egl_khr_image_client.c \
	interface/khronos/ext/egl_brcm_global_image_client.c \
	interface/khronos/ext/egl_brcm_flush_client.c \
	interface/khronos/ext/egl_brcm_driver_monitor_client.c \
	interface/khronos/ext/egl_brcm_image_update_control_client.c \
	interface/khronos/ext/ext_gl_oes_query_matrix.c \
	interface/khronos/ext/ext_gl_oes_draw_texture.c \
	interface/khronos/ext/ext_gl_debug_marker.c \
	interface/khronos/glxx/glxx_client.c \
	interface/khronos/vg/vg_int_mat3x3.c \
	interface/khronos/vg/vg_client.c \
	middleware/khronos/common/2708/khrn_tfconvert_4.c \
	middleware/khronos/common/2708/khrn_worker_4.c \
	middleware/khronos/common/2708/khrn_render_state_4.c \
	middleware/khronos/common/2708/khrn_pool_4.c \
	middleware/khronos/common/2708/khrn_nmem_4.c \
	middleware/khronos/common/2708/khrn_interlock_4.c \
	middleware/khronos/common/2708/khrn_image_4.c \
	middleware/khronos/common/2708/khrn_hw_4.c \
	middleware/khronos/common/2708/khrn_fmem_4.c \
	middleware/khronos/common/2708/khrn_copy_buffer_4.c \
	middleware/khronos/common/2708/khrn_prod_4.c \
	middleware/khronos/common/2708/khrn_cle_debug_4.c \
	middleware/khronos/common/khrn_tformat.c \
	middleware/khronos/common/khrn_misc.c \
	middleware/khronos/common/khrn_math.c \
	middleware/khronos/common/khrn_map_64.c \
	middleware/khronos/common/khrn_map.c \
	middleware/khronos/common/khrn_llat.c \
	middleware/khronos/common/khrn_interlock.c \
	middleware/khronos/common/khrn_image.c \
	middleware/khronos/common/khrn_fleaky_map.c \
	middleware/khronos/common/khrn_color.c \
	middleware/khronos/common/khrn_bf_dummy.c \
	middleware/khronos/common/khrn_workarounds.c \
	middleware/khronos/egl/abstract_server/egl_platform_abstractserver.c \
	middleware/khronos/egl/abstract_server/egl_platform_abstractpixmap.c \
	middleware/khronos/egl/egl_disp.c \
	middleware/khronos/egl/egl_server.c \
	middleware/khronos/ext/gl_oes_query_matrix.c \
	middleware/khronos/ext/gl_oes_egl_image.c \
	middleware/khronos/ext/gl_oes_draw_texture.c \
	middleware/khronos/ext/egl_brcm_driver_monitor.c \
	middleware/khronos/ext/egl_brcm_global_image.c \
	middleware/khronos/ext/egl_brcm_global_image_id.c \
	middleware/khronos/ext/egl_khr_image.c \
	middleware/khronos/ext/egl_brcm_image_update_control.c \
	middleware/khronos/gl11/2708/gl11_shader_4.c \
	middleware/khronos/gl11/2708/gl11_shadercache_4.c \
	middleware/khronos/gl11/2708/gl11_support_4.c \
	middleware/khronos/gl11/gl11_matrix.c \
	middleware/khronos/gl11/gl11_server.c \
	middleware/khronos/gl20/2708/gl20_shader_4.c \
	middleware/khronos/gl20/2708/gl20_support_4.c \
	middleware/khronos/gl20/gl20_shader.c \
	middleware/khronos/gl20/gl20_server.c \
	middleware/khronos/gl20/gl20_program.c \
	middleware/khronos/glsl/2708/glsl_allocator_4.c \
	middleware/khronos/glsl/2708/glsl_fpu_4.c \
	middleware/khronos/glsl/2708/glsl_qdisasm_4.c \
	middleware/khronos/glsl/2708/glsl_scheduler_4.c \
	middleware/khronos/glsl/2708/glsl_verify_4.c \
	middleware/khronos/glsl/prepro/glsl_prepro_directive.c \
	middleware/khronos/glsl/prepro/glsl_prepro_eval.c \
	middleware/khronos/glsl/prepro/glsl_prepro_expand.c \
	middleware/khronos/glsl/prepro/glsl_prepro_macro.c \
	middleware/khronos/glsl/prepro/glsl_prepro_token.c \
	middleware/khronos/glsl/glsl_symbols.c \
	middleware/khronos/glsl/glsl_stringbuilder.c \
	middleware/khronos/glsl/glsl_mendenhall.c \
	middleware/khronos/glsl/glsl_map.c \
	middleware/khronos/glsl/glsl_intern.c \
	middleware/khronos/glsl/glsl_header.c \
	middleware/khronos/glsl/glsl_globals.c \
	middleware/khronos/glsl/glsl_fastmem.c \
	middleware/khronos/glsl/glsl_errors.c \
	middleware/khronos/glsl/glsl_extensions.c \
	middleware/khronos/glsl/glsl_dataflow_visitor.c \
	middleware/khronos/glsl/glsl_dataflow_print.c \
	middleware/khronos/glsl/glsl_dataflow.c \
	middleware/khronos/glsl/glsl_const_functions.c \
	middleware/khronos/glsl/glsl_compiler.c \
	middleware/khronos/glsl/glsl_builders.c \
	middleware/khronos/glsl/glsl_ast_visitor.c \
	middleware/khronos/glsl/glsl_ast.c \
	middleware/khronos/glsl/glsl_ast_print.c \
	middleware/khronos/glsl/lex.yy.c \
	middleware/khronos/glsl/y.tab.c \
	middleware/khronos/glxx/2708/glxx_shader_4.c \
	middleware/khronos/glxx/2708/glxx_inner_4.c \
	middleware/khronos/glxx/2708/glxx_hw_4.c \
	middleware/khronos/glxx/2708/glxx_attr_sort_4.c \
	middleware/khronos/glxx/glxx_texture.c \
	middleware/khronos/glxx/glxx_shared.c \
	middleware/khronos/glxx/glxx_renderbuffer.c \
	middleware/khronos/glxx/glxx_framebuffer.c \
	middleware/khronos/glxx/glxx_buffer.c \
	middleware/khronos/glxx/glxx_server.c \
	middleware/khronos/glxx/glxx_server_cr.c \
	middleware/khronos/vg/2708/vg_tess_4.c \
	middleware/khronos/vg/2708/vg_shader_md_4.c \
	middleware/khronos/vg/2708/vg_shader_fd_4.c \
	middleware/khronos/vg/2708/vg_segment_lengths_4.c \
	middleware/khronos/vg/2708/vg_path_4.c \
	middleware/khronos/vg/2708/vg_hw_4.c \
	middleware/khronos/vg/2708/vg_bf_4.c \
	middleware/khronos/vg/vg_tess.c \
	middleware/khronos/vg/vg_set.c \
	middleware/khronos/vg/vg_server.c \
	middleware/khronos/vg/vg_segment_lengths.c \
	middleware/khronos/vg/vg_scissor.c \
	middleware/khronos/vg/vg_ramp.c \
	middleware/khronos/vg/vg_path.c \
	middleware/khronos/vg/vg_paint.c \
	middleware/khronos/vg/vg_mask_layer.c \
	middleware/khronos/vg/vg_image.c \
	middleware/khronos/vg/vg_hw.c \
	middleware/khronos/vg/vg_font.c \
	middleware/khronos/vg/vg_bf.c \
	vcfw/rtos/abstract/rtos_abstract_mem.c \
	interface/vcos/pthreads/vcos_pthreads.c \
	interface/vcos/generic/vcos_mem_from_malloc.c \
	interface/vcos/generic/vcos_generic_event_flags.c \
	interface/vcos/generic/vcos_generic_named_sem.c \
	interface/vcos/generic/vcos_abort.c \
	android_platform_library_loader.c \
	default_android_pl/default_android.c \
	default_android_pl/display_android.c \
	default_android_pl/message_queue.c \
	nexus_pl/memory_nexus.c \
	nexus_pl/hardware_nexus.c

LOCAL_MODULE := libGLES_nexus
LOCAL_MODULE_TAGS:= optional
LOCAL_CFLAGS:= $(NEXUS_CFLAGS) $(GLES_CFLAGS) -DANDROID
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)

include $(BUILD_SHARED_LIBRARY)

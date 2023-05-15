#!/usr/bin/perl
#############################################################################
#
#		Copyright (c) 2004-2008, Broadcom Corporation.
#		All rights reserved.
#		Confidential Property of Broadcom Corporation.
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bapi_callbacks.pm $
# $brcm_Revision: 15 $
# $brcm_Date: 8/13/08 6:06p $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/api/build/proxy/bapi_callbacks.pm $
# 
# 15   8/13/08 6:06p mward
# PR32047: PCM capture API. Merge to main.
# 
# PR32047/1   5/16/08 6:39p mward
# PR32047: Initial PCM capture API implementation, ignores display.
# 
# 14   10/15/07 10:50a mward
# PR35863: Missing }
# 
# 13   10/11/07 10:02a dlwin
# PR 35927: Coverity Defect ID:3029 CHECKED_RETURN
# 
# 12   6/20/07 5:31p jgarrett
# PR 26474: Adding boutput_hdmi_status and hdcp_status_callback
# 
# 11   6/20/07 11:54a erickson
# PR32381: added b_unregister_callback and
# BSETTOP_IOCTL_free_handle_callbacks. when a handle is closed, its
# callbacks are freed in user and kernel space.
# 
# 10   6/11/07 10:42a erickson
# PR31914: update proxy with bdecode_settings callbacks
# 
# 9   2/23/07 4:00p erickson
# PR26790: don't recreate callbacks for outparams
# 
# 8   1/5/07 11:52a erickson
# PR26790: for proxy-layer callback conversion, we must convert back on
# callback out params.
# 
# 7   12/15/06 10:01a gmohile
# PR 26498: pass in settop api handle to register callback
# 
# 6   9/14/06 5:40p jgarrett
# PR 24052: Adding NULL callback check to the driver side
# 
# 5   6/30/06 5:25p jgarrett
# PR 20139: Removing compiler warning from callbacks
# 
# 4   6/9/06 3:19p jgarrett
# PR 21946: Adding new callbacks for hdmi_disconnect,
# decode_still_picture
# 
# 3   5/12/05 7:11p mphillip
# PR12953: Register hotplug callback with the kernel proxy layer for
# building in kernel-mode
# 
# 2   4/28/05 4:49p mphillip
# PR14721: Update the callbacks to handle kernel-mode builds
# 
# 1   2/7/05 6:55p dlwin
# Merge down for release 2005_REFSW_MERGETOMAIN:
# 
# Irvine_BSEAVSW_Devel/2   7/23/04 2:41p erickson
# PR11771: updated proxy layer for settop api changes
# 
# Irvine_BSEAVSW_Devel/1   7/9/04 1:38p erickson
# PR11771: rebuilt proxy layer for settop api dataflow redesign, and
# handle all callbacks with new perl module
#
#############################################################################
use strict;

package bapi_callbacks;

#
# list of all callbacks in the settop api
# 
# must be kept in sync with bsettop_callback_ioctl.h and bsettop_callback_mgr.c
# do this to double check: cd BSEAV/api/include; grep bsettop_callback *.h|wc
#
# definition
# 0: function name where callback is passed in
# 1: enum for callback (see bsettop_callback_ioctl.h)
# 2: name of settop api associated with callback (used for unregistering callback upon close)
# 3: name of structure which contains the callback (only needed for register)
# 4: name of structure member for the callback (only needed for register)
# 5: type of operation: "in" or "out"
# 6: unregister function (not used for "out" functions). In general, this should be the close function, not the stop. If you start/stop, the callback
#    handle will be overwritten. It's possible to start, but have start fail, then close. The callback must be freed on close.
#
my @callbacks = (
["bplaypump_start", "ioctl_settopapi_bplaypump_read_callback",
	"playpump", "params", "read_callback", "in", "bplaypump_close"],
["brecpump_start", "ioctl_settopapi_brecpump_data_write_callback",
	"recpump", "params", "data_write_callback", "in", "brecpump_close"],
["brecpump_start", "ioctl_settopapi_brecpump_index_write_callback",
	"recpump", "params", "index_write_callback", "in", "brecpump_close"],
["brecpump_start", "ioctl_settopapi_brecpump_overflow_callback",
	"recpump", "params", "overflow", "in", "brecpump_close"],
["buser_input_set_settings", "ioctl_settopapi_buser_input_data_ready_callback",
	"ui", "settings", "data_ready_callback", "in", "buser_input_close"],
["bmessage_start", "ioctl_settopapi_bmessage_data_ready_callback",
	"message_stream", "params", "data_ready_callback", "in", "bmessage_close"],
["bmessage_start", "ioctl_settopapi_bmessage_overflow_callback",
	"message_stream", "params", "overflow", "in", "bmessage_close"],
["bgraphics_set", "ioctl_settopapi_bgraphics_operation_complete_callback",
	"graphics", "settings", "operation_complete_callback", "in", "bgraphics_close"],
["bpcm_play_start", "ioctl_settopapi_bpcm_play_callback",
	"pcmplay", "settings", "callback", "in", "bpcm_play_close"],
["bstream_vbi_set_settings", "ioctl_settopapi_bvbi_cc_data_ready_callback",
	"stream", "settings", "cc_data_ready_callback", "in", "bstream_close"], # NOTE: This won't unregister for playback streams, but there's a fixed number, so the leak is contained.
["btuner_tune_qam", "ioctl_settopapi_btuner_qam_params_lock_callback",
	"tuner", "params", "lock_callback", "in", "btuner_close"],
["btuner_tune_oob", "ioctl_settopapi_btuner_oob_params_lock_callback",
	"tuner", "params", "lock_callback", "in", "btuner_close"],
["btuner_tune_sds", "ioctl_settopapi_btuner_sds_params_lock_callback",
	"tuner", "params", "lock_callback", "in", "btuner_close"],
["btuner_tune_vsb", "ioctl_settopapi_btuner_vsb_params_lock_callback",
	"tuner", "params", "lock_callback", "in", "btuner_close"],
["boutput_hdmi_set", "ioctl_settopapi_boutput_dvi_settings_hdmi_hotplug_callback",
	"dvi", "settings", "hdmi_hotplug_callback", "in", ""],  # NOTE: There's no way to unregister this, but fortunately boutput_dvi_open returns a static handle, so the leak is contained.
["boutput_hdmi_set", "ioctl_settopapi_boutput_dvi_settings_hdmi_disconnect_callback",
	"dvi", "settings", "hdmi_disconnect_callback", "in", ""],  # NOTE: There's no way to unregister this, but fortunately boutput_dvi_open returns a static handle, so the leak is contained.
["boutput_hdmi_set", "ioctl_settopapi_boutput_dvi_settings_hdmi_hdcp_status_callback",
	"dvi", "settings", "hdmi_hdcp_status_callback", "in", ""],  # NOTE: There's no way to unregister this, but fortunately boutput_dvi_open returns a static handle, so the leak is contained.
["boutput_hdmi_get", "ioctl_settopapi_boutput_dvi_settings_hdmi_hotplug_callback",
	"dvi", "settings", "hdmi_hotplug_callback", "out", ""],
["boutput_hdmi_get", "ioctl_settopapi_boutput_dvi_settings_hdmi_disconnect_callback",
	"dvi", "settings", "hdmi_disconnect_callback", "out", ""],
["boutput_hdmi_get", "ioctl_settopapi_boutput_dvi_settings_hdmi_hdcp_status_callback",
	"dvi", "settings", "hdmi_hdcp_status_callback", "out", ""],
["bdecode_set", "ioctl_settopapi_bdecode_settings_still_picture_done",
	"decode", "settings", "still_picture_done", "in", "bdecode_close"],
["bdecode_get", "ioctl_settopapi_bdecode_settings_still_picture_done",
	"decode", "settings", "still_picture_done", "out", ""],
["bdecode_set", "ioctl_settopapi_bdecode_settings_video_source_changed",
	"decode", "settings", "video.source_changed", "in", "bdecode_close"],
["bdecode_get", "ioctl_settopapi_bdecode_settings_video_source_changed",
	"decode", "settings", "video.source_changed", "out", ""],
["bdecode_set", "ioctl_settopapi_bdecode_settings_audio_source_changed",
	"decode", "settings", "audio.source_changed", "in", "bdecode_close"],
["bdecode_get", "ioctl_settopapi_bdecode_settings_audio_source_changed",
	"decode", "settings", "audio.source_changed", "out", ""],
["bpcm_capture_start", "ioctl_settopapi_bpcm_capture_callback",
	"pcmcapture", "settings", "callback", "in", "bpcm_capture_close"]
);

sub build_user_callback {
	my $func = shift;
	my $in_param = shift; # boolean
	my $cb;
	my $text = "";

	for $cb (@callbacks) {
		if ($cb->[0] eq $func->{FUNCNAME}) {
			my $param = "$cb->[3]\-\>$cb->[4]";
			my $context = "(void*)$cb->[3]\-\>callback_context";
			if ($cb->[5] eq "in" && $in_param == 1) {
				$text .= "  b_register_callback($cb->[1], (void*)$cb->[2], $param, $context);\n";
			}
			elsif ($cb->[5] eq "out" && $in_param == 0) {
				$text .= "  $param = b_lookup_callback($cb->[1], (void*)$cb->[2]);\n";
			}
		}
		elsif ($cb->[6] eq $func->{FUNCNAME} && $in_param == 1) {
			$text .= "  b_unregister_callback($cb->[1], (void*)$cb->[2]);\n";
			# tell driver to close all callbacks for this handle
			$text .= "  rc = ioctl(g_ioctl_fd, BSETTOP_IOCTL_free_handle_callbacks, (void*)$cb->[2]);\n";
			$text .= "  if (rc) {BSETTOP_ERROR(berr_external_error);}\n";
		}
	}
	return $text;
}

sub build_kernel_callback {
	my $func = shift;
	my $cb;
	my $text = "";

	for $cb (@callbacks) {
		if ($cb->[0] eq $func->{FUNCNAME} && $cb->[5] eq "in") {
			my $param = "$cb->[3].$cb->[4]";
			my $handle = "st.$cb->[2]";
			my $enumtype = "$cb->[1]";
			my $context = "$cb->[3].callback_context";
			$text .= "  if ( $param ) { $param = b_proxy_create_callback($enumtype, $handle, $context); }\n";
		}
	}
	return $text;
}

1;

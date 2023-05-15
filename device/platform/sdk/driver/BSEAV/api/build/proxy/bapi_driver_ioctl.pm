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
# $brcm_Workfile: bapi_driver_ioctl.pm $
# $brcm_Revision: 7 $
# $brcm_Date: 1/23/08 3:09p $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/api/build/proxy/bapi_driver_ioctl.pm $
# 
# 7   1/23/08 3:09p mphillip
# PR38923: Mosaic decode handles also require managing
# 
# 6   11/21/07 3:43p gmohile
# PR 36570 : fix array sizes
# 
# 5   1/10/07 10:34a erickson
# PR26351: bdecode_acquire_capture_buffer needs to have its bsurface_t
# destroyed
# 
# 4   3/10/06 4:26p vle
# PR 20136: add pcmplay to the list of handles
# 
# 3   11/2/05 11:24a vsilyaev
# PR 17883: Don't shutdown SettopApi stack if signal is pending
# 
# 2   9/12/05 12:35p mphillip
# PR16870: Fix kernel-mode macrovision support
# 
# 1   2/7/05 6:55p dlwin
# Merge down for release 2005_REFSW_MERGETOMAIN:
# 
# Irvine_BSEAVSW_Devel/15   12/2/04 4:45p vsilyaev
# PR 13351: Added code to pass through NULL pointers as arguments.
# 
# Irvine_BSEAVSW_Devel/14   8/31/04 1:36p erickson
# PR11869: buser_input must also be a managed handle
# 
# Irvine_BSEAVSW_Devel/13   8/12/04 9:28a erickson
# PR11135: reworked bhandle_mgr autogen code to be data driven and close
# down in correct order
# 
# Irvine_BSEAVSW_Devel/12   7/23/04 2:41p erickson
# PR11771: updated proxy layer for settop api changes
# 
# Irvine_BSEAVSW_Devel/11   7/9/04 1:38p erickson
# PR11771: rebuilt proxy layer for settop api dataflow redesign, and
# handle all callbacks with new perl module
# 
# Irvine_BSEAVSW_Devel/10   6/29/04 11:27a erickson
# PR11135: handle overflow callback
# 
# Irvine_BSEAVSW_Devel/9   6/24/04 2:37p erickson
# PR11135: adapted proxy layer to bsettop_callback
# 
# Irvine_BSEAVSW_Devel/8   6/24/04 10:24a erickson
# PR11135: added "don't modify" headers
# 
# Irvine_BSEAVSW_Devel/7   6/21/04 4:38p erickson
# PR11135: use double curly braces to prevent stack overflow in the
# switch statement
# 
# Irvine_BSEAVSW_Devel/6   6/18/04 12:33p erickson
# PR11135: destroy callbacks when closing
# 
# Irvine_BSEAVSW_Devel/5   6/17/04 9:37a erickson
# PR11135: recpump callback enabled
#
# Irvine_BSEAVSW_Devel/4   6/16/04 5:15p erickson
# PR11135: added callback support
#
# Irvine_BSEAVSW_Devel/3   6/1/04 11:28a erickson
# PR11135: added support for strings (const char *)
#
# Irvine_BSEAVSW_Devel/2   5/25/04 11:46a erickson
# PR11135: playback must be managed
#
# Irvine_BSEAVSW_Devel/1   5/17/04 11:13a erickson
# PR11135: added initial linux proxy impl
#
#############################################################################
use strict;
use bapi_callbacks;

package bapi_driver_ioctl;

# For managed handles, the open function must return a handle.
# The close function must take only the handle as a parameter.
# The order of this list determines the order in which handles will be autoclosed
# by bhandle_mgr. It will close from the top down.
my @managed_handles = (
["btuner_tune_rf", "bstream_close", "stream"],
["btuner_tune_linein", "bstream_close", "stream"],
["bstream_open", "bstream_close", "stream"],
["bstream_open_child", "bstream_close", "stream"],
["btuner_open", "btuner_close", "tuner"],
["bplaypump_open", "bplaypump_close", "playpump"],
["brecpump_open", "brecpump_close", "recpump"],
["bdecode_window_open", "bdecode_window_close", "window"],
["bdecode_window_open_mosaic", "bdecode_window_close", "window"],
["bdecode_window_clone", "bdecode_window_close", "window"],
["bdecode_open", "bdecode_close", "decode"],
["bdecode_open_mosaic", "bdecode_close", "decode"],
["buser_input_open", "buser_input_close", "ui"],
["bsurface_create", "bsurface_destroy", "surface"],
["bdecode_acquire_capture_buffer", "bsurface_destroy", "surface"],
["bgraphics_open", "bgraphics_close", "graphics"],
["bdisplay_open", "bdisplay_close", "display"],
["bpcm_play_open", "bpcm_play_close", "pcmplay"]
);

sub build_addhandle {
	my $func = shift;
	my $mh;
	my $text = "";

	for $mh (@managed_handles) {
		if ($mh->[0] eq $func->{FUNCNAME}) {
			my $close_func = $mh->[1];
			$text .= "  bhandle_mgr_add(file->private_data, st.__retval, (settopapi_close_func)$close_func, \"$close_func\");\n";
			last;
		}
	}
	return $text;
}

sub build_removehandle {
	my $func = shift;
	my $mh;
	my $text = "";

	for $mh (@managed_handles) {
		if ($mh->[1] eq $func->{FUNCNAME}) {
			my $close_func = $mh->[1];
			my $handle = $mh->[2];
			$text .= "  bhandle_mgr_remove(file->private_data, st.$handle, \"$close_func\");\n";
			last;
		}
	}
	return $text;
}

sub build_closelist
{
	my ($filename, @funcs) = @_;
	my $mh;
	
	open FILE, ">$filename" || die "Cannot open $filename";

	print FILE "/*********************************\n";
	print FILE "*\n";
	print FILE "* This file is autogenerated. Do not modify.\n";
	print FILE "*\n";
	print FILE "*********************************/\n";

	print FILE "#include \"bhandle_mgr.h\"\n";
	print FILE "#include \"bsettop.h\"\n";
	print FILE "static settopapi_close_func g_closefuncs[] = {\n";
	for $mh (@managed_handles) {
		my $close_func = $mh->[1];
		print FILE "  (settopapi_close_func) $close_func,\n";
	}
	print FILE "  0\n";
	print FILE "};\n";
	close FILE;
}

sub parse
{
	my ($filename, @funcs) = @_;
	my $func;
	open FILE, ">$filename" || die "Cannot open $filename";

	print FILE "/*********************************\n";
	print FILE "*\n";
	print FILE "* This file is autogenerated. Do not modify.\n";
	print FILE "*\n";
	print FILE "*********************************/\n";

	#
	# This function only generates the case statements. The switch statement
	# and the rest of the function context can be found in BSEAV/linux/driver/97038/bcmdriver.c.
	#
	for $func (@funcs) {
		my $params = $func->{PARAMS};
		my $param;

		print FILE "case BSETTOP_IOCTL_$func->{FUNCNAME}:\n";
		# Using double curly braces causes the stack to be allocated for
		# each case statement instead of for the whole switch statement.
		# This prevents stack overflow in the kernel. If this compiler trick stops
		# working in the future, use a union or put each case statement in a function.
		print FILE "  {{\n";

		if (!$func->{NOSTRUCT}) {
			print FILE "  ioctl_settopapi_struct_$func->{FUNCNAME} st;\n";
			for $param (@$params) {
				if ($param->{ISREF}) {
					if ($param->{ARRAYLENGTH}) {
						if ($param->{ARRAYLENGTH} ne "__maxstringlength") {
							print FILE "  $param->{TYPE} $param->{NAME};\n";
						}
					}
					else {
						print FILE "  $param->{BASETYPE} $param->{NAME};\n";
					}
				}
			}
			if ($func->{FUNCNAME} eq "bdisplay_set") {
				print FILE "  bmacrovision_tables mv_tables;\n";
			}
			print FILE "\n";

			print FILE "  rc = copy_from_user(&st, (void*)arg, sizeof(st));\n";
			print FILE "  if (rc) break;\n";
		}

		# This bracket is necessary in order to dynamically allocate arrays
		# using values in st.
		print FILE "  {\n";

		for $param (@$params) {
			my $arraylength = $param->{ARRAYLENGTH};
			if ($arraylength && !($arraylength eq "__maxstringlength")) {
				$arraylength = "st." . $arraylength;
			}

			if ($param->{ISREF}) {
				if ($param->{ARRAYLENGTH}) {
					if ($param->{ARRAYLENGTH} ne "__maxstringlength") {
						print FILE "  $param->{BASETYPE} $param->{NAME}_array[$arraylength];\n";
						print FILE "  $param->{NAME} = $param->{NAME}_array;\n";
					}
					else {
						print FILE "  $param->{BASETYPE} $param->{NAME}\[$arraylength\];\n";
					}
				}
				if ($param->{INPARAM}) {
					if ($param->{ARRAYLENGTH}) {
						print FILE "  rc = copy_from_user((void*)$param->{NAME}, st.$param->{NAME}, $arraylength*sizeof($param->{BASETYPE}));\n";
						print FILE "  if (rc) break;\n";
					}
					else {
						print FILE "  if (st.$param->{NAME}) {\n";
						print FILE "  rc = copy_from_user((void*)&$param->{NAME}, st.$param->{NAME}, sizeof($param->{NAME}));\n";
						print FILE "  if (rc) break;\n";
						print FILE "  }\n";
					}
				}
			}
		}

		if ($func->{FUNCNAME} eq "bdisplay_set") {
			print FILE "  if (st.settings && settings.macrovision_tables) {\n";
			print FILE "    rc = copy_from_user(&mv_tables, (void*)settings.macrovision_tables, sizeof(mv_tables));\n";
			print FILE "    if (rc) break;\n";
			print FILE "    settings.macrovision_tables = &mv_tables;\n";
			print FILE "  }\n";
		}

		# switch user-mode callback for a kernel-mode callback
		my $callback_text = bapi_callbacks::build_kernel_callback($func);
		if ($callback_text) {
			printf FILE $callback_text;
		}

		print FILE "\n  /* Make the function call */\n";
		if ($func->{RETTYPE} =~ /\*$/) {
			print FILE "  $func->{RETTYPE} __retval = $func->{FUNCNAME}(";
		}
		elsif ($func->{RETTYPE} eq "void") {
			print FILE "  $func->{FUNCNAME}(";
		}
		else {
			print FILE "  st.__retval = $func->{FUNCNAME}(";
		}
		my $i;
		for $param (@$params) {
			if ($param->{ISREF}) {
				if ($param->{ARRAYLENGTH}) {
					print FILE "$param->{NAME}";
				}
				else {
					print FILE "st.$param->{NAME} ? &$param->{NAME}:NULL";
				}
			}
			else {
				print FILE "st.$param->{NAME}";
			}
			if ($i++ != $#$params) {
				print FILE ",";
			}
		}
		print FILE ");\n";
		if ($func->{RETTYPE} =~ /\*$/) {
			print FILE "  if (__retval) {\n";
			print FILE "    st.__retval = *__retval;\n";
			print FILE "  }\n";
			print FILE "  else {\n";
			print FILE "    rc = -EIO; break;\n";
			print FILE "  }\n";
		}

		#
		# interface with the bhandle_mgr
		#
		my $addition = build_addhandle($func);
		if ($addition) {
			print FILE $addition;
		}
		my $addition = build_removehandle($func);
		if ($addition) {
			print FILE $addition;
		}

		if ($func->{RETTYPE} eq "void") {
			print FILE "  rc = 0;\n\n";
		}
		else {
			# Don't fail the ioctl if the settop api call failed. The
			# app will need to detect the error.
			print FILE "  rc = copy_to_user((void*)arg, &st, sizeof(st));\n";
			print FILE "  if (rc) {rc = -EFAULT; break;}\n";
		}

		for $param (@$params) {
			my $arraylength = $param->{ARRAYLENGTH};
			if ($arraylength && !($arraylength eq "__maxstringlength")) {
				$arraylength = "st." . $arraylength;
			}

			if ($param->{ISREF} && !$param->{INPARAM}) {
				print FILE "  if (st.$param->{NAME}) {\n";
				if ($param->{ARRAYLENGTH}) {
					print FILE "    rc = copy_to_user(st.$param->{NAME}, $param->{NAME}, $arraylength*sizeof($param->{BASETYPE}));\n";
				}
				else {
					print FILE "    rc = copy_to_user(st.$param->{NAME}, &$param->{NAME}, sizeof($param->{NAME}));\n";
				}
				print FILE "    if (rc) {rc = -EFAULT; break;}\n";
				print FILE "  }\n";
			}
		}

		print FILE "  }\n";
		print FILE "  }}\n";
		print FILE "  break;\n\n";
	}
	close FILE;
}

1;


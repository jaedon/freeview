############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bape_raaga.inc $
# $brcm_Revision: Hydra_Software_Devel/36 $
# $brcm_Date: 10/7/12 8:19p $
#
# Module Description:
#
# Revision History:
#
# Created: 08/27/2009 by Jon Garrett
#
# $brcm_Log: /magnum/portinginterface/ape/build/bape_raaga.inc $
# 
# Hydra_Software_Devel/36   10/7/12 8:19p jgarrett
# SW7346-914: Merge to main branch
# 
# Hydra_Software_Devel/SW7346-914/1   9/27/12 2:52p jgarrett
# SW7346-914: Adding ancillary data
# 
# Hydra_Software_Devel/35   6/25/12 4:20p jgarrett
# SW7425-2952: Restructuring magnum directories
# 
# Hydra_Software_Devel/34   5/3/12 5:05p jgarrett
# SW7425-2075: Merge to main branch
# 
# Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:48p jgarrett
# SW7425-2075: Adding new FMT interfaces
# 
# Hydra_Software_Devel/33   3/5/12 9:22a jgarrett
# SW7425-2455: Removing ADC and RF Inputs
# 
# Hydra_Software_Devel/32   1/31/12 6:17p jgarrett
# SW7425-2268: Adding initial voice conferencing support
# 
# Hydra_Software_Devel/31   12/7/11 1:25p gskerl
# SW7429-18: Removed chip-specific linking of bape_eq_coeff_priv.c. It's
# now linked unconditionally
# 
# Hydra_Software_Devel/30   12/5/11 7:00p gskerl
# SW7429-18: Removed chip-specific selection of bape_src_dtv_priv.c. Use
# bape_src_priv.c for all chips
# 
# Hydra_Software_Devel/30   12/5/11 7:00p gskerl
# SW7429-18: Removed chip-specific selection of bape_src_dtv_priv.c. Use
# bape_src_priv.c for all chips
# 
# Hydra_Software_Devel/29   11/14/11 3:25p gskerl
# SW7429-18: Merging 7429 changes back to main branch.
# 
# Hydra_Software_Devel/SW7429-18/1   10/21/11 6:29p jgarrett
# SW7429-18: Initial compileable version for 7429
# 
# Hydra_Software_Devel/28   8/30/11 9:37a jgarrett
# SWDTV-6627: Removing EQ coefficient programming code for STB chips
# 
# Hydra_Software_Devel/27   8/26/11 9:50p sgadara
# SWDTV-6627: [35233] Add Equalizer support
# 
# Hydra_Software_Devel/26   8/24/11 11:53a jgarrett
# SW7425-724: Adding RF Audio Encoder
# 
# Hydra_Software_Devel/25   8/12/11 7:01p sgadara
# SWDTV-6627: [35233] Add first cut of Equalizer support
# 
# Hydra_Software_Devel/24   7/15/11 5:51p jgarrett
# SWDTV-6761: Adding StudioSound API
# 
# Hydra_Software_Devel/23   7/8/11 6:38p jgarrett
# SWDTV-6760: Adding I2sMultiOutput
# 
# Hydra_Software_Devel/22   7/8/11 4:30p gskerl
# SW7552-72: Added support for NCO/Mclkgen audio clock sources
# 
# Hydra_Software_Devel/21   5/31/11 6:37p jgarrett
# SW7425-406: Adding initial DSP mixer code
# 
# Hydra_Software_Devel/20   5/27/11 7:48p gskerl
# SW7425-321: Split mixer code into multiple files. Added framework for
# dispatching mixer API calls to multiple mixer types.
# 
# Hydra_Software_Devel/19   5/14/11 11:33a jgarrett
# SWDTV-6763: Adding 3D Surround
# 
# Hydra_Software_Devel/18   5/14/11 11:22a jgarrett
# SWDTV-6762: Adding Audyssey ADV/ABX
# 
# Hydra_Software_Devel/17   5/13/11 6:39p jgarrett
# SW7425-410: Adding AVL
# 
# Hydra_Software_Devel/16   5/11/11 7:04p jgarrett
# SW7422-410: Adding CustomProcessing
# 
# Hydra_Software_Devel/15   5/3/11 6:59p gskerl
# SW7422-354: First attempt at adding support for the audio return
# channel
# 
# Hydra_Software_Devel/14   4/11/11 5:54p jgarrett
# SWDTV-6305: Adding ADC/RF Inputs for DTV
# 
# Hydra_Software_Devel/13   4/6/11 1:23a jgarrett
# SW35330-35: Merge to main branch
# 
# Hydra_Software_Devel/SW35330-35/2   4/5/11 7:13p jgarrett
# SW35330-35: PCM Playback working on 35230
# 
# Hydra_Software_Devel/SW35330-35/1   4/5/11 12:49p jgarrett
# SW35330-35: FMM Abstraction refactoring to support DTV
# 
# Hydra_Software_Devel/12   3/24/11 11:48a jgarrett
# SW7422-146: Fixing kernel mode builds
# 
# Hydra_Software_Devel/11   3/21/11 7:08p jgarrett
# SW7422-356: Adding MuxOutput
# 
# Hydra_Software_Devel/10   3/10/11 7:03p jgarrett
# SW7422-146: Refactored DFIFO code, added support for input capture from
# compressed/multichannel
# 
# Hydra_Software_Devel/9   3/9/11 3:53p jgarrett
# SW7422-146: Adding DFIFO interface
# 
# Hydra_Software_Devel/8   2/28/11 1:28p jgarrett
# SW7422-146: Filter graph reworked to remove mixer dependencies
# 
# Hydra_Software_Devel/7   2/2/11 2:46p jgarrett
# SW7422-146: Adding status
# 
# Hydra_Software_Devel/6   1/27/11 5:47p jgarrett
# SW7422-146: Adding support for multichannel and ac3 user parameters
# 
# Hydra_Software_Devel/5   1/12/11 4:24p jgarrett
# SW7422-146: Adding additional APIs
# 
# Hydra_Software_Devel/4   1/7/11 5:21p jgarrett
# SW7422-146: Adding SPDIF/HDMI input APIs
# 
# Hydra_Software_Devel/3   1/6/11 2:33p jgarrett
# SW7422-146: Adding initial input capture API
# 
# Hydra_Software_Devel/2   12/17/10 3:58p jgarrett
# SW7422-146: Nexus APE integration on 7422
# 
# Hydra_Software_Devel/1   12/16/10 4:04p jgarrett
# SW7422-146: Initial compilable APE for 7422
# 
############################################################

#
# Conditional Options for this Makefile
#

# Define this module
MAGNUM_MODULES += BAPE

# Module Includes
BAPE_INCLUDES += $(MAGNUM)/portinginterface/ape/include \
                 $(MAGNUM)/portinginterface/ape/include/raaga \
                 $(MAGNUM)/portinginterface/ape/src/raaga

# Module Defines
BAPE_DEFINES += BAPE_RAAGA=1

# Module Sources
ifneq ($(BAPE_IMG_ONLY),y)
BAPE_SOURCES += $(MAGNUM)/portinginterface/ape/src/raaga/bape.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_3d_surround.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_arc.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_audyssey_abx.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_audyssey_adv.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_auto_volume_level.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_bf_priv.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_custom_processing.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_dac.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_decoder.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_decoder_ancillary_data.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_decoder_settings_priv.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_decoder_status_priv.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_dolby_digital_reencode.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_dolby_volume.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_dp_priv.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_dsp_utils_priv.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_dummy_output.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_echo_canceller.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_encoder.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_eq_coeff_priv.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_equalizer.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_fmm_isr.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_fmt_priv.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_fs_priv.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_i2s_input.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_i2s_output.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_i2s_multi_output.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_input_capture.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_input_port_priv.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_iop_priv.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_mai_input.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_mai_output.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_mixer.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_mixer_dsp.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_mixer_standard.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_ms_fw.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_mux_output.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_nco.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_output_capture.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_path_priv.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_playback.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_pll.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_reg_priv.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_resources.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_rf_encoder.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_rfmod.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_spdif_input.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_spdif_output.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_src_priv.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_studio_sound.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_tru_surround.c \
                $(MAGNUM)/portinginterface/ape/src/raaga/bape_tru_volume.c

endif


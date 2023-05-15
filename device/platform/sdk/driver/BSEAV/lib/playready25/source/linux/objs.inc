# @@@+++@@@@******************************************************************
# **
# ** Microsoft (r) PlayReady (r)
# ** Copyright (c) Microsoft Corporation. All rights reserved.
# **
# @@@+++@@@@******************************************************************

WMDRM_MD_OBJ = $(LIBDIR)/obj
WMDRM_MD_OBJ_TEST = $(LIBDIR)/obj_test

WMDRM_MD_CORE_OBJS = \
    $(WMDRM_MD_OBJ)/aes128.o \
    $(WMDRM_MD_OBJ)/aesbox.o \
    $(WMDRM_MD_OBJ)/apppolicyimplstub.o \
    $(WMDRM_MD_OBJ)/apppolicyutilstub.o \
    $(WMDRM_MD_OBJ)/apprevocationimplreal.o \
    $(WMDRM_MD_OBJ)/apprevocationutilreal.o \
    $(WMDRM_MD_OBJ)/bignum.o \
    $(WMDRM_MD_OBJ)/bignumnoinline.o \
    $(WMDRM_MD_OBJ)/blackbox.o \
    $(WMDRM_MD_OBJ)/blackboxdevcertsreal.o \
    $(WMDRM_MD_OBJ)/blackboxhal.o \
    $(WMDRM_MD_OBJ)/blackboxinternal.o \
    $(WMDRM_MD_OBJ)/blackboxkeyfileread.o \
    $(WMDRM_MD_OBJ)/blackboxlegacy.o \
    $(WMDRM_MD_OBJ)/blackboxmodules.o \
    $(WMDRM_MD_OBJ)/blackboxprndrx.o \
    $(WMDRM_MD_OBJ)/blackboxprndtx.o \
    $(WMDRM_MD_OBJ)/blackboxslk.o \
    $(WMDRM_MD_OBJ)/certparsercacheimplreal.o \
    $(WMDRM_MD_OBJ)/certparsercacheutilreal.o \
    $(WMDRM_MD_OBJ)/cleanstorereal.o \
    $(WMDRM_MD_OBJ)/cleanstoreutilreal.o \
    $(WMDRM_MD_OBJ)/contentrevocationlevlstub.o \
    $(WMDRM_MD_OBJ)/contentrevocationlicstub.o \
    $(WMDRM_MD_OBJ)/contentrevocationutilstub.o \
    $(WMDRM_MD_OBJ)/crtimplreal.o \
    $(WMDRM_MD_OBJ)/crtutilreal.o \
    $(WMDRM_MD_OBJ)/deletedstoreimplstub.o \
    $(WMDRM_MD_OBJ)/deletedstoreutilstub.o \
    $(WMDRM_MD_OBJ)/deskey.o \
    $(WMDRM_MD_OBJ)/devicerevocationimplreal.o \
    $(WMDRM_MD_OBJ)/devicerevocationutilreal.o \
    $(WMDRM_MD_OBJ)/digitogcd.o \
    $(WMDRM_MD_OBJ)/divide.o \
    $(WMDRM_MD_OBJ)/divisibility.o \
    $(WMDRM_MD_OBJ)/dlautilreal.o \
    $(WMDRM_MD_OBJ)/drmactivationconstants.o \
    $(WMDRM_MD_OBJ)/drmactivationimpstub.o \
    $(WMDRM_MD_OBJ)/drmactivationutilstub.o \
    $(WMDRM_MD_OBJ)/drmantirollbackclockconstants.o \
    $(WMDRM_MD_OBJ)/drmbase64implreal.o \
    $(WMDRM_MD_OBJ)/drmbcertbuilder.o \
    $(WMDRM_MD_OBJ)/drmbcertparser.o \
    $(WMDRM_MD_OBJ)/drmbcrlparser.o \
    $(WMDRM_MD_OBJ)/drmbytemanipimplreal.o \
    $(WMDRM_MD_OBJ)/drmcertparser.o \
    $(WMDRM_MD_OBJ)/drmcertparserconstants.o \
    $(WMDRM_MD_OBJ)/drmchain.o \
    $(WMDRM_MD_OBJ)/drmchkdebug.o \
    $(WMDRM_MD_OBJ)/drmcleanstore.o \
    $(WMDRM_MD_OBJ)/drmconstants.o \
    $(WMDRM_MD_OBJ)/drmcontentrevocationconstants.o \
    $(WMDRM_MD_OBJ)/drmcrtcommon.o \
    $(WMDRM_MD_OBJ)/drmdatastore.o \
    $(WMDRM_MD_OBJ)/drmdebug.o \
    $(WMDRM_MD_OBJ)/drmdevcertconstants.o \
    $(WMDRM_MD_OBJ)/drmdevcert.o \
    $(WMDRM_MD_OBJ)/drmdlaconstants.o \
    $(WMDRM_MD_OBJ)/drmdlaimplreal.o \
    $(WMDRM_MD_OBJ)/drmdomainconstants.o \
    $(WMDRM_MD_OBJ)/drmembedding.o \
    $(WMDRM_MD_OBJ)/drmestimplreal.o \
    $(WMDRM_MD_OBJ)/drmhdrbuilder.o \
    $(WMDRM_MD_OBJ)/drmhdrbuilderconstants.o \
    $(WMDRM_MD_OBJ)/drmhdsimplreal.o \
    $(WMDRM_MD_OBJ)/drmhdsutilreal.o \
    $(WMDRM_MD_OBJ)/drmheaderparser.o \
    $(WMDRM_MD_OBJ)/drmheaderparserconstants.o \
    $(WMDRM_MD_OBJ)/drminitparserconstants.o \
    $(WMDRM_MD_OBJ)/drminitstrings.o \
    $(WMDRM_MD_OBJ)/drmint64.o \
    $(WMDRM_MD_OBJ)/drmkeyfile.o \
    $(WMDRM_MD_OBJ)/drmkeyfileformat.o \
    $(WMDRM_MD_OBJ)/drmkeyfilewrite.o \
    $(WMDRM_MD_OBJ)/drmkeygen.o \
    $(WMDRM_MD_OBJ)/drmlicacqresp.o \
    $(WMDRM_MD_OBJ)/drmlicacqv3.o \
    $(WMDRM_MD_OBJ)/drmlicacqv3constants.o \
    $(WMDRM_MD_OBJ)/drmlicensesyncconstants.o \
    $(WMDRM_MD_OBJ)/drmliceval.o \
	$(WMDRM_MD_OBJ)/drmlicgenstub.o \
    $(WMDRM_MD_OBJ)/drmlicstore.o \
    $(WMDRM_MD_OBJ)/drmm2tsbitstream.o \
    $(WMDRM_MD_OBJ)/drmm2tsbuffermgr.o \
    $(WMDRM_MD_OBJ)/drmm2tsecmbuilder.o \
    $(WMDRM_MD_OBJ)/drmm2tsencryptor.o \
    $(WMDRM_MD_OBJ)/drmm2tsheaderparsers.o \
    $(WMDRM_MD_OBJ)/drmm2tslogs.o \
    $(WMDRM_MD_OBJ)/drmm2tsmediaformatparsers.o \
    $(WMDRM_MD_OBJ)/drmm2tspmtrebuilder.o \
    $(WMDRM_MD_OBJ)/drmmanager.o \
    $(WMDRM_MD_OBJ)/drmmanagerconstants.o \
    $(WMDRM_MD_OBJ)/drmmathsafe.o \
    $(WMDRM_MD_OBJ)/drmmeterconstants.o \
    $(WMDRM_MD_OBJ)/drmmoveconstants.o \
    $(WMDRM_MD_OBJ)/drmndcertconstants.o \
    $(WMDRM_MD_OBJ)/drmnoncestore.o \
    $(WMDRM_MD_OBJ)/drmperformancereal.o \
    $(WMDRM_MD_OBJ)/drmplayreadyobj.o \
    $(WMDRM_MD_OBJ)/drmplayreadyobjconstants.o \
    $(WMDRM_MD_OBJ)/drmpolicystate.o \
    $(WMDRM_MD_OBJ)/drmprndformat.o \
    $(WMDRM_MD_OBJ)/drmprndinternal.o \
    $(WMDRM_MD_OBJ)/drmprndmanager.o \
    $(WMDRM_MD_OBJ)/drmprndmanagerreceiver.o \
    $(WMDRM_MD_OBJ)/drmprndmanagertransmitter.o \
    $(WMDRM_MD_OBJ)/drmprndprotocoltransmitterstub.o \
	$(WMDRM_MD_OBJ)/drmprndprotocolreceiverstub.o \
    $(WMDRM_MD_OBJ)/drmprochecksum.o \
    $(WMDRM_MD_OBJ)/drmprofilingimplstub.o \
    $(WMDRM_MD_OBJ)/drmpubkeydeviceexclusion.o \
    $(WMDRM_MD_OBJ)/drmremovaldatestore.o \
    $(WMDRM_MD_OBJ)/drmrevocation.o \
    $(WMDRM_MD_OBJ)/drmrevocationconstants.o \
    $(WMDRM_MD_OBJ)/drmrevocationstore.o \
    $(WMDRM_MD_OBJ)/drmsecureclockconstants.o \
    $(WMDRM_MD_OBJ)/drmsoapxmlconstants.o \
    $(WMDRM_MD_OBJ)/drmsoapxmlutility.o \
    $(WMDRM_MD_OBJ)/drmstkallocimplreal.o \
    $(WMDRM_MD_OBJ)/drmstrsafeimplreal.o \
    $(WMDRM_MD_OBJ)/drmsymoptconstants.o \
    $(WMDRM_MD_OBJ)/drmtoolsconstants.o \
    $(WMDRM_MD_OBJ)/drmutf.o \
    $(WMDRM_MD_OBJ)/drmutilitiescrypto.o \
    $(WMDRM_MD_OBJ)/drmutilitiesgeneral.o \
    $(WMDRM_MD_OBJ)/drmutilitiesgeneralconstants.o \
    $(WMDRM_MD_OBJ)/drmutilitiestime.o \
    $(WMDRM_MD_OBJ)/drmversionconstants.o \
    $(WMDRM_MD_OBJ)/drmviewrightsconstants.o \
    $(WMDRM_MD_OBJ)/drmwmdrmconstants.o \
    $(WMDRM_MD_OBJ)/drmwmdrmexprevalreal.o \
    $(WMDRM_MD_OBJ)/drmwmdrmexprfuncreal.o \
    $(WMDRM_MD_OBJ)/drmwmdrmexprvariablereal.o \
    $(WMDRM_MD_OBJ)/drmwmdrmlicacqrespreal.o \
    $(WMDRM_MD_OBJ)/drmwmdrmlicchainreal.o \
    $(WMDRM_MD_OBJ)/drmwmdrmlicenseparserreal.o \
    $(WMDRM_MD_OBJ)/drmwmdrmlicevalreal.o \
    $(WMDRM_MD_OBJ)/drmwmdrmnetconstants.o \
    $(WMDRM_MD_OBJ)/drmwmdrmoplreal.o \
    $(WMDRM_MD_OBJ)/drmwmdrmutilreal.o \
    $(WMDRM_MD_OBJ)/drmxb.o \
    $(WMDRM_MD_OBJ)/drmxbbuilder.o \
    $(WMDRM_MD_OBJ)/drmxbparser.o \
    $(WMDRM_MD_OBJ)/drmxmlbuilderconstants.o \
    $(WMDRM_MD_OBJ)/drmxmlbuildercrypto.o \
    $(WMDRM_MD_OBJ)/drmxmlbuildergeneralansi.o \
    $(WMDRM_MD_OBJ)/drmxmlbuildergeneralunicode.o \
    $(WMDRM_MD_OBJ)/drmxmlparserconstants.o \
    $(WMDRM_MD_OBJ)/drmxmlparsercrypto.o \
    $(WMDRM_MD_OBJ)/drmxmlparsergeneral.o \
    $(WMDRM_MD_OBJ)/drmxmlsigbuilder.o \
    $(WMDRM_MD_OBJ)/drmxmlsigconstants.o \
    $(WMDRM_MD_OBJ)/drmxmlsiggeneral.o \
    $(WMDRM_MD_OBJ)/drmxmlutilities.o \
    $(WMDRM_MD_OBJ)/drmxmr.o \
    $(WMDRM_MD_OBJ)/drmxmrbuilder.o \
    $(WMDRM_MD_OBJ)/drmxmrparser.o \
    $(WMDRM_MD_OBJ)/ecex2001.o \
    $(WMDRM_MD_OBJ)/ecexpon.o \
    $(WMDRM_MD_OBJ)/ecppq.o \
    $(WMDRM_MD_OBJ)/ecurve.o \
    $(WMDRM_MD_OBJ)/field.o \
    $(WMDRM_MD_OBJ)/kdiv.o \
    $(WMDRM_MD_OBJ)/kinitpr.o \
    $(WMDRM_MD_OBJ)/kinvert.o \
    $(WMDRM_MD_OBJ)/kmuladd.o \
    $(WMDRM_MD_OBJ)/lucas.o \
    $(WMDRM_MD_OBJ)/metercertrevocationutilreal.o \
    $(WMDRM_MD_OBJ)/modelrevocationimplstub.o \
    $(WMDRM_MD_OBJ)/modelrevocationutilstub.o \
    $(WMDRM_MD_OBJ)/modexp.o \
    $(WMDRM_MD_OBJ)/modmulch1.o \
    $(WMDRM_MD_OBJ)/modsqrt.o \
    $(WMDRM_MD_OBJ)/modular.o \
    $(WMDRM_MD_OBJ)/mpaddsubcmp.o \
    $(WMDRM_MD_OBJ)/mpalloc.o \
    $(WMDRM_MD_OBJ)/mpgcdex.o \
    $(WMDRM_MD_OBJ)/mpmul22.o \
    $(WMDRM_MD_OBJ)/mprand.o \
    $(WMDRM_MD_OBJ)/multlow.o \
    $(WMDRM_MD_OBJ)/ndtconstants.o \
    $(WMDRM_MD_OBJ)/oemaes.o \
    $(WMDRM_MD_OBJ)/oemaesmulti.o \
    $(WMDRM_MD_OBJ)/oemcbc.o \
    $(WMDRM_MD_OBJ)/oemcipher.o \
    $(WMDRM_MD_OBJ)/oemcriticalsection.o \
    $(WMDRM_MD_OBJ)/oemdebug.o \
    $(WMDRM_MD_OBJ)/oemderivedkeys.o \
    $(WMDRM_MD_OBJ)/oemdevstore.o \
    $(WMDRM_MD_OBJ)/oemeccp160.o \
    $(WMDRM_MD_OBJ)/oemeccp160impl.o \
    $(WMDRM_MD_OBJ)/oemeccp256.o \
    $(WMDRM_MD_OBJ)/oemeccp256impl.o \
    $(WMDRM_MD_OBJ)/oemfileio.o \
    $(WMDRM_MD_OBJ)/oemimpl.o \
    $(WMDRM_MD_OBJ)/oemimplrand.o \
    $(WMDRM_MD_OBJ)/oemimpl_common.o \
    $(WMDRM_MD_OBJ)/oeminfo.o \
    $(WMDRM_MD_OBJ)/oeminfo_common.o \
    $(WMDRM_MD_OBJ)/oemkeyfile.o \
    $(WMDRM_MD_OBJ)/oemmd5.o \
    $(WMDRM_MD_OBJ)/oemndr.o \
    $(WMDRM_MD_OBJ)/oemndt.o \
    $(WMDRM_MD_OBJ)/oemparsers_bcert.o \
    $(WMDRM_MD_OBJ)/oemparsers_bcrl.o \
    $(WMDRM_MD_OBJ)/oemparsers_revinfo.o \
    $(WMDRM_MD_OBJ)/oemparsers_utf8.o \
    $(WMDRM_MD_OBJ)/oempkcrypto.o \
    $(WMDRM_MD_OBJ)/oemprofile_common.o \
    $(WMDRM_MD_OBJ)/oemrsa.o \
    $(WMDRM_MD_OBJ)/oemrsakeygen.o \
    $(WMDRM_MD_OBJ)/oemrsaoaeppss.o \
    $(WMDRM_MD_OBJ)/oemrsaoaeppssimpl.o \
    $(WMDRM_MD_OBJ)/oemsha1.o \
    $(WMDRM_MD_OBJ)/oemsha256.o \
    $(WMDRM_MD_OBJ)/oemshahash.o \
    $(WMDRM_MD_OBJ)/oemshahmac.o \
    $(WMDRM_MD_OBJ)/oemsparsexmrparser.o \
    $(WMDRM_MD_OBJ)/oemxmlhash.o \
    $(WMDRM_MD_OBJ)/rc4.o \
	$(WMDRM_MD_OBJ)/oem_config.o	\
	$(WMDRM_MD_OBJ)/oem_init.o	\
	$(WMDRM_MD_OBJ)/oem_key_region.o	\
    $(WMDRM_MD_OBJ)/tee.o \
    $(WMDRM_MD_OBJ)/teeder.o \
    $(WMDRM_MD_OBJ)/teeimpl.o \
    $(WMDRM_MD_OBJ)/teestate.o \
    $(WMDRM_MD_OBJ)/teestateversion.o \
    $(WMDRM_MD_OBJ)/teeutil.o \
    $(WMDRM_MD_OBJ)/bcm_oemimpl.o\
    $(WMDRM_MD_OBJ)/bcm_oem_init.o \
    $(WMDRM_MD_OBJ)/bcm_oem_m2m_dma.o \





   

#
# Production vs Test bits
#------------------------------
ifneq "$(PLAYREADY_TEST)" "1"
WMDRM_MD_CORE_OBJS +=                     \
    $(WMDRM_MD_OBJ)/oemtime.o             \
    $(WMDRM_MD_OBJ)/oemhal.o              \
    $(WMDRM_MD_OBJ)/oemhaldevcerts.o      \
    $(WMDRM_MD_OBJ)/oemhaloemimpl.o
else
WMDRM_MD_CORE_OBJS +=                     \
    $(WMDRM_MD_OBJ_TEST)/oemtime.o        \
    $(WMDRM_MD_OBJ)/oemhal.o              \
    $(WMDRM_MD_OBJ)/oemhaldevcerts.o      \
    $(WMDRM_MD_OBJ_TEST)/oemhaloemimpl.o
endif

# CHK vs FRE bits
#------------------------------
ifeq "$(PLAYREADY_BUILD_TYPE)" "CHK"
WMDRM_MD_CORE_OBJS +=                  \
    $(WMDRM_MD_OBJ)/drmcontract.o      \
    $(WMDRM_MD_OBJ)/contractutilreal.o
else
WMDRM_MD_CORE_OBJS +=                  \
    $(WMDRM_MD_OBJ)/drmcontractstub.o  \
    $(WMDRM_MD_OBJ)/contractutilstub.o
endif

WMDRM_MD_ALL_OBJS = $(WMDRM_MD_CORE_OBJS)

# Copy Opl - real or stub
#------------------------------
ifneq "$(DRM_BUILD_PROFILE)" "950"
WMDRM_MD_ALL_OBJS += \
    $(WMDRM_MD_OBJ)/copyoplimplreal.o \
    $(WMDRM_MD_OBJ)/copyoplutilreal.o
else
WMDRM_MD_ALL_OBJS += \
    $(WMDRM_MD_OBJ)/copyoplimplstub.o \
    $(WMDRM_MD_OBJ)/copyoplutilstub.o
endif

# Move - real or stub
#------------------------------
ifneq "$(DRM_BUILD_PROFILE)" "950"
WMDRM_MD_ALL_OBJS += \
    $(WMDRM_MD_OBJ)/drmmoveformat.o \
    $(WMDRM_MD_OBJ)/drmmoveimp.o \
    $(WMDRM_MD_OBJ)/drmmovestore.o \
    $(WMDRM_MD_OBJ)/moveutilreal.o
else
WMDRM_MD_ALL_OBJS += \
    $(WMDRM_MD_OBJ)/drmmoveimplstub.o \
    $(WMDRM_MD_OBJ)/moveutilstub.o
endif


# Symopt - real or stub
#------------------------------
ifneq "$(DRM_BUILD_PROFILE)" "950"
WMDRM_MD_ALL_OBJS += \
    $(WMDRM_MD_OBJ)/symoptdevicedevcertreal.o \
    $(WMDRM_MD_OBJ)/symoptlacqreal.o \
    $(WMDRM_MD_OBJ)/symoptlicreal.o \
    $(WMDRM_MD_OBJ)/symoptutilreal.o
else
WMDRM_MD_ALL_OBJS += \
    $(WMDRM_MD_OBJ)/symoptdevicedevcertstub.o \
    $(WMDRM_MD_OBJ)/symoptlacqstub.o \
    $(WMDRM_MD_OBJ)/symoptlicstub.o \
    $(WMDRM_MD_OBJ)/symoptutilstub.o
endif

# Thumbnail - real or stub
#------------------------------
ifneq "$(DRM_BUILD_PROFILE)" "950"
WMDRM_MD_ALL_OBJS += \
    $(WMDRM_MD_OBJ)/thumbnailutilreal.o
else
WMDRM_MD_ALL_OBJS += \
    $(WMDRM_MD_OBJ)/thumbnailutilstub.o
endif

# XML Hash - real or stub
#------------------------------
ifneq "$(DRM_BUILD_PROFILE)" "950"
WMDRM_MD_ALL_OBJS += \
    $(WMDRM_MD_OBJ)/xmlhashimplreal.o \
    $(WMDRM_MD_OBJ)/xmlhashutilreal.o
else
WMDRM_MD_ALL_OBJS += \
    $(WMDRM_MD_OBJ)/xmlhashimplstub.o \
    $(WMDRM_MD_OBJ)/xmlhashutilstub.o
endif

# HDS Block Header Cache - real or stub
#------------------------------
ifneq "$(DRM_BUILD_PROFILE)" "950"
WMDRM_MD_ALL_OBJS += \
    $(WMDRM_MD_OBJ)/hdsblockheadercacheutilreal.o
else
WMDRM_MD_ALL_OBJS += \
    $(WMDRM_MD_OBJ)/hdsblockheadercacheutilstub.o
endif


# PRND Protocol Rx - real or stub
#------------------------------
#ifneq "$(DRM_BUILD_PROFILE)" "950"
#WMDRM_MD_ALL_OBJS += \
#    $(WMDRM_MD_OBJ)/drmprndprotocolreceiver.o
#else
#WMDRM_MD_ALL_OBJS += \
#    $(WMDRM_MD_OBJ)/drmprndprotocolreceiverstub.o
#endif



# WMDRMNET - real or stub
#------------------------------
ifneq "$(DRM_BUILD_PROFILE)" "950"
WMDRM_MD_ALL_OBJS +=                   \
    $(WMDRM_MD_OBJ)/drmndcertbuilder.o \
    $(WMDRM_MD_OBJ)/drmnddevicecert.o  \
    $(WMDRM_MD_OBJ)/drmxmlrsakeys.o    \
    $(WMDRM_MD_OBJ)/wmdrmnetimplreal.o \
    $(WMDRM_MD_OBJ)/wmdrmnetutilreal.o
else
WMDRM_MD_ALL_OBJS +=                       \
    $(WMDRM_MD_OBJ)/drmndcertbuilderstub.o \
    $(WMDRM_MD_OBJ)/drmxmlrsakeysstub.o    \
    $(WMDRM_MD_OBJ)/wmdrmnetimplstub.o     \
    $(WMDRM_MD_OBJ)/wmdrmnetutilstub.o
endif

# Metering - real or stub
#------------------------------
ifneq "$(DRM_BUILD_PROFILE)" "950"
WMDRM_MD_ALL_OBJS += \
    $(WMDRM_MD_OBJ)/drmmeterapi.o       \
    $(WMDRM_MD_OBJ)/drmmetercertstore.o \
    $(WMDRM_MD_OBJ)/drmmeterimp.o       \
    $(WMDRM_MD_OBJ)/drmmeterimpv1.o     \
    $(WMDRM_MD_OBJ)/meteringutilreal.o
else
WMDRM_MD_ALL_OBJS += \
    $(WMDRM_MD_OBJ)/drmmeterapistub.o       \
    $(WMDRM_MD_OBJ)/drmmetercertstorestub.o \
    $(WMDRM_MD_OBJ)/drmmeterimpstub.o       \
    $(WMDRM_MD_OBJ)/meteringutilstub.o
endif

# Secure Clock - real or stub
#------------------------------
ifneq "$(DRM_BUILD_PROFILE)" "950"
WMDRM_MD_ALL_OBJS += \
    $(WMDRM_MD_OBJ)/secureclockmanager.o  \
    $(WMDRM_MD_OBJ)/secureclockrequest.o  \
    $(WMDRM_MD_OBJ)/secureclockresponse.o \
    $(WMDRM_MD_OBJ)/secureclockutil.o
else
WMDRM_MD_ALL_OBJS += \
    $(WMDRM_MD_OBJ)/drmsecurelockmgrextern.o  \
    $(WMDRM_MD_OBJ)/drmsecurelockrequextern.o \
    $(WMDRM_MD_OBJ)/drmsecurelockrespextern.o \
    $(WMDRM_MD_OBJ)/drmsecurelockutilextern.o
endif

# Domains - real or stub
#------------------------------
ifneq "$(DRM_BUILD_PROFILE)" "950"
WMDRM_MD_ALL_OBJS += \
    $(WMDRM_MD_OBJ)/drmdomainapi.o          \
    $(WMDRM_MD_OBJ)/drmdomainimp.o          \
    $(WMDRM_MD_OBJ)/drmdomainkeyxmrparser.o \
    $(WMDRM_MD_OBJ)/drmdomainstore.o
else
WMDRM_MD_ALL_OBJS += \
    $(WMDRM_MD_OBJ)/drmdomainstub.o
endif

# View Rights - real or stub
#------------------------------
ifneq "$(DRM_BUILD_PROFILE)" "950"
WMDRM_MD_ALL_OBJS += \
    $(WMDRM_MD_OBJ)/drmviewrightsimplreal.o \
    $(WMDRM_MD_OBJ)/drmviewrightsutilreal.o
else
WMDRM_MD_ALL_OBJS += \
    $(WMDRM_MD_OBJ)/drmviewrightsimplstub.o \
    $(WMDRM_MD_OBJ)/drmviewrightsutilstub.o
endif

# Device Store - real or stub
#------------------------------
ifneq "$(DRM_BUILD_PROFILE)" "950"
WMDRM_MD_ALL_OBJS += \
    $(WMDRM_MD_OBJ)/devicestoreutilreal.o \
    $(WMDRM_MD_OBJ)/drmdevicestore.o      \
    $(WMDRM_MD_OBJ)/drmdevstore.o         
else
WMDRM_MD_ALL_OBJS += \
    $(WMDRM_MD_OBJ)/devicestoreutilstub.o \
    $(WMDRM_MD_OBJ)/drmdevicestorestub.o  \
    $(WMDRM_MD_OBJ)/drmdevstorestub.o     
endif

# License Sync - real or stub
#------------------------------
ifneq "$(DRM_BUILD_PROFILE)" "950"
WMDRM_MD_ALL_OBJS += \
    $(WMDRM_MD_OBJ)/drmgensyncchallenge.o \
    $(WMDRM_MD_OBJ)/drmsyncstore.o        \
    $(WMDRM_MD_OBJ)/licensesyncutilreal.o         
else
WMDRM_MD_ALL_OBJS += \
    $(WMDRM_MD_OBJ)/drmgensyncchallengestub.o \
    $(WMDRM_MD_OBJ)/drmsyncstorestub.o        \
    $(WMDRM_MD_OBJ)/licensesyncutilstub.o  
endif

# Asset-less - real or stub
#------------------------------
WMDRM_MD_ASSETS_REAL_OBJS = \
    $(WMDRM_MD_OBJ)/antirollbackclockimplreal.o \
    $(WMDRM_MD_OBJ)/antirollbackclockutilreal.o \
    $(WMDRM_MD_OBJ)/drmblobcachereal.o          \
    $(WMDRM_MD_OBJ)/certcachereal.o             \
    $(WMDRM_MD_OBJ)/certcacheutilreal.o         \
    $(WMDRM_MD_OBJ)/devcertkeygenimplreal.o     \
    $(WMDRM_MD_OBJ)/drmdeviceassetsutilreal.o   \
    $(WMDRM_MD_OBJ)/drmdevicedevcertreal.o      \
    $(WMDRM_MD_OBJ)/drmrevocationstoresst.o     \
    $(WMDRM_MD_OBJ)/drmsecstorereal.o

WMDRM_MD_ASSETS_STUB_OBJS = \
    $(WMDRM_MD_OBJ)/antirollbackclockimplstub.o \
    $(WMDRM_MD_OBJ)/antirollbackclockutilstub.o \
    $(WMDRM_MD_OBJ)/drmblobcachestub.o          \
    $(WMDRM_MD_OBJ)/certcachestub.o             \
    $(WMDRM_MD_OBJ)/certcacheutilstub.o         \
    $(WMDRM_MD_OBJ)/devcertkeygenimplstub.o     \
    $(WMDRM_MD_OBJ)/drmdeviceassetsutilstub.o   \
    $(WMDRM_MD_OBJ)/drmdevicedevcertstub.o      \
    $(WMDRM_MD_OBJ)/drmrevocationstorehds.o     \
    $(WMDRM_MD_OBJ)/drmsecstorestub.o


ifneq "$(DRM_BUILD_PROFILE)" "950"
WMDRM_MD_ALL_OBJS += \
    $(WMDRM_MD_ASSETS_REAL_OBJS)
else
WMDRM_MD_ALL_OBJS += \
    $(WMDRM_MD_ASSETS_STUB_OBJS)
endif

# Other additions
#------------------------------
WMDRM_MD_OTHER_OBJS =                            \
    $(WMDRM_MD_OBJ)/drmerror.o                   \
    $(WMDRM_MD_OBJ)/drmresults.o                 \
    $(WMDRM_MD_OBJ)/drmtoolsinit.o               \
    $(WMDRM_MD_OBJ)/drmtoolsutils.o              \
    $(WMDRM_MD_OBJ)/drminitparser.o              \
    $(WMDRM_MD_OBJ)/drmndtmanager.o              \
    $(WMDRM_MD_OBJ)/drmpfd.o                     \
    $(WMDRM_MD_OBJ)/drmptrlist.o                 \
    $(WMDRM_MD_OBJ)/ndtencrypt.o                 \
    $(WMDRM_MD_OBJ)/ndtlicense.o                 \
    $(WMDRM_MD_OBJ)/ndtmanager_internal.o        \
    $(WMDRM_MD_OBJ)/ndtnetmsg.o                  \
    $(WMDRM_MD_OBJ)/ndtreg.o                     \
    $(WMDRM_MD_OBJ)/asfmediabuffer.o             \
    $(WMDRM_MD_OBJ)/asfsample.o                  \
    $(WMDRM_MD_OBJ)/drmndtasf.o                  \
    $(WMDRM_MD_OBJ)/drmorderedlist.o             \
    $(WMDRM_MD_OBJ)/leakybucket.o                \
    $(WMDRM_MD_OBJ)/mux.o                        \
    $(WMDRM_MD_OBJ)/muxstream.o                  \
    $(WMDRM_MD_OBJ)/ndtasfdrm.o                  \
    $(WMDRM_MD_OBJ)/ndtasfguids.o                \
    $(WMDRM_MD_OBJ)/ndtasf_internal.o            \
    $(WMDRM_MD_OBJ)/packetwriter.o               \
    $(WMDRM_MD_OBJ)/sendwindow.o

WMDRM-MD_CMDLNPARS_LIB = $(WMDRM_MD_OBJ)/cmdlnpars.o         \

WMDRM-MD_DRMPFDMANAGER_LIB = $(WMDRM_MD_OBJ)/drmpfdmanager.o \

ifneq "$(DRM_BUILD_PROFILE)" "950"
WMDRM_MD_ALL_OBJS += \
    $(WMDRM_MD_OTHER_OBJS)           \
#    $(WMDRM_MD_OBJ)/oemstartup.o
else
_DRM_PK_MISC_LIBS += \
    $(WMDRM_MD_OTHER_OBJS)                       \
    $(WMDRM_MD_OBJ_TEST)/oemstartup.o            \
    /opt/toolchains/i686-linux-elf/lib/libsec.so
endif

# =================================================================================================

# Test LIBs
# ---------

_DRM_TESTCORE_LIB  = $(WMDRM_MD_OBJ)/apiParams.o         \
                     $(WMDRM_MD_OBJ)/drmtestrsautils.o   \
                     $(WMDRM_MD_OBJ)/logger.o            \
                     $(WMDRM_MD_OBJ)/PKCommonTestAPI.o   \
                     $(WMDRM_MD_OBJ)/tclib.o             \
                     $(WMDRM_MD_OBJ)/TestVariableArray.o \

_DRM_TESTUTILS_LIB = $(WMDRM_MD_OBJ)/CheckSum.o          \
                     $(WMDRM_MD_OBJ)/DerivedKeys.o       \
                     $(WMDRM_MD_OBJ)/domain.o            \
                     $(WMDRM_MD_OBJ)/LicGenXMR.o         \
                     $(WMDRM_MD_OBJ)/NdtDrmXmr.o         \
                     $(WMDRM_MD_OBJ)/rightsparser.o      \
                     $(WMDRM_MD_OBJ)/SetClientID.o       \
                     $(WMDRM_MD_OBJ)/stringutils.o       \
                     $(WMDRM_MD_OBJ)/stubcert.o          \
                     $(WMDRM_MD_OBJ)/tcontentkey.o       \
                     $(WMDRM_MD_OBJ)/tgeneratorutils.o   \
                     $(WMDRM_MD_OBJ)/tGetClientID.o      \
                     $(WMDRM_MD_OBJ)/timeutils.o         \
                     $(WMDRM_MD_OBJ)/tLicGen.o           \
                     $(WMDRM_MD_OBJ)/tResponseGen.o      \
                     $(WMDRM_MD_OBJ)/tstheadergen.o      \
                     $(WMDRM_MD_OBJ)/tstHDSutils.o       \
                     $(WMDRM_MD_OBJ)/tstkeyfile.o        \
                     $(WMDRM_MD_OBJ)/tstutils.o          \
                     $(WMDRM_MD_OBJ)/XMRLicGen.o         \

_DRM_TESTOEM_LIB   = $(WMDRM_MD_OBJ)/tOEMImp.o           \
                     $(WMDRM_MD_OBJ)/tOEMImp_PD.o        \
                     $(WMDRM_MD_OBJ)/toem_paths.o        \

_DRM_TESTNETIO_LIB = $(WMDRM_MD_OBJ)/NetIOTestAPI.o      \

_DRM_TEST_LIBS     = $(_DRM_TESTCORE_LIB)  \
                     $(_DRM_TESTUTILS_LIB) \
                     $(_DRM_TESTOEM_LIB)   \
                     $(_DRM_TESTNETIO_LIB) \
                     $(_DRM_PK_MISC_LIBS)  \

# Test Area LIBs
# --------------
_DRM_TEST_BLACKBOX_LIB =      $(WMDRM_MD_OBJ)/blackboxtest.o         \
                              $(WMDRM_MD_OBJ)/binding.o              \
                              $(WMDRM_MD_OBJ_TEST)/oemhaloemimpl.o   \

_DRM_TEST_PRND_LIB =          $(WMDRM_MD_OBJ)/prndformattest.o       \
                              $(WMDRM_MD_OBJ)/prndprotocoltest.o     \
                              $(WMDRM_MD_OBJ)/prndtoplevelapitest.o  \
                              $(WMDRM_MD_OBJ)/prndnoassettxtest.o    \

_DRM_TEST_CERTS_LIB =         $(WMDRM_MD_OBJ)/TestBCert.o            \
                              $(WMDRM_MD_OBJ)/TestBCertBuilder.o     \
                              $(WMDRM_MD_OBJ)/TestBCertParser.o      \
                              $(WMDRM_MD_OBJ)/TestNDCert.o           \
                              $(WMDRM_MD_OBJ)/TestRSAKeys.o          \
                         
_DRM_TEST_XMR_LIB =           $(WMDRM_MD_OBJ)/XmrTestAPI.o           \

_DRM_TEST_KEYFILE_LIB =       $(WMDRM_MD_OBJ)/KeyFileTestAPI.o       \

_DRM_TEST_LICGEN_LIB =        $(WMDRM_MD_OBJ)/licgentest.o           \

_DRM_TEST_M2TSENCRYPTOR_LIB = $(WMDRM_MD_OBJ)/m2tsencryptortestapi.o \

_DRM_TEST_MATHSAFE_LIB =      $(WMDRM_MD_OBJ)/mathsafetest.o         \

_DRM_TEST_COREEMBEDDING_LIB = $(WMDRM_MD_OBJ)/CoreEmbedding.o        \

_DRM_TEST_DRMMANAGER_LIB =    $(WMDRM_MD_OBJ)/dlrb.o                           \
                              $(WMDRM_MD_OBJ)/DRMManagerTestAPI.o              \
                              $(WMDRM_MD_OBJ)/DRMMGR_ContentPropTestAPI.o      \
                              $(WMDRM_MD_OBJ)/DRMMGR_DevicePropertyTestAPI.o   \
                              $(WMDRM_MD_OBJ)/DRMMGR_DomainsTestAPI.o          \
                              $(WMDRM_MD_OBJ)/DRMMGR_EncryptTestAPI.o          \
                              $(WMDRM_MD_OBJ)/DRMMGR_EnvelopeTestAPI.o         \
                              $(WMDRM_MD_OBJ)/DRMMGR_HDSTestAPI.o              \
                              $(WMDRM_MD_OBJ)/DRMMGR_InitializationTestAPI.o   \
                              $(WMDRM_MD_OBJ)/DRMMGR_LicAcqTestAPI.o           \
                              $(WMDRM_MD_OBJ)/DRMMGR_LicenseQueryTestAPI.o     \
                              $(WMDRM_MD_OBJ)/DRMMGR_LicenseResponseTestAPI.o  \
                              $(WMDRM_MD_OBJ)/DRMMGR_LicenseTestAPI.o          \
                              $(WMDRM_MD_OBJ)/DRMMGR_MeteringTestAPI.o         \
                              $(WMDRM_MD_OBJ)/DRMMGR_ReaderTestAPI.o           \
                              $(WMDRM_MD_OBJ)/DRMMGR_RevocationTestAPI.o       \
                              $(WMDRM_MD_OBJ)/DRMMGR_SecureClockTestAPI.o      \
                              $(WMDRM_MD_OBJ)/DRMMGR_StoreMgmtTestAPI.o        \
                              $(WMDRM_MD_OBJ)/DRMMGR_SyncListTestAPI.o         \
                              $(WMDRM_MD_OBJ)/DRMMGR_UpgradeTestAPI.o          \
                              $(WMDRM_MD_OBJ)/DRMMGR_TimeTestAPI.o             \

_DRM_TEST_HDS_LIB =           $(WMDRM_MD_OBJ)/testhds.o                        \

_DRM_TEST_METERING_LIB =      $(WMDRM_MD_OBJ)/V3MeteringTestHelpers.o          \
                              $(WMDRM_MD_OBJ)/MeteringTestAPI.o                \

_DRM_TEST_CRYPTO_LIB =        $(WMDRM_MD_OBJ)/CryptoTest.o           \
                              $(WMDRM_MD_OBJ)/tstcryptodata.o        \

_DRM_TEST_SECCLOCK_LIB =      $(WMDRM_MD_OBJ)/secclock.o             \

_DRM_TEST_LICEVAL_LIB =       $(WMDRM_MD_OBJ)/LicEval.o              \

_DRM_TEST_LIVETV_LIB =        $(WMDRM_MD_OBJ)/LiveTVTestAPI.o        \
                              $(WMDRM_MD_OBJ)/DerivedKeyTestAPI.o    \

_DRM_TEST_HEADER_LIB =        $(WMDRM_MD_OBJ)/header.o               \

_DRM_TEST_PERFORMANCE_LIB =   $(WMDRM_MD_OBJ)/PerformanceTestAPI.o   \

_DRM_TEST_DOMAINAPI_LIB =     $(WMDRM_MD_OBJ)/DomainTestAPI.o        \

_DRM_TEST_DOMAINSTORE_LIB =   $(WMDRM_MD_OBJ)/DomainStoreTestAPI.o   \

_DRM_TEST_LICSTORE_LIB =      $(WMDRM_MD_OBJ)/testlicstore.o         \

_DRM_TEST_DATASTORE_LIB =     $(WMDRM_MD_OBJ)/testdatastore.o        \

_DRM_TEST_LICACQV3_LIB =      $(WMDRM_MD_OBJ)/testlicacqv3.o         \

_DRM_TEST_SECSTORE_LIB =      $(WMDRM_MD_OBJ)/testsecstore.o         \

_DRM_TEST_DEVICESTORE_LIB =   $(WMDRM_MD_OBJ)/testdevicestore.o      \

_DRM_TEST_LICPARSER_LIB =     $(WMDRM_MD_OBJ)/licpsr.o               \

_DRM_TEST_METERCERTSTORE_LIB = $(WMDRM_MD_OBJ)/MeterCertStoreTestAPI.o         \

_DRM_TEST_DEVICEDEVCERT_LIB = $(WMDRM_MD_OBJ)/ddctest.o              \

_DRM_TEST_POLICYSTATE_LIB =   $(WMDRM_MD_OBJ)/PolicyStateTestAPI.o   \

_DRM_TEST_DEVCERTPARSER_LIB = $(WMDRM_MD_OBJ)/dcParser.o             \

_DRM_TEST_INITPARSER_LIB =    $(WMDRM_MD_OBJ)/initparsertest.o       \

_DRM_TEST_NONCESTORE_LIB =    $(WMDRM_MD_OBJ)/NonceStoreTestAPI.o    \

_DRM_TEST_VIEWRIGHTS_LIB =    $(WMDRM_MD_OBJ)/ViewRights.o           \

_DRM_TEST_UTILS_LIB =         $(WMDRM_MD_OBJ)/drmutiltest.o          \

_DRM_TEST_OEM_LIB =           $(WMDRM_MD_OBJ)/refOEMtest.o           \

_DRM_TEST_XMLBUILDER_LIB =    $(WMDRM_MD_OBJ)/testxmlbuilder.o       \

_DRM_TEST_XMLBUILDERA_LIB =   $(WMDRM_MD_OBJ)/testxmlbuildera.o      \

_DRM_TEST_XMLPARSER_LIB =     $(WMDRM_MD_OBJ)/testXMLParser.o        \

_DRM_TEST_XMLSIG_LIB =        $(WMDRM_MD_OBJ)/xmlsignaturetest.o     \

_DRM_TEST_SYNCLIST_LIB =      $(WMDRM_MD_OBJ)/testsynclist.o         \

_DRM_TEST_EXPREVAL_LIB =      $(WMDRM_MD_OBJ)/ExprEval.o             \
                               
_DRM_TEST_UTF_LIB =           $(WMDRM_MD_OBJ)/utftest.o              \

_DRM_TEST_TEST_SUITE_NDT_LIB = $(WMDRM_MD_OBJ)/drmdecrypt.o          \
                               $(WMDRM_MD_OBJ)/testdevstore.o        \
                               $(WMDRM_MD_OBJ)/testSuiteNDT.o        \
                               
_DRM_TEST_CONSOLENDR_LIB =    $(WMDRM_MD_OBJ)/testNDR.o              \




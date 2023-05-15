/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __NATIVE_JLABS_CHANNEL
#define __NATIVE_JLABS_CHANNEL

#if defined OIPF && defined JLABS

#include "NativeOOIFTypes.h"
#include "NativeChannel.h"
#include "opljlabslocalsystem.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum{
	JLABS_DTT_NETWORK = 0,
	JLABS_BS_NETWORK = 1,
	JLABS_CATV_NETWORK = 2,
	JLABS_ALL_NETWORK = 3,
	JLABS_CS1_NETWORK = 4,
	JLABS_CS2_NETWORK = 5,
	JLABS_JCHITS_NETWORK = 6,
	JLABS_END_NETWORK = 6,
}jlabsChannelListNetwork;

typedef void* jlabsChannelHandle;
typedef void* jlabsChannelListHandle;

typedef void (resultCreateMaskListener)(int result);

/**
   Registers new listener on ChannelConfig related events.
   \param listener The ligstener
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_jlabsChannelConfigGetChannelListsByNetType(jlabsChannelListNetwork network, int *pNumOfNetwork, int ** ppNumOfChannelsByNetwork, jlabsChannelListHandle **handles);
OOIFReturnCode native_jlabsChannelConfigFreeChList(jlabsChannelListHandle pHandle);
OOIFReturnCode native_jlabsChannelConfigFreeNumOfChannel(int *pHandle);

OOIFReturnCode native_jlabsChannelReleaseHandle(jlabsChannelHandle id);

OOIFReturnCode native_jlabsChannelGetField(jlabsChannelHandle identifier, const char *field, const char **retval);
OOIFReturnCode native_jlabsChannelGetLogo(jlabsChannelHandle identifier, OOIFNumber width, OOIFNumber height, const char **retval);
OOIFReturnCode native_jlabsChannelGetProviderName(jlabsChannelHandle identifier, const char **retval);
OOIFReturnCode native_jlabsChannelGetInvisible(jlabsChannelHandle identifier, OOIFBoolean *retval);
OOIFReturnCode native_jlabsChannelSetInvisible(jlabsChannelHandle identifier, OOIFBoolean visible);
OOIFReturnCode native_jlabsChannelGetOneTouch(jlabsChannelHandle identifier, OOIFNumber *retval);
OOIFReturnCode native_jlabsChannelSetOneTouch(jlabsChannelHandle identifier, OOIFNumber oneTouch);
OOIFReturnCode native_jlabsChannelListGetName(jlabsChannelListHandle identifier, int length, const char **retval);
OOIFReturnCode native_jlabsChannelListGetAreaCode(jlabsChannelListHandle identifier, int length, OOIFNumber *retval);
OOIFReturnCode native_jlabsChannelListGetSignalLevel(jlabsChannelListHandle identifier, int length, const char **retval);
OOIFReturnCode native_jlabsChannelListGetAgc(jlabsChannelListHandle identifier, int length, OOIFNumber *retval);
OOIFReturnCode native_jlabsChannelListGetBer(jlabsChannelListHandle identifier, int length, OOIFNumber *retval);
OOIFReturnCode native_jlabsChannelListGetSnr(jlabsChannelListHandle identifier, int length, OOIFNumber *retval);
OOIFReturnCode native_jlabsChannelListGetBranchNumber(jlabsChannelListHandle identifier, int length, OOIFNumber *retval);
OOIFReturnCode native_jlabsChannelListGetOneTouch(jlabsChannelListHandle identifier, int length, OOIFNumber *retval);
OOIFReturnCode native_jlabsChannelListSetOneTouch(jlabsChannelListHandle identifier, int length, OOIFNumber value);
OOIFReturnCode native_jlabsChannelListReleaseChListHandle(jlabsChannelHandle *pHandle);

OOIFReturnCode native_jlabsChannelListCollectionGetNetworkTypeName(jlabsChannelListNetwork eNetworkTypeName, const char **retval);
OOIFReturnCode native_jlabsChannelListCollectionGetLastUpdateDate(jlabsChannelListNetwork networkTypeName, OOIFNumber *retval);

OOIFReturnCode native_jlabsChannelConfigCreateChannelMask(OOIFBoolean *retval);
OOIFReturnCode native_jlabsChannelConfigSetListener(resultCreateMaskListener listener);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* OIPF && JLABS */

#endif /* __NATIVE_JLABS_CHANNEL */

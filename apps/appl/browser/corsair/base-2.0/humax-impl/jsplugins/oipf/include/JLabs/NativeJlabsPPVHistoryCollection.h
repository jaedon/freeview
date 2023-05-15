/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#if defined OIPF && defined JLABS

#ifndef __NATIVE_JLABS_PPVHISTORYCOLLECTION
#define __NATIVE_JLABS_PPVHISTORYCOLLECTION

#include "NativeOOIFTypes.h"
#include "NativeJlabsPPVHistory.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
typedef void (uploadPPVViewlogDataListener)(int result);

/*** B.3.4.1.6. jlabsSystemInfo Å¬·¡½º ***/

/* GETTER - read only */
/* ... */
OOIFReturnCode native_jlabsPPVHistoryCollectionGetTotalPurchasedFee(OOIFNumber *retval);

/* funciton */
OOIFReturnCode native_jlabsPPVHistoryCollectionPPVViewlogData(OOIFNumber*retval);

OOIFReturnCode native_jlabsPPVHistoryCollectionDeleteAllPPVPrivateData(OOIFNumber*retval);
OOIFReturnCode native_jlabsPPVHistoryCollectionCancelPPVViewlogDataUpload(OOIFNumber*retval);
OOIFReturnCode native_jlabsPPVHistoryCollectionSetListener(uploadPPVViewlogDataListener listener);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_JLABS_PPVHISTORYCOLLECTION */

#endif // #if defined OIPF && defined JLABS

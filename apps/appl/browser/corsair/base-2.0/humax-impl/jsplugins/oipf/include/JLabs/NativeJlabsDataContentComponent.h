/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#if defined OIPF && defined JLABS

#ifndef __NATIVE_JLABSDATACONTENTCOMPONENT__
#define __NATIVE_JLABSDATACONTENTCOMPONENT__

#include "NativeOOIFTypes.h"
#include "NativeAVComponent.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
OOIFReturnCode native_jlabsDataComponentReleaseHandle(AVComponentHandle id);
OOIFReturnCode native_jlabsDataComponentGetMultiView(AVComponentData *data, int *count, char ***multiView);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_JLABSDATACONTENTCOMPONENT__ */
#endif /*OIPF JLABS*/

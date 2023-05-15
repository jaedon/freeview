/**************************************************************************************/
/**
 * @file oplprogramme.h
 *
 * Programme
 *
 **************************************************************************************
 **/

 #ifndef __OPLLOGCAPTURE_H__
 #define __OPLLOGCAPTURE_H__

#include "bpltype.h"
#include <hlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void OPL_LogCapture_SetStartInfoOfWidget(const char* widgetName);
void OPL_LogCapture_SetInstallingWidgetInfo(const char* widgetUrl);
void OPL_LogCapture_SetInstalledWidgetInfo(const char* widgetName);
void OPL_LogCapture_SetInstallationFailReasonOfWidget(int failReason);

#ifdef __cplusplus
 }
#endif

 #endif

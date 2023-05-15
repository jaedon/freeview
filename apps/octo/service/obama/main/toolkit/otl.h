/* $Header: $ */

#ifndef OTL_H__
#define OTL_H__

// OTL OTL
// OBAMA Toolkit Library
// 이름 구림

#ifdef __cplusplus
extern "C" {
#endif

#include <hlib.h>
#include <dlib.h>
#include <octo_common.h>

#include "converter/otl_conv_common.h"
#include "converter/otl_conv_service.h"
#include "converter/otl_conv_recording.h"
#include "converter/otl_conv_operator.h"

#include "recording/otl_hjm.h"

#include "upgrade/otl_upgrade.h"

#include "ramdisk/otl_ramdisk.h"

#include "linkedlist.h"

#include "xml/otl_xml.h"

#include "log_report/otl_log_report.h"

HERROR	OTL_Init (void);

#ifdef __cplusplus
}
#endif

#endif // OTL_H__



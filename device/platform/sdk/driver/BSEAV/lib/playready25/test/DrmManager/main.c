/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <DRMManagerTestAPI.h>
#include <MeteringTestAPI.h>
#include <HDSTestAPI.h>
#include <drmsecureclock.h>
#include <drmantirollbackclock.h>
#include <drmrevocation.h>
#include <drmapprevocation.h>
#include <drmdevicerevocation.h>
#include <drmwmdrmnet.h>
#include <drmdla.h>
#include <drmsyncstore.h>


static DRM_BOOL DRM_CALL DRM_AreDLAAndLicenseSyncSupported( DRM_VOID )
{
    return DRM_WMDRMDLA_IsWMDRMDLASupported() && DRM_SNC_IsLicenseSyncSupported();
}


IMPLEMENT_DEFAULT_WRAPTEST

BEGIN_TEST_SCRIPT_LIST
    TEST_SCRIPT_ENTRY("DailyTestDrmManager")
    TEST_SCRIPT_ENTRY("TestDrmMgrSecLevel150")
    TEST_SCRIPT_ENTRY("DailyTestDrmManagerInternal")

#if !DRM_SUPPORT_SECUREOEMHAL
    TEST_SCRIPT_ENTRY("DailyTestDrmManagerCocktailEncryptedXMR")
#endif

    TEST_SCRIPT_ENTRY_CONDITIONAL("TestDrmMgrSecureClockOn", DRM_CLK_IsSecureClockSupported)

#if DRM_SUPPORT_LOCKING
    TEST_SCRIPT_ENTRY("TestDrmMgrFileLockingOn")
#endif

    TEST_SCRIPT_ENTRY_CONDITIONAL("TestDrmMgrAntiRollback", DRM_ARCLK_IsAntirollbackClockSupported)

    TEST_SCRIPT_ENTRY_CONDITIONAL("TestDrmMgrDlaLicSync", DRM_AreDLAAndLicenseSyncSupported)
#if DRM_TEST_SUPPORT_NET_IO
    TEST_SCRIPT_ENTRY_CONDITIONAL("TstDrmManagerDLA", DRM_WMDRMDLA_IsWMDRMDLASupported)
    TEST_SCRIPT_ENTRY_CONDITIONAL("TstDrmManagerDLAInternal", DRM_WMDRMDLA_IsWMDRMDLASupported)
#endif /* DRM_TEST_SUPPORT_NET_IO */

    /* Check of All Possible Revocation Settings */
    TEST_SCRIPT_ENTRY_CONDITIONAL("TestDrmMgrRevOn", DRM_REVOCATION_IsRevocationSupported)
    TEST_SCRIPT_ENTRY_CONDITIONAL("TestDrmMgrAppRevOn", DRM_APPREVOCATION_IsAppRevocationSupported)
    TEST_SCRIPT_ENTRY_CONDITIONAL("TestDrmMgrDeviceRevOn", DRM_DEVICEREVOCATION_IsDeviceRevocationSupported)
    TEST_SCRIPT_ENTRY_CONDITIONAL("TestDrmMgrWMDRMNDRevOn", DRM_WMDRMNET_IsWmdrmnetSupported)
    TEST_SCRIPT_ENTRY_CONDITIONAL("TestDrmMgrAppAndDeviceRevOn", DRM_REVOCATION_AreAppAndDeviceRevocationSupported)
    TEST_SCRIPT_ENTRY_CONDITIONAL("TestDrmMgrAppAndWMDRMNDRevOn", DRM_REVOCATION_AreAppAndWmdrmnetRevocationSupported)
    TEST_SCRIPT_ENTRY_CONDITIONAL("TestDrmMgrDeviceAndWMDRMNDRevOn", DRM_REVOCATION_AreDeviceAndWmdrmnetRevocationSupported)
    TEST_SCRIPT_ENTRY_CONDITIONAL("TestDrmMgrAllRevOn", DRM_REVOCATION_AreAppAndDeviceAndWmdrmnetRevocationSupported)
#if DRM_SUPPORT_PROFILING
    TEST_SCRIPT_ENTRY("TestDrmMgrProfilingOn")
#endif
END_TEST_SCRIPT_LIST

BEGIN_TEST_MAIN(DrmManager)
    BEGIN_TEST_MODULE_LIST
        PKTEST_MODULE_ENTRY(DrmManager)
        PKTEST_MODULE_ENTRY(Metering)
        PKTEST_MODULE_ENTRY( HDS )
    END_TEST_MODULE_LIST
END_TEST_MAIN

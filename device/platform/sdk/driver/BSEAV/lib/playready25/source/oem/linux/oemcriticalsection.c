/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_OEMCRITICALSECTION_C
#include <pthread.h>
#include <oem.h>
#include <drmerr.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_VOID DRM_CALL Oem_CritSec_Initialize(
    __inout OEM_CRITICAL_SECTION *pCS )
{
    pthread_mutex_t* m = (pthread_mutex_t*)pCS;
    pthread_mutexattr_t mattr;
    pthread_mutexattr_init(&mattr);
    pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(m, &mattr);
}

DRM_API DRM_VOID DRM_CALL Oem_CritSec_Delete(
    __inout OEM_CRITICAL_SECTION *pCS )
{
    pthread_mutex_t *m = (pthread_mutex_t*)pCS;
    pthread_mutex_destroy(m);
}

DRM_API DRM_VOID DRM_CALL Oem_CritSec_Enter(
    __inout OEM_CRITICAL_SECTION *pCS )
{
    pthread_mutex_t* m = (pthread_mutex_t*)pCS;
    pthread_mutex_lock(m);
}

DRM_API DRM_VOID DRM_CALL Oem_CritSec_Leave(
    __inout OEM_CRITICAL_SECTION *pCS )
{
    pthread_mutex_t* m = (pthread_mutex_t*)pCS;
    pthread_mutex_unlock(m);
}



EXIT_PK_NAMESPACE_CODE;

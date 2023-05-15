/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __NFERR_H__
#define __NFERR_H__

#include <nrdbase/NFErrorStack.h>
#include <nrdbase/NFErrorCode.h>

namespace netflix {

// Just remap NFErr's to NFErrorStack's
typedef NFErrorStack NFErr;

}

#endif // __NFERR_H__

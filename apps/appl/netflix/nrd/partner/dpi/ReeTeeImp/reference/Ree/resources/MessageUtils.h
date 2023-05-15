/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef MESSAGE_UTILS_H_
#define MESSAGE_UTILS_H_

#include "IMessage.h"

uint32_t getRPCParamDescriptionOfValueTypeParam(nfValue_t *value);

uint32_t getRPCParamDescriptionOfMemoryReferenceTypeParam(nfMemoryReference_t *memRef);


#endif /* MESSAGE_UTILS_H_ */
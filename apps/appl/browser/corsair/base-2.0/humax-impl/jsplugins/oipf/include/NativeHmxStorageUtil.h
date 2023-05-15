
#ifndef __NATIVEHMXSTORAGEUTIL_H_
#define __NATIVEHMXSTORAGEUTIL_H_

#include "NativeOOIFTypes.h"

typedef void* PhysicalStorageHandle;

typedef struct
{
	char* uuid;
	OOIFNumber totalSpace;
} PhysicalStorageData;

typedef void* LogicalStorageHandle;

typedef struct
{
	char* id;
	char* label;
	OOIFNumber totalSpace;
	OOIFNumber freeSpace;
	char* formatType;
} LogicalStorageData;

#endif // __NATIVEHMXSTORAGEUTIL_H_

#ifndef GARBAGECOLLECTOR_C_H
#define GARBAGECOLLECTOR_C_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool GC_Init(void);
bool GC_DeInit(void);
bool GC_AddMemory(void*address, size_t size);
bool GC_RemoveMemory(void*address);

#ifdef __cplusplus
}
#endif

#endif // GARBAGECOLLECTOR_C_H

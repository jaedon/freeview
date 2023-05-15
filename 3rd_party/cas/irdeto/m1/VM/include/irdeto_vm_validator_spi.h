#ifndef VM_VALIDATOR_SPI_H
#define VM_VALIDATOR_SPI_H

#include "irdeto_vm_validator_type.h"

typedef void *uc_mutex_handle;
/* 
* The following method must be implemented by the integrator.
* They are called by the vm validator library.
*/
uc_result UniversalClientSPI_Mutex_Open(uc_mutex_handle * pMutexHandle);
uc_result UniversalClientSPI_Mutex_Close(uc_mutex_handle * pMutexHandle);
uc_result UniversalClientSPI_Mutex_Lock(uc_mutex_handle mutexHandle);
uc_result UniversalClientSPI_Mutex_Unlock(uc_mutex_handle mutexHandle);
uc_result UniversalClientSPI_DateTime_GetTimeOfDay(uc_time* pCurrentTime);
void* UniversalClientSPI_Memory_Malloc(uc_uint32 bytes);
void UniversalClientSPI_Memory_Free(void * lpVoid);

/* 
* This method is called by vm validator library to load the bytecode file.
* The implementation of this method must allocate a buffer, then copy the content of the bytecode file
* into the buffer and return them back through parameter outbuf. This buffer is freed by validator.
*/
uc_uint32 UniversalClientSPI_File_LoadImage(uc_char* imageName,uc_buffer_st* outbuf);

/* 
* The following method must be implemented by the integrator.
* They are called inside validator library.
*/
uc_uint32 UniversalClient_Stdlib_strlen(const uc_char *pString);
void *UniversalClient_Stdlib_memcpy(void *pDest,const void *pSource,uc_uint32 count);
void *UniversalClient_Stdlib_memset(void *dest,uc_uint8 c,uc_uint32 count);
uc_sint32 UniversalClient_Stdlib_memcmp(const uc_uint8 *pBuf1,const uc_uint8 *pBuf2,uc_uint32 len);
uc_sint32 UniversalClient_Stdlib_printf(const uc_char* pFormat,...);
uc_uint32 UniversalClient_Stdlib_sprintf(uc_char *buffer, const uc_char *format,...);

#endif

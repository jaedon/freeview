#ifndef VM_VALIDATOR_API_H
#define VM_VALIDATOR_API_H

#include "irdeto_vm_validator_type.h"
/* This method is the entry of the testing. 
Integrators only need to call this method to get the testing result.
loopCount is the count of the internal loops, its value could be decided by CDI. 
*/
void Run(uc_char* imageName, uc_uint32 executeCnt);

#endif


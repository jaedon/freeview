#include "bstd.h"

int BCHIPIF_Init(char *hostname);
int BCHIPIF_Close(void) ;

int BCHIPIF_ReadRegister( uint32_t reg_addr, uint32_t *data );
int BCHIPIF_WriteRegister( uint32_t reg_addr, uint32_t data );
int BCHIPIF_ReadMemory( uint32_t mem_addr, void *data, size_t size );
int BCHIPIF_WriteMemory( uint32_t mem_addr, void *data, size_t size );

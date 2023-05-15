#ifndef __HUMAX_RTS_H__
#define __HUMAX_RTS_H__

typedef struct {
    uint32_t uId;
    uint32_t uValue;
} RTS_CLIENT;

/*
  Model specific RTS will be updated with table below if defined during nexus platform init.
  Completes table first and updates gpRTS_Optimized valued from NULL to this table's address.  
  CAUTION!!: Last Entry's uId value shall be 0xFFFFFFFF
RTS_CLIENT gaMODEL_Optimized_RTS[] = {              
		{0,    0x0031f00a},                               
		~~~~~~~~~~~~~~~~~~                              
		{127,  0x0069200c},                               
		{0xFFFFFFFF, 0xFFFFFFFF}                          
}; 
*/                                                  
                                                      
RTS_CLIENT *gpRTS_Optimized = NULL; /*gaMODEL_Optimized_RTS;*/
RTS_CLIENT *gpRTS_Optimized_MEMC1 = NULL; /*gaMODEL_Optimized_RTS;*/
#endif


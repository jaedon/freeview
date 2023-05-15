#ifndef DSMCC_CAROUSEL
#define DSMCC_CAROUSEL

#include "dsmcc-cache.h"
#include "dsmcc-receiver.h"

typedef struct object_carousel 
{
	Cache_All_t *pstCacheAll;
	
	struct cache_module_data	*pstCacheModuleData; 
	struct dsmcc_dsi 	*pstDsi;

	/*
		# downloadId_inOjectCarousel
		
		1. Data Carousel 
			dsmccMessageHeader == dsmccDownloadDataHedader 이므로 transcation_id 와 download_id가 동일하다.
		2. Object Carousel
			dsmccMessageHeader 의 transaction_id 는 다른 용도 사용됨..13818-6 에서 2.DSM-CC Message Header 참조..
			ProfileBody 의 BIOP::ObjectLocation 의 carousel_id 와 DII 의 download_id와 동일함...

		추후에 Data Carousel 과 Object Carousel 을 동일한 DsmccLib를 사용하고 싶다면 download_Id를 가져오는 위치를 잘 파악해야 됨..
	*/

	unsigned long downloadId_inObjectCarousel;	 
	unsigned short usPid;
}ObjectCarousel_t;


#endif

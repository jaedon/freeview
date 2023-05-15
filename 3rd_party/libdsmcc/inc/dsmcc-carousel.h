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
			dsmccMessageHeader == dsmccDownloadDataHedader �̹Ƿ� transcation_id �� download_id�� �����ϴ�.
		2. Object Carousel
			dsmccMessageHeader �� transaction_id �� �ٸ� �뵵 ����..13818-6 ���� 2.DSM-CC Message Header ����..
			ProfileBody �� BIOP::ObjectLocation �� carousel_id �� DII �� download_id�� ������...

		���Ŀ� Data Carousel �� Object Carousel �� ������ DsmccLib�� ����ϰ� �ʹٸ� download_Id�� �������� ��ġ�� �� �ľ��ؾ� ��..
	*/

	unsigned long downloadId_inObjectCarousel;	 
	unsigned short usPid;
}ObjectCarousel_t;


#endif

#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <zlib.h>

#include "dsmcc-receiver.h"
#include "dsmcc-descriptor.h"
#include "dsmcc-cache.h"
#include "dsmcc-biop.h"
#include "dsmcc-util.h"
#include "libdsmcc.h"


#define ______EXTERN_FUNCTION______
extern void dsmcc_biop_process_data(Cache_All_t *pstCacheAll, CacheModuleData_t *pstCacheModuleData);

#define ______STATIC_FUNCTION______
static void dsmcc_add_module_info(struct dsmcc_status *pstDsmccStatus, DsmccDii_t	*pstDii, ObjectCarousel_t *pstObjCarousel)
{
	int i, num_blocks, found;
	CacheModuleData_t *pstCacheModuleData= NULL;
	DsmccDes_t 	*desc, *last;

	/* loop through modules and add to cache list if no module with
	 * same id or a different version. */

	for(i = 0; i < pstDii->number_modules; i++)
	{
		found = 0;

		for (pstCacheModuleData = pstObjCarousel->pstCacheModuleData; pstCacheModuleData!=NULL; pstCacheModuleData=pstCacheModuleData->next)
		{
			if((pstCacheModuleData->carousel_id == pstDii->download_id)
				&& (pstCacheModuleData->module_id == pstDii->pstDsmccModules[i].module_id))
			{
				/* already known */
				if(pstCacheModuleData->module_version==pstDii->pstDsmccModules[i].module_version)
				{
					found =  1;
					break;
				}
				else
				{
					/* If the cache is already received perfectly, then do not remove it */
					if (pstCacheModuleData->cached == 1)
					{
						found =  1;
						break;
					}

					if(pstCacheModuleData->pstDsmccDescriptors!= NULL)
					{
						desc = pstCacheModuleData->pstDsmccDescriptors;
						while(desc != NULL)
						{
							last = desc;
							desc = desc->next;
							dsmcc_desc_free(last);
						}
					}

					if(pstCacheModuleData->pucModuleData!= NULL)
					{
						free(pstCacheModuleData->pucModuleData);
					}

					if(pstCacheModuleData->bstatus != NULL)
					{
						free(pstCacheModuleData->bstatus);
					}

					if(pstCacheModuleData->blocks != NULL)
					{
						DsmccDdb_t	*pstDdb = pstCacheModuleData->blocks;
						DsmccDdb_t	*pstNextDdb = NULL;

						while(pstDdb != NULL)
						{
							pstNextDdb = pstDdb->next;
							if ( pstDdb->blockdata != NULL )
							{
								free(pstDdb->blockdata);
							}
							free(pstDdb);
							pstDdb = pstNextDdb;
						}
					}

                	if(pstCacheModuleData->prev != NULL)
					{
						pstCacheModuleData->prev->next = pstCacheModuleData->next;
						if(pstCacheModuleData->next!=NULL)
						{
							pstCacheModuleData->next->prev=pstCacheModuleData->prev;
						}
                	}
					else
					{
						pstObjCarousel->pstCacheModuleData = pstCacheModuleData->next;
						if(pstCacheModuleData->next!=NULL)
						{
				    			pstCacheModuleData->next->prev = NULL;
						}
					}

                	free(pstCacheModuleData);
					break;
				}
			}
		}

		// 저장된 module 정보가 없음..그래서 추가해야 됨..
	   	if(found == 0)
		{
			if(pstObjCarousel->pstCacheModuleData != NULL) // 저장된 Module 정보가 있다.
			{
				for (pstCacheModuleData=pstObjCarousel->pstCacheModuleData; pstCacheModuleData->next!=NULL; pstCacheModuleData=pstCacheModuleData->next) {;}

				pstCacheModuleData->next = (CacheModuleData_t *)malloc(sizeof(CacheModuleData_t));
				pstCacheModuleData->next->prev = pstCacheModuleData;
				pstCacheModuleData = pstCacheModuleData->next;
			}
			else // 저장된 Module 정보가 하나도 없다..
			{
				pstObjCarousel->pstCacheModuleData = (CacheModuleData_t *)malloc(sizeof(CacheModuleData_t));
				pstCacheModuleData = pstObjCarousel->pstCacheModuleData;
				pstCacheModuleData->prev = NULL;
			}

			pstCacheModuleData->carousel_id 	= pstDii->download_id;
			pstCacheModuleData->module_id 		= pstDii->pstDsmccModules[i].module_id;
			pstCacheModuleData->module_version 	= pstDii->pstDsmccModules[i].module_version;
			pstCacheModuleData->module_size 	= pstDii->pstDsmccModules[i].module_size;
			pstCacheModuleData->ulReadPosition 	= pstCacheModuleData->block_num = 0;
			num_blocks 						= pstCacheModuleData->module_size / pstDii->block_size;

			if((pstCacheModuleData->module_size % pstDii->block_size) != 0)
			{
				num_blocks++;
			}

			pstCacheModuleData->bstatus=(char*)malloc(((num_blocks/8)+1)*sizeof(char));
			bzero(pstCacheModuleData->bstatus, (num_blocks/8)+1);

 			pstCacheModuleData->pucModuleData= NULL;
			pstCacheModuleData->next = NULL;
			pstCacheModuleData->blocks = NULL;
			pstCacheModuleData->assoc_tag = pstDii->pstDsmccModules[i].modinfo.stBiopTap.assoc_tag;

			/* Steal the descriptors  TODO this is very bad... */
			pstCacheModuleData->pstDsmccDescriptors = pstDii->pstDsmccModules[i].modinfo.descriptors;
			pstDii->pstDsmccModules[i].modinfo.descriptors = NULL;
			pstCacheModuleData->cached = 0;
		}
	}
}


static void dsmcc_add_module_data (struct dsmcc_status *pstDsmccStatus, DsmccDownloadDataHeader_t *pstDownloadDataHeader, DsmccDdb_t *pstDdb, unsigned char *pucRawData, unsigned short usPid)
{
	unsigned char *data = NULL;
	unsigned long data_len = 0;
	unsigned int	unCacheStatus = 0;

	int index=0, ret=0, found = 0;
	ObjectCarousel_t	*pstObjectCarousel = NULL;
	CacheModuleData_t *pstCacheModuleData = NULL;
	DsmccDes_t*desc = NULL;
	DsmccDdb_t *lb, *pb, *nb;

	// Cache 된 정보 중에 동일한 Pid 와 DownloadId를 찾아라..
	for (index=0;index<MAXCAROUSELS;index++)
	{
		pstObjectCarousel = &pstDsmccStatus->stObjCarousels[index];
		if(pstObjectCarousel != NULL)
		{
			if((pstObjectCarousel->downloadId_inObjectCarousel== pstDownloadDataHeader->download_id)
				&& pstObjectCarousel->usPid == usPid)
			{
				break;
			}
		}
	}

	//printf("[dsmcc_add_module_data] index (%d) \n", index);

	// 동일한 Pid 와 DownloadId를 못찾은 경우..
	if(index == MAXCAROUSELS)
	{
		/* TODO carousel not yet known! is this possible ? */
		return;
	}

	// Cache 된 정보 중에 동일한 ModuleId 와 DownloadId를 찾아라..
	for (pstCacheModuleData = pstObjectCarousel->pstCacheModuleData; pstCacheModuleData != NULL; pstCacheModuleData = pstCacheModuleData->next)
	{
		if((pstCacheModuleData->carousel_id == pstDownloadDataHeader->download_id)
			&& (pstCacheModuleData->module_id == pstDdb->module_id))
		{
			found = 1;
			break;
	    }
	}

	// 동일한 ModuleId 와 DownloadId를 못찾은 경우..
	if(found == 0)
	{
		return;
	}

	// 동일한 module version 이 있다...
	if(pstCacheModuleData->module_version == pstDdb->module_version)
	{
		if(pstCacheModuleData->cached)
		{
			return;/* Already got it */
		}
		else
		{
			/* Check if we have this block already or not. If not append to list */
			if(BLOCK_GOT(pstCacheModuleData->bstatus, pstDdb->block_number) == 0)
			{
				if((pstCacheModuleData->blocks==NULL)
					|| (pstCacheModuleData->blocks->block_number>pstDdb->block_number) )
				{
					nb=pstCacheModuleData->blocks;	/* NULL or previous first in list */
					pstCacheModuleData->blocks=(DsmccDdb_t*)malloc(sizeof(DsmccDdb_t));
					lb=pstCacheModuleData->blocks;
				}
				else
				{
					for(pb=lb=pstCacheModuleData->blocks; (lb!=NULL) && (lb->block_number < pstDdb->block_number); pb=lb,lb=lb->next)
					{ ; }

					nb = pb->next;
					pb->next = (DsmccDdb_t*)malloc(sizeof(DsmccDdb_t));
					lb=pb->next;
				}

				lb->module_id = pstDdb->module_id;
				lb->module_version = pstDdb->module_version;
				lb->block_number = pstDdb->block_number;
				lb->blockdata = (unsigned char*)malloc(pstDdb->len);
				memcpy(lb->blockdata, pucRawData, pstDdb->len);
				lb->len = pstDdb->len;
				pstCacheModuleData->ulReadPosition += pstDdb->len;
				lb->next = nb;
				BLOCK_SET(pstCacheModuleData->bstatus, pstDdb->block_number);
			}
		}

		if(pstCacheModuleData->ulReadPosition >= pstCacheModuleData->module_size)
		{
			pstObjectCarousel->pstCacheAll->usPid= pstObjectCarousel->usPid;

			/* Re-assemble the blocks into the complete module */
			pstCacheModuleData->pucModuleData = (unsigned char*)malloc(pstCacheModuleData->module_size);
			pstCacheModuleData->ulReadPosition = 0;
			pb=lb=pstCacheModuleData->blocks;
			while(lb!=NULL)
			{
				memcpy(pstCacheModuleData->pucModuleData+pstCacheModuleData->ulReadPosition,lb->blockdata,lb->len);
				pstCacheModuleData->ulReadPosition += lb->len;

				pb=lb; lb=lb->next;

				if(pb->blockdata!=NULL)
				{
					free(pb->blockdata);
				}

				free(pb);
			}

			pstCacheModuleData->blocks = NULL;

			/* Uncompress.... TODO - scan for compressed descriptor */
			for(desc=pstCacheModuleData->pstDsmccDescriptors;desc !=NULL; desc=desc->next)
			{
				if(desc && (desc->tag == TAG_COMPRESSED_MODULE)) { break; }
			}

			if(desc != NULL)
			{
				data_len=desc->data.compressed.original_size+1;
				data = (unsigned char *)malloc(data_len+1);

				ret = uncompress(data, &data_len, pstCacheModuleData->pucModuleData, pstCacheModuleData->module_size);
				if(ret == Z_DATA_ERROR)
				{
					if(data != NULL) { free(data); }
					pstCacheModuleData->ulReadPosition = 0;
					if(pstCacheModuleData->pucModuleData!=NULL)
					{
						free(pstCacheModuleData->pucModuleData);
						pstCacheModuleData->pucModuleData = NULL;
					}

					return;
				}
				else if(ret == Z_BUF_ERROR)
				{
					if(data != NULL) { free(data); }
					pstCacheModuleData->ulReadPosition = 0;
					if(pstCacheModuleData->pucModuleData!=NULL)
					{
						free(pstCacheModuleData->pucModuleData);
						pstCacheModuleData->pucModuleData = NULL;
					}

					return;
				}
				else if(ret == Z_MEM_ERROR)
				{
					if(data != NULL) { free(data); }
					pstCacheModuleData->ulReadPosition = 0;
					if(pstCacheModuleData->pucModuleData!=NULL)
					{
						free(pstCacheModuleData->pucModuleData);
						pstCacheModuleData->pucModuleData = NULL;
					}

					return;
				}

				if(pstCacheModuleData->pucModuleData!=NULL)
				{
					free(pstCacheModuleData->pucModuleData);
				}

				pstCacheModuleData->pucModuleData = data;

				// Return list of streams that directory needs
				dsmcc_biop_process_data(pstObjectCarousel->pstCacheAll, pstCacheModuleData);
				pstCacheModuleData->cached = 1;
			}
			else
			{
		   		/* not compressed */

				// Return list of streams that directory needs
				dsmcc_biop_process_data(pstObjectCarousel->pstCacheAll, pstCacheModuleData);
				pstCacheModuleData->cached = 1;
			}

			for(pstCacheModuleData = pstObjectCarousel->pstCacheModuleData; pstCacheModuleData != NULL; pstCacheModuleData = pstCacheModuleData->next)
			{
	    		if(pstCacheModuleData->cached == 0)
				{
					unCacheStatus = 1;
					break;
	    		}
			}

			if (unCacheStatus == 0)
			{
				pstCacheModuleData = pstObjectCarousel->pstCacheModuleData;
				if (g_libdsmcc_complete_callback != NULL)
				{
					g_libdsmcc_complete_callback (pstObjectCarousel->usPid, pstObjectCarousel); /* if you need the more infomation regarding to the biop, refer the filecache struct */
				}

			}
		}
	}
}

static unsigned short dsmcc_CheckDsiInfoByPid(struct dsmcc_status *status, unsigned short usPid)
{
	unsigned short 	usIndex ;
	ObjectCarousel_t 	*pstObjectCarousel = NULL;

	/* Find which object carousel this pid's data belongs to */
	for(usIndex=0;usIndex<MAXCAROUSELS;usIndex++)
	{
		pstObjectCarousel = &status->stObjCarousels[usIndex];
		if (pstObjectCarousel->usPid == usPid)
		{
			if(pstObjectCarousel->pstDsi == NULL)
			{
				break;
			}
		}
	}

	if (usIndex == MAXCAROUSELS)
	{
		for (usIndex=0; usIndex<MAXCAROUSELS; usIndex++)
		{
			pstObjectCarousel = &status->stObjCarousels[usIndex];
			if (pstObjectCarousel->pstDsi == NULL)
			{
				break;
			}
		}
	}

	if(usIndex == MAXCAROUSELS)
		return 1;
	else
		return 0;
}

// 13818-6 , 7.3.3 Download Data Block.
static int dsmcc_DownloadDataBlock(struct dsmcc_status *pstDsmccStatus, DsmccDownloadDataHeader_t *pstDownloadDataHeader, unsigned char *pucRawData, int nSecLen, unsigned usPid)
{
	int offset =0;
	DsmccDdb_t *pstDdb;

	pstDdb = (DsmccDdb_t *)malloc(sizeof(DsmccDdb_t));
	if(pstDdb == NULL)
	{
		return -1;
	}

	// module Id (2byte)
	pstDdb->module_id = (pucRawData[offset] << 8) | pucRawData[offset+1];
	offset+=2;

	// module version(1byte)
	pstDdb->module_version = pucRawData[offset];
	offset+=1;

	// reserved byte(1byte)
	offset+=1;

	// block number(2byte)
	pstDdb->block_number = (pucRawData[offset] << 8) | pucRawData[offset+1];
	offset+=2;

	pstDdb->len = pstDownloadDataHeader->message_len - 6;

	pstDdb->next = NULL;	/* Not used here, used to link all data blocks in order in AddModuleData. Hmmm. */
	pstDdb->blockdata = NULL;

	dsmcc_add_module_data(pstDsmccStatus, pstDownloadDataHeader, pstDdb, pucRawData+offset, usPid);

	free(pstDdb);

	return 0;
}

static int dsmcc_HeaderMessage(DsmccMessageHeader_t *pstMsgHeader, unsigned char *Data)
{
	// protocol Discriminator
        pstMsgHeader->protocol = Data[0];

	// protcoal value shall be 0x11
        if(pstMsgHeader->protocol != 0x11) {
                return -1;
        }

	// dsmcc type
	// 13818-6,  Table 2-2 MPEG-2 DSM-CC dsmccType values 참조.
	// hbbtv는 download message 만 처리하기 위해서 dsmcctype = 0x03 인 것만 처리한다.
        pstMsgHeader->dsmcctype = Data[1];
        if(pstMsgHeader->dsmcctype != 0x03)
	{
                return -1;
        }

	// message id
        pstMsgHeader->message_id = (Data[2] << 8) | Data[3];

	// transcation id
	// Object Carousel 인 경우 DII 의 Download_ID 와 동일하지 않음.
	// Data Carousel 인 경우 DII 의 Download_ID 와 동일하지 않음.
        pstMsgHeader->transaction_id = (Data[4] << 24) | (Data[5] << 16) | (Data[6] << 8) | Data[7];

        // reverved
        // Data[8] */

	// adapationLength
	// Data[9] */

	// message length
        pstMsgHeader->message_len = (Data[10] << 8) | Data[11];

	// 확인 필요..
        if(pstMsgHeader->message_len > 4076)  /* Beyond valid length */
	{
		// Dsmcc Section Length
		// The value in this field shall not exceed 4093
		// (indicating a section maximum data length of 4084 bytes, following the last-section-number field and up to the CRC_32/checksum field).
		// Max length (4093 byte) -  DSM-CC Section Header (8byte) - checksum/CRC_32(4byte)
		return -1;
        }

	return 0;
}

static int dsmcc_HeaderDownloadData(DsmccDownloadDataHeader_t *pstDownloadDataHeader, unsigned char *Data)
{
	// protocol Discriminator
        pstDownloadDataHeader->protocol = Data[0];

	// protcoal value shall be 0x11
        if(pstDownloadDataHeader->protocol != 0x11) {
                return 1;
        }

	// dsmcc type
	// 13818-6,  Table 2-2 MPEG-2 DSM-CC dsmccType values 참조.
	// hbbtv는 download message 만 처리하기 위해서 dsmcctype = 0x03 인 것만 처리한다.
        pstDownloadDataHeader->dsmcctype = Data[1];
        if(pstDownloadDataHeader->dsmcctype != 0x03)
	{
                return 1;
        }

	// message id
        pstDownloadDataHeader->message_id = (Data[2] << 8) | Data[3];

	// download id
        pstDownloadDataHeader->download_id= (Data[4] << 24) | (Data[5] << 16) | (Data[6] << 8) | Data[7];

        // reverved
        // Data[8] */

	// adapationLength
	// Data[9] */

	// message length
        pstDownloadDataHeader->message_len = (Data[10] << 8) | Data[11];

	// 확인 필요..
        if(pstDownloadDataHeader->message_len > 4076)  /* Beyond valid length */
	{
		// Dsmcc Section Length
		// The value in this field shall not exceed 4093
		// (indicating a section maximum data length of 4084 bytes, following the last-section-number field and up to the CRC_32/checksum field).
		// Max length (4093 byte) -  DSM-CC Section Header (8byte) - checksum/CRC_32(4byte)
		return 1;
        }

	return 0;
}

static unsigned short dsmcc_FindDsiInfoByPid(struct dsmcc_status *status, unsigned short usPid)
{
	unsigned short 	usIndex ;
	ObjectCarousel_t	*pstObjCarousel = NULL;

	/* Find which object carousel this pid's data belongs to */
	for(usIndex=0;usIndex<MAXCAROUSELS;usIndex++)
	{
		pstObjCarousel = &status->stObjCarousels[usIndex];
		if (pstObjCarousel->usPid == usPid)
		{
			if(pstObjCarousel->pstDsi == NULL)
			{
				break;
			}
		}
	}

	if (usIndex == MAXCAROUSELS)
	{
		for (usIndex=0; usIndex<MAXCAROUSELS; usIndex++)
		{
			pstObjCarousel = &status->stObjCarousels[usIndex];
			if (pstObjCarousel->pstDsi == NULL)
			{
				break;
			}
		}
	}

	return usIndex;
}


static int dsmcc_ServiceGatewayInfo(struct dsmcc_status *pstDsmccStatus, unsigned char *pucRawData, unsigned short usPid)
{
	int ret, offset = 0;
	unsigned short		usIndex;
	ObjectCarousel_t	*pstObjCarousel;

	usIndex = dsmcc_FindDsiInfoByPid(pstDsmccStatus, usPid);
	if(usIndex == MAXCAROUSELS)
	{
		return -1;
	}

	pstObjCarousel = &pstDsmccStatus->stObjCarousels[usIndex];
	if(pstObjCarousel == NULL)
	{
		return -1;
	}

	pstObjCarousel->pstDsi = (DsmccDsi_t *)malloc(sizeof(DsmccDsi_t));
	if(pstObjCarousel->pstDsi== NULL)
	{
		return -1;
	}

	memset(pstObjCarousel->pstDsi, 0x00, sizeof(DsmccDsi_t));
	pstObjCarousel->usPid = usPid;

	// IOR_IOP 처리...
	ret = dsmcc_biop_process_Ior(&(pstObjCarousel->pstDsi->stBiopIor), pucRawData);
	if(ret < 0)
	{
		if(pstObjCarousel->pstDsi != NULL)
		{
			free(pstObjCarousel->pstDsi);
		}
		return -1;
	}
	offset +=ret;

	pstObjCarousel->downloadId_inObjectCarousel = pstObjCarousel->pstDsi->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.carousel_id;

	/* skip taps and context */
	offset+=2;

	// profile data 처리 후 나머지 정보에 대한 처리 필요함..
	/* TODO process descriptors */
	// 실제로 사용하지 않는 데이터인데..왜 저장을 할까요 ?
	pstObjCarousel->pstDsi->user_data_len = pucRawData[offset++];
	if(pstObjCarousel->pstDsi->user_data_len > 0)
	{
		pstObjCarousel->pstDsi->user_data = (unsigned char *)malloc(pstObjCarousel->pstDsi->user_data_len);
		memcpy(pstObjCarousel->pstDsi->user_data, pucRawData+offset, pstObjCarousel->pstDsi->user_data_len);
	}
	else
	{
		pstObjCarousel->pstDsi->user_data = NULL;
	}

	return 0;
}

// 13818-6 Table 7-12 DownloadServerInitiate message
static int dsmcc_DownloadServerInitiate(struct dsmcc_status *pstDsmccStatus, unsigned char *pucRawData, int nSecLen, unsigned short usPid)
{
	int	nCompDescLength =0, nPrivateDataLen = 0, nTypeId ;
        int 	offset= 0, ret;

	// Server ID (20byte)
	offset += 20;

	// compatibilydescriptorlength (2byte)
	// TS_102 006 V.1.2.1 spec에서는 compdescriptor를 처리해줘야 된다..
	// IOR(inter object referenced format)에서는 항상 0 이다..
	nCompDescLength = (pucRawData[offset] << 8) | pucRawData[offset+1];
	offset +=2;
        offset += nCompDescLength;

	// privateDataLength(2byte)
	nPrivateDataLen = (pucRawData[offset] << 8) | pucRawData[offset+1];
        offset+=2;

	// 확인 필요..
	// private data 가 없으면 Data Carousel
	if(nPrivateDataLen == 0)
	{
		//????
	}

	// DataCarousel: GroupInfoByte structure + dsmcc_CarouselDescriptors
	// ObjectCarousel: BIOP::ServiceGatewayInformation
	// because we do not know, if DC or OC, we make a check for BIOP
	// U-U Objects type_id "srg\0"  and "DSM:"
	// (normally we should remember a databroadcast_id for decision)

	// private_data_len 가 byte 이상인 경우는 private data 로 간주  ???????
	if(nPrivateDataLen >= 8 )
	{
		nTypeId = ((pucRawData[offset+4] << 24) | (pucRawData[offset+5] << 16) | (pucRawData[offset+6] << 8)  | (pucRawData[offset+7]));
	}
	else
	{
		nTypeId = 0;
	}

	//type id 중 상위 4byte가 "srg\0"  and "DSM: 가 아닌 경우는 Data Carousel
	// ETSI TR 101 202 V.1.2.1 Table4-4 : U-U Object type_id 참조.
	if (nTypeId != 0x73726700 && nTypeId != 0x53443a4d )
	{
		// Data Carousel
		// GroupInfoIndication.
		return 0;
	}
	else
	{
		// Pid 로 동일한 Dsi info를 찾는다..
		ret = dsmcc_CheckDsiInfoByPid(pstDsmccStatus, usPid);
		if(ret != 0)
		{
			return 1;
		}

		// offset = Server ID + compatibilitydescriptorlength + Compatibility Descriptor Data + privateDataLength
		ret = dsmcc_ServiceGatewayInfo(pstDsmccStatus, pucRawData+offset, usPid );
	}

	return ret;

}

// 13818-6 , Table7-6 DownloadInfoRepose and DownloadInfoIndication message
static int dsmcc_DownloadInfoIndication(struct dsmcc_status *pstDsmccStatus, unsigned char *pucRawData, int nSecLen, unsigned short usPid)
{
	DsmccDii_t *pstDii;
	ObjectCarousel_t *pstObjectCarousel = NULL;
	int offset=0, ret;
	int nCompDescLength = 0;
	unsigned long ulDownload_id;
	unsigned short usIndex ;

	// download ID (4byte)
	ulDownload_id = (pucRawData[0] << 24) | (pucRawData[1] << 16) | (pucRawData[2] << 8)  | (pucRawData[3]) ;
	offset+=4;

	for(usIndex=0;usIndex<MAXCAROUSELS;usIndex++)
	{
		pstObjectCarousel = &pstDsmccStatus->stObjCarousels[usIndex];

		if(pstObjectCarousel != NULL)
		{
			//find same ESPID & download id
			if((pstObjectCarousel->downloadId_inObjectCarousel == ulDownload_id) && (pstObjectCarousel->usPid == usPid))
			{
				break;
			}
		}
	}

	if(usIndex == MAXCAROUSELS)
	{
		// printf ("[dsmcc_process_section_info] Unknown PID 0x%04x\n", pid);
		/* No known carousels yet (possible?) TODO ! */
		return -1;
	}

	pstDii = (DsmccDii_t*)malloc(sizeof(DsmccDii_t));
	if(pstDii == NULL)
	{
		return -1;
	}

	memset(pstDii, 0x00, sizeof(DsmccDii_t));

	// download ID (4byte)
	pstDii->download_id = ulDownload_id;

	// block size(2byte)
	pstDii->block_size = pucRawData[offset] << 8 | pucRawData[offset+1];
	offset+=2;

	// window size(1byte) + ackPeriod(1byte) + tCDownloadWindow(4byte)
	offset+=6; /* not used fields */

	// tCDownloadSeenario (4byte) - 사용하지 않음.
	pstDii->tc_download_scenario = (pucRawData[offset] << 24) | (pucRawData[offset+1] << 16) | (pucRawData[offset+2] << 8)| pucRawData[offset+3];
	offset+=4;

	//skip unused compatibility descriptor len
	nCompDescLength = (pucRawData[offset] << 8) | pucRawData[offset+1];
	offset +=2;
        offset += nCompDescLength;

	// numberOfMoudules (2byte)
	pstDii->number_modules = (pucRawData[offset] << 8) | pucRawData[offset+1];
	offset+=2;

	if(pstDii->number_modules > 0)
	{
		pstDii->pstDsmccModules = (DsmccModuelInfo_t*)malloc(sizeof(DsmccModuelInfo_t) * pstDii->number_modules);
		memset(pstDii->pstDsmccModules, 0, sizeof(DsmccModuelInfo_t) * pstDii->number_modules);

		for(usIndex = 0; usIndex < pstDii->number_modules; usIndex++)
		{
			// moduleId(2byte)
			pstDii->pstDsmccModules[usIndex].module_id = (pucRawData[offset] << 8) | pucRawData[offset+1];
			offset+=2;

			// moduleSize(4byte)
			pstDii->pstDsmccModules[usIndex].module_size = (pucRawData[offset] << 24) | (pucRawData[offset+1] << 16) | (pucRawData[offset+2] << 8) | pucRawData[offset+3];
			offset+=4;

			// module Version(1byte)
			pstDii->pstDsmccModules[usIndex].module_version = pucRawData[offset];
			offset += 1;

			// module Length(1byte)
			pstDii->pstDsmccModules[usIndex].module_info_len = pucRawData[offset];
			offset += 1;

			// module byte -> 13818-6 11.3.3.2Module Delivery Parameters
			ret = dsmcc_biop_process_module_info(&pstDii->pstDsmccModules[usIndex].modinfo, pucRawData+offset);
			if(ret > 0)
			{
				offset += ret;
			}
			else
			{
				/* TODO error */
			}
		}
	}

	//pstDii->private_data_len = (pucRawData[offset] << 8) | pucRawData[offset+1];
	dsmcc_add_module_info(pstDsmccStatus, pstDii, pstObjectCarousel);

	/* Free most of the memory up... all that effort for nothing */

	for(usIndex = 0; usIndex < pstDii->number_modules; usIndex++)
	{
		if(pstDii->pstDsmccModules[usIndex].modinfo.stBiopTap.selector_data != NULL)
		{
			free(pstDii->pstDsmccModules[usIndex].modinfo.stBiopTap.selector_data);
			pstDii->pstDsmccModules[usIndex].modinfo.stBiopTap.selector_data = NULL;
		}
	}

	free(pstDii->pstDsmccModules);	/* TODO clean up properly... done? */
	free(pstDii);

	return 0;
}

static void dsmcc_UserNetworkMessage(struct dsmcc_status *pstDsmccStatus, unsigned char *pucRawData, int nSecLen, unsigned short usPid)
{
	DsmccMessageHeader_t stMsgHeader;
	int ret;

	memset(&stMsgHeader, 0x00, sizeof(DsmccMessageHeader_t));

	ret = dsmcc_HeaderMessage(&stMsgHeader, pucRawData+DSMCC_MESSAGE_HEADER_OFFSET);
	if(ret <  0)
	{
		return;
	}

	// 13818-6 의 Table 7-4 DSM-CC Download messageId assignments.
	if(stMsgHeader.message_id == DSMCC_MESSAGE_DSI) // DownloadServerInitiate
	{
		ret = dsmcc_DownloadServerInitiate(pstDsmccStatus, pucRawData+DSMCC_DSI_OFFSET, nSecLen, usPid);
		if (ret != 0)
		{
			// error 를 어떻게 처리해야 되나요..?
			printf ("[dsmcc_UserNetworkMessage]  Error DSMCC_MESSAGE_DSI :: PID 0x%04x\n", usPid);
		}
	}
	else if (stMsgHeader.message_id == DSMCC_MESSAGE_DII) // DownloadInfoIndication
	{
		ret = dsmcc_DownloadInfoIndication(pstDsmccStatus, pucRawData+DSMCC_DII_OFFSET, nSecLen, usPid);
		if (ret != 0)
		{
			// error 를 어떻게 처리해야 되나요..?
			printf ("[dsmcc_UserNetworkMessage]  Error DSMCC_MESSAGE_DII :: PID 0x%04x\n", usPid);
		}
	}
	else
	{
		/* Error */
	}
}

static void dsmcc_DownloadDataMessage(struct dsmcc_status *pstDsmccStatus, unsigned char *pucRawData, int nSecLen, unsigned short usPid)
{
	DsmccDownloadDataHeader_t stDownloadDataHeader;
	int ret;

	memset(&stDownloadDataHeader, 0x00, sizeof(DsmccDownloadDataHeader_t));

	ret = dsmcc_HeaderDownloadData(&stDownloadDataHeader, pucRawData+DSMCC_DOWNLOAD_DATA_HEADER_OFFSET);
	if(ret != 0)
	{
		printf("[dsmcc_DownloadDataMessage] ==== dsmcc_HeaderDownloadData error \n");

		return;
	}

	dsmcc_DownloadDataBlock (pstDsmccStatus, &stDownloadDataHeader, pucRawData+DSMCC_DDB_OFFSET, nSecLen, usPid);
}

static void dsmcc_descriptor_list( struct dsmcc_status *pstDsmccStatus, unsigned char *pucRawData, int nSecLen, unsigned short usPid)
{
	unsigned short			usEventId;
	unsigned char			*pBuf = pucRawData, *pEventDescription = NULL, ucTag;
	unsigned int 			uiDescriptorLength = 0, uiNPT_MSB, uiNPT_LSB;

	if ( g_libdsmcc_writedesriptorevent_callback )
	{
		ucTag = pucRawData[0];
		uiDescriptorLength = pucRawData[1];
		usEventId = ( pucRawData[2] << 8 ) | pucRawData[3];

		uiNPT_MSB = pucRawData[7] & 0x01;
		uiNPT_LSB = pucRawData[8] << 24 | pucRawData[9] << 16 | pucRawData[10] << 8 | pucRawData[11];

		if ( uiDescriptorLength < 10 )
		{
			return;
		}

		if ( uiDescriptorLength > 11 )
		{
			pEventDescription = malloc( uiDescriptorLength - 10 );
			if ( pEventDescription == NULL )
			{
				return;
			}
			memcpy( pEventDescription, &pBuf[12], uiDescriptorLength - 10 );
		}

		g_libdsmcc_writedesriptorevent_callback( usEventId, uiNPT_MSB, uiNPT_LSB, pEventDescription, uiDescriptorLength - 10, usPid);

		if ( pEventDescription != NULL )
		{
			free(pEventDescription);
			pEventDescription = NULL;
		}
	}

	return;
}

// dsmcc_ServiceGatewayInfo 함수를 기반으로 만든 parsing function
int dsmcc_ParseDsiSgi (unsigned char *pucRawData, int nSecLen, DsmccDsiInfo_t *pstDsiInfo)
{
	BIOP_Ior_t		stBiopIor;
	int				ret = -1;

	memset(&stBiopIor, 0, sizeof(BIOP_Ior_t));

	// IOR_IOP 처리...
	ret = dsmcc_biop_process_Ior(&stBiopIor, pucRawData);
	if(ret < 0)
	{
		goto END_FUNC;
	}

	pstDsiInfo->biop_connbinder_assoctag = stBiopIor.profile.stBiopProfileBody.stBiopDsmConnbiner.stBiopTap.assoc_tag;
	pstDsiInfo->biop_objloc_carouselid = stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.carousel_id;

	if ((stBiopIor.profile.stBiopProfileBody.stBiopDsmConnbiner.stBiopTap.selector_len == 0x0A) &&
		(stBiopIor.profile.stBiopProfileBody.stBiopDsmConnbiner.stBiopTap.selector_data))
	{
		unsigned char		*selector = stBiopIor.profile.stBiopProfileBody.stBiopDsmConnbiner.stBiopTap.selector_data;

		pstDsiInfo->biop_connbinder_transactionid = ((selector[2] << 24) | (selector[3] << 16) | (selector[4] << 8)	| (selector[5]));
		pstDsiInfo->biop_connbinder_timeout = ((selector[6] << 24) | (selector[7] << 16) | (selector[8] << 8)	| (selector[9]));
	}
	ret = 0;

END_FUNC:
	if (stBiopIor.type_id)			free(stBiopIor.type_id);
	if (stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.objkey)	free(stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.objkey);
	if (stBiopIor.profile.stBiopProfileBody.stBiopDsmConnbiner.stBiopTap.selector_data)	free(stBiopIor.profile.stBiopProfileBody.stBiopDsmConnbiner.stBiopTap.selector_data);
	return ret;
}

// dsmcc_DownloadServerInitiate 함수를 기반으로 만든 parsing function
int dsmcc_ParseDsiData (unsigned char *pucRawData, int nSecLen, DsmccDsiInfo_t *pstDsiInfo)
{
	int		nCompDescLength =0, nPrivateDataLen = 0, nTypeId ;
	int 	offset= 0, ret;

	// Server ID (20byte)
	offset += 20;

	// compatibilydescriptorlength (2byte)
	// TS_102 006 V.1.2.1 spec에서는 compdescriptor를 처리해줘야 된다..
	// IOR(inter object referenced format)에서는 항상 0 이다..
	nCompDescLength = (pucRawData[offset] << 8) | pucRawData[offset+1];
	offset += 2;
    offset += nCompDescLength;

	// privateDataLength(2byte)
	nPrivateDataLen = (pucRawData[offset] << 8) | pucRawData[offset+1];
    offset+=2;

	// 확인 필요..
	// private data 가 없으면 Data Carousel
	if(nPrivateDataLen == 0)
	{
		//????
	}

	// DataCarousel: GroupInfoByte structure + dsmcc_CarouselDescriptors
	// ObjectCarousel: BIOP::ServiceGatewayInformation
	// because we do not know, if DC or OC, we make a check for BIOP
	// U-U Objects type_id "srg\0"  and "DSM:"
	// (normally we should remember a databroadcast_id for decision)

	// private_data_len 가 byte 이상인 경우는 private data 로 간주  ???????
	if(nPrivateDataLen >= 8 )
	{
		nTypeId = ((pucRawData[offset+4] << 24) | (pucRawData[offset+5] << 16) | (pucRawData[offset+6] << 8)  | (pucRawData[offset+7]));
	}
	else
	{
		nTypeId = 0;
	}

	//type id 중 상위 4byte가 "srg\0"  and "DSM: 가 아닌 경우는 Data Carousel
	// ETSI TR 101 202 V.1.2.1 Table4-4 : U-U Object type_id 참조.
	if (nTypeId != 0x73726700 && nTypeId != 0x53443a4d )
	{
		// Data Carousel
		// GroupInfoIndication.
		return -1;
	}
	else
	{
		// offset = Server ID + compatibilitydescriptorlength + Compatibility Descriptor Data + privateDataLength
		ret = dsmcc_ParseDsiSgi(pucRawData + offset, nSecLen - offset, pstDsiInfo);
	}

	return ret;
}

// dsmcc_DownloadInfoIndication 함수를 기반으로 만든 parsing function
int dsmcc_ParseDiiData (unsigned char *pucRawData, int nSecLen, DsmccDiiInfo_t *pstDiiInfo)
{
	int				offset=0, ret;
	int				nCompDescLength = 0;
	unsigned int	nCnt;
	unsigned short	number_modules;
	unsigned short	usIndex ;

	// download ID (4byte)
	pstDiiInfo->download_id = (pucRawData[0] << 24) | (pucRawData[1] << 16) | (pucRawData[2] << 8)  | (pucRawData[3]) ;
	offset+=4;

	// block size(2byte)
	offset+=2;

	// window size(1byte) + ackPeriod(1byte) + tCDownloadWindow(4byte)
	offset+=6; /* not used fields */

	// tCDownloadSeenario (4byte) - 사용하지 않음.
	offset+=4;

	//skip unused compatibility descriptor len
	nCompDescLength = (pucRawData[offset] << 8) | pucRawData[offset+1];
	offset +=2;
	offset += nCompDescLength;

	// numberOfMoudules (2byte)
	number_modules = (pucRawData[offset] << 8) | pucRawData[offset+1];
	offset+=2;

	pstDiiInfo->num_module = number_modules;
	pstDiiInfo->module_total_size = 0;
	pstDiiInfo->num_assoctag = 0;


	if(number_modules > 0)
	{
		for(usIndex = 0; usIndex < number_modules; usIndex++)
		{
			BIOP_ModuleInfo_t		 stModInfo;
			DsmccDes_t				*desc, *nextdesc;

			// moduleId(2byte)
			offset+=2;

			// moduleSize(4byte)
			pstDiiInfo->module_total_size += (pucRawData[offset] << 24) | (pucRawData[offset + 1] << 16) | (pucRawData[offset + 2] << 8) | (pucRawData[offset + 3]) ;
			offset+=4;

			// module Version(1byte)
			offset += 1;

			// module Length(1byte)
			offset += 1;

			memset(&stModInfo, 0, sizeof(BIOP_ModuleInfo_t));

			// module byte -> 13818-6 11.3.3.2Module Delivery Parameters
			ret = dsmcc_biop_process_module_info(&stModInfo, pucRawData+offset);
			if(ret > 0)
			{
				offset += ret;
			}
			else
			{
				return -1;
			}

			for (nCnt = 0; nCnt < pstDiiInfo->num_assoctag; nCnt++)
			{
				if (pstDiiInfo->biop_module_assoctag[nCnt] == stModInfo.stBiopTap.assoc_tag)
				{
					break;
				}
			}

			if ((nCnt >= pstDiiInfo->num_assoctag) && (pstDiiInfo->num_assoctag < MAXCAROUSELS))
			{
				pstDiiInfo->biop_module_assoctag[pstDiiInfo->num_assoctag] = stModInfo.stBiopTap.assoc_tag;
				pstDiiInfo->num_assoctag++;
			}

			if (stModInfo.stBiopTap.selector_data)
			{
				free(stModInfo.stBiopTap.selector_data);
			}

			desc = stModInfo.descriptors;
			while (desc)
			{
				nextdesc = desc->next;
				dsmcc_desc_free(desc);
				desc = nextdesc;
			}
		}
	}

	return 0;
}

// dsmcc_DownloadDataBlock 함수를 기반으로 만든 parsing function
int dsmcc_ParseDdbData (unsigned char *pucRawData, int nSecLen, DsmccDownloadDataHeader_t *pstDdHeader, DsmccDdbInfo_t *pstDdbInfo)
{
	int offset =0;

	// module Id (2byte)
	pstDdbInfo->module_id = (pucRawData[offset] << 8) | pucRawData[offset+1];
	offset+=2;

	// module version(1byte)
	pstDdbInfo->module_version = pucRawData[offset];
	offset+=1;

	// reserved byte(1byte)
	offset+=1;

	// block number(2byte)
	pstDdbInfo->block_number = (pucRawData[offset] << 8) | pucRawData[offset+1];
	offset+=2;

	return 0;
}

#define ______GOBAL_FUNCTION______

void dsmcc_init(struct dsmcc_status *status, const char *channel)
{
	int i;

	status->buffers = NULL;

	for(i=0;i<MAXCAROUSELS;i++)
	{
		status->stObjCarousels[i].pstCacheModuleData = NULL;
		status->stObjCarousels[i].pstCacheAll = malloc(sizeof(Cache_All_t));
		status->stObjCarousels[i].pstDsi = NULL;
		status->stObjCarousels[i].downloadId_inObjectCarousel= 0;
		status->stObjCarousels[i].usPid = 0;
		dsmcc_cache_init(status->stObjCarousels[i].pstCacheAll, channel);
	}
}

void dsmcc_objcar_free(ObjectCarousel_t *pstObjCarousel)
{
	CacheModuleData_t *cachep, *cachepnext;
	DsmccDes_t 	*desc, *last;
	DsmccDdb_t	*pstDdb;

	/* Free gateway info */
	if(pstObjCarousel->pstDsi!= NULL)
	{
		if(pstObjCarousel->pstDsi->user_data != NULL)
		{
			free(pstObjCarousel->pstDsi->user_data);
			pstObjCarousel->pstDsi->user_data = NULL;
		}

		if(pstObjCarousel->pstDsi->stBiopIor.type_id != NULL)
		{
			free(pstObjCarousel->pstDsi->stBiopIor.type_id);
			pstObjCarousel->pstDsi->stBiopIor.type_id = NULL;
		}

		if(pstObjCarousel->pstDsi->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.objkey != NULL)
		{
			free(pstObjCarousel->pstDsi->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.objkey);
			pstObjCarousel->pstDsi->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.objkey = NULL;
		}

		if(pstObjCarousel->pstDsi->stBiopIor.profile.stBiopProfileBody.stBiopDsmConnbiner.taps_count>0) {
			if(pstObjCarousel->pstDsi->stBiopIor.profile.stBiopProfileBody.stBiopDsmConnbiner.stBiopTap.selector_data!=NULL)
			{
				free(pstObjCarousel->pstDsi->stBiopIor.profile.stBiopProfileBody.stBiopDsmConnbiner.stBiopTap.selector_data);
				pstObjCarousel->pstDsi->stBiopIor.profile.stBiopProfileBody.stBiopDsmConnbiner.stBiopTap.selector_data=NULL;
			}
		}

		free(pstObjCarousel->pstDsi);
		pstObjCarousel->pstDsi = NULL;
	}

	/* Free cache info */
	cachep = pstObjCarousel->pstCacheModuleData;
	while(cachep!=NULL)
	{
	      cachepnext = cachep->next;
	      if(cachep->pucModuleData!= NULL) { /* should be empty */
		      free(cachep->pucModuleData);
	      }

	      if(cachep->bstatus != NULL) {
		      free(cachep->bstatus);
	      }
#if 1	/* JWKIM, Memory leak issue 수정 */
		if(cachep->blocks != NULL)
		{
			pstDdb = cachep->blocks;
			while(pstDdb != NULL)
			{
				last = (DsmccDes_t *)pstDdb->next;
				if ( pstDdb->blockdata != NULL )
				{
					free(pstDdb->blockdata);
				}
				free(pstDdb);
				pstDdb = (DsmccDdb_t*)last;
			}
		}
#endif
		if(cachep->pstDsmccDescriptors != NULL)
		{ /* TODO badness */
			desc = cachep->pstDsmccDescriptors;
			while(desc != NULL)
			{
				last = desc->next;
				dsmcc_desc_free(desc);
				desc = last;
			}
		}

		free(cachep);
		cachep = cachepnext;
	}

	dsmcc_cache_free(pstObjCarousel->pstCacheAll);
}

void dsmcc_free(struct dsmcc_status *status)
{
	struct pid_buffer *buf, *lbuf;
	int i = 0;

	/* Free any carousel data and cached data.
	 * TODO - actually cache on disk the cache data
	 * TODO - more terrible memory madness, this all needs improving
	 */

	if(status == NULL)
		return;

	for (i=0; i<MAXCAROUSELS; ++i )
	{
		ObjectCarousel_t *pstObjCarousel = (ObjectCarousel_t *)&(status->stObjCarousels[i]);
		dsmcc_objcar_free(pstObjCarousel);
	}

	if(status->buffers != NULL)
	{
		buf=status->buffers;
		while(buf!=NULL)
		{
			lbuf = buf->next;
			free(buf);
			buf = lbuf;
		}
	}

	status->buffers = NULL;

	free(status);
}

void dsmcc_process_section(struct dsmcc_status *pstDsmccStatus,unsigned char *pucRawData ,int nSecLen, unsigned short usPid)
{
	unsigned char		table_id;
	unsigned long		crc32_decode;
	unsigned short section_len;

#if defined(CONFIG_DEBUG) && 0
	{
		int nIndex, nCount = 0;

		if(pucRawData[0] == DSMCC_SECTION_DOWNLOAD_DATA_MESSAGE)
		{
			printf("\n[dsmcc_process_section]========================== usPid (0x%x) Length(%d)\n", usPid, nSecLen);

			for(nIndex = 0; (nIndex < nSecLen || nIndex < 200); nIndex ++)
			{
				nCount++;
				printf("%02x ", pucRawData[nIndex]);

				if(nCount == 16)
				{
					printf("\n");
					nCount=0;
				}
			}

			if(nCount != 0)
				printf("\n");

			printf("=====================================================\n");
		}
	}
#endif

	table_id = pucRawData[0];
	section_len = ((pucRawData[1] & 0xF) << 8) | (pucRawData[2]) ;
	section_len += 3;/* 3 bytes before length count starts */

	//printf("\n\n\n[dsmcc_process_section] nSecLen(%d) section_len(%d) \n\n\n" , nSecLen, section_len);

	//  CRC check;
	crc32_decode = dsmcc_crc32(pucRawData, section_len);
	if(crc32_decode != 0)
	{
		printf("[dsmcc_process_section]  crc32 error  \n");
		return;
	}

	// 13818-6 -> Table 9.2. DSMCC Section Format
	if(table_id == DSMCC_SECTION_USER_NETWORK_MESSAGE) // userNetworkMessage
	{
		dsmcc_UserNetworkMessage(pstDsmccStatus, pucRawData, nSecLen, usPid);
	}
	else if(table_id == DSMCC_SECTION_DOWNLOAD_DATA_MESSAGE)	// downloadDataMessage
	{
		dsmcc_DownloadDataMessage(pstDsmccStatus, pucRawData, nSecLen, usPid);
	}
	else if(table_id == DSMCC_SECTION_DESCRIPTOR_LIST) // DSMCC_Descriptor_list
	{
		dsmcc_descriptor_list(pstDsmccStatus, pucRawData + 8, nSecLen, usPid);
	}
	else
	{
		//printf("[dsmcc_process_section] Not Support TableID (0x%x) \n", table_id);
		/* 0x3A(LLCSNAP) 0x3E(Private Data) 처리하지 않는다... */
	}
}

int dsmcc_parse_dsi_section (unsigned char *pucRawData, int nSecLen, DsmccDsiInfo_t *pstDsiInfo)
{
	DsmccMessageHeader_t		 stMsgHeader;
	int ret;

	if(pucRawData[0] != DSMCC_SECTION_USER_NETWORK_MESSAGE)
	{
		return -1;
	}

	if (nSecLen < DSMCC_DSI_OFFSET)
	{
		return -1;
	}

	memset(&stMsgHeader, 0x00, sizeof(DsmccMessageHeader_t));
	ret = dsmcc_HeaderMessage(&stMsgHeader, pucRawData + DSMCC_MESSAGE_HEADER_OFFSET);
	if(ret <  0)
	{
		return -1;
	}

	if(stMsgHeader.message_id != DSMCC_MESSAGE_DSI) // DownloadServerInitiate
	{
		return -1;
	}

	pstDsiInfo->protocol		= stMsgHeader.protocol;
	pstDsiInfo->dsmcctype		= stMsgHeader.dsmcctype;
	pstDsiInfo->message_id		= stMsgHeader.message_id;
	pstDsiInfo->transaction_id	= stMsgHeader.transaction_id;

	return dsmcc_ParseDsiData(pucRawData + DSMCC_DSI_OFFSET, nSecLen - DSMCC_DSI_OFFSET, pstDsiInfo);
}

int dsmcc_parse_dii_section (unsigned char *pucRawData, int nSecLen, DsmccDiiInfo_t *pstDiiInfo)
{
	DsmccMessageHeader_t		 stMsgHeader;
	int ret;

	if(pucRawData[0] != DSMCC_SECTION_USER_NETWORK_MESSAGE)
	{
		return -1;
	}

	if (nSecLen < DSMCC_DII_OFFSET)
	{
		return -1;
	}

	memset(&stMsgHeader, 0x00, sizeof(DsmccMessageHeader_t));
	ret = dsmcc_HeaderMessage(&stMsgHeader, pucRawData + DSMCC_MESSAGE_HEADER_OFFSET);
	if(ret !=  0)
	{
		return -1;
	}

	if(stMsgHeader.message_id != DSMCC_MESSAGE_DII) // DownloadServerInitiate
	{
		return -1;
	}

	pstDiiInfo->protocol		= stMsgHeader.protocol;
	pstDiiInfo->dsmcctype		= stMsgHeader.dsmcctype;
	pstDiiInfo->message_id		= stMsgHeader.message_id;
	pstDiiInfo->transaction_id	= stMsgHeader.transaction_id;

	return dsmcc_ParseDiiData(pucRawData + DSMCC_DII_OFFSET, nSecLen - DSMCC_DII_OFFSET, pstDiiInfo);
}

int dsmcc_parse_ddb_section (unsigned char *pucRawData, int nSecLen, DsmccDdbInfo_t *pstDdbInfo)
{
	DsmccDownloadDataHeader_t	 stDdHeader;
	int ret;

	if(pucRawData[0] != DSMCC_SECTION_DOWNLOAD_DATA_MESSAGE)
	{
		return -1;
	}

	if (nSecLen < DSMCC_DDB_OFFSET)
	{
		return -1;
	}

	memset(&stDdHeader, 0x00, sizeof(DsmccDownloadDataHeader_t));
	ret = dsmcc_HeaderDownloadData(&stDdHeader, pucRawData+DSMCC_DOWNLOAD_DATA_HEADER_OFFSET);
	if(ret !=  0)
	{
		return -1;
	}

	pstDdbInfo->protocol = stDdHeader.protocol;
	pstDdbInfo->dsmcctype = stDdHeader.dsmcctype;
	pstDdbInfo->message_id = stDdHeader.message_id;
	pstDdbInfo->download_id = stDdHeader.download_id;

	return dsmcc_ParseDdbData(pucRawData + DSMCC_DII_OFFSET, nSecLen - DSMCC_DII_OFFSET, &stDdHeader, pstDdbInfo);
}



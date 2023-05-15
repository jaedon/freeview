/******************************************************************************
 *    (c)2012 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: test_widevine_vod.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 10/18/12 9:42a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/security/common_drm/examples/widevine/test_widevine_vod.c $
 * 
 * 1   10/18/12 9:42a gayatriv
 * sw7425-3930:for some reason earlier checkin was an empty file. checking
 * in again
 * 
 * 3   4/24/12 8:17p teofil
 * SW7425-2928: added NEXUS_Window_Close(window)
 * 
 * 2   4/24/12 4:21p cdisc
 * SW7425-2928: merging with new common crypto code
 *
 * 1   3/21/12 1:36p cdisc
 * SWSECURITY-114: adding xpt widevine support and nor support
 * 
 *****************************************************************************/
/* Nexus example app: playback and decode CA protected stream. */

#include "nexus_platform.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_input.h"
#include "nexus_audio_output.h"
#include "nexus_spdif_output.h"
#include "nexus_component_output.h"
#include "nexus_composite_output.h"
#include "nexus_hdmi_output.h"
#include "nexus_playback.h"
#include "nexus_file.h"
#include "nexus_security.h"
#include <string.h> /* for memcpy */

#include "drm_widevine.h"
#include "drm_init.h"

#include <stdio.h>
#include <assert.h>
#include "bstd.h"
#include "bkni.h"

/* the following define the input file and its characteristics -- these will vary by input file */
/*#define TEST_UNENCRYPTED*/
/*#define TEST_SPIDERMAN*/
#define TEST_COMMON_DRM

#ifdef TEST_UNENCRYPTED
#define FILE_NAME "cnnticker.mpg"
#define VIDEO_PID 0x21
#define AUDIO_PID 0x22
#define VIDEO_CODEC NEXUS_VideoCodec_eMpeg2
#else

#ifdef TEST_SPIDERMAN
#define FILE_NAME "spiderman_aes.ts"
#define VIDEO_PID 0x11
#define AUDIO_PID 0x14
#define VIDEO_CODEC NEXUS_VideoCodec_eMpeg2
#else
#define FILE_NAME "outfile.mpg"
#define VIDEO_PID 0x120
#define AUDIO_PID 0x131
#define VIDEO_CODEC NEXUS_VideoCodec_eH264
#endif /* TEST_SPIDERMAN */
#endif

#define TEST_PLAT 
/* or don't, i.e. TEST_CARD - remove the prev #define */

#ifdef TEST_PLAT
#define PLAT_DRM_BIN_FILE_NAME "WIDEVINE_SETTOP_KEYBOX_00000000.bin"
#endif

#define TRANSPORT_TYPE NEXUS_TransportType_eTs

#define AUDIO_CODEC NEXUS_AudioCodec_eMpeg
int widevine_security_config( DrmCryptoOperation drm_op);
int data_encrypt(void *inBuf,uint32_t size, void* outBuf);
int data_decrypt(void* *inBuf, uint32_t size, void * OutBuf);
#if 0
	unsigned char iv0[16] = { 0x00};
#else
uint8_t iv0[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
#endif
	unsigned char *test=NULL,*result=NULL;


/* encrypted buffer*/
uint8_t enc_buf[50] =  {0x73,0x6c,0x37,0x15,0xbf,0xd2,0x77,0x97,
                        0x57,0x46,0x84,0xc4,0x4f,0x8e,0x5c,0xbc,
                        0x11,0x86,0xa9,0x5c,0x73,0x73,0xb0,0xa5,
                        0x60,0xc6,0x65,0xf9,0x3c,0x6c,0xbc,0x1c,
                        0x81,0x11,0x5e,0xf4,0x8e,0x76,0xf5,0xf6,
                        0x06,0xd9,0x1d,0x6b,0x9f,0xb3,0xda,0x87,0x55,0x55};

                        /*73 6C 37 15 BF D2 77 97
                        57 46 84 C4 4F 8E 5C BC
                        11 86 A9 5C 73 73 B0 A5
                        60 C6 65 F9 3C 6C BC 1C
                        81 11 5E F4 8E 76 F5 F6
                        06 D9 1D 6B 9F B3 DA 87*/
uint32_t test_buf_len = 50;




int widevine_security_config(DrmCryptoOperation drm_op)
{
	DRM_Widevine_Init_t inputWvInitStruct;
	DrmRC drm_rc = Drm_Success;
	 uint8_t ecm_even[32] = {0xe6, 0x56, 0x07, 0xde, 0x61, 0xf2, 0x05, 0x2c,
    						0x71, 0x40, 0xc5, 0x0a, 0xe1, 0x56, 0x8f, 0x5f,
    						0x95, 0x4c, 0x20, 0xda, 0x4c, 0x7e, 0xa9, 0x2f,
						0xe7, 0x98, 0xa5, 0xde, 0xd2, 0x5b, 0x53, 0x4b};
	 #if 0 

    uint8_t ecm_odd[32] = {0x18, 0x99, 0x35, 0x79, 0x07, 0x83, 0x40, 0x42,
    						0x2f, 0x6e, 0xe3, 0x9e, 0xf8, 0x3b, 0x88, 0xca,
							0x28, 0x20, 0x31, 0x26, 0x03, 0x73, 0x49, 0x2a,
							0xe5, 0x0c, 0x08, 0xbe, 0xa8, 0xf8, 0x5f, 0xe9 };
#else

	uint8_t ecm_odd[32] = {0x42, 0xb2, 0x3a, 0xc5, 0xe7, 0x08, 0xa0, 0x11,
		               0x38, 0xab, 0x35, 0x34, 0xd5, 0x05, 0xce, 0x17,
		               0xa6, 0xdd, 0x03, 0x3e, 0xfd, 0xe1, 0xe2, 0xf8,
		               0xc8, 0xd0, 0x43, 0x9b, 0x0c, 0x96, 0xaf, 0xd2};
#endif

#if 0
    uint8_t id[16] = {0x00};
#else
	  uint8_t id[4] = {0};
#endif

#if 0
    uint8_t common_key[16] = {0x31, 0x9f, 0xf5, 0xa5, 0xb2, 0x74, 0x6d, 0x9b, 0x74, 0xa6, 0x34, 0x3c, 0xaa, 0xf0, 0x1e, 0xf1};
#else
	uint8_t common_key[16] = {0x7e, 0xca, 0xd3, 0x2e, 0xec, 0x06, 0x5f, 0xf4, 0xeb, 0xa6, 0xc6, 0x32, 0x11, 0x5e, 0x0c, 0x0c};
#endif


	
	
	
	
	printf("\n call DRM_Widevine_Initialize....");
	inputWvInitStruct.key_file = "WIDEVINE_SETTOP_KEYBOX_00000000.bin";
	inputWvInitStruct.mode = DRM_WidevineDecryptMode_eVod;
    DRM_Widevine_Initialize(inputWvInitStruct);
	printf("done");

	DRM_Widevine_SetDrmOp(drm_op);
    drm_rc = DRM_Widevine_LoadCwFixed(common_key, ecm_odd, id, DrmSecurityKeyType_eEven);
	/* drm_rc = DRM_Widevine_LoadCw(common_key, ecm_odd, id, DrmSecurityKeyType_eEven);*/
    if(drm_rc != Drm_Success)
	{
        printf("\tMAIN - DRM_Widevine_LoadCw (even) failed, rc %d\n", drm_rc);
        goto ErrorExit;
    }
	else
	{
		printf("\n\tMAIN - CW ID: 0x%02x 0x%02x 0x%02x 0x%02x\n\n", id[0], id[1], id[2], id[3]);
	}
	DRM_Widevine_SetDrmOp(drm_op);
	/* drm_rc = DRM_Widevine_LoadCw(common_key, ecm_odd, id, DrmSecurityKeyType_eOdd);*/
    drm_rc = DRM_Widevine_LoadCwFixed(common_key, ecm_odd, id, DrmSecurityKeyType_eOdd);
    if(drm_rc != Drm_Success)
	{
        printf("\tMAIN - DRM_Widevine_LoadCw (odd) failed, rc %d\n", drm_rc);
        goto ErrorExit;
    }
	else
	{
		printf("\n\tMAIN - CW ID: 0x%02x 0x%02x 0x%02x 0x%02x\n\n", id[0], id[1], id[2], id[3]);
	}

ErrorExit: return drm_rc;

}
#define INFILE	"/mnt/nfs/bbc_cut.pes"
#define OUTFILE "/mnt/nfs/outfile_enc.pes"

enum OpMode{
	FileEncrypt,
	FileDecrypt,
	FileProbe
}op;
 
 char infile[256]={0};
 char outfile[256]={0};

 
 int main(int argc, char* argv[])
{
	FILE *inF=NULL;
	FILE *outF=NULL;
	uint8_t header[16];
	int i=0;int ret=0;
	DrmRC drm_rc = Drm_Success;

	
		printf("\n using default filenames infile.pes and outfile_enc.pes");
		strcpy(infile,"/mnt/nfs/bbc_cut.pes");
		strcpy(outfile,"/mnt/nfs/outfile_enc.pes");
		op=FileProbe;
	
		for ( i = 1; i < argc; i++) 
		{
			if ( !strcmp(argv[i], "-i") ) 
			{
            
                strcpy(infile, argv[++i]);
				printf("\n infile is %s",infile);
           	}
			else if ( !strcmp(argv[i], "-o") ) 
			{
				strcpy(outfile, argv[++i]);
				printf("\n outfile is %s",outfile);
			}
		
			else if ( !strcmp(argv[i], "-probe") ) 
			{
				op=FileProbe;
			}
			else if ( !strcmp(argv[i], "-dec") )
			{
				op=FileDecrypt;
			}
			else if ( !strcmp(argv[i], "-enc") )
			{
				op=FileEncrypt;
			}
		}
	
			getchar();
	inF=fopen(infile, "rb");
	printf("\n  Reading file %s...",infile);
	if(inF==NULL)
	{
		printf("\n Error Reading file %s...",infile);	
		exit(1);
	}

	outF=fopen(outfile, "wb");
	if(outF==NULL)
	{
		printf("\n Error outfile Reading file...");	
		exit(1);
	}

		getchar();
	if(op==FileDecrypt||op==FileEncrypt)
	{
		 NEXUS_PlatformSettings platformSettings;
		NEXUS_Platform_GetDefaultSettings(&platformSettings);
		platformSettings.openFrontend = false;
		NEXUS_Platform_Init(&platformSettings);
		printf("\n Setting up key config.....");
		if(0==widevine_security_config(/*DrmCryptoOperation_eEncrypt*/op))
			 printf("\n Widevine security config sucess!");
		 else
			 printf("\n Widevine security config failed!");
		printf("\ndone");
#if 1
		
			printf("\n+++++++++++++++Testing with test vector+++++++++++++\n");
			printf("\ntest vector in clear:");

			 NEXUS_Memory_Allocate(50, NULL,(void **) &test);
			  NEXUS_Memory_Allocate(50, NULL,(void **) &result);
			  memset(result, 0, 50);
			  memcpy(result, enc_buf, sizeof(enc_buf));
		if(op==FileEncrypt)
		{
			for ( i=0;i<50;i++){
				test[i]=0x55;
				printf("%02x ", test[i]);
				}
				printf("\n");

			drm_rc=DRM_Widevine_Encrypt(test,48, 
                    test,iv0, 
                     DrmDestinationType_eInPlace,  NEXUS_SecurityKeyType_eOdd);
			/*drm_rc=DRM_Widevine_P_Encrypt(payload_buf,payload_buf,payload_size,iv0,NEXUS_SecurityKeyType_eOdd);*/
			if(drm_rc!= Drm_Success){
				printf("\nEncrypt Failed !!!!");
				
			}
			
			printf("\ntest vector encrypted:");
		for ( i=0;i<50;i++)
			printf("%02x ", test[i]);
		printf("\n");
		getchar();
}else if(op==FileDecrypt)
{		/*DRM_Widevine_Finalize();*/

	/*	if(0==widevine_security_config(DrmCryptoOperation_eDecrypt))
			 printf("\n Widevine security config sucess!");
		 else
			 printf("\n Widevine security config failed!");
		printf("\ndone");*/


		drm_rc=DRM_Widevine_Decrypt(result,48, 
                  result,iv0, 
                     DrmDestinationType_eInPlace,  NEXUS_SecurityKeyType_eOdd);
			/*drm_rc=DRM_Widevine_P_Encrypt(payload_buf,payload_buf,payload_size,iv0,NEXUS_SecurityKeyType_eOdd);*/
			if(drm_rc!= Drm_Success){
				printf("\nEncrypt Failed !!!!");
				
			}
		printf("\ntest vector in decrypted:");
		for ( i=0;i<16;i++)
			printf("%02x ",result[i]);
		printf("\n");
		printf("\n+++++++++++++++DONE!!+++++++++++++\n");
		getchar();
}
#endif
	}
		unsigned long curr_size=0x10,curr_offset=0x0, payload_size=0x0, pes_pkt_len=0x0;
			unsigned long  next_size=0x0,next_offset=0x0,payload_offset=0x0,header_sz=0x0;
			uint8_t* payload_buf=NULL;
			int readsz=0,writesz=0;
	fseek(inF,next_offset,0x0);
	while( !feof(inF) )
	{
		readsz=fread(header,sizeof(uint8_t),curr_size,inF);
		if(readsz==0)
			break;
		printf("\n************Start************");
		for ( i=0;i<readsz;i++)
			printf("%02x ", header[i]);
		printf("\n");
		/*next_size=((*(header+4)))<<8 | (*(header+5)) + (*(header+8));*/

		next_size= (*(header+4))<<8 | (*(header+5));
		pes_pkt_len=next_size;
		next_size+=(*(header+8));
		
		payload_offset = 8+(*(header+8))+curr_offset+1;
		header_sz=(8+(*(header+8)));
		payload_size=pes_pkt_len-(3+(*(header+8)));
		
		next_offset=curr_offset+next_size+1;
		printf("\npkt sz is 0x%x,remaining hdr bytes:0x%x,pespayload size is 0x%x,header_sz is 0x%x",((uint32_t)(*(header+4)))<<8 | (uint32_t)(*(header+5)),(*(header+8)) ,payload_size,header_sz);
		printf("\nnext_offset is 0x%x, next_size is 0x%x, curr_offset is 0x%x,payload_offset is 0x%x",next_offset, next_size,curr_offset,payload_offset);

		/* write the header in clear to outputfile)*/
		writesz = fwrite(header,sizeof(uint8_t),header_sz+1,outF);
		if(writesz!=(header_sz+1))
		{
			printf("\nWARNING!!! Write in complete, writesz=0x%x, size=0x%x",writesz,header_sz);
		}
		/*free(payload_buf);*/
		

		ret = fseek(inF,payload_offset,0x0);
		if(ret!=0)
		{
			printf("\n seek failed could be end of file");
			break;
		}
		if(op==FileProbe)
			payload_buf=(uint8_t *) malloc(payload_size);
		else
			NEXUS_Memory_Allocate(payload_size, NULL,(void **) &payload_buf);
	if(payload_buf==NULL)
		{
			printf("\n malloc failed for payload buffer exiting...");
			break;
		}
			
		readsz=fread(payload_buf,sizeof(uint8_t),payload_size,inF);
		if(readsz != payload_size)
		{
			printf("\nWARNING! read incorrect size, readsz=0x%x, payload_size=0x%x",readsz,payload_size);	
		}

		/*encrypty payload data*/
		/*dump the enc data into the out file*/
		if(op==FileEncrypt)
		{
			printf("\n leaving %d bytes in clear",(payload_size%16));
			drm_rc=DRM_Widevine_Encrypt(payload_buf,payload_size-(payload_size%16), 
                    payload_buf,iv0, 
                     DrmDestinationType_eInPlace,  NEXUS_SecurityKeyType_eEven);
			/*drm_rc=DRM_Widevine_P_Encrypt(payload_buf,payload_buf,payload_size,iv0,NEXUS_SecurityKeyType_eOdd);*/
			if(drm_rc!= Drm_Success){
				printf("\nEncrypt Failed !!!!");
				break;
			}
		}
		else if (op==FileDecrypt)
		{
			printf("\n leaving %d bytes in clear",(payload_size%16));
			drm_rc=DRM_Widevine_Decrypt(payload_buf,payload_size-(payload_size%16), 
                    payload_buf,iv0, 
                     DrmDestinationType_eInPlace,  NEXUS_SecurityKeyType_eEven);
			/*drm_rc=DRM_Widevine_Decrypt(payload_buf,payload_buf,payload_size,iv0,NEXUS_SecurityKeyType_eOdd);*/
			if(drm_rc!= Drm_Success){
				printf("\nDecrypt Failed !!!!");
				break;
			}
		}

		writesz=fwrite(payload_buf,sizeof(uint8_t),payload_size,outF);
		if(writesz != payload_size)
		{
			printf("\nWARNING! read incorrect size,writesz=0x%x, payload_size=0x%x",writesz,payload_size);	
		}

		ret = fseek(inF,next_offset,0x0);
		if(ret!=0)
		{
			printf("\n seek failed could be end of file");
			break;
		}
		curr_offset=next_offset;
		if(op==FileProbe)
			free(payload_buf);
		else
			NEXUS_Memory_Free(payload_buf);
/*getchar();*/
	}

		fclose(inF);
		fclose(outF);


}





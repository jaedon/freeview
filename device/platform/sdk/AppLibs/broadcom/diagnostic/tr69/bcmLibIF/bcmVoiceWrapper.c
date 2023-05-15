/*****************************************************************************
//
//  Copyright (c) 2004  Broadcom Corporation
//  All Rights Reserved
//  No portions of this material may be reproduced in any form without the
//  written permission of:
//          Broadcom Corporation
//          16215 Alton Parkway
//          Irvine, California 92619
//  All information contained in this document is Broadcom Corporation
//  company private, proprietary, and trade secret.
//
******************************************************************************
//
//  Filename:       bcmVoiceWrapper.c
//  Author:         Farhan Ali
//  Creation Date:  07/29/05
//
******************************************************************************
//  Description:
//             WT-104 get/set functions
//
//
*****************************************************************************/



#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <codec.h>
#include <globalCfg.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <net.h>


#include <cfgmgr_voip_defs.h>
#include <cfgmgr_voip_params.h>
#include <endpointdrv.h>

#include "bcmtypes.h"
#include "bcmcfm.h"
#include "bcmVoiceWrapper.h"
#include "../inc/tr69cdefs.h"
#include "../SOAPParser/CPEframework.h"
#include "../SOAPParser/RPCState.h"

/*------------------------Private Variables/Structs/Functions start--------------------*/


typedef struct{
   CFGMGR_VOIP_ITEM item;
   int isGet;
   int line;
   int instance;
} CFGMGR_VOIP_IPC_PACKET;

typedef struct{
   unsigned int id;
   char name[64];
   char suppPtime[64];
   unsigned int bitRate;
} WT104_CODEC_MAP;

WT104_CODEC_MAP wt104CodecMap[] =
{
   {CODEC_PCMU,     "G.711MuLaw",   "10,20,30", 100},
   {CODEC_PCMA,     "G.711ALaw",    "10,20,30", 100},
#if XCFG_G7231_SUPPORT   
   {CODEC_G7231_63, "G.723.1",      "10,20,30", 100 },
#endif  
#if XCFG_G726_SUPPORT   
   { CODEC_G726_32, "G.726",        "10,20,30", 100 },
#endif   
#if XCFG_G728_SUPPORT   
   {CODEC_G728,     "G.728",        "10,20,30", 100 },
#endif   
#if XCFG_G729_SUPPORT   
   {CODEC_G729,     "G.729a",       "10,20,30", 100 },
#endif   
#if XCFG_FAX_SUPPORT   
   {CODEC_T38,      "X_Brcm_T38",   "10,20,30", 100 },
#endif   
   {CODEC_NTE,      "X_Brcm_NTE",   "10,20,30", 100 },
   {CODEC_UNKNOWN,      "",   "",  }
}; 

static int getParamFromVoiceApp ( CFGMGR_VOIP_ITEM * reqItem,int line, int instance );
static int setParamInVoiceApp( CFGMGR_VOIP_ITEM * reqItem, int line, int instance );
static int isVoipEndptInitialized( void );

/*------------------------Private Variables/Structs/Functions end--------------------*/

/*------------------------extern Variables/Structs/Functions start--------------------*/
extern InstanceDesc    *instanceStack[];
extern int             instanceDepth;        /* current stack pointer points at current instance desc pointer */
/*------------------------extenr Variables/Structs/Functions end--------------------*/

extern TRxObjNode voiceServiceDesc[];
extern TRxObjNode codecsDesc[];
extern TRxObjNode voiceProfDesc[];
extern TRxObjNode vpPhyIntDesc[];
extern TRxObjNode vpLineCodecListDesc[];
extern TRxObjNode vpLineDesc[];
extern TRxObjNode vpSipEvSubscDesc[];

void initVoiceInstances(void)
{
   TRxObjNode *n;
   TRxObjNode *n2;
   int i,id,id2,j;
   InstanceDesc *voiceServiceInstanceDesc;
   InstanceDesc *voiceProfileInstanceDesc;
   InstanceDesc *vpLineInstanceDesc;
   
   
   /* There is only a single Voice Device Instance on this design */
   n = voiceServiceDesc;
   id = 1; /* getVoiceDeviceInstanceId() This value needs to be persistent */
   voiceServiceInstanceDesc = getNewInstanceDesc(n, NULL, id);
   
   /* Initialize per number of codecs */ 
   /* VoiceService.{i}.Capabilities.Codecs.{i}. */
   for(i=0;i<CFGMGR_VOIP_MAX_CODECS;i++)
   {       
      n = codecsDesc;
      id = i+1; /* getCodecsInstanceId() This value needs to be persistent */
      getNewInstanceDesc(n, voiceServiceInstanceDesc, id);
   }    
   
   /* Only single voice profile is supported by our design */
   /* VoiceService.{i}.VoiceProfile.{i}. */
   n = voiceProfDesc;
   id = 1; /* () This value needs to be persistent */
   voiceProfileInstanceDesc = getNewInstanceDesc(n, voiceServiceInstanceDesc, id);
   
   /* SipEventSubscription is not supported, this is just placeholder */
   /* VoiceService.{i}.VoiceProfile.{i}.Sip.EventSubscribe.{i}. */
   n = vpSipEvSubscDesc;
   id = 1; /* () This value needs to be persistent */
   getNewInstanceDesc(n, voiceProfileInstanceDesc, id);
   
   /* Initialize per number of physical interfaces per FXS same as line no. */        
   /* VoiceService.{i}.VoiceProfile.{i}.PhyInterface.{i}. */
   for(i=0;i<GLOBAL_CFG_MAX_ENDPT;i++)
   {
      n = vpPhyIntDesc;      
      id = i+1; /* () This value needs to be persistent */                       
      getNewInstanceDesc(n, voiceProfileInstanceDesc, id);       
   }
   
   /* Initialize per number of lines */
   /* VoiceService.{i}.VoiceProfile.{i}.Line.{i}. */
   for(i=0;i<GLOBAL_CFG_MAX_ENDPT;i++)
   {       
      n = vpLineDesc;
      id = i+1; /* () This value needs to be persistent */
      vpLineInstanceDesc = getNewInstanceDesc(n, voiceProfileInstanceDesc, id);
      
      /* Initialize per number of codecs */        
      /* VoiceService.{i}.VoiceProfile.{i}.Line.{i}.Codec.List.{i}. */
      for(j=0;j<CFGMGR_VOIP_MAX_CODECS;j++)
      {       
         n2 = vpLineCodecListDesc;    
         id2 = j+1; /* () This value needs to be persistent */                       
         getNewInstanceDesc(n2, vpLineInstanceDesc, id2);                                                
      }
   } 
}


TRX_STATUS getVoiceServiceNumberOfEntries(char **value)
{
   *value = strdup("1");
   return TRX_OK;
}

TRX_STATUS getVoiceProfNOfE(char **value)
{
   *value = strdup("1");
   return TRX_OK;
}
   
TRX_STATUS getMaxProfCnt(char **value)
{
   *value = strdup("1");
   return TRX_OK;
}

TRX_STATUS getMaxSesCnt(char **value)
{
   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_DEVICE_SESSION_COUNT;
   if ( getParamFromVoiceApp( &item , 0 , 0 ) == 1 )
   {
      char buf[10];
      sprintf(buf,"%d",item.itemVal.valInt);
      *value=strdup(buf);
      return TRX_OK;
   }
   else
      return TRX_ERR;
}
  
TRX_STATUS getSigProtocols(char **value)
{
   *value = strdup("SIP");
   return TRX_OK;
}
  
TRX_STATUS getFaxT38sup(char **value)
{
   int i=0;
   while ( wt104CodecMap[i].id != CODEC_UNKNOWN )
   {
      if ( wt104CodecMap[i].id == CODEC_T38 )
         break;
      i++;
   }
   
   if ( wt104CodecMap[i].id == CODEC_UNKNOWN )
      *value = strdup("false");
   else
      *value = strdup("true");
      
   return TRX_OK;
}
  
TRX_STATUS getFaxPassThrSup(char **value)
{
   int i=0;
   while ( wt104CodecMap[i].id != CODEC_UNKNOWN )
   {
      if ( wt104CodecMap[i].id == CODEC_T38 )
         break;
      i++;
   }
   
   if ( wt104CodecMap[i].id == CODEC_T38 )
      *value = strdup("false");
   else
      *value = strdup("true");

   return TRX_OK;
}
  
TRX_STATUS getMdmPassThrSup(char **value)
{
   *value = strdup("true");

   return TRX_OK;
}
  
TRX_STATUS getToneGenSup(char **value)
{
   *value = strdup("false");
   return TRX_OK;
}
  
TRX_STATUS getRingGenSup(char **value)
{
   *value = strdup("false");
   return TRX_OK;
}
  
TRX_STATUS getVoiceLineTest(char **value)
{
   *value = strdup("false");
   return TRX_OK;
}
  

TRX_STATUS getSipRole(char **value)
{
   *value = strdup("UserAgent");
   return TRX_OK;
}
  
TRX_STATUS getSipExt(char **value)
{
   *value = strdup("INVITE,ACK,OPTIONS,BYE,CANCEL,REGISTER,REFER,INFO,PRACK,SUSBSCRIBE,NOTIFY,MESSAGE,PING,UPDATE,SERVICE");
   return TRX_OK;
}
  
TRX_STATUS getSipTransport(char **value)
{
   *value = strdup("UDP,TCP");
   return TRX_OK;
}
  
TRX_STATUS getSipURISch(char **value)
{
  *value = strdup("sip");
   return TRX_OK;
}
  
TRX_STATUS getSipEvSubs(char **value)
{
   *value = strdup("false");
   return TRX_OK;
}
  
TRX_STATUS getSipRespMap(char **value)
{   
   *value = strdup("false");
   return TRX_OK;
}

//NOT SUPPORTED----START 
TRX_STATUS getSipTLSAuthProt(char **value)
{
   *value = strdup("Null");     
   return TRX_OK;
}
  
TRX_STATUS getSipTLSAuthKey(char **value)
{
   *value = strdup("0");  
   return TRX_OK;
}
  
TRX_STATUS getSipTLSEncProt(char **value)
{
   *value = strdup("Null");  
   return TRX_OK;
}
  
TRX_STATUS getSipTLSEncKeySz(char **value)
{
   *value = strdup("0");  
   return TRX_OK;
}
  
TRX_STATUS getSipTLSKeyExchProt(char **value)
{
   *value = strdup("RSA");  
   return TRX_OK;
}
//NOT SUPPORTED----END  

TRX_STATUS getCodecEntryId(char **value)
{       
   InstanceDesc *idp;
		
   if ( (idp = getCurrentInstanceDesc()) ) 
   {
      uint32 index = idp->instanceID;
      char    buf[10];
      snprintf(buf,sizeof(buf),"%lu",index);
      *value = strdup(buf);
      return TRX_OK;
   }
   return TRX_ERR;
}
  
TRX_STATUS getCodec(char **value)
{
   InstanceDesc *idp;
		
   if ( (idp = getCurrentInstanceDesc()) ) 
   {
      uint32 index = idp->instanceID;
      index--;    // Codec instances in VODSLCFM start from zero, instance/entry ids in WT-104 start from 1
      CFGMGR_VOIP_ITEM item;
      item.itemType = CFGMGR_VOIP_ITEM_CODEC_NAME;
      if ( getParamFromVoiceApp( &item , 0 , index ) == 1 )
      {
         int i=0;
         while( wt104CodecMap[i].id != CODEC_UNKNOWN )
         {
            if(wt104CodecMap[i].id==item.itemVal.valInt)
               break;
            i++;
         }
               
         *value = strdup(wt104CodecMap[i].name);
         return TRX_OK;
      }
   }
   return TRX_ERR;
}
  
TRX_STATUS getCodecBitRate(char **value)
{
   InstanceDesc *idp;
		
   if ( (idp = getCurrentInstanceDesc()) ) 
   {
      uint32 index = idp->instanceID;
      index--;    // Codec instances in VODSLCFM start from zero, instance/entry ids in WT-104 start from 1
      CFGMGR_VOIP_ITEM item;
      item.itemType = CFGMGR_VOIP_ITEM_CODEC_NAME;
      if ( getParamFromVoiceApp( &item , 0 , index ) == 1 )
      {
         int i=0;
         while( wt104CodecMap[i].id != CODEC_UNKNOWN )
         {
            if(wt104CodecMap[i].id==item.itemVal.valInt)
               break;
            i++;
         }
               
         char buf[10];      
         snprintf(buf,sizeof(buf),"%d",wt104CodecMap[i].bitRate);
         *value = strdup(buf);
         return TRX_OK;
      }
   }
   return TRX_ERR;
}
  
TRX_STATUS getCodecPtime(char **value)
{
   InstanceDesc *idp;
		
   if ( (idp = getCurrentInstanceDesc()) ) 
   {
      uint32 index = idp->instanceID;
      index--;    // Codec instances in VODSLCFM start from zero, instance/entry ids in WT-104 start from 1
      CFGMGR_VOIP_ITEM item;
      item.itemType = CFGMGR_VOIP_ITEM_CODEC_NAME;
      if ( getParamFromVoiceApp( &item , 0 , index ) == 1 )
      {
         int i=0;
         while( wt104CodecMap[i].id != CODEC_UNKNOWN )
         {
            if(wt104CodecMap[i].id==item.itemVal.valInt)
               break;
            i++;
         }
               
         *value = strdup(wt104CodecMap[i].suppPtime);
         return TRX_OK;
      }
   }
   return TRX_ERR;
}
  
TRX_STATUS getCodecSilSup(char **value)
{
   //all codecs now support silience suppression
   *value = strdup("true");
   return TRX_OK;
}

TRX_STATUS getVPEnable(char **value)
{
   *value = strdup("Enabled");
   return TRX_OK;
}
  
TRX_STATUS getVPReset(char **value)
{
   *value = strdup("false");
   return TRX_OK;
}

TRX_STATUS setVPReset(char *value)
{
   CFGMGR_VOIP_ITEM item;

   if ( strcmp(value,"true") == 0 )
   {
      /* Temporary solution - Just change any unsafe param in VODSLCFM and it will cause sipstack restart */
      item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_CALL_SERVER;
      if ( getParamFromVoiceApp( &item , 0 , 0 ) == 1 )
      {
         item.itemValType = CFGMGR_VOIP_ITEM_CHAR;
         if ( setParamInVoiceApp( &item , 0 , 0 ) == 1 )
            return TRX_OK;   
      }
      
   }
   return TRX_ERR;   
}
  
TRX_STATUS getVPNOfLines(char **value)
{
   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_DEVICE_LINE_COUNT;
   if ( getParamFromVoiceApp( &item , 0 , 0 ) == 1 )
   {   
      char buf[10];
      sprintf(buf,"%d",item.itemVal.valInt);
      *value=strdup(buf);   
      return TRX_OK;
   }
   else
      return TRX_ERR; 
}

char voiceProfName[64] = "Broadcom Voice Profile";  
TRX_STATUS getVPName(char **value)
{
   *value = strdup(voiceProfName);

   return TRX_OK;
}
  
TRX_STATUS getVPSigPro(char **value)
{
   *value = strdup("SIP");
   return TRX_OK;
}
  
TRX_STATUS getVPMaxSession(char **value)
{
   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_DEVICE_SESSION_COUNT;
   if ( getParamFromVoiceApp( &item , 0 , 0 ) == 1 )
   {
      char buf[10];
      sprintf(buf,"%d",item.itemVal.valInt);
      *value=strdup(buf);     
      return TRX_OK;
   }
   else
      return TRX_ERR;
}
  
TRX_STATUS getVPDTMFMethod(char **value)
{
   *value=strdup("InBand");
   return TRX_OK;
}

char servProvName[256]="PacketXChange Telephony Services";

TRX_STATUS getVPServprovName(char **value)
{
   *value=strdup("servProvName");
   return TRX_OK;
}

/*---------Proxy Server start -------*/  
TRX_STATUS getProxServer(char **value)
{
   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_CALL_SERVER;
   if ( getParamFromVoiceApp( &item , 0 , 0 ) == 1 )
   {
      *value=strdup(item.itemVal.valChar);   
      return TRX_OK;
   }
   else
      return TRX_ERR;
   
}


TRX_STATUS setProxServer(char *value)
{  
   uint32 index = 0;
   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_CALL_SERVER;
   sprintf(item.itemVal.valChar,"%s",value);
   item.itemValType = CFGMGR_VOIP_ITEM_CHAR;
   
   //Provmodule has shared proxy address across lines
   if ( setParamInVoiceApp( &item , 0 , 0 ) == 1 )
   {      
      //Change NVRAM params as well
      void *info;
       
      BcmCfm_objGet(BCMCFM_OBJ_VOICE, &info, &index);
      PBcmCfm_VoiceCfg_t pVoice = (PBcmCfm_VoiceCfg_t)info;
   	
      BcmCfm_free((void *)pVoice->proxAddr);
      BcmCfm_alloc(strlen(value) +1, (void **)&(pVoice->proxAddr));
      strcpy(pVoice->proxAddr, value);
       
      BcmCfm_objSet(BCMCFM_OBJ_VOICE, info, index);      
      return TRX_OK;
   }
   else 
      return TRX_ERR;
}
  
TRX_STATUS getProxServerPort(char **value)
{
   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_CALL_SERVER_PORT;
   if ( getParamFromVoiceApp( &item , 0 , 0 ) == 1 )
   {   
      char buf[10];
      sprintf(buf,"%d",item.itemVal.valInt);
      *value=strdup(buf);
      return TRX_OK;
   }
   else
      return TRX_ERR;   
}

TRX_STATUS setProxServerPort(char *value)
{
   uint32 index = 0;
   CFGMGR_VOIP_ITEM item;

   item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_CALL_SERVER_PORT;
   item.itemVal.valInt = atoi(value);
   item.itemValType = CFGMGR_VOIP_ITEM_INT;
   
   if( setParamInVoiceApp( &item , 0 , 0 ) == 1 )
   {      
      //Change NVRAM params as well
      void *info;
       
      BcmCfm_objGet(BCMCFM_OBJ_VOICE, &info, &index);
      PBcmCfm_VoiceCfg_t pVoice = (PBcmCfm_VoiceCfg_t)info;
      
      BcmCfm_free((void *)pVoice->proxPort);
      BcmCfm_alloc(strlen(value) +1, (void **)&(pVoice->proxPort));
      strcpy(pVoice->proxPort, value);
       
      BcmCfm_objSet(BCMCFM_OBJ_VOICE, info, index);  
      
      return TRX_OK;   
   }
   else
      return TRX_ERR;
}

TRX_STATUS getProxServerTransport(char **value)
{
   *value = strdup("UDP");   
   return TRX_OK;
}
/*---------Proxy Server end -------*/

/*---------Reg Server start -------*/  
TRX_STATUS getRegServer(char **value)
{
   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_REGISTRATION_SERVER;
   if ( getParamFromVoiceApp( &item , 0 , 0 ) == 1 )
   {
      *value=strdup(item.itemVal.valChar);   
      return TRX_OK;
   }
   else
      return TRX_ERR;
}

TRX_STATUS setRegServer(char *value)
{
   uint32 index = 0;
   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_REGISTRATION_SERVER;
   sprintf(item.itemVal.valChar,"%s",value);
   item.itemValType = CFGMGR_VOIP_ITEM_CHAR;
   
   //Provmodule has shared registrar address across lines
   if ( setParamInVoiceApp( &item , 0 , 0 ) == 1 )
   {      
      //Change NVRAM params as well
      void *info;
       
      BcmCfm_objGet(BCMCFM_OBJ_VOICE, &info, &index);
      PBcmCfm_VoiceCfg_t pVoice = (PBcmCfm_VoiceCfg_t)info;
   	
      BcmCfm_free((void *)pVoice->regAddr);
      BcmCfm_alloc(strlen(value) +1, (void **)&(pVoice->regAddr));
      strcpy(pVoice->regAddr, value);
       
      BcmCfm_objSet(BCMCFM_OBJ_VOICE, info, index);  
    
      return TRX_OK;
   }
   else
      return TRX_ERR;      
}
  
TRX_STATUS getRegServerPort(char **value)
{
   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_REGISTRATION_SERVER_PORT;
   if ( getParamFromVoiceApp( &item , 0 , 0 ) == 1 )
   {   
      char buf[10];
      sprintf(buf,"%d",item.itemVal.valInt);
      *value=strdup(buf);   
      return TRX_OK;
   }
   else
      return TRX_ERR;
}

TRX_STATUS setRegServerPort(char *value)
{
   uint32 index = 0;
   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_CALL_SERVER_PORT;
   item.itemVal.valInt = atoi(value);
   item.itemValType = CFGMGR_VOIP_ITEM_INT;
   

   if ( setParamInVoiceApp( &item , 0 , 0 ) == 1 )
   {      
      //Change NVRAM params as well
      void *info;
       
      BcmCfm_objGet(BCMCFM_OBJ_VOICE, &info, &index);
      PBcmCfm_VoiceCfg_t pVoice = (PBcmCfm_VoiceCfg_t)info;
   	
      BcmCfm_free((void *)pVoice->regPort);
      BcmCfm_alloc(strlen(value) +1, (void **)&(pVoice->regPort));
      strcpy(pVoice->regPort, value);
       
      BcmCfm_objSet(BCMCFM_OBJ_VOICE, info, index);  
    
      return TRX_OK;   
   }
   else
      return TRX_ERR;   
}
  
TRX_STATUS getRegServerTransport(char **value)
{
   *value = strdup("UDP");   
   return TRX_OK;
}
/*---------Reg Server end -------*/
  
TRX_STATUS getUADomain(char **value)
{
   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_DOMAIN;
   if ( getParamFromVoiceApp( &item , 0 , 0 ) == 1)
   {
      *value=strdup(item.itemVal.valChar);   
      return TRX_OK;
   }
   else
      return TRX_ERR;   
}

TRX_STATUS setUADomain(char *value)
{
   uint32 index = 0;
   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_DOMAIN;
   sprintf(item.itemVal.valChar,"%s",value);
   item.itemValType = CFGMGR_VOIP_ITEM_CHAR;
   
   if ( setParamInVoiceApp( &item , 0 , 0 ) == 1 )
   {      
      //Change NVRAM params as well
      void *info;
       
      BcmCfm_objGet(BCMCFM_OBJ_VOICE, &info, &index);
      PBcmCfm_VoiceCfg_t pVoice = (PBcmCfm_VoiceCfg_t)info;
   	
      BcmCfm_free((void *)pVoice->domain);
      BcmCfm_alloc(strlen(value) +1, (void **)&(pVoice->domain));
      strcpy(pVoice->domain, value);
       
      BcmCfm_objSet(BCMCFM_OBJ_VOICE, info, index);  
    
      return TRX_OK;   
   }
   else
      return TRX_ERR;   
}
  
TRX_STATUS getUAPort(char **value)
{
   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_LOCAL_PORT;
   if ( getParamFromVoiceApp( &item , 0 , 0 ) == 1 )
   {
      char buf[10];
      sprintf(buf,"%d",item.itemVal.valInt);
      *value=strdup(buf);   
      return TRX_OK;   
   }
   else
      return TRX_ERR;   
}

TRX_STATUS setUAPort(char *value)
{
   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_LOCAL_PORT;
   item.itemVal.valInt = atoi(value);
   item.itemValType = CFGMGR_VOIP_ITEM_INT;
   if ( setParamInVoiceApp( &item , 0 , 0 ) == 1 )
      return TRX_OK;   
   else
      return TRX_ERR;   
}

  
TRX_STATUS getUATransport(char **value)
{
   *value = strdup("UDP");   
   return TRX_OK;
}
 
TRX_STATUS getObProxy(char **value)
{
   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_OUTBOUND_PROXY;
   if ( getParamFromVoiceApp( &item , 0 , 0 ) == 1 )
   {
      *value=strdup(item.itemVal.valChar);   
      return TRX_OK;   
   }
   else
      return TRX_ERR;      
}
 
TRX_STATUS setObProxy(char *value)
{
   uint32 index = 0;
   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_OUTBOUND_PROXY;
   sprintf(item.itemVal.valChar,"%s",value);
   item.itemValType = CFGMGR_VOIP_ITEM_CHAR;
   
   if ( setParamInVoiceApp( &item , 0 , 0 ) == 1 )
   {   
      //Change NVRAM params as well
      void *info;
       
      BcmCfm_objGet(BCMCFM_OBJ_VOICE, &info, &index);
      PBcmCfm_VoiceCfg_t pVoice = (PBcmCfm_VoiceCfg_t)info;
      
      BcmCfm_free((void *)pVoice->obProxyAddr);
      BcmCfm_alloc(strlen(value) +1, (void **)&(pVoice->obProxyAddr));
      strcpy(pVoice->obProxyAddr, value);
       
      BcmCfm_objSet(BCMCFM_OBJ_VOICE, info, index);  
      
      return TRX_OK;   
   }
   else
      return TRX_ERR;      
}
 
TRX_STATUS getObProxyPort(char **value)
{
   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_OUTBOUND_PROXY_PORT;
   if ( getParamFromVoiceApp( &item , 0 , 0 ) == 1 )
   {   
      char buf[10];
      sprintf(buf,"%d",item.itemVal.valInt);
      *value=strdup(buf);   
      return TRX_OK;   
   }
   else
      return TRX_ERR;   
}

TRX_STATUS setObProxyPort(char *value)
{
   uint32 index = 0;
   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_OUTBOUND_PROXY_PORT;
   item.itemVal.valInt = atoi(value);
   item.itemValType = CFGMGR_VOIP_ITEM_INT;
   
   if ( setParamInVoiceApp( &item , 0 , 0 ) == 1 )
   {      
      //Change NVRAM params as well
      void *info;
       
      BcmCfm_objGet(BCMCFM_OBJ_VOICE, &info, &index);
      PBcmCfm_VoiceCfg_t pVoice = (PBcmCfm_VoiceCfg_t)info;
   	
      BcmCfm_free((void *)pVoice->obProxyPort);
      BcmCfm_alloc(strlen(value) +1, (void **)&(pVoice->obProxyPort));
      strcpy(pVoice->obProxyPort, value);
       
      BcmCfm_objSet(BCMCFM_OBJ_VOICE, info, index);  
    
      return TRX_OK;    
   }
   else
      return TRX_ERR;      
}

TRX_STATUS getOrganization(char **value)
{
   *value = strdup("BroadCom");   
   return TRX_OK;
}

TRX_STATUS getRegPeriod(char **value)
{
   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_REGISTRATION_PERIOD;
   if ( getParamFromVoiceApp( &item , 0 , 0 ) == 1 )
   {
      char buf[10];
      sprintf(buf,"%d",item.itemVal.valInt);
      *value=strdup(buf);   
      return TRX_OK;   
   }
   else
      return TRX_ERR;   
}
  
TRX_STATUS setRegPeriod(char *value)
{
   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_REGISTRATION_PERIOD;
   item.itemVal.valInt = atoi(value);
   item.itemValType = CFGMGR_VOIP_ITEM_INT;
   
   if ( setParamInVoiceApp( &item , 0 , 0 ) == 1 )
      return TRX_OK;
   else
      return TRX_ERR;      
}

//NOT SUPPORTED----START  

TRX_STATUS getRegExp(char **value)
{
   *value = strdup("0");  
   return TRX_OK;
}
  
TRX_STATUS getDSCPMark(char **value)
{
   *value = strdup("0");  
   return TRX_OK;
}
  
TRX_STATUS getVLANIDMark(char **value)
{
   *value = strdup("0");  
   return TRX_OK;
}
  
TRX_STATUS getEthPrioMark(char **value)
{
   *value = strdup("0");  
   return TRX_OK;
}
//NOT SUPPORTED----END  
  
TRX_STATUS getSipSubscEvNOfE(char **value)
{
   *value = strdup("0");   
   return TRX_OK;
}
  
TRX_STATUS getSipRespMapNOfE(char **value)
{
   *value = strdup("0");   
   return TRX_OK;
}
  

//NOT SUPPORTED----START
TRX_STATUS getSipEvSubscEvent(char **value)
{
   *value = strdup("");   
   return TRX_OK;
}
  
TRX_STATUS getSipEvSubsNotif(char **value)
{
   *value = strdup("");   
   return TRX_OK;
}
  
TRX_STATUS getSipEvSubsNotifPort(char **value)
{
   *value = strdup("0");   
   return TRX_OK;
}
  
TRX_STATUS getSipEvSubsNotifTrans(char **value)
{
   *value = strdup("UDP");   
   return TRX_OK;
}
//NOT SUPPORTED----END

  
TRX_STATUS getRTPLocPortMin(char **value)
{
   *value = strdup("0");   
   return TRX_OK;
}
  
TRX_STATUS getRTPLocPortMax(char **value)
{
   *value = strdup("0");   
   return TRX_OK;
}
  
TRX_STATUS getRTPDSCPMark(char **value)
{
   *value = strdup("-");   
   return TRX_OK;
}
  
TRX_STATUS getRTPTelEvpayloadType(char **value)
{
   *value = strdup("-");   
   return TRX_OK;
}
  



TRX_STATUS getVPLineCodecEntryId(char **value)
{
   InstanceDesc *idp;
		
   if ( (idp = getCurrentInstanceDesc()) ) 
   {
      uint32 index = idp->instanceID;
      char    buf[10];
      snprintf(buf,sizeof(buf),"%lu",index);
      *value = strdup(buf);
      return TRX_OK;
   }
   return TRX_ERR;
}
  
TRX_STATUS getVPLineCodecName(char **value)
{
   InstanceDesc *idp;
		
   if ( (idp = getCurrentInstanceDesc()) ) 
   {
      uint32 index = idp->instanceID;
      index--;    // Codec instances in VODSLCFM start from zero, instance/entry ids in WT-104 start from 1
      CFGMGR_VOIP_ITEM item;
      item.itemType = CFGMGR_VOIP_ITEM_CODEC_NAME;
      if ( getParamFromVoiceApp( &item , 0 , index ) == 1 )
      {
         int i=0;
         while( wt104CodecMap[i].id != CODEC_UNKNOWN )
         {
            if(wt104CodecMap[i].id==item.itemVal.valInt)
               break;
            i++;
         }
               
         *value = strdup(wt104CodecMap[i].name);
         return TRX_OK;
      }       
   }
   return TRX_ERR; 
}
  
TRX_STATUS getVPLineCodecRate(char **value)
{
   InstanceDesc *idp;
		
   if ( (idp = getCurrentInstanceDesc()) ) 
   {
      uint32 index = idp->instanceID;
      index--;    // Codec instances in VODSLCFM start from zero, instance/entry ids in WT-104 start from 1
      CFGMGR_VOIP_ITEM item;
      item.itemType = CFGMGR_VOIP_ITEM_CODEC_NAME;
      if ( getParamFromVoiceApp( &item , 0 , index ) == 1 )
      {
         int i=0;
         while( wt104CodecMap[i].id != CODEC_UNKNOWN )
         {
            if(wt104CodecMap[i].id==item.itemVal.valInt)
               break;
            i++;
         }
               
         char buf[10];      
         snprintf(buf,sizeof(buf),"%d",wt104CodecMap[i].bitRate);
         *value = strdup(buf);
         return TRX_OK;
      }
   }
   return TRX_ERR; 
}
  
TRX_STATUS getVPLineCodecPtime(char **value)
{
   InstanceDesc *idp;
   //get which line is being queried 
   InstanceDesc *curr = instanceStack[instanceDepth-1];
   int lineId = curr->instanceID - 1;     //line 0 has instance id 1...
		
   if ( (idp = getCurrentInstanceDesc()) ) 
   {
      uint32 index = idp->instanceID;
      index--;    // Codec instances in VODSLCFM start from zero, instance/entry ids in WT-104 start from 1
      CFGMGR_VOIP_ITEM item;
      item.itemType = CFGMGR_VOIP_ITEM_CODEC_PTIME;
      if ( getParamFromVoiceApp( &item , lineId , index ) == 1 )
      {
         char    buf[10];
         snprintf(buf,sizeof(buf),"%d",item.itemVal.valInt);
         *value = strdup(buf);               
         return TRX_OK;
      }
   }
   return TRX_ERR;  
}

TRX_STATUS setVPLineCodecPtime(char *value)
{
   InstanceDesc *idp;
		
   //get which line is being queried 
   InstanceDesc *curr = instanceStack[instanceDepth-1];
   int lineId = curr->instanceID - 1;     //line 0 has instance id 1...
      
   if ( (idp = getCurrentInstanceDesc()) ) 
   {
      uint32 index = idp->instanceID;
      index--;    // Codec instances in VODSLCFM start from zero, instance/entry ids in WT-104 start from 1
      CFGMGR_VOIP_ITEM item;
      item.itemType = CFGMGR_VOIP_ITEM_CODEC_PTIME;
      item.itemVal.valInt = atoi(value);
      item.itemValType = CFGMGR_VOIP_ITEM_INT;
      if ( setParamInVoiceApp( &item , lineId , index ) == 1 ) 
         return TRX_OK;                
   }
   return TRX_ERR;        
}
  
TRX_STATUS getVPLineCodecSilenceSup(char **value)
{
   InstanceDesc *idp;
   
   //get which line is being queried 
   InstanceDesc *curr = instanceStack[instanceDepth-1];
   int lineId = curr->instanceID - 1;     //line 0 has instance id 1...
   		
   if ( (idp = getCurrentInstanceDesc()) ) 
   {
      uint32 index = idp->instanceID;
      index--;    // Codec instances in VODSLCFM start from zero, instance/entry ids in WT-104 start from 1
      CFGMGR_VOIP_ITEM item;
      item.itemType = CFGMGR_VOIP_ITEM_CODEC_SILENCESUPPRESSION;
      if ( getParamFromVoiceApp( &item , lineId , index ) == 1 )
      {
         char    buf[10];
         snprintf(buf,sizeof(buf),"%d",item.itemVal.valInt);
         *value = strdup(buf);
         return TRX_OK;
      }
   }
   return TRX_ERR; 
}

TRX_STATUS setVPLineCodecSilenceSup(char *value)
{
   InstanceDesc *idp;
		
   //get which line is being queried 
   InstanceDesc *curr = instanceStack[instanceDepth-1];
   int lineId = curr->instanceID - 1;     //line 0 has instance id 1...
      
   if ( (idp = getCurrentInstanceDesc()) ) 
   {
      uint32 index = idp->instanceID;
      index--;    // Codec instances in VODSLCFM start from zero, instance/entry ids in WT-104 start from 1
      CFGMGR_VOIP_ITEM item;
      item.itemType = CFGMGR_VOIP_ITEM_CODEC_SILENCESUPPRESSION;
      item.itemVal.valInt = atoi(value);
      item.itemValType = CFGMGR_VOIP_ITEM_INT;
      if ( setParamInVoiceApp( &item , lineId , index ) == 1 )
         return TRX_OK;        
   }
   return TRX_ERR;    
}
  
TRX_STATUS getVPLineCodecEnable(char **value)
{        
   *value = strdup("true");
   return TRX_OK;
}
  
TRX_STATUS getVPLineCodecPriority(char **value)
{
   InstanceDesc *idp;
		
   //get which line is being queried 
   InstanceDesc *curr = instanceStack[instanceDepth-1];
   int lineId = curr->instanceID - 1;     //line 0 has instance id 1...
      
   if ( (idp = getCurrentInstanceDesc()) ) 
   {
      uint32 index = idp->instanceID;
      index--;    // Codec instances in VODSLCFM start from zero, instance/entry ids in WT-104 start from 1
      CFGMGR_VOIP_ITEM item;
      item.itemType = CFGMGR_VOIP_ITEM_CODEC_PRIORITY;
      if ( getParamFromVoiceApp( &item , lineId , index ) == 1 )
      {      
         char    buf[10];
         snprintf(buf,sizeof(buf),"%d",item.itemVal.valInt);
         *value = strdup(buf);
         return TRX_OK;
      }
   }
   return TRX_ERR;  
}

TRX_STATUS setVPLineCodecPriority(char *value)
{
   InstanceDesc *idp;
		
   //get which line is being queried 
   InstanceDesc *curr = instanceStack[instanceDepth-1];
   int lineId = curr->instanceID - 1;     //line 0 has instance id 1...
      
   if ( (idp = getCurrentInstanceDesc()) ) 
   {
      uint32 index = idp->instanceID;
      index--;    // Codec instances in VODSLCFM start from zero, instance/entry ids in WT-104 start from 1
      CFGMGR_VOIP_ITEM item;
      item.itemType = CFGMGR_VOIP_ITEM_CODEC_PRIORITY;
      item.itemVal.valInt = atoi(value);
      item.itemValType = CFGMGR_VOIP_ITEM_INT;
      if ( setParamInVoiceApp( &item , lineId , index ) == 1 )      
         return TRX_OK;                
   }
   return TRX_ERR;     
}
  
TRX_STATUS getLineEnable(char **value)
{
   InstanceDesc *curr = getCurrentInstanceDesc();
   int lineId = curr->instanceID - 1;     //line 0 has instance id 1...
   uint32 index = 0;

   void *info;
   BcmCfm_objGet(BCMCFM_OBJ_VOICE, &info, &index);
   PBcmCfm_VoiceCfg_t pVoice = (PBcmCfm_VoiceCfg_t)info;
   
   *value = strdup("Enabled");

   if ( lineId == 0 )
   {
      if( strcmp(pVoice->ext1,"") == 0 )
         *value = strdup("Disabled");
   }
   else if ( lineId == 1 )
   {
      if( strcmp(pVoice->ext2,"") == 0 )
         *value = strdup("Disabled");
   }
   else if ( lineId == 2 )
   {
      if( strcmp(pVoice->ext3,"") == 0 )
         *value = strdup("Disabled");
   }
   else if ( lineId == 3 )
   {
      if( strcmp(pVoice->ext4,"") == 0 )
         *value = strdup("Disabled");
   }

   BcmCfm_objFree(BCMCFM_OBJ_VOICE, info);
 
   return TRX_OK;    
}

TRX_STATUS setLineEnable(char *value)
{
   InstanceDesc *curr = getCurrentInstanceDesc();
   int lineId = curr->instanceID - 1;     //line 0 has instance id 1...   /* Update sip displayName here */
   uint32 index = 0;
   
   CFGMGR_VOIP_ITEM item;
   
   char setValue[64];
   char dialPlan[64];
   char dp[] = "%.*s|xx+*|xx+#|*6[0189]|*7[0-35]|*74%.*s|*90";

   if ( strcmp(value,"Disabled") == 0 )
      setValue[0] = '\0'   ;                     //Set to Null to disable line
    else
      sprintf( setValue,"000%d", (lineId+1) );     //Set to 000X when enabling
   
   
   /* Update dial plan here */
   sprintf( dialPlan, dp, 
            strlen(setValue), "xxxxxxxxxxxxxxxxxxxx", 
            strlen(setValue), "xxxxxxxxxxxxxxxxxxxx"
          );
          
   item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_DIALPLAN;
   sprintf(item.itemVal.valChar,"%s",dialPlan);
   item.itemValType = CFGMGR_VOIP_ITEM_CHAR;   
   if ( setParamInVoiceApp( &item , lineId , 0 ) == -1)      
      return TRX_ERR;

   /* Update dial DisplayName here */
   item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_USERNAME;
   sprintf(item.itemVal.valChar,"%s",setValue);
   item.itemValType = CFGMGR_VOIP_ITEM_CHAR;   
   if ( setParamInVoiceApp( &item , lineId , 0 ) == -1)      
      return TRX_ERR;

   /* Update sip userID here */ 
   item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_USERID;
   sprintf(item.itemVal.valChar,"%s",setValue);
   item.itemValType = CFGMGR_VOIP_ITEM_CHAR;   
   if ( setParamInVoiceApp( &item , lineId , 0 ) == -1)      
      return TRX_ERR;
   
   /* Update sip AuthName here */ 
   item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_AUTHNAME;
   sprintf(item.itemVal.valChar,"%s",setValue);
   item.itemValType = CFGMGR_VOIP_ITEM_CHAR;   
   if ( setParamInVoiceApp( &item , lineId , 0 ) == -1)      
      return TRX_ERR;


   /* Change nvram extension */
   
   void *info;
   BcmCfm_objGet(BCMCFM_OBJ_VOICE, &info, &index);
   PBcmCfm_VoiceCfg_t pVoice = (PBcmCfm_VoiceCfg_t)info;
   
   if ( lineId == 0 )
   {
      BcmCfm_free((void *)pVoice->ext1);
      BcmCfm_alloc(strlen(setValue) +1, (void **)&(pVoice->ext1));
      strcpy(pVoice->ext1,setValue);
   }
   else if ( lineId == 1 )
   {
      BcmCfm_free((void *)pVoice->ext2);
      BcmCfm_alloc(strlen(setValue) +1, (void **)&(pVoice->ext2));
      strcpy(pVoice->ext2,setValue);
   }
   else if ( lineId == 2 )
   {
      BcmCfm_free((void *)pVoice->ext3);
      BcmCfm_alloc(strlen(setValue) +1, (void **)&(pVoice->ext3));
      strcpy(pVoice->ext3,setValue);
   }
   else if ( lineId == 3 )
   {
      BcmCfm_free((void *)pVoice->ext4);
      BcmCfm_alloc(strlen(setValue) +1, (void **)&(pVoice->ext4));
      strcpy(pVoice->ext4,setValue);
   }
   
   BcmCfm_objSet(BCMCFM_OBJ_VOICE, info, index);  

   return TRX_OK;
}
  
TRX_STATUS getLineDirNum(char **value)
{
   //get which line is being queried 
   InstanceDesc *curr = instanceStack[instanceDepth];
   int lineId = curr->instanceID - 1;     //line 0 has instance id 1...
   
   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_USERID;
   if ( getParamFromVoiceApp( &item , lineId , 0 ) == 1)
   {
      *value=strdup(item.itemVal.valChar);     
      return TRX_OK;
   }
   else
      return TRX_ERR;
}
  
TRX_STATUS setLineDirNum(char *value)
{
   InstanceDesc *curr = getCurrentInstanceDesc();
   int lineId = curr->instanceID - 1;     //line 0 has instance id 1...   /* Update sip displayName here */
   uint32 index = 0;
   
   CFGMGR_VOIP_ITEM item;
   
   char dialPlan[64];
   char dp[] = "%.*s|xx+*|xx+#|*6[0189]|*7[0-35]|*74%.*s|*90";

   
   /* Update dial plan here */
   sprintf( dialPlan, dp, 
            strlen(value), "xxxxxxxxxxxxxxxxxxxx", 
            strlen(value), "xxxxxxxxxxxxxxxxxxxx"
          );
          
   item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_DIALPLAN;
   sprintf(item.itemVal.valChar,"%s",dialPlan);
   item.itemValType = CFGMGR_VOIP_ITEM_CHAR;   
   if ( setParamInVoiceApp( &item , lineId , 0 ) == -1)      
      return TRX_ERR;

   /* Update dial DisplayName here */
   item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_USERNAME;
   sprintf(item.itemVal.valChar,"%s",value);
   item.itemValType = CFGMGR_VOIP_ITEM_CHAR;   
   if ( setParamInVoiceApp( &item , lineId , 0 ) == -1)      
      return TRX_ERR;

   /* Update sip userID here */ 
   item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_USERID;
   sprintf(item.itemVal.valChar,"%s",value);
   item.itemValType = CFGMGR_VOIP_ITEM_CHAR;   
   if ( setParamInVoiceApp( &item , lineId , 0 ) == -1)      
      return TRX_ERR;
   
   /* Update sip AuthName here */ 
   item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_AUTHNAME;
   sprintf(item.itemVal.valChar,"%s",value);
   item.itemValType = CFGMGR_VOIP_ITEM_CHAR;   
   if ( setParamInVoiceApp( &item , lineId , 0 ) == -1)      
      return TRX_ERR;


   /* Change nvram extension */
   
   void *info;
   BcmCfm_objGet(BCMCFM_OBJ_VOICE, &info, &index);
   PBcmCfm_VoiceCfg_t pVoice = (PBcmCfm_VoiceCfg_t)info;
   
   if ( lineId == 0 )
   {
      BcmCfm_free((void *)pVoice->ext1);
      BcmCfm_alloc(strlen(value) +1, (void **)&(pVoice->ext1));      
      strcpy(pVoice->ext1,value);
   }
   else if ( lineId == 1 )
   {
      BcmCfm_free((void *)pVoice->ext2);
      BcmCfm_alloc(strlen(value) +1, (void **)&(pVoice->ext2));
      strcpy(pVoice->ext2,value);
   }
   else if ( lineId == 2 )
   {
      BcmCfm_free((void *)pVoice->ext3);
      BcmCfm_alloc(strlen(value) +1, (void **)&(pVoice->ext3));
      strcpy(pVoice->ext3,value);
   }
   else if ( lineId == 3 )
   {
      BcmCfm_free((void *)pVoice->ext4);
      BcmCfm_alloc(strlen(value) +1, (void **)&(pVoice->ext4));
      strcpy(pVoice->ext4,value);
   }
   
   BcmCfm_objSet(BCMCFM_OBJ_VOICE, info, index);  

   return TRX_OK;
}

TRX_STATUS getLineStatus(char **value)
{
   //NA
   *value = strdup("Up");      
   return TRX_OK;
}
  
TRX_STATUS getLineCallState(char **value)
{
   //NA
   *value = strdup("Idle");   
   return TRX_OK;
}
  

TRX_STATUS getLineSIPAuthUName(char **value)
{
   //get which line is being queried 
   InstanceDesc *curr = instanceStack[instanceDepth];
   int lineId = curr->instanceID - 1;     //line 0 has instance id 1...
   
   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_AUTHNAME;
   if ( getParamFromVoiceApp( &item , lineId , 0 ) == 1 )
   {
      *value=strdup(item.itemVal.valChar);     
      return TRX_OK;
   }
   else
      return TRX_ERR;
}

TRX_STATUS setLineSIPAuthUName(char *value)
{
   InstanceDesc *curr = getCurrentInstanceDesc();
   int lineId = curr->instanceID - 1;     //line 0 has instance id 1...   /* Update sip displayName here */
   uint32 index = 0;
   
   CFGMGR_VOIP_ITEM item;
   
   char dialPlan[64];
   char dp[] = "%.*s|xx+*|xx+#|*6[0189]|*7[0-35]|*74%.*s|*90";
   
   /* Update dial plan here */
   sprintf( dialPlan, dp, 
            strlen(value), "xxxxxxxxxxxxxxxxxxxx", 
            strlen(value), "xxxxxxxxxxxxxxxxxxxx"
          );
          
   item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_DIALPLAN;
   sprintf(item.itemVal.valChar,"%s",dialPlan);
   item.itemValType = CFGMGR_VOIP_ITEM_CHAR;   
   if ( setParamInVoiceApp( &item , lineId , 0 ) == -1)      
      return TRX_ERR;

   /* Update dial DisplayName here */
   item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_USERNAME;
   sprintf(item.itemVal.valChar,"%s",value);
   item.itemValType = CFGMGR_VOIP_ITEM_CHAR;   
   if ( setParamInVoiceApp( &item , lineId , 0 ) == -1)      
      return TRX_ERR;

   /* Update sip userID here */ 
   item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_USERID;
   sprintf(item.itemVal.valChar,"%s",value);
   item.itemValType = CFGMGR_VOIP_ITEM_CHAR;   
   if ( setParamInVoiceApp( &item , lineId , 0 ) == -1)      
      return TRX_ERR;
   
   /* Update sip AuthName here */ 
   item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_AUTHNAME;
   sprintf(item.itemVal.valChar,"%s",value);
   item.itemValType = CFGMGR_VOIP_ITEM_CHAR;   
   if ( setParamInVoiceApp( &item , lineId , 0 ) == -1)      
      return TRX_ERR;


   /* Change nvram extension */
   
   void *info;
   BcmCfm_objGet(BCMCFM_OBJ_VOICE, &info, &index);
   PBcmCfm_VoiceCfg_t pVoice = (PBcmCfm_VoiceCfg_t)info;
   
   if ( lineId == 0 )
   {
      BcmCfm_free((void *)pVoice->ext1);
      BcmCfm_alloc(strlen(value) +1, (void **)&(pVoice->ext1));
      strcpy(pVoice->ext1,value);
   }
   else if ( lineId == 1 )
   {
      BcmCfm_free((void *)pVoice->ext2);
      BcmCfm_alloc(strlen(value) +1, (void **)&(pVoice->ext2));
      strcpy(pVoice->ext2,value);
   }
   else if ( lineId == 2 )
   {
      BcmCfm_free((void *)pVoice->ext3);
      BcmCfm_alloc(strlen(value) +1, (void **)&(pVoice->ext3));
      strcpy(pVoice->ext3,value);
   }
   else if ( lineId == 3 )
   {
      BcmCfm_free((void *)pVoice->ext4);
      BcmCfm_alloc(strlen(value) +1, (void **)&(pVoice->ext4));
      strcpy(pVoice->ext4,value);
   }
   
   BcmCfm_objSet(BCMCFM_OBJ_VOICE, info, index);  

   return TRX_OK;
}
  
TRX_STATUS getLineSIPAuthPass(char **value)
{
   //get which line is being queried 
   InstanceDesc *curr = instanceStack[instanceDepth];
   int lineId = curr->instanceID - 1;     //line 0 has instance id 1...
   
   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_PASSWORD;
   if ( getParamFromVoiceApp( &item , lineId , 0 ) == 1 )
   {
      *value=strdup(item.itemVal.valChar);   
      return TRX_OK;  
   }
   else 
      return TRX_ERR;
   
}

TRX_STATUS setLineSIPAuthPass(char *value)
{
   //get which line is being queried 
   InstanceDesc *curr = instanceStack[instanceDepth];
   int lineId = curr->instanceID - 1;     //line 0 has instance id 1...
   uint32 index = 0;
      
   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_PASSWORD;
   sprintf(item.itemVal.valChar,"%s",value);
   item.itemValType = CFGMGR_VOIP_ITEM_CHAR;
   
   if ( setParamInVoiceApp( &item , lineId , 0 ) == -1 )
      return TRX_ERR;
      
   //Change NVRAM params as well
   void *info;
    
   BcmCfm_objGet(BCMCFM_OBJ_VOICE, &info, &index);
   PBcmCfm_VoiceCfg_t pVoice = (PBcmCfm_VoiceCfg_t)info;
   
   if ( lineId == 0 )
   {
      BcmCfm_free((void *)pVoice->passwd1);
      BcmCfm_alloc(strlen(value) +1, (void **)&(pVoice->passwd1));
      strcpy(pVoice->passwd1, value);
   }
   else if ( lineId == 1 )
   {
      BcmCfm_free((void *)pVoice->passwd2);
      BcmCfm_alloc(strlen(value) +1, (void **)&(pVoice->passwd2));
      strcpy(pVoice->passwd2, value);
   }
   else if ( lineId == 2 )
   {
      BcmCfm_free((void *)pVoice->passwd3);
      BcmCfm_alloc(strlen(value) +1, (void **)&(pVoice->passwd3));
      strcpy(pVoice->passwd3, value);
   }
   else if ( lineId == 3 )
   {
      BcmCfm_free((void *)pVoice->passwd4);
      BcmCfm_alloc(strlen(value) +1, (void **)&(pVoice->passwd4));
      strcpy(pVoice->passwd4, value);
   }
             
   BcmCfm_objSet(BCMCFM_OBJ_VOICE, info, index);     
   return TRX_OK;   
}
  
TRX_STATUS getLineSIPURI(char **value)
{ 
   //get which line is being queried 
   InstanceDesc *curr = instanceStack[instanceDepth];  
   int lineId = curr->instanceID - 1;     //line 0 has instance id 1...
   
   struct in_addr interfaceAddr;
   CFGMGR_VOIP_ITEM itemAuthName, itemDomain, itemInterfaceAddr;
   char uriString[389];
   
   itemAuthName.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_AUTHNAME;
   if ( getParamFromVoiceApp( &itemAuthName , lineId , 0 ) == -1 )
      return TRX_ERR;
   
   itemDomain.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_DOMAIN;
   if ( getParamFromVoiceApp( &itemDomain , 0 , 0 ) == -1)
      return TRX_ERR;
   
   if ( strcmp( itemDomain.itemVal.valChar , "0.0.0.0") != 0 && strcmp( itemDomain.itemVal.valChar , "") != 0 )
   {
      printf("%s@%s\n",itemAuthName.itemVal.valChar,itemDomain.itemVal.valChar);
      sprintf(uriString,"%s@%s",itemAuthName.itemVal.valChar,itemDomain.itemVal.valChar);
   }
   else
   {
      itemInterfaceAddr.itemType = CFGMGR_VOIP_ITEM_NETPROTOCOL_INTERFACE_ADDRESS;
      if ( getParamFromVoiceApp( &itemInterfaceAddr , lineId , 0 ) == -1 )
         return TRX_ERR;
         
      interfaceAddr.s_addr = itemInterfaceAddr.itemVal.valInt;
      
      printf("%s@%s\n",itemAuthName.itemVal.valChar,inet_ntoa(interfaceAddr));
      sprintf(uriString,"%s@%s\n",itemAuthName.itemVal.valChar,inet_ntoa(interfaceAddr));   
   }
              
   *value = strdup(uriString);   
   return TRX_OK;   
}

TRX_STATUS setLineSIPURI(char *value)
{
   char *pToken = NULL, *pLast = NULL;
   
   pToken = strtok_r(value, "@", &pLast);
   if ( pToken != NULL )
   {
      printf("TR-104: SIP USER NAME %s\n",pToken);
      
      /* Store authname,useid,username,dir,dialplan */
      if ( setLineSIPAuthUName(pToken) == TRX_ERR )
         return TRX_ERR;
               
      pToken = strtok_r(NULL, " ", &pLast);
      if ( pToken != NULL )
      {
         printf("TR-104: SIP DOMAIN %s\n",pToken);
         /* Store domain*/  
         if ( setUADomain(pToken) == TRX_ERR )
            return TRX_ERR;
      }
      return TRX_OK;
   }
   else
      return TRX_ERR;   
}

  
//NOT SUPPORTED----END  

TRX_STATUS setStatResetStats(char **value)
{
   return TRX_OK;
}
  
TRX_STATUS getStatPacketsSent(char **value)
{
   InstanceDesc *curr = getCurrentInstanceDesc();
   int lineId = curr->instanceID - 1;     //line 0 has instance id 1...   /* Update sip displayName here */

   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_STAT_PKT_SENT;
   if ( getParamFromVoiceApp( &item , lineId , 0 ) == 1 )
   {
      char buf[10];
      sprintf(buf,"%d",item.itemVal.valInt);
      *value=strdup(buf);   
      return TRX_OK;
   }
   else 
      return TRX_ERR;
}
  
TRX_STATUS getStatPacketsRcvd(char **value)
{
   InstanceDesc *curr = getCurrentInstanceDesc();
   int lineId = curr->instanceID - 1;     //line 0 has instance id 1...   /* Update sip displayName here */

   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_STAT_PKT_RECV;
   if ( getParamFromVoiceApp( &item , lineId , 0 ) == 1 )
   {
      char buf[10];
      sprintf(buf,"%d",item.itemVal.valInt);
      *value=strdup(buf);   
      return TRX_OK;
   }
   else 
      return TRX_ERR;
}
  
TRX_STATUS getStatBytesSent(char **value)
{
   InstanceDesc *curr = getCurrentInstanceDesc();
   int lineId = curr->instanceID - 1;     //line 0 has instance id 1...   /* Update sip displayName here */

   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_STAT_BYTES_SENT;
   if ( getParamFromVoiceApp( &item , lineId , 0 ) == 1 )
   {
      char buf[10];
      sprintf(buf,"%d",item.itemVal.valInt);
      *value=strdup(buf);   
      return TRX_OK;
   }
   else 
      return TRX_ERR;
}
  
TRX_STATUS getStatBytesRcvd(char **value)
{
   InstanceDesc *curr = getCurrentInstanceDesc();
   int lineId = curr->instanceID - 1;     //line 0 has instance id 1...   /* Update sip displayName here */

   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_STAT_BYTES_RECV;
   if ( getParamFromVoiceApp( &item , lineId , 0 ) == 1 )
   {
      char buf[10];
      sprintf(buf,"%d",item.itemVal.valInt);
      *value=strdup(buf);   
      return TRX_OK;
   }
   else 
      return TRX_ERR;
}
  
TRX_STATUS getStatPacketsLost(char **value)
{
   InstanceDesc *curr = getCurrentInstanceDesc();
   int lineId = curr->instanceID - 1;     //line 0 has instance id 1...   /* Update sip displayName here */

   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_STAT_PKT_LOST;
   if ( getParamFromVoiceApp( &item , lineId , 0 ) == 1 )
   {
      char buf[10];
      sprintf(buf,"%d",item.itemVal.valInt);
      *value=strdup(buf);   
      return TRX_OK;
   }
   else 
      return TRX_ERR;
}
  
/*   
TRX_STATUS getStatOverruns(char **value)
{
   return TRX_OK;
}
  
TRX_STATUS getStatUnderruns(char **value)
{
   return TRX_OK;
}
*/
  
TRX_STATUS getStatInCallsRcvd(char **value)
{
   InstanceDesc *curr = getCurrentInstanceDesc();
   int lineId = curr->instanceID - 1;     //line 0 has instance id 1...   /* Update sip displayName here */

   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_STAT_INCALL_RECV;
   if ( getParamFromVoiceApp( &item , lineId , 0 ) == 1 )
   {
      char buf[10];
      sprintf(buf,"%d",item.itemVal.valInt);
      *value=strdup(buf);   
      return TRX_OK;
   }
   else 
      return TRX_ERR;
}
  
TRX_STATUS getStatInCallsAns(char **value)
{
   InstanceDesc *curr = getCurrentInstanceDesc();
   int lineId = curr->instanceID - 1;     //line 0 has instance id 1...   /* Update sip displayName here */

   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_STAT_INCALL_ANSW;
   if ( getParamFromVoiceApp( &item , lineId , 0 ) == 1 )
   {
      char buf[10];
      sprintf(buf,"%d",item.itemVal.valInt);
      *value=strdup(buf);   
      return TRX_OK;
   }
   else 
      return TRX_ERR;
}
  
TRX_STATUS getStatInCallsConn(char **value)
{
   InstanceDesc *curr = getCurrentInstanceDesc();
   int lineId = curr->instanceID - 1;     //line 0 has instance id 1...   /* Update sip displayName here */

   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_STAT_INCALL_CONN;
   if ( getParamFromVoiceApp( &item , lineId , 0 ) == 1 )
   {
      char buf[10];
      sprintf(buf,"%d",item.itemVal.valInt);
      *value=strdup(buf);   
      return TRX_OK;
   }
   else 
      return TRX_ERR;
}
  
TRX_STATUS getStatInCallsFail(char **value)
{
   InstanceDesc *curr = getCurrentInstanceDesc();
   int lineId = curr->instanceID - 1;     //line 0 has instance id 1...   /* Update sip displayName here */

   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_STAT_INCALL_FAIL;
   if ( getParamFromVoiceApp( &item , lineId , 0 ) == 1 )
   {
      char buf[10];
      sprintf(buf,"%d",item.itemVal.valInt);
      *value=strdup(buf);   
      return TRX_OK;
   }
   else 
      return TRX_ERR;
}
  
TRX_STATUS getStatOutCallsAttempt(char **value)
{
   InstanceDesc *curr = getCurrentInstanceDesc();
   int lineId = curr->instanceID - 1;     //line 0 has instance id 1...   /* Update sip displayName here */

   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_STAT_OUTCALL_ATT;
   if ( getParamFromVoiceApp( &item , lineId , 0 ) == 1 )
   {
      char buf[10];
      sprintf(buf,"%d",item.itemVal.valInt);
      *value=strdup(buf);   
      return TRX_OK;
   }
   else 
      return TRX_ERR;
}
  
TRX_STATUS getStatOutCallsAns(char **value)
{
   InstanceDesc *curr = getCurrentInstanceDesc();
   int lineId = curr->instanceID - 1;     //line 0 has instance id 1...   /* Update sip displayName here */

   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_STAT_OUTCALL_ANSW;
   if ( getParamFromVoiceApp( &item , lineId , 0 ) == 1 )
   {
      char buf[10];
      sprintf(buf,"%d",item.itemVal.valInt);
      *value=strdup(buf);   
      return TRX_OK;
   }
   else 
      return TRX_ERR;
}
  
TRX_STATUS getStatOutCallsConn(char **value)
{
   InstanceDesc *curr = getCurrentInstanceDesc();
   int lineId = curr->instanceID - 1;     //line 0 has instance id 1...   /* Update sip displayName here */

   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_STAT_OUTCALL_CONN;
   if ( getParamFromVoiceApp( &item , lineId , 0 ) == 1 )
   {
      char buf[10];
      sprintf(buf,"%d",item.itemVal.valInt);
      *value=strdup(buf);   
      return TRX_OK;
   }
   else 
      return TRX_ERR;
}
  
TRX_STATUS getStatOutCallsFail(char **value)
{
   InstanceDesc *curr = getCurrentInstanceDesc();
   int lineId = curr->instanceID - 1;     //line 0 has instance id 1...   /* Update sip displayName here */

   CFGMGR_VOIP_ITEM item;
   item.itemType = CFGMGR_VOIP_ITEM_STAT_OUTCALL_FAIL;
   if ( getParamFromVoiceApp( &item , lineId , 0 ) == 1 )
   {
      char buf[10];
      sprintf(buf,"%d",item.itemVal.valInt);
      *value=strdup(buf);   
      return TRX_OK;
   }
   else 
      return TRX_ERR;
}
  
/* 
TRX_STATUS getStatCallsDropped(char **value)
{
   return TRX_OK;
}
  
TRX_STATUS getStatTotCallTime(char **value)
{
   return TRX_OK;
}
  
TRX_STATUS getStatServerDwnTime(char **value)
{
   return TRX_OK;
}
*/  

TRX_STATUS getTestState(char **value)
{
   *value = strdup("Error_TestNotSupported");
   return TRX_OK;
}
  
TRX_STATUS getTestSelector(char **value)
{
   *value = strdup("BRCM_TEST");  
   return TRX_OK;
}
  
TRX_STATUS getPhnConn(char **value)
{
   *value = strdup("false");  
   return TRX_OK;
}
//NOT SUPPORTED----END  
 

  
/****************************************************************************
*
*  getParamFromVoiceApp
*
*  PURPOSE:
*     Gets paramater from CFGMGR module
*
*  PARAMETERS:
*     CFGMGR_VOIP_ITEM * reqItem = Item to get
*     int line = Line associated with item
*     int instance = Instance associated with item
*
*  RETURNS:
*     
*
*  NOTES:
*
***************************************************************************/
static int getParamFromVoiceApp ( CFGMGR_VOIP_ITEM * reqItem,int line, int instance )
{
   int rdfd, wrfd, ret_val, numread;
   CFGMGR_VOIP_IPC_PACKET ipcPacket;
   char * packetPtr;
   
   packetPtr = (char *)&ipcPacket;
   
   ipcPacket.item = *reqItem;
   ipcPacket.isGet = 1;
   ipcPacket.line = line;
   ipcPacket.instance = instance;

   if ( isVoipEndptInitialized() )
   {      
      /* Create the first named - pipe */
      ret_val = mkfifo(NP1, 0666);
      
      if ((ret_val == -1) && (errno != EEXIST)) 
      {
         perror("Error creating the named pipe");
         return (-1);
      }
      
      ret_val = mkfifo(NP2, 0666);
      
      if ((ret_val == -1) && (errno != EEXIST)) 
      {
         perror("Error creating the named pipe");
         return (-1);
      }
      
      /* Open the first named pipe for writng */
      wrfd = open(NP1, O_WRONLY);
      if (wrfd == -1)
      {
         perror("Error opening the named pipe for writing");
         return (-1);        
      }
      write(wrfd, packetPtr, sizeof(ipcPacket));
      close(wrfd);	//JJC
      
      /* Open the second named pipe for reading */
      rdfd = open(NP2, O_RDONLY);
      if (rdfd == -1)    
      {
         perror("Error opening the named pipe for reading");
         return (-1);
      }
      numread = read(rdfd, packetPtr, sizeof(ipcPacket));
	  close(rdfd);	//JJC

      *reqItem = ipcPacket.item;
      return(1);
   }
   else
   {
      perror("Voice app not initialized!!");
      return(-1);
   }


}

/****************************************************************************
*
*  setParamInVoiceApp
*
*  PURPOSE:
*     Sets paramater in CFGMGR module
*
*  PARAMETERS:
*     CFGMGR_VOIP_ITEM * reqItem = Item to get
*     int line = Line associated with item
*     int instance = Instance associated with item
*
*  RETURNS:
*     
*
*  NOTES:
*
***************************************************************************/
static int setParamInVoiceApp( CFGMGR_VOIP_ITEM * reqItem, int line, int instance )
{
   int rdfd, wrfd, ret_val, numread;
   CFGMGR_VOIP_IPC_PACKET ipcPacket;
   char * packetPtr;
   
   packetPtr = (char *)&ipcPacket;
   
   ipcPacket.item = *reqItem;
   ipcPacket.isGet = -1;
   ipcPacket.line = line;
   ipcPacket.instance = instance;

   if ( isVoipEndptInitialized() )
   {
      /* Create the first named - pipe */
      
      ret_val = mkfifo(NP1, 0666);
            
      if ((ret_val == -1) && (errno != EEXIST)) 
      {
       perror("Error creating the named pipe");
       return (-1);
      }
      
      ret_val = mkfifo(NP2, 0666);
      
      if ((ret_val == -1) && (errno != EEXIST)) 
      {
       perror("Error creating the named pipe");
       return (-1);
      }
      
      /* Open the first named pipe for writng */
      wrfd = open(NP1, O_WRONLY);
      if (wrfd == -1)
      {
       perror("Error opening the named pipe for writing");
       return (-1);        
      }
      write(wrfd, packetPtr, sizeof(ipcPacket));
	  close(wrfd);	//JJC
      
      /* Open the second named pipe for reading */
      rdfd = open(NP2, O_RDONLY);
      if (rdfd == -1)    
      {
       perror("Error opening the named pipe for reading");
       return (-1);
      }
      numread = read(rdfd, packetPtr, sizeof(ipcPacket));
	  close(rdfd);	//JJC

      *reqItem = ipcPacket.item;
            
      return(1);   
   }       
   else
   {
      perror("Voice app not initialized!!");
      return(-1);
   }

}

static int isVoipEndptInitialized( void )
{
   ENDPOINTDRV_ISINITIALIZED_PARM isInitParm;
   int retVal = 0;
   int filehandle = open("/dev/bcmendpoint0", O_RDWR);

   isInitParm.size = sizeof(ENDPOINTDRV_ISINITIALIZED_PARM);

   if ( ioctl( filehandle, ENDPOINTIOCTL_ISINITIALIZED, &isInitParm ) != 0 )
   {
      perror("isVoipEndptInitialized: error during ioctl");
   }
   else
   {
      retVal = isInitParm.isInitialized;
   }

   close(filehandle);

   return( retVal );
}

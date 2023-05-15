

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0595 */
/* Compiler settings for prnd_upnp_txservice.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0595 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

/* verify that the <rpcsal.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCSAL_H_VERSION__
#define __REQUIRED_RPCSAL_H_VERSION__ 100
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __prnd_upnp_txservice_h__
#define __prnd_upnp_txservice_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IUPnPService_PlayReadyTransmitterDual_FWD_DEFINED__
#define __IUPnPService_PlayReadyTransmitterDual_FWD_DEFINED__
typedef interface IUPnPService_PlayReadyTransmitterDual IUPnPService_PlayReadyTransmitterDual;

#endif 	/* __IUPnPService_PlayReadyTransmitterDual_FWD_DEFINED__ */


#ifndef __IUPnPService_PlayReadyTransmitterDual_FWD_DEFINED__
#define __IUPnPService_PlayReadyTransmitterDual_FWD_DEFINED__
typedef interface IUPnPService_PlayReadyTransmitterDual IUPnPService_PlayReadyTransmitterDual;

#endif 	/* __IUPnPService_PlayReadyTransmitterDual_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_prnd_upnp_txservice_0000_0000 */
/* [local] */ 

const wchar_t PRND_TRANSMITTER_UPNP_SERVICE_TYPE_ID[  ]	=	L"urn:microsoft_com:service:PRNDTransmitter:1";

const wchar_t PRND_TRANSMITTER_UPNP_DEFAULT_SERVICE_ID[  ]	=	L"urn:microsoft_com:serviceId:PRNDTransmitter";

typedef /* [v1_enum] */ 
enum PLAYREADYTRANSMITTERSERVICE_DISPIDS
    {
        DISPID_A_ARG_TYPE_SESSIONID	= 1,
        DISPID_A_ARG_TYPE_LICENSEPUSHTYPE	= ( DISPID_A_ARG_TYPE_SESSIONID + 1 ) ,
        DISPID_A_ARG_TYPE_LICENSEPUSHADDRESS	= ( DISPID_A_ARG_TYPE_LICENSEPUSHTYPE + 1 ) ,
        DISPID_A_ARG_TYPE_GENERICDATA	= ( DISPID_A_ARG_TYPE_LICENSEPUSHADDRESS + 1 ) ,
        DISPID_REGISTERDEVICE	= ( DISPID_A_ARG_TYPE_GENERICDATA + 1 ) ,
        DISPID_FETCHLICENSE	= ( DISPID_REGISTERDEVICE + 1 ) 
    } 	PLAYREADYTRANSMITTERSERVICE_DISPIDS;



extern RPC_IF_HANDLE __MIDL_itf_prnd_upnp_txservice_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_prnd_upnp_txservice_0000_0000_v0_0_s_ifspec;

#ifndef __IUPnPService_PlayReadyTransmitterDual_INTERFACE_DEFINED__
#define __IUPnPService_PlayReadyTransmitterDual_INTERFACE_DEFINED__

/* interface IUPnPService_PlayReadyTransmitterDual */
/* [object][unique][dual][uuid] */ 


EXTERN_C const IID IID_IUPnPService_PlayReadyTransmitterDual;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2fb7c97a-6d3c-4957-87e0-74c8fd5ec510")
    IUPnPService_PlayReadyTransmitterDual : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_A_ARG_TYPE_SessionID( 
            /* [retval][out] */ __RPC__deref_out_opt SAFEARRAY * *pA_ARG_TYPE_SessionID) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_A_ARG_TYPE_LicensePushType( 
            /* [retval][out] */ __RPC__out long *pA_ARG_TYPE_LicensePushType) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_A_ARG_TYPE_LicensePushAddress( 
            /* [retval][out] */ __RPC__deref_out_opt BSTR *pA_ARG_TYPE_LicensePushAddress) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_A_ARG_TYPE_GenericData( 
            /* [retval][out] */ __RPC__deref_out_opt SAFEARRAY * *pA_ARG_TYPE_GenericData) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RegisterDevice( 
            /* [in] */ __RPC__in_opt IUnknown *punkRemoteEndpointInfo,
            /* [in] */ __RPC__in VARIANT *pOldSessionID,
            /* [in] */ long LicensePushType,
            /* [in] */ __RPC__in BSTR LicensePushAddress,
            /* [in] */ __RPC__in VARIANT *pRegistrationRequestMessage,
            /* [out][in] */ __RPC__inout VARIANT *pResponseSessionID,
            /* [out][in] */ __RPC__inout VARIANT *pRegistrationResponseMessage) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FetchLicense( 
            /* [in] */ __RPC__in_opt IUnknown *punkRemoteEndpointInfo,
            /* [in] */ __RPC__in VARIANT *pSessionID,
            /* [in] */ __RPC__in VARIANT *pLicenseRequestMessage,
            /* [out][in] */ __RPC__inout VARIANT *pLicenseResponseMessage) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUPnPService_PlayReadyTransmitterDualVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IUPnPService_PlayReadyTransmitterDual * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IUPnPService_PlayReadyTransmitterDual * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IUPnPService_PlayReadyTransmitterDual * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            __RPC__in IUPnPService_PlayReadyTransmitterDual * This,
            /* [out] */ __RPC__out UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            __RPC__in IUPnPService_PlayReadyTransmitterDual * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            __RPC__in IUPnPService_PlayReadyTransmitterDual * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
            /* [range][in] */ __RPC__in_range(0,16384) UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUPnPService_PlayReadyTransmitterDual * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_A_ARG_TYPE_SessionID )( 
            __RPC__in IUPnPService_PlayReadyTransmitterDual * This,
            /* [retval][out] */ __RPC__deref_out_opt SAFEARRAY * *pA_ARG_TYPE_SessionID);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_A_ARG_TYPE_LicensePushType )( 
            __RPC__in IUPnPService_PlayReadyTransmitterDual * This,
            /* [retval][out] */ __RPC__out long *pA_ARG_TYPE_LicensePushType);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_A_ARG_TYPE_LicensePushAddress )( 
            __RPC__in IUPnPService_PlayReadyTransmitterDual * This,
            /* [retval][out] */ __RPC__deref_out_opt BSTR *pA_ARG_TYPE_LicensePushAddress);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_A_ARG_TYPE_GenericData )( 
            __RPC__in IUPnPService_PlayReadyTransmitterDual * This,
            /* [retval][out] */ __RPC__deref_out_opt SAFEARRAY * *pA_ARG_TYPE_GenericData);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RegisterDevice )( 
            __RPC__in IUPnPService_PlayReadyTransmitterDual * This,
            /* [in] */ __RPC__in_opt IUnknown *punkRemoteEndpointInfo,
            /* [in] */ __RPC__in VARIANT *pOldSessionID,
            /* [in] */ long LicensePushType,
            /* [in] */ __RPC__in BSTR LicensePushAddress,
            /* [in] */ __RPC__in VARIANT *pRegistrationRequestMessage,
            /* [out][in] */ __RPC__inout VARIANT *pResponseSessionID,
            /* [out][in] */ __RPC__inout VARIANT *pRegistrationResponseMessage);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FetchLicense )( 
            __RPC__in IUPnPService_PlayReadyTransmitterDual * This,
            /* [in] */ __RPC__in_opt IUnknown *punkRemoteEndpointInfo,
            /* [in] */ __RPC__in VARIANT *pSessionID,
            /* [in] */ __RPC__in VARIANT *pLicenseRequestMessage,
            /* [out][in] */ __RPC__inout VARIANT *pLicenseResponseMessage);
        
        END_INTERFACE
    } IUPnPService_PlayReadyTransmitterDualVtbl;

    interface IUPnPService_PlayReadyTransmitterDual
    {
        CONST_VTBL struct IUPnPService_PlayReadyTransmitterDualVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUPnPService_PlayReadyTransmitterDual_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUPnPService_PlayReadyTransmitterDual_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUPnPService_PlayReadyTransmitterDual_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUPnPService_PlayReadyTransmitterDual_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUPnPService_PlayReadyTransmitterDual_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUPnPService_PlayReadyTransmitterDual_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUPnPService_PlayReadyTransmitterDual_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUPnPService_PlayReadyTransmitterDual_get_A_ARG_TYPE_SessionID(This,pA_ARG_TYPE_SessionID)	\
    ( (This)->lpVtbl -> get_A_ARG_TYPE_SessionID(This,pA_ARG_TYPE_SessionID) ) 

#define IUPnPService_PlayReadyTransmitterDual_get_A_ARG_TYPE_LicensePushType(This,pA_ARG_TYPE_LicensePushType)	\
    ( (This)->lpVtbl -> get_A_ARG_TYPE_LicensePushType(This,pA_ARG_TYPE_LicensePushType) ) 

#define IUPnPService_PlayReadyTransmitterDual_get_A_ARG_TYPE_LicensePushAddress(This,pA_ARG_TYPE_LicensePushAddress)	\
    ( (This)->lpVtbl -> get_A_ARG_TYPE_LicensePushAddress(This,pA_ARG_TYPE_LicensePushAddress) ) 

#define IUPnPService_PlayReadyTransmitterDual_get_A_ARG_TYPE_GenericData(This,pA_ARG_TYPE_GenericData)	\
    ( (This)->lpVtbl -> get_A_ARG_TYPE_GenericData(This,pA_ARG_TYPE_GenericData) ) 

#define IUPnPService_PlayReadyTransmitterDual_RegisterDevice(This,punkRemoteEndpointInfo,pOldSessionID,LicensePushType,LicensePushAddress,pRegistrationRequestMessage,pResponseSessionID,pRegistrationResponseMessage)	\
    ( (This)->lpVtbl -> RegisterDevice(This,punkRemoteEndpointInfo,pOldSessionID,LicensePushType,LicensePushAddress,pRegistrationRequestMessage,pResponseSessionID,pRegistrationResponseMessage) ) 

#define IUPnPService_PlayReadyTransmitterDual_FetchLicense(This,punkRemoteEndpointInfo,pSessionID,pLicenseRequestMessage,pLicenseResponseMessage)	\
    ( (This)->lpVtbl -> FetchLicense(This,punkRemoteEndpointInfo,pSessionID,pLicenseRequestMessage,pLicenseResponseMessage) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUPnPService_PlayReadyTransmitterDual_INTERFACE_DEFINED__ */



#ifndef __UPnPService_PlayReadyTransmitter_LIBRARY_DEFINED__
#define __UPnPService_PlayReadyTransmitter_LIBRARY_DEFINED__

/* library UPnPService_PlayReadyTransmitter */
/* [version][uuid] */ 



EXTERN_C const IID LIBID_UPnPService_PlayReadyTransmitter;
#endif /* __UPnPService_PlayReadyTransmitter_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     __RPC__in unsigned long *, unsigned long            , __RPC__in BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     __RPC__in unsigned long *, __RPC__in BSTR * ); 

unsigned long             __RPC_USER  LPSAFEARRAY_UserSize(     __RPC__in unsigned long *, unsigned long            , __RPC__in LPSAFEARRAY * ); 
unsigned char * __RPC_USER  LPSAFEARRAY_UserMarshal(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in LPSAFEARRAY * ); 
unsigned char * __RPC_USER  LPSAFEARRAY_UserUnmarshal(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out LPSAFEARRAY * ); 
void                      __RPC_USER  LPSAFEARRAY_UserFree(     __RPC__in unsigned long *, __RPC__in LPSAFEARRAY * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     __RPC__in unsigned long *, unsigned long            , __RPC__in VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     __RPC__in unsigned long *, __RPC__in VARIANT * ); 

unsigned long             __RPC_USER  BSTR_UserSize64(     __RPC__in unsigned long *, unsigned long            , __RPC__in BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal64(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal64(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out BSTR * ); 
void                      __RPC_USER  BSTR_UserFree64(     __RPC__in unsigned long *, __RPC__in BSTR * ); 

unsigned long             __RPC_USER  LPSAFEARRAY_UserSize64(     __RPC__in unsigned long *, unsigned long            , __RPC__in LPSAFEARRAY * ); 
unsigned char * __RPC_USER  LPSAFEARRAY_UserMarshal64(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in LPSAFEARRAY * ); 
unsigned char * __RPC_USER  LPSAFEARRAY_UserUnmarshal64(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out LPSAFEARRAY * ); 
void                      __RPC_USER  LPSAFEARRAY_UserFree64(     __RPC__in unsigned long *, __RPC__in LPSAFEARRAY * ); 

unsigned long             __RPC_USER  VARIANT_UserSize64(     __RPC__in unsigned long *, unsigned long            , __RPC__in VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal64(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal64(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree64(     __RPC__in unsigned long *, __RPC__in VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



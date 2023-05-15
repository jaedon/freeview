
#ifndef USB_INTERFACE__INCLUDED__
#define USB_INTERFACE__INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

/**************************** Function Declarations ***************************/

lc_result LC_USB_SetCD5Data( lc_uchar * pCD5, lc_sint32 nLength );
lc_result LC_USBDataRequest(lc_uchar *out_pData,lc_uint32 length,lc_uint32 offset );

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* USB_INTERFACE__INCLUDED__ */



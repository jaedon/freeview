
#ifndef KEYUPD_H__INCLUDED__
#define KEYUPD_H__INCLUDED__

/* Default maximum increment for Key version,  */
#define DEFAULT_MAX_INCREMENT_FOR_KEY_VERSION (99)

/* Typical UK value */
typedef enum _lc_typical_system_id
{
    SYS_ID_MANKEY = 0x0000,

    SYS_ID_DEVKEY = 0xFFFF
    
} lc_typical_system_id;


/* Key update message */
#define SIZE_CLEARCONTROL (4)    /* size of clear section of control block */
#define SIZE_NEWCONTROL   (16)   /* size of complete control block */

#define SIZE_FIXED        (8)    /* fixed header length */
#define SIZE_PUBKEY       (104)  /* available space for public key */
#define MASTERMODLEN      (128)
#define MASTEREXPLEN      (8)

typedef struct _lc_key_update_message
{
   lc_uchar abClearControl [SIZE_CLEARCONTROL];  /* 0:4,sysid:12,keyver:16 */
   lc_uchar abFixed [SIZE_FIXED];                /* 0x8011223344556677:64 */
   lc_uchar abFullControl [SIZE_NEWCONTROL];     /* (0:4,sysid:12,keyver:16)*2,(sigver)*3,?rsabypass:1,?defclear:1,rsrvd:14 */
   lc_uchar abPubKey [SIZE_PUBKEY];              /* modlen:8,explen:8,invmod:(modlen*8),exp:(explen*8),(0xA5:8)*n */
} lc_key_update_message;



/** 
 * public key authentication.
 * This routine accepts the 1104 bit (138 byte) key message from
 * the head-end.  If the message is valid, the IRD code
 * download/verifier public key is updated.
 */
lc_result LC_AuthenticateUK(
    const lc_uchar* pabMsg, 
    lc_uint32 wMsgLen, 
    lc_bool *pIsAManKey);

/** 
 * Get public key from UK module.
 */
lc_result LC_RetrieveUK(lc_uchar** ppUkData, lc_uint32* pUkLength);

/** 
 * Check the validity of master key.
 */
lc_result LC_CheckMasterKey(void);

/** 
 * Get clear (decrypted) public key.
 */
lc_result LC_GetClearKeyData (lc_key_update_message* pClearKeyData);


#endif /* KEYUPD_H__INCLUDED__ */



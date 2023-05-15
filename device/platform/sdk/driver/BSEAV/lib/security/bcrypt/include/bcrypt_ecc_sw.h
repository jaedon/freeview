/********************************************************************************************
 * ECC implementaion is from "Freeme" open source, see bellow for license term.
 * 
 *A license????  For anonymously published software?  Yes!  The purpose
 *here is to outline how I would like the software used.  Putting
 *together all this has been a lot of work, and I hope people can
 *respect the purpose behind the software, as spelled out here.
 *
 *	1.  The purpose of this software is to re-assert your rights over
 *    fair use of audio files that you have legally purchased or
 *    otherwise obtained legally.  Please use it for that purpose only.
 *    Do not use it to unprotect files you don't have a legal right to,
 *    or to unprotect legal files for the purpose of re-distributing
 *    them to others who do not have a legal right to the content.  In
 *    other words, in use of this software obey traditional copyright
 *    laws -- but the DMCA may be completely ignored as far as this
 *    license concerned (although you must accept responsibility for
 *    ignoring this law, since it is enforceable).
 *
 *  2. This is free software, without any warranties, guarantees, or any
 *    assurance that it will work as described.  It relies on certain
 *    other software (from Microsoft) operating as it currently does,
 *    so I don't take any responsibility for what happens if Microsoft
 *    updates their software to render this useless, or even if they
 *    put bombs in their new software to erase all your files if they
 *    detect this software.  But I sure hope they wouldn't do that.
 *
 * $brcm_Workfile: bcrypt_ecc_sw.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 5/23/12 2:24p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /BSEAV/lib/security/bcrypt/include/bcrypt_ecc_sw.h $
 * 
 * 1   5/23/12 2:24p celysee
 * SW7425-2894: Adding common drm adn dtcp-ip
 * 
 * 2   4/18/12 6:12p celysee
 * SW7425-2894: Support for DTCP-IP with robustness rules
 * 
 * 1   8/16/11 2:23p cdisc
 * SW7425-1114: adding Common DRM for 7425 into AppLibs
 * 
 * dtv_v3.0/1   6/15/11 5:30p cdisc
 * SWDTV-7493:[ see Broadcom Issue Tracking JIRA for more info ].
 *  General_Comments: This checkin adds support for Openssl functionality
 *  and multithreading
 * File_Specific_Comments:
 * Users should see drm_common_swcrypto.h for new API definitions.
 * Reviewed_By:
 * QA_Concerns:
 * What_Else_Affected: N/A
 * Unit_Testing_Performed: Netflix/WV work on CARD and Vizio 35230.
 * Review_Comments: N/A
 * 
 * dev_dtv_v3.0_SWDTV-7493/1   6/15/11 11:39a cdisc
 * SWDTV-7493: adding openssl and multithread support
 * 
 *****************************************************************************/

#if !defined( _ECC_H )
#define _ECC_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <openssl/bn.h>

/*
 * If the "neg" field of the BIGNUM struct is set, then this point is
 * the identity.  This is a terrible way to do this, since it's not
 * clear what the future of this flag is -- however, for now it works,
 * and it's fast...
 */

typedef struct eccpt_st {
	BIGNUM *x, *y;
} ECCpt;

typedef struct eccparam_st {
	BIGNUM *modulus;	/* Curve is over Z_modulus */
	BIGNUM *a, *b;		/* Curve coefficients */
  BIGNUM *n;        /* Order of Generator */
	ECCpt generator;	/* Generator for our operations */
	ECCpt pubkey;		/* Public key */
	BIGNUM *privkey;	/* Corresponding private key */
} ECC;


ECC *ECC_new_set(BIGNUM * p, BIGNUM * a, BIGNUM * b, ECCpt g, BIGNUM *n);
void ECC_free(ECC * ecc);

void ECCpt_init(ECCpt * pt);
void ECCpt_free(ECCpt * pt);

int ECCpt_is_valid_pt(ECCpt * a, ECC * ecc);
int ECCpt_is_equal(ECCpt * a, ECCpt * b);
void ECCpt_add(ECCpt * r, ECCpt * a, ECCpt * b, ECC * ecc);
void ECCpt_mul(ECCpt * r, ECCpt * a, BIGNUM * n, ECC * ecc);

#ifdef __cplusplus
}
#endif

#endif

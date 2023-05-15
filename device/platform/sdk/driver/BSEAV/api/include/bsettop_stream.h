/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_stream.h $
 * $brcm_Revision: 21 $
 * $brcm_Date: 2/16/10 5:09p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/include/bsettop_stream.h $
 * 
 * 21   2/16/10 5:09p vsilyaev
 * SW3556-1051: Added bdecode_timestamp_order
 * 
 * 20   5/11/09 1:35p vsilyaev
 * PR54955: NW record of TTS streamer over HTTP: add a flag to stream
 * status to indicate if this stream is associated w/ TTS content
 * 
 * PR54955/1   5/11/09 1:17p ssood
 * PR54955: NW record of TTS streamer over HTTP: add a flag to bstream
 * status to indicate if this stream is associated w/ TTS content
 * 
 * 19   2/12/08 5:20p vsilyaev
 * PR 36787: Added M2 support
 * 
 * 18   8/22/07 5:29p jgarrett
 * PR 32044: Merging to main branch
 * 
 * 17   8/22/07 5:26p jgarrett
 * PR 31818: Adding external encryption algorithm support
 * 
 * 7405_pip_support/1   8/21/07 2:38p katrep
 * PR34204: Added mosaic mode support for 7405.
 * 
 * PR32044/1   6/26/07 5:54p jgarrett
 * PR 32044: Adding Mosaic Mode APIs
 * 
 * 16   7/16/07 3:16p vsilyaev
 * PR 32275: Added use_cps flag
 * 
 * 15   7/10/07 5:05p vsilyaev
 * PR 32849: Added list of streamer and input band object ID's
 * 
 * 14   4/27/07 4:10p vsilyaev
 * PR 29967: Added PCR monitor API
 * 
 * 13   4/19/07 2:04p jgarrett
 * PR 29901: Updating documentation for bstream_set_mpeg_parameters
 * 
 * 12   3/14/07 3:27p vsilyaev
 * PR 26200: Changes in bcrypto configuration structures
 * 
 * 11   1/18/07 11:36a vsilyaev
 * PR 27186: Differentiate between parser and input bands. AKA, fixing
 * bmessage from playback source
 * 
 * 10   11/27/06 4:26p mphillip
 * PR26154: Fix bencryption_params for key structs
 * 
 * 9   11/17/06 10:34a erickson
 * PR24374: update docs
 * 
 * 8   10/24/06 1:45a jrubio
 * PR25112:Adding support for rec PID 0
 * 
 * 7   10/18/06 1:31p mphillip
 * PR24031: Separation of odd/even key setting for 7038, settop api-level
 * CA change
 * 
 * 6   7/11/06 5:47p mphillip
 * PR19544: Initial commit of bcrypt_ stubs
 * 
 * 5   7/11/06 9:58a jrubio
 * PR20382: Added necessary structures for dual stream mode ( dual pes
 * decode)
 * 
 * 4   8/10/05 3:17p erickson
 * PR16478: added bstream_set_mpeg_parameters
 * 
 * 3   7/11/05 2:27p erickson
 * PR16044: updated documentation for bmessage
 * 
 * 2   5/19/05 6:53p mphillip
 * PR15267: Basic SetTop API support for decryption of AES and DVB files
 * for playback
 * 
 * 1   2/7/05 7:06p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/24   10/28/04 7:43p vsilyaev
 * PR 13097: Added video coding type.
 * 
 * Irvine_BSEAVSW_Devel/23   9/23/04 3:59p marcusk
 * PR12698: Use upper 8 bits of audio pid to specify track number for
 * AC3/DTS in VOB streams.
 * 
 * Irvine_BSEAVSW_Devel/22   9/23/04 2:28p marcusk
 * PR12698: Updated with final comments. Store state of previous audio
 * decode for remapping when switching audio between main and pip.
 * 
 * Irvine_BSEAVSW_Devel/21   7/14/04 2:37p erickson
 * PR11682: added support for changing encryption params during decode
 * 
 * Irvine_BSEAVSW_Devel/20   7/8/04 3:16p erickson
 * PR11655: clarified encryption documentation
 * 
 * Irvine_BSEAVSW_Devel/19   6/25/04 9:15a erickson
 * PR11135: updated comments
 * 
 * Irvine_BSEAVSW_Devel/18   6/8/04 7:40a erickson
 * PR11135: added iniital playpump impl for 97038
 * 
 * Irvine_BSEAVSW_Devel/17   6/2/04 2:44p erickson
 * PR11204: changed/removed deprecated settop api functions
 * 
 * Irvine_BSEAVSW_Devel/16   5/25/04 11:42a erickson
 * PR11204: added bband_get and bband_settings in preparation for
 * deprecation of bband_t public impl
 * 
 * Irvine_BSEAVSW_Devel/15   11/4/03 2:42p erickson
 * documentation changes
 * 
 * Irvine_BSEAVSW_Devel/14   10/31/03 11:58a erickson
 * settop api changes after internal review
 * 
 * Irvine_BSEAVSW_Devel/13   10/28/03 11:27a erickson
 * settop api reworks after internal design view
 * 
 * Irvine_BSEAVSW_Devel/12   10/22/03 11:23a erickson
 * settop api pass-through type conversion
 * 
 * Irvine_BSEAVSW_Devel/11   9/30/03 1:13a erickson
 * updated docjet documentation
 * 
 * Irvine_BSEAVSW_Devel/10   9/25/03 1:28p erickson
 * added cpu-based encryption to settop api and util
 * 
 * Irvine_BSEAVSW_Devel/9   9/17/03 5:04p erickson
 * updated documentation for DocJet
 * 
 * Irvine_BSEAVSW_Devel/8   8/22/03 1:22p erickson
 * stream status uses ptr to mpeg
 * 
 * Irvine_BSEAVSW_Devel/7   8/21/03 4:31p erickson
 * fixed up comments
 * 
 * Irvine_BSEAVSW_Devel/6   8/18/03 11:02a erickson
 * initial bringup for brutus 7320, still in progress
 * 
 * Irvine_BSEAVSW_Devel/5   7/30/03 1:43p erickson
 * moved BSETTOP_MAX_PROGRAMS from bsettop_types.h
 * 
 * Irvine_BSEAVSW_Devel/4   7/30/03 12:13p erickson
 * support absoluted band selection
 * 
 * Irvine_BSEAVSW_Devel/3   7/25/03 4:22p erickson
 * in the middle of rework, does not compile
 * 
 * Irvine_BSEAVSW_Devel/2   7/22/03 7:09p vsilyaev
 * Renamed pidtype to the stream type.
 *
 * Irvine_BSEAVSW_Devel/1   7/9/03 5:19p erickson
 * rework for new bband_t model
 *
 ***************************************************************************/
#ifndef BSETTOP_STREAM_H__
#define BSETTOP_STREAM_H__

#ifdef __cplusplus
extern "C" {
#endif

/*=****************************************
The stream functions are needed to manipulate digital streams.

The digital tuning functions (found in bsettop_tuner.h) will return a bband_t handle.
That band handle represents a path of MPEG2 Transport in the chip.
In order to decode a program, you need to provide MPEG2 data. The band and
MPEG2 data are joined together using bstream_open in order to create
a stream which can be decoded or recorded.

Obtaining MPEG2 data must be done in an application-specific way. One way
is to parse MPEG2 Transport data for PSI information. This can be done
using the bsettop_message API and some PSI parsing software.
*******************************************/

/* There are three families of algorithms:
Summary:
Encryption types for use in bencryption_params.

Description:

Ciphers can be used to encrypt/decrypt streams of data, and they generally work on blocks of data.
A cipher requires a block mode (bcrypt_blockmode).  Alignment requirements vary by algorithm, 
consult the bcrypt documentation for more.

Hashes are used to create a fixed-size fingerprint value for a block of data.  
These hashes can be used for verification, or as the data used for a sign/verify algorithm.

Public key algorithms are used for digital signing and verifying of data.[*]
These usually fall under public key systems, and often require some form of public key exchange.
One type of usage involves signing e-mails using a certificate.

Of the common algorithms listed, DSA/SHA1/DES/3DES are US Government algorithms devised for and used in 
governmental security applications.  Due to ongoing advances in cryptography and computing power, 3DES is an 
extension of DES to extend the life of the cipher.  There was a cryptographic competition held to create a 
replacement.  AES is the result.  Whereas DSA is not generally used for day to day sign/verify operations 
outside of the US government, DES, Triple DES, and SHA-1 all entered into regular usage.

RSA is a public key algorithm devised outside the US Government.

[*] To be precise, the sign/verify operations actually involve encryption/decryption, but the nature of the 
algorithms prevents them from being effective streaming ciphers.  As a result, an encrypted and signed block of data will 
usually have a random session key protected by the public key system used to sign the block of data in the first place.
If one is operating at a "decrypt this stream" level, the above overview is sufficient.
*/
typedef enum bencryption_type {
	bencryption_type_none = 0,	/* no encryption */
	bencryption_type_3des,		/* 3DES Cipher ("Triple DES"), see FIPS */
	bencryption_type_des,		/* DEA Cipher (Defense Encryption Algorithm, commonly known as DES), see FIPS 46-3 */
	bencryption_type_aes,		/* AES Cipher (Advanced Encryption Standard, the DEA replacement), see FIPS  */
	bencryption_type_dvb,		/* DVB-CS Cipher (SuperScrambler) */
	bencryption_type_sha1,		/* SHA-1 hash (Secure Hash Algorith) */
	bencryption_type_md5,		/* MD5 hash (*/
	bencryption_type_rsa,		/* RSA public key sign/verify */
	bencryption_type_dsa,		/* DSA public key sign/verify */
	bencryption_type_rc4,		/* RC4 Cipher */
	bencryption_type_c2,		/* Cryptomeria Cipher (C2) (Media-based cipher) */
	bencryption_type_css,		/* CSS (DVD) */
	bencryption_type_external,	/* external algorithm (encrypted PVR only, see bsettop_crypto.h for the long_key structure to use) */
	bencryption_type_multi2		/* Multi2 (CA only) */
} bencryption_type;

/*
Summary:
CA residue mode enum

Description:
This enum selects a residue mode for certain conditional access operations.
*/
typedef enum bencryption_residue {
	bencryption_residue_none = 0,	/* no residue mode */
	bencryption_residue_block_term,	/* residual block termination */
	bencryption_residue_cipher_text	/* cipher-text stealing */
} bencryption_residue;

/* Cipher block modes.  Not all block modes are supported by all cipher algorithms in all hardware implementations.
 * See the bcrypto documentation for more detail.
 */
/*
Summary:
Cryptographic block mode selection
Description:
This controls which block mode to use with the chosen cipher.  As a general rule, ECB should not be used to encrypt 
more than a single block of data (8 bytes for most algorithms).  CBC is the usual choice here.
*/
typedef enum bcrypto_blockmode {
	bcrypto_blockmode_ecb,
	bcrypto_blockmode_cbc,
	bcrypto_blockmode_cfb,
	bcrypto_blockmode_ofb,
	bcrypto_blockmode_ctr
} bcrypto_blockmode;

/*
Summary:
Encryption type and key for use in network encryption and PVR encryption.

Description:
type contains the algorithm type (see bencryption_type for details).

key_length indicates the length of the key, in bits.

key is a big-endian byte array of the key.  Only as many bytes as needed should be used.  For per-algorithm specificity, please consult the bcrypt documentation.

iv contains initialization vectors for specific cipher block modes.  This is ignored on non-cipher algorithms, and in ECB mode.

long_key should be used to point to the key if 256 bits is inadequate to store the desired key.  If key_length is greater than 256, the key will be pulled from this variable rather than key (to allow for very large keys while still keeping cipher usage simple).  Certain encryption modes (for instance, RSA) have a specific struct pointed to by this, key_length in this case will be the size of the struct (in bits) rather than necessarily the size of the key (which will be inside the struct).  See bsettop_crypto.h for details.

Please consult the bcrypto documention for the precise key layout for any given algorithm.

When used with bstream_set_encryption, the members odd, even, and pid control how the key is applied.  For instance, pid can specify an audio or video pid, and it can be applied to even, odd, or both (if both even and odd are true) packets.
*/
typedef struct bencryption_params {
	bencryption_type type;      /* encryption type. If set to "none", all other values in this structure are ignored. */
	bcrypto_blockmode blockmode;/* cipher block mode, e.g. CBC, ECB, etc. */
	bencryption_residue residue;/* residue mode, should be bencryption_residue_none except in special cases */
	bool odd;                   /* encryption keys are applied for 'odd' packets */
	bool even;                  /* encryption keys are applied for 'even' packets */
	bool key_ladder;            /* derive key via key_ladder (requires long_key to point to bcrypto_keyladder_data) */
	bool use_cps;               /* only used in encrypted PVR under special circumstances */
	uint16_t pid;               /* apply key to this pid, 0x1fff to apply to all pids in this bstream */
	unsigned key_length;        /* the length of the encryption key, in bits, formerly called "keysize". */
	uint8_t key[32];            /* encryption key. */
	uint8_t iv[32];             /* initialization vector, used for some cipher block modes such as CBC. */
	uint16_t m2_round;          /* used with multi2 */
	uint8_t m2_key_select;      /* used with multi2 */
	uint8_t m2_sys_key[32];     /* used with multi2 */
	const void *long_key;       /* encryption key. */
} bencryption_params;

/*
Summary:
Initializes the bencryption_params data structure.
*/
void bencryption_params_init(
	bencryption_params *encryption	/* [out] */
);

/*
Summary:
Maximum number of video or audio programs which the bstream_mpeg data structure stores.
stores.
Description:
If your application needs more, you'll need to override this value.
*/
#define BSETTOP_MAX_PROGRAMS 8

/*
Summary:
This number indicates to record the PAT PID 0. Please use this define
only in the anciallary.pid section of bstream_mpeg 
Description:
*/
#define BSETTOP_PAT_PID 0x2000

/*
Summary:
MPEG parameters for a single program with the same pcr pid which are passed to bstream_open and bplayback_start.

Description:
The reason for multiple video and audio pids is to support multiple elementary
streams within the same program. All video and audio pids should use the same
pcr pid.

Multiple audio elementary streams are needed for multiple language support (i.e. English
and Spanish). Multiple video elementary streams might be needed for multiple
viewing angles of the same program.

You should not use this structure to store mutliple programs for channel change.
Each "channel" should have its own bstream_mpeg structure because each channel
has its own pcr pid.

For PES decode the pid value is used to represent the PES packet id.  Typically this
will be 0xE0 for video, 0xC0 for mpeg audio, and 0xBD for ac3 audio.  

For VOB decode, video and mpeg audio type streams are handled identically to a PES
based decode.  AC3 and DTS tracks ares slightly different as the upper 8 bits 
of the pid value represent the substream id that is to be decoded.  For example: 
pid==0x00BD would correspond to AC3/DTS track 0, while 0x01BD would correspond to 
AC3/DTS track 1.
*/
typedef struct bstream_mpeg {
  struct {
  	uint16_t pid;
  	bvideo_codec format;
  } video[BSETTOP_MAX_PROGRAMS];	/* video stream pids */
  struct {
  	uint16_t pid;
  	baudio_format format;
  } audio[BSETTOP_MAX_PROGRAMS];	/* audio stream pids */
  struct {
	  uint16_t pid;
  } ancillary[BSETTOP_MAX_PROGRAMS];	/* data channels */
  uint16_t pcr_pid;
  bstream_mpeg_type mpeg_type;
  bencryption_params encryption;
  bdecode_timestamp_order timestamp_order;
} bstream_mpeg;

/*
Summary:
	Required to initialize int bstream_mpeg data structure.
*/
void bstream_mpeg_init(
	bstream_mpeg *mpeg		/* [out] */
	);

/**
Summary:
	Get the input band for a streamer input.

Description:
	For now, the following id's have these meanings:
	0 = streamer 1 (TS0_IN parallel input on board)
	1 = streamer 2 (TS1_IN parallel input on board)
	100..106 = absolute input band 0..6. used for system debug.

	For platform portability reasons please consider using list of streamers and input_bands from the
	bconfig_board_resources .

	Notice that there is no streamer object. The streamer id is assigned to a band based on the
	platform.

**/
bband_t bstreamer_attach(
	bobject_t id,
	bstream_mpeg_type mpeg_type	/* The known mpeg type for this streamer. Only the developer
								knows what this is. */
	);

/**
Summary:
Dynamically create a digital stream using a band and mpeg parameters.

Description:
This is used for digital streams which are created from a tuned band or streamer band.
The application supplies MPEG parameters which are obtained from scanning PSI information
or stored separately.

On some platforms, you can call bstream_open on the same band more than once. In this case,
the transport block routes the data from one input band to multiple parser bands.

You must call bstream_close to release resources.
**/
bstream_t bstream_open(
	bband_t band,	/* input band */
	const bstream_mpeg *mpegparams	/* MPEG pids to filter */
	);

/**
Summary:
Update encryption parameters after a stream has been created.

Description:
This is used for switching odd/even network encryption keys during decode.
**/
bresult bstream_set_encryption(
	bstream_t stream,
	const bencryption_params *encryption
	);

/**
Summary:
Join a stream.

Description:
This joins a video stream and audio stream together. The
combined stream is now the video_source.
**/

void bstream_join(bstream_t video_source, bstream_t audio_source);

/**
Summary:
Open a child stream from a parent stream.

Description:
This will create a child stream utilizing the same internal transport pipe as
the parent stream.  This is primarily used in mosaic mode, where streams
are expected to be on the same transport pipe.  The parent stream can be from
either bstream_open (live decode) or from bplayback/bplaypump (PVR/IP playback).
The returned stream should be closed by bstream_close.
**/

bstream_t bstream_open_child(bstream_t parent, const bstream_mpeg *mpegparams);

/**
Summary:
Close a stream.

Description:
This is required if you use bstream_open to obtain a stream.
This is allowed but not required if you get an analog stream from btuner or 
any stream from bplayback or bplaypump.
**/
void bstream_close(
	bstream_t stream	/* Handle returned by bstream_open, btuner_tune_rf,
							btuner_tune_linein or bplayback_start. */
	);

/*
Summary:
	Digital stream status returned by bstream_get_mpeg_parameters.
**/
typedef struct bstream_status {
	bband_t band; /* This is an input band. It is never a prser band. For playback streams, this
			will always be NULL. Use bband_settings to get the input band number. */
	unsigned parser_band; /* This is internal information about the parser band used for this
			and is included here for diagnostic information only. */
	bstream_mpeg mpeg; 
	bdecode_t decode; /* if the stream is being decoded */
    bool timestamp_enabled; /* true if timestamp is being enabled for this stream */
} bstream_status;

/**
Summary:
	Get status information from a digital stream.
Description:
	This is useful for getting MPEG parameters from an encoded stream where the bsettop_mpeg structure
	is created internally.
	If the stream is analog, it will fail.
**/
bresult
bstream_get_mpeg_parameters(
	bstream_t stream,
	bstream_status *status	/* [out]*/
	);

/*
Summary:
Update the bstream_mpeg parameters of a digital stream that is already open.
 
Description:
Changes are incremental: new pids are added; unused pids are removed; unchanged
pids are unaffected (i.e. no glitching or errors introduced).
 
For decode, either from live or playback, there will be no change in decode until a detach/attach 
of either video or audio is done. The application is responsible for resetting any audio or video program index before
reattaching. See bdecode_set_audio_program and bdecode_set_video_program.
This does not change the PCR being used for TSM. That cannot be changed without a stop/start.
 
For record, the addition or removal of the pids is immediate. Also, this does not change the video 
pid being indexed. That cannot be changed without a stop/start.
 
This is not supported on legacy platforms.
*/
bresult
bstream_set_mpeg_parameters(
	bstream_t stream,
	const bstream_mpeg *mpeg
	);
	
/**
Summary:
Settings for a input band.
Description:
These are read-only settings which can be obtained with bband_get.
**/
typedef struct bband_settings {
	unsigned bandnum;				/* The transport input band number. This is
				not a parser band. See bstream_status for parser band. */   
	bstream_mpeg_type mpeg_type;	/* The type of data on that input band */
	btuner_t tuner;					/* What tuner this band is associated with.
										For steamers, this is NULL. */
} bband_settings;

/**
Summary:
Get settings for a band.
**/
void bband_get(
	bband_t band,
	bband_settings *settings /* [out] */
	);


/* 
 * Start PCR monitoring. get accumulated information with
 * bstream_get_pcr_status.
 * If the PCR block is already in use with a different stream, this function
 * will fail.
 * */
bresult bstream_start_pcr_monitor(bstream_t stream);

/* stop PCR monitoring.  */
void bstream_stop_pcr_monitor(bstream_t stream);

typedef struct bstream_pcr_status {
	uint32_t pcr_count; /* number of PCR's received since call to bstream_start_pcr_monitor */
	bool pcr_valid; /* valid PCR was captured from the stream since call to bstream_start_pcr_monitor */
	bool monitor_started; /* indicated whether PCR monitor is enabled */	
} bstream_pcr_status;

/* get status of the PCR monitor */
bresult bstream_get_pcr_status(bstream_t stream, bstream_pcr_status *status);

#ifdef __cplusplus
}
#endif

#endif /* BSETTOP_STREAM_H__ */

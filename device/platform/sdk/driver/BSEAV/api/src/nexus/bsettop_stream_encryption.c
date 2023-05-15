/***************************************************************************
 *	   Copyright (c) 2003-2009, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_stream_encryption.c $
 * $brcm_Revision: 4 $
 * $brcm_Date: 1/7/09 4:04p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/bsettop_stream_encryption.c $
 * 
 * 4   1/7/09 4:04p mphillip
 * PR50093: Add Multi2 support to the nexus shim
 * 
 * 3   12/24/08 11:57a jgarrett
 * PR 50703: Allowing security module to be absent
 * 
 * 2   7/23/08 9:48a erickson
 * PR45058: fix static array overrun bug
 *
 * 1   7/2/08 5:11p vishk
 * PR 40020: bsettop_stream (crypto, network decryption): Develop
 * SettopAPI-to-Nexus shim layer
 *
 ***************************************************************************/

#include "bsettop_impl.h"
#include <string.h>


BDBG_MODULE(stream_encryption);


void bencryption_params_init(bencryption_params *encryption)
{
	if (encryption) {
		BKNI_Memset(encryption, 0, sizeof(*encryption));
		encryption->pid = 0x1fff; /* default for bstream_set_encryption usage is to apply to all pids unless the pid is set */
	}
}

bresult bstream_p_lookup_pid_info(bstream_t stream, unsigned short pid, unsigned *pid_channel, NEXUS_KeySlotHandle *KeySlotHandle, unsigned int *index)
{
	int i;
	NEXUS_PidChannelStatus Status;

	for (i=0;i<BSETTOP_MAX_PIDS;i++) {
		/* Check the implementation of pid != 0xFFFF */
		if (stream->pids[i].pid == pid) {
			*pid_channel = stream->pids[i].pidchannel;
			*KeySlotHandle = stream->pids[i].KeySlotHandle;
			return b_ok;
		}
	}

	for (i=0;i<BSETTOP_MAX_PIDS;i++) {
		if(!(stream->pids[i].pid)){
			break;
		}
	}
	if (i == BSETTOP_MAX_PIDS) {
		goto error;
	}

	*index = i;

	if (!NEXUS_PidChannel_GetStatus(stream->consumers.decode->video_decode->nPidChannel, &Status)){
		if(Status.pid == pid) {
			*pid_channel = Status.pidChannelIndex ;
			*KeySlotHandle = NULL;
			stream->pids[i].pid = Status.pid;
			stream->pids[i].pidchannel = Status.pidChannelIndex ;
			return b_ok;
		}
	}

	if (!NEXUS_PidChannel_GetStatus(stream->consumers.decode->audio_decode->nPidChannel, &Status)){
		if(Status.pid == pid) {
			*pid_channel = Status.pidChannelIndex ;
			*KeySlotHandle = NULL;
			stream->pids[i].pid = Status.pid;
			stream->pids[i].pidchannel = Status.pidChannelIndex ;
			return b_ok;
		}
	}

error:
	*pid_channel = 0xFFFFFFFF; /* bad number */
	BDBG_WRN(("Unable to find pid 0x%x on pidparser %p", pid, *pid_channel));
	return BSETTOP_ERROR(berr_not_available);
}

bresult bstream_p_reset_pid_info(bstream_t stream, unsigned short pid)
{
	int i;
	for (i=0;i<BSETTOP_MAX_PIDS;i++) {
		if (stream->pids[i].pid == pid) {
			stream->pids[i].pid = 0;
			stream->pids[i].pidchannel = 0xFFFFFFFF;
			stream->pids[i].KeySlotHandle = NULL; /* Should i do this here? or does the free key slot handle take care of it? */
			return 0;
		}
	}
	return BSETTOP_ERROR(b_ok);
}

/*
 * There is special handling of stream->storage.mpeg.encryption for the purposes of this function.
 * It is assumed that the algorithm will be the same for odd and even keys for video vs. audio.
 *
 */

bresult bstream_set_encryption(bstream_t stream, const bencryption_params *encryption)
{
#if NEXUS_HAS_SECURITY
	NEXUS_KeySlotHandle KeySlotHandle = NULL;
	NEXUS_SecurityClearKey ClearKey;
	NEXUS_SecurityKeySlotSettings Settings;
	unsigned int pid_channel, index=0;
	bool call_configure = false;
	NEXUS_SecurityAlgorithmSettings config_algorithm;
	NEXUS_Error rc;

	if (encryption->pid == 0x1fff) {
		bresult rc = b_ok;
		if (stream->consumers.decode) {
			bencryption_params per_pid_encryption;
			BKNI_Memcpy(&per_pid_encryption, encryption, sizeof(bencryption_params));
			per_pid_encryption.pid = stream->mpeg.video[stream->consumers.decode->video_program].pid;
			rc = bstream_set_encryption(stream,&per_pid_encryption);
			if (rc == b_ok) {
				per_pid_encryption.pid = stream->mpeg.audio[stream->consumers.decode->audio_program].pid;
				rc = bstream_set_encryption(stream,&per_pid_encryption);
			}
		} else {
			bencryption_params per_pid_encryption;
			BKNI_Memcpy(&per_pid_encryption, encryption, sizeof(bencryption_params));
			per_pid_encryption.pid = stream->mpeg.video[0].pid;
			rc = bstream_set_encryption(stream,&per_pid_encryption);
			if (rc == b_ok && stream->mpeg.audio[0].pid) {
				per_pid_encryption.pid = stream->mpeg.audio[0].pid;
				rc = bstream_set_encryption(stream,&per_pid_encryption);
			}
			if (rc == b_ok && stream->mpeg.audio[1].pid) {
				per_pid_encryption.pid = stream->mpeg.audio[1].pid;
				rc = bstream_set_encryption(stream,&per_pid_encryption);
			}
		}
		return rc;
	}

	if (encryption->type == bencryption_type_none) {
		KeySlotHandle = NULL;
		BDBG_MSG(("bstream_set_encryption: clearing encryption for %02x", encryption->pid));

		if (!bstream_p_lookup_pid_info(stream, encryption->pid, &pid_channel, &KeySlotHandle, &index)) {
			if(KeySlotHandle) {
				rc = NEXUS_Security_RemovePidChannelFromKeySlot(KeySlotHandle, pid_channel);
				if (rc != 0) {
					BDBG_MSG(("NEXUS_Security_RemovePidChannelFromKeySlot: failed "));
					return BSETTOP_ERROR(berr_invalid_parameter);
				}
				NEXUS_Security_FreeKeySlot(KeySlotHandle);
				bstream_p_reset_pid_info(stream, encryption->pid);
			}
		}
		/* using stream->mpeg.encryption to store configuration information */
		stream->mpeg.encryption.type = bencryption_type_none;
		stream->mpeg.encryption.odd = false;
		stream->mpeg.encryption.even = false;
		stream->ca_state.video_odd_key_config = 0;
		stream->ca_state.video_even_key_config = 0;
		stream->ca_state.audio_odd_key_config = 0;
		stream->ca_state.audio_even_key_config = 0;
	}
	else
	{
		if (encryption->key_ladder) {
			BDBG_ERR(("bstream_set_encryption: keyladder is not supported"));
			return BSETTOP_ERROR(berr_invalid_parameter);
		}

		if (bstream_p_lookup_pid_info(stream, encryption->pid, &pid_channel, &KeySlotHandle, &index)) {
			BDBG_ERR(("bstream_set_encryption: failed to look up pid channel for pid: %02x",encryption->pid));
			return BSETTOP_ERROR(berr_invalid_parameter);
		}

		if (!KeySlotHandle) {
			NEXUS_Security_GetDefaultKeySlotSettings(&Settings);
			Settings.keySlotEngine = NEXUS_SecurityEngine_eCa;
			BDBG_MSG(("bstream_set_encryption: Allocating new key slot (type: %d)"));

			KeySlotHandle = NEXUS_Security_AllocateKeySlot(&Settings);
			stream->pids[index].KeySlotHandle = KeySlotHandle;
		}

		/* BKNI_Memset(&config_algorithm, 0, sizeof(NEXUS_SecurityAlgorithmSettings)); */
		NEXUS_Security_GetDefaultAlgorithmSettings(&config_algorithm);

		switch (encryption->residue) {
		case bencryption_residue_none:
			BDBG_MSG(("bstream_set_encryption: configuring no residue"));
			config_algorithm.terminationMode = NEXUS_SecurityTerminationMode_eClear;
			break;
		case bencryption_residue_block_term:
			BDBG_MSG(("bstream_set_encryption: configuring block termination mode"));
			config_algorithm.terminationMode = NEXUS_SecurityTerminationMode_eBlock;
			break;
		case bencryption_residue_cipher_text:
			BDBG_MSG(("bstream_set_encryption: configuring cipher text stealing"));
			config_algorithm.terminationMode = NEXUS_SecurityTerminationMode_eCipherStealing;
			break;
		}

		switch (encryption->type) {
		case bencryption_type_dvb:
			BDBG_MSG(("bstream_set_encryption: configuring DVB for %s",stream->mpeg.mpeg_type == bstream_mpeg_type_pes ? "PES" : "TS"));
			config_algorithm.algorithm = NEXUS_SecurityAlgorithm_eDvb;
			config_algorithm.dvbScramLevel = stream->mpeg.mpeg_type == bstream_mpeg_type_pes ? NEXUS_SecurityDvbScrambleLevel_ePes : NEXUS_SecurityDvbScrambleLevel_eTs;
			ClearKey.keySize = 8; /* 8 bytes (64 bits) */
			break;
		case bencryption_type_aes:
			BDBG_MSG(("bstream_set_encryption: configuring AES for %s",encryption->blockmode == bcrypto_blockmode_cbc ? "CBC" : "ECB"));
			config_algorithm.algorithm = NEXUS_SecurityAlgorithm_eAes;
			config_algorithm.algorithmVar = encryption->blockmode == bcrypto_blockmode_cbc ? NEXUS_SecurityAlgorithmVariant_eCbc : NEXUS_SecurityAlgorithmVariant_eEcb;
			ClearKey.keySize = 16; /* 16 bytes (128 bits) */
			break;
		case bencryption_type_des:
			BDBG_MSG(("bstream_set_encryption: configuring DES for %s",encryption->blockmode == bcrypto_blockmode_cbc ? "CBC" : "ECB"));
			config_algorithm.algorithm = NEXUS_SecurityAlgorithm_eDes;
			config_algorithm.algorithmVar = encryption->blockmode == bcrypto_blockmode_cbc ? NEXUS_SecurityAlgorithmVariant_eCbc : NEXUS_SecurityAlgorithmVariant_eEcb;
			ClearKey.keySize = 8; /* 8 bytes (64 bits) */
			break;
		case bencryption_type_3des:
			BDBG_MSG(("bstream_set_encryption: configuring 3DES for %s",encryption->blockmode == bcrypto_blockmode_cbc ? "CBC" : "ECB"));
			config_algorithm.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
			config_algorithm.algorithmVar = encryption->blockmode == bcrypto_blockmode_cbc ? NEXUS_SecurityAlgorithmVariant_eCbc : NEXUS_SecurityAlgorithmVariant_eEcb;
			ClearKey.keySize = 16; /* 16 bytes (128 bits) */
			break;
        case bencryption_type_multi2:
            BDBG_MSG(("bstream_set_encryption: configuring multi2 for %s",encryption->blockmode == bcrypto_blockmode_cbc ? "CBC" : "ECB"));
            config_algorithm.algorithm = NEXUS_SecurityAlgorithm_eMulti2;
            config_algorithm.algorithmVar = encryption->blockmode == bcrypto_blockmode_cbc ? NEXUS_SecurityAlgorithmVariant_eCbc : NEXUS_SecurityAlgorithmVariant_eEcb;
            config_algorithm.multi2KeySelect = encryption->m2_key_select;
            ClearKey.keySize = 8; /* 8 bytes (64 bits) */
            break;
		default:
			BDBG_ERR(("bstream_set_encryption: unsupported CA algorithm"));
			return BSETTOP_ERROR(berr_invalid_parameter);
		}

		if (encryption->use_cps) {
			call_configure = true;
		} else {
			if (stream->consumers.decode) {
				if (encryption->pid == stream->mpeg.video[stream->consumers.decode->video_program].pid) {
					if (encryption->odd && (encryption->pid != stream->ca_state.video_odd_key_config) ) {
						stream->ca_state.video_odd_key_config = encryption->pid;
						stream->ca_state.video_even_key_config = encryption->pid;
						call_configure = true;
					}
					if (encryption->even && (encryption->pid != stream->ca_state.video_odd_key_config) ) {
						stream->ca_state.video_odd_key_config = encryption->pid;
						stream->ca_state.video_even_key_config = encryption->pid;
						call_configure = true;
					}
				}
				if (encryption->pid == stream->mpeg.audio[stream->consumers.decode->audio_program].pid) {
					if (encryption->odd && (encryption->pid != stream->ca_state.audio_odd_key_config) ) {
						stream->ca_state.audio_odd_key_config = encryption->pid;
						stream->ca_state.audio_even_key_config = encryption->pid;
						call_configure = true;
					}
					if (encryption->even && (encryption->pid != stream->ca_state.audio_odd_key_config) ) {
						stream->ca_state.audio_odd_key_config = encryption->pid;
						stream->ca_state.audio_even_key_config = encryption->pid;
						call_configure = true;
					}
				}
			} else {
				if (encryption->pid == stream->mpeg.video[0].pid) {
					if (encryption->odd && (encryption->pid != stream->ca_state.video_odd_key_config)) {
						stream->ca_state.video_odd_key_config = encryption->pid;
						call_configure = true;
					}
					if (encryption->even && (encryption->pid != stream->ca_state.video_even_key_config)) {
						stream->ca_state.video_even_key_config = encryption->pid;
						call_configure = true;
					}
				}
				if (encryption->pid == stream->mpeg.audio[0].pid ||
					encryption->pid == stream->mpeg.audio[1].pid ) {
					if (encryption->odd && (encryption->pid != stream->ca_state.audio_odd_key_config)) {
						stream->ca_state.audio_odd_key_config = encryption->pid;
						call_configure = true;
					}
					if (encryption->even && (encryption->pid != stream->ca_state.audio_even_key_config)) {
						stream->ca_state.audio_even_key_config = encryption->pid;
						call_configure = true;
					}
				}
			}
		}

		if (call_configure) {

			rc = NEXUS_Security_ConfigAlgorithm( KeySlotHandle, &config_algorithm);
			if (rc != 0) {
				BDBG_MSG(("bstream_set_encryption: failed to configure algorithm"));
				return BSETTOP_ERROR(berr_invalid_parameter);
			}
			stream->mpeg.encryption.type = encryption->type;

			rc = NEXUS_Security_AddPidChannelToKeySlot(KeySlotHandle, pid_channel);
			if (rc != 0) {
				BDBG_ERR(("bstream_set_encryption: failed to configure pid key pointer table"));
				return BSETTOP_ERROR(berr_invalid_parameter);
			}
		}

		if (encryption->key_ladder) {
			BDBG_ERR(("bstream_set_encryption: Keyladder is not supported"));
			return BSETTOP_ERROR(berr_invalid_parameter);
		}
		else {
            if (encryption->type == bencryption_type_multi2) {
                NEXUS_SecurityMulti2Settings config_multi2;

                NEXUS_Security_GetDefaultMulti2Settings(&config_multi2);
                config_multi2.multi2KeySelect = encryption->m2_key_select;
                config_multi2.multi2Rounds = encryption->m2_round;
                memcpy(config_multi2.multi2SysKey, encryption->m2_sys_key, 32);

                if ( NEXUS_Security_ConfigMulti2(KeySlotHandle, &config_multi2) != 0 )
                {
                    BDBG_ERR(("bstream_set_encryption: multi2 configuration failed"));
                    return BSETTOP_ERROR(berr_external_error);
                }
            }

			/* Route a clear key */
			BKNI_Memcpy(ClearKey.keyData,encryption->key,encryption->key_length/8);
			ClearKey.keyEntryType = encryption->odd ? NEXUS_SecurityKeyType_eOdd : NEXUS_SecurityKeyType_eEven;
			BDBG_MSG(("NEXUS_Security_LoadClearKey called if odd key is set"));

			rc = NEXUS_Security_LoadClearKey( KeySlotHandle, &ClearKey);
			if (rc != 0) {
				BDBG_MSG(("bstream_set_encryption: failed to route user key"));
				return BSETTOP_ERROR(berr_invalid_parameter);
			}
			if (encryption->odd && encryption->even) {
				ClearKey.keyEntryType = NEXUS_SecurityKeyType_eEven;
				BDBG_MSG(("NEXUS_Security_LoadClearKey called if odd n even key are set"));

				rc = NEXUS_Security_LoadClearKey( KeySlotHandle, &ClearKey);
				if (rc != 0) {
					BDBG_MSG(("bstream_set_encryption: failed to route user key"));
					return BSETTOP_ERROR(berr_invalid_parameter);
				}
			}
			if (encryption->blockmode == bcrypto_blockmode_cbc) {
				ClearKey.keySize = 16; /* 16 bytes */
				BKNI_Memcpy(ClearKey.keyData,encryption->iv,16);
				ClearKey.keyEntryType = NEXUS_SecurityKeyType_eIv;
				BDBG_MSG(("NEXUS_Security_LoadClearKey called for IV"));

				rc = NEXUS_Security_LoadClearKey( KeySlotHandle, &ClearKey);
				if (rc != 0) {
					BDBG_MSG(("bstream_set_encryption: failed to load IV"));
					return BSETTOP_ERROR(berr_invalid_parameter);
				}
			}
		}
	}
#else
BSTD_UNUSED(stream);
BSTD_UNUSED(encryption);
#endif /* NEXUS_HAS_SECURITY */
	return b_ok;
}


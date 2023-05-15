#ifndef	__DI_PUMP_H__
#define	__DI_PUMP_H__

#include <htype.h>
#include <di_err.h>

typedef enum DI_PUMP_TransportType {
    DI_PUMP_TRANSPORT_TYPE_ES,        	/* Elementary stream. No container or muxing. */
    DI_PUMP_TRANSPORT_TYPE_TS,        	/* MPEG2 transport stream */
    DI_PUMP_TRANSPORT_TYPE_PES,  		/* MPEG2 packetized elementary stream, this includes MPEG2 Program Stream  streams */
    DI_PUMP_TRANSPORT_TYPE_MAX
} DI_PUMP_TRANSPORT_TYPE_e;

typedef enum DI_PUMP_PidType {
	DI_PUMP_PID_TYPE_VIDEO,            	/* Used to indicate video stream */
	DI_PUMP_PID_TYPE_AUDIO,            	/* Used to indicate audio stream */
	DI_PUMP_PID_TYPE_AUDIO_SUB,            	/* Used to indicate audio sub stream */	
	DI_PUMP_PID_TYPE_OTHER,            	/* Used to indicate stream type different from above. Useful for PCR, PSI or other non-audio/video data. */
	DI_PUMP_PID_TYPE_MAX				/* Unknown pid type */
} DI_PUMP_PID_TYPE_e;

typedef enum DI_PUMP_CodecType {
	DI_PUMP_CODEC_TYPE_MPEG,
	DI_PUMP_CODEC_TYPE_H264,	
	DI_PUMP_CODEC_TYPE_H265,	
	DI_PUMP_CODEC_TYPE_AAC,
	DI_PUMP_CODEC_TYPE_AC3,
	DI_PUMP_CODEC_TYPE_AC3PLUS,
	DI_PUMP_CODEC_TYPE_MAX				/* Unknown Codec  type */
} DI_PUMP_CODEC_TYPE_e;


typedef struct
{
	DI_PUMP_TRANSPORT_TYPE_e eTransPortType;
	DI_PUMP_PID_TYPE_e ePidType;
	DI_PUMP_CODEC_TYPE_e eCodecType;
	HUINT32	ulPID;
	HBOOL bSecurityPath;
} DI_PUMP_OPEN_t;

typedef enum DI_PUMP_PtsType
{
    DI_PUMP_PtsType_eCoded,                     /* PTS value came from the stream. */
    DI_PUMP_PtsType_eInterpolatedFromValidPTS,  /* PTS has been interpolated over time from a valid (i.e. coded) PTS from the stream. */
    DI_PUMP_PtsType_eHostProgrammedPTS,         /* PTS was set by the application and did not come from the stream. */
    DI_PUMP_PtsType_eInterpolatedFromInvalidPTS /* PTS has been interpolated over time from an invalid value. */
} DI_PUMP_PtsType; /*To be deleted*/

typedef struct DI_PUMP_Status
{
    HBOOL started;               /*To be deleted*/
    
    HUINT32 fifoDepth;           /* Depth in bytes of the playback buffer */
    HUINT32 fifoSize;            /* Size in bytes of the playback buffer */
    HUINT32 descFifoDepth;       /*To be deleted*/
    HUINT32 descFifoSize;        /*To be deleted*/
    void *bufferBase;            /* Pointer to the base of buffer*/
    HUINT64 bytesPlayed;         /*To be deleted*/
    HUINT32 index;               /*To be deleted*/

    HUINT32 pacingTsRangeError; /*To be deleted*/
    HUINT32 syncErrors;         /*To be deleted*/
    HUINT32 resyncEvents;       /*To be deleted*/
    HUINT32 streamErrors;       /*To be deleted*/
    DI_PUMP_PtsType mediaPtsType;    /*To be deleted*/
    HUINT32 mediaPts;    		/*To be deleted*/
} DI_PUMP_Status;


DI_ERR_CODE DI_PUMP_Open(HUINT32 ulPumpId,DI_PUMP_OPEN_t tOpen);


DI_ERR_CODE DI_PUMP_Close(HUINT32 ulPumpId);


DI_ERR_CODE DI_PUMP_Start(HUINT32 ulPumpId);


DI_ERR_CODE DI_PUMP_Stop(HUINT32 ulPumpId);


DI_ERR_CODE DI_PUMP_GetBuffer(HUINT32 ulPumpId, void **ppBuffer, HUINT32 *pulSize);

DI_ERR_CODE DI_PUMP_Flush(HUINT32 ulPumpId);

DI_ERR_CODE DI_PUMP_GetStatus(HUINT32 ulPumpId, DI_PUMP_Status *ptPumpStatus);

DI_ERR_CODE DI_PUMP_WriteComplete(HUINT32 ulPumpId, HUINT32 ulSkip, HUINT32 ulSize);

DI_ERR_CODE DI_PUMP_SetSync();



#endif /* !__DI_PUMP_H__ */


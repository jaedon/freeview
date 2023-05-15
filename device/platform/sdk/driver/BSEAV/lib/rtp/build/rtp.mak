B_RTP_LIB = ${BSEAV}/lib/rtp
B_RTP_CFLAGS += -I${B_RTP_LIB} -I${BSEAV}/lib/utils -I${BSEAV}/lib/asf -I${BSETTOP}/src/pvr
B_RTP_VPATH = ${B_RTP_LIB} ${BSEAV}/lib/utils ${BSEAV}/lib/asf
B_RTP_OBJ = brtp.o brtp_parser.o brtp_parser_h264.o brtp_parser_mpeg4.o 
B_RTP_OBJ += btimestamp.o brtp_util.o brtp_play.o brtp_delay.o
B_RTP_OBJ += bbase64.o biobits.o
B_RTP_OBJ += bsettop_playback_rtsp.o
B_RTP_OBJ += lm.o lm_lgpl.o


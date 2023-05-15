#
obj-y := di_iptuner.o
obj-y += di_iptuner_dlna.o
obj-y += di_iptuner_dnp.o
obj-y += di_iptuner_underflow.o
obj-y += di_iptuner_http.o
obj-y += di_iptuner_http_chnk.o
obj-y += di_iptuner_mms.o
obj-y += di_iptuner_stream.o

# for SSTR
ifeq (${CONFIG_MEDIA_SSTR},y)
HUMAX_LIBXML2_INCLUDE=$(HUMAX_WORK_DIR)/../packages/libxml2/include
inccflag-y += -I$(HUMAX_LIBXML2_INCLUDE)/
inccflag-y += -I$(HUMAX_LIBXML2_INCLUDE)/libxml
obj-y += di_iptuner_sstr.o
obj-y += sstr_manifestparser.o
obj-y += sstr_controller.o
obj-y += sstr_heuristics.o
obj-y += sstr_buffermgr.o
obj-y += util_dllist.o
endif

# for HLS
obj-y += di_iptuner_hls.o
obj-y += hls_m3u8_parser.o
obj-y += hls_controller.o
obj-y += sstr_heuristics.o
obj-y += sstr_buffermgr.o
obj-y += util_dllist.o
ifeq (${CONFIG_HLS_UNDERFLOW_MONITORING_FOR_BBC_CATAL},y)
obj-y += di_iptuner_underflow_hls.o
endif

obj-y += mms_asf.o
obj-y += mms_buffer.o
obj-y += mms_http.o
obj-y += mms_tcpudp.o
obj-y += rtsp_lib.o
obj-y += rtsp_ms.o
obj-y += rtsp_sdp_ms.o
obj-y += stream_http.o
obj-y += stream_mms.o
obj-y += stream_rtsp.o
obj-y += transport_io.o
obj-y += transport_tcp.o

#ifneq ($(findstring DI_IPTUNER_SECURITY_PROTOCOL_TYPE_OPENSSL, $(CONFIG_DI_IPTUNER_SECURITY_PROTOCOL_TYPE)),)
ifeq ($(CONFIG_OPENSSL),y)
obj-y += transport_tls.o
endif

obj-y += transport_udp.o
obj-y += util_b64_encode.o
obj-y += util_getaddrinfo.o
obj-y += util_md5.o
obj-y += util_url.o

#
obj-y := di_iptuner.o
#obj-y += di_iptuner_dlna.o
obj-y += di_iptuner_dnp.o
obj-y += di_iptuner_http.o
obj-y += di_iptuner_underflow.o
obj-y += di_iptuner_mms.o
obj-y += di_iptuner_stream.o
obj-y += di_iptuner_pool.o
obj-y += di_iptuner_adaptive.o
obj-y += di_iptuner_multicast.o
obj-y += dnp_olt.o
obj-y += adp_buffermgr.o
obj-y += di_iptuner_cls_monitor.o

# for SSTR
ifeq (${CONFIG_MEDIA_SSTR},y)
ifeq (${CONFIG_LIBXML2},y)
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
endif

# for HLS
obj-y += di_iptuner_hls.o
obj-y += hls_m3u8_parser.o
obj-y += hls_controller.o
obj-y += sstr_heuristics.o
obj-y += sstr_buffermgr.o
obj-y += util_dllist.o

# for DASH
ifeq (${CONFIG_MEDIA_DASH},y) 
ifeq (${CONFIG_LIBXML2},y)
HUMAX_LIBXML2_INCLUDE=$(HUMAX_WORK_DIR)/../packages/libxml2/include
inccflag-y += -I$(HUMAX_LIBXML2_INCLUDE)/
inccflag-y += -I$(HUMAX_LIBXML2_INCLUDE)/libxml
obj-y += di_iptuner_dash.o
obj-y += dash_main.o
obj-y += dash_mpdparser.o
obj-y += dash_controller.o
obj-y += adp_heuristics.o
obj-y += util_dllist.o
#obj-y += util_memtool.o
obj-y += util_mp4header_parser.o
endif
endif

# for ABR
#obj-y += di_iptuner_abr.o
#obj-y += abr_queue.o
#obj-y += abr_mpdparser.o
#obj-y += abr_mpdcontroller.o

# for Internet Radio
obj-y += di_iptuner_inetradio.o

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
obj-y += stream_multicast.o
obj-y += transport_io.o
obj-y += transport_tcp.o
obj-y += transport_rtp.o
obj-y += di_iptuner_rtsp.o
obj-y += rtsp_session.o
obj-y += rtsp_sdp.o

# for TLS
obj-y += transport_tls.o

inccflag-y += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/nexus/base/include
ifeq (${CONFIG_DEVICE_MODE_KERNEL},y)
inccflag-y += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/nexus/base/include/linuxkernel
endif
ifeq (${CONFIG_DEVICE_MODE_USER},y)
inccflag-y += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/nexus/base/include/linuxuser
endif

obj-y += transport_udp.o
obj-y += util_b64_encode.o
obj-y += util_getaddrinfo.o
obj-y += util_md5.o
obj-y += util_url.o
obj-y += util_clock.o
obj-y += util_index_parser.o
obj-y += util_sys.o


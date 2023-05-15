/**************************************************************
 *	@file		octo_websocket.c
 *	This module for OCTO 2.0's Open API Interface for HTML5 WEB SOCKET
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2011/09/07
 *	@author			humax
 *	@brief			OCTO Open API Interface for WEB SOCKET
 **************************************************************/

/*
	TEST OCTO WEBSOCKET SERVER....
	JUST TEST.......
	JUST TEST.......
	JUST TEST.......
	JUST TEST.......
	JUST TEST.......
	JUST TEST.......
	JUST TEST.......
	JUST TEST.......
	JUST TEST.......
	JUST TEST.......
	JUST TEST.......
	JUST TEST.......
	JUST TEST.......
	JUST TEST.......

	for TESTING, you should change WEBSOCKET_PORT to other.
*/

#include	<hlib.h>
#include	<oapi.h>

#include	<octo_common.h>
#include	<bus.h>

#include	<libwebsockets.h>




/**************************************************************
	internal structures and variables
 **************************************************************/
#define	WEBSOCKET_PORT		7681
#define	WEBSOCKET_USE_SSL	FALSE

/**************************************************************
	private functions
 **************************************************************/
static void
dump_handshake_info(struct lws_tokens *lwst)
{
	int n;
	static const char *token_names[WSI_TOKEN_COUNT] = {
		/*[WSI_TOKEN_GET_URI]		=*/ "GET URI",
		/*[WSI_TOKEN_HOST]		=*/ "Host",
		/*[WSI_TOKEN_CONNECTION]	=*/ "Connection",
		/*[WSI_TOKEN_KEY1]		=*/ "key 1",
		/*[WSI_TOKEN_KEY2]		=*/ "key 2",
		/*[WSI_TOKEN_PROTOCOL]		=*/ "Protocol",
		/*[WSI_TOKEN_UPGRADE]		=*/ "Upgrade",
		/*[WSI_TOKEN_ORIGIN]		=*/ "Origin",
		/*[WSI_TOKEN_DRAFT]		=*/ "Draft",
		/*[WSI_TOKEN_CHALLENGE]		=*/ "Challenge",

		/* new for 04 */
		/*[WSI_TOKEN_KEY]		=*/ "Key",
		/*[WSI_TOKEN_VERSION]		=*/ "Version",
		/*[WSI_TOKEN_SWORIGIN]		=*/ "Sworigin",

		/* new for 05 */
		/*[WSI_TOKEN_EXTENSIONS]	=*/ "Extensions",

		/* client receives these */
		/*[WSI_TOKEN_ACCEPT]		=*/ "Accept",
		/*[WSI_TOKEN_NONCE]		=*/ "Nonce",
		/*[WSI_TOKEN_HTTP]		=*/ "Http",
		/*[WSI_TOKEN_MUXURL]	=*/ "MuxURL",
	};

	for (n = 0; n < WSI_TOKEN_COUNT; n++) {
		if (lwst[n].token == NULL)
			continue;

		HxLOG_Debug("    %s = %s\n", token_names[n], lwst[n].token);
	}
}

#define	MAX_MESSAGE_QUEUE	64

struct per_session_data_lws_octo
{
	struct	libwebsocket *wsi;
	int		ringbuffer_tail;
};

struct	a_message
{
	void	*payload;
	size_t	len;
};

static	struct	a_message	ringbuffer[MAX_MESSAGE_QUEUE];
static	int		ringbuffer_head = 0;
static	struct	per_session_data_lws_octo	*_psd_octo = NULL;

static	int		callback_octo_protocol(struct libwebsocket_context *context, struct libwebsocket *wsi,
			enum libwebsocket_callback_reasons reason, void *user, void *in, size_t len)
{
	int		n;
	struct	per_session_data_lws_octo	*udata = user;

	HxLOG_Debug("Reason : [%d]\n", reason);
	switch (reason)
	{
		case LWS_CALLBACK_ESTABLISHED:
			HxLOG_Debug("callback_octo_protocol : LWS_CALLBACK_ESTABLISHED\n");
			udata->ringbuffer_tail = ringbuffer_head;
			udata->wsi = wsi;

			_psd_octo = udata;
			break;

		case LWS_CALLBACK_SERVER_WRITEABLE:
			HxLOG_Debug("callback_octo_protocol : LWS_CALLBACK_SERVER_WRITEABLE\n");

			if (udata->ringbuffer_tail != ringbuffer_head)
			{
				HxLOG_Debug("write socket : [%s]\n", ringbuffer[udata->ringbuffer_tail].payload);
				n = libwebsocket_write(wsi, (unsigned char *)
					   ringbuffer[udata->ringbuffer_tail].payload +
					   LWS_SEND_BUFFER_PRE_PADDING,
					   ringbuffer[udata->ringbuffer_tail].len,
									LWS_WRITE_TEXT);
				if (n < 0) {
					HxLOG_Error("ERROR writing to socket");
					break;
				}

				if (udata->ringbuffer_tail == (MAX_MESSAGE_QUEUE - 1))
					udata->ringbuffer_tail = 0;
				else
					udata->ringbuffer_tail++;

				if (((ringbuffer_head - udata->ringbuffer_tail) % MAX_MESSAGE_QUEUE) < (MAX_MESSAGE_QUEUE - 15))
					libwebsocket_rx_flow_control(wsi, 1);

				libwebsocket_callback_on_writable(context, wsi);

			}
			break;

		case LWS_CALLBACK_BROADCAST:
			HxLOG_Debug("callback_octo_protocol : LWS_CALLBACK_BROADCAST\n");
			n = libwebsocket_write(wsi, in, len, LWS_WRITE_TEXT);
			if (n < 0)
				HxLOG_Error("Octo_protocol write failed\n");
			break;

		case LWS_CALLBACK_RECEIVE:
			HxLOG_Debug("callback_octo_protocol : LWS_CALLBACK_RECEIVE\n");
			{
				static	int	__request_id = 0;

				HxLOG_Debug("from client(%d) : [%s]\n", len, in);
				APP_MGR_PostMessage(eMEVT_BUS_OAPI_JSON, (Handle_t)WEBSOCKET_PORT, HLIB_STD_StrDup(in), len, __request_id);
			}

			libwebsocket_callback_on_writable_all_protocol(
						       libwebsockets_get_protocol(wsi));
			break;
		/*
		 * this just demonstrates how to use the protocol filter. If you won't
		 * study and reject connections based on header content, you don't need
		 * to handle this callback
		 */

		case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
			HxLOG_Debug("callback_octo_protocol : LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION\n");
			dump_handshake_info((struct lws_tokens *)(long)user);
			/* you could return non-zero here and kill the connection */
			break;

		default:
			HxLOG_Debug("Reason : [%X]\n", reason);
			break;
	}

	return 0;
}

static	struct	libwebsocket_protocols		_oweb_protocols[] =
{
	{
		"octo-protocol-via-websocket",
		callback_octo_protocol,
		sizeof(struct per_session_data_lws_octo),
	},
	{
		NULL, NULL, 0		/* End of list */
	}
};

static	struct	libwebsocket_context	*_websocket_context = NULL;

static	void	_websocket_task(void *arg)
{
/*
	const	HCHAR	*cert_path = "/tmp/websocket/server.pem";
	const	HCHAR	*key_path  = "/tmp/websocket/server.key.pen";

	HCHAR	interface_name[128] = "";
*/
	const	HCHAR*	interface = NULL;

	HxLOG_Debug("OCTO WEB Socket Initializing...\n");

	_websocket_context = libwebsocket_create_context(WEBSOCKET_PORT, interface, _oweb_protocols, libwebsocket_internal_extensions, NULL, NULL, -1, -1, 0);
	if (_websocket_context == NULL)
	{
		HxLOG_Error("Octo WEB Socket Creation Failed!!!\n");
		return;
	}

	HxLOG_Debug("Start WebSocket Task..\n");
	while (1)
	{
		libwebsocket_service(_websocket_context, 50);
	}

	libwebsocket_context_destroy(_websocket_context);
}

/**************************************************************
	public functions
 **************************************************************/
void	OCTO_WEBSOCKET_Init(void)
{
	HULONG		OctoWebSocketId;



	HxSTD_MemSet(ringbuffer, 0, sizeof(struct a_message) * MAX_MESSAGE_QUEUE);

	VK_TASK_Create(_websocket_task, 40, (128 * 1024), "OctoWebSocket", (void *)NULL, &OctoWebSocketId, 0);
	VK_TASK_Start(OctoWebSocketId);
}

void	OCTO_WEBSOCKET_Write(HINT32 clientSocket, HINT32 request_id, HCHAR *command)
{
	HINT32	len;

	if (clientSocket != WEBSOCKET_PORT)
		return;

	if (ringbuffer[ringbuffer_head].payload)
		HLIB_STD_MemFree(ringbuffer[ringbuffer_head].payload);

	if (_psd_octo)
	{
		len = HxSTD_StrLen(command) + 1;
		ringbuffer[ringbuffer_head].payload = HLIB_STD_MemCalloc(LWS_SEND_BUFFER_PRE_PADDING + len + LWS_SEND_BUFFER_POST_PADDING);
		ringbuffer[ringbuffer_head].len = len;
		HxSTD_MemCopy(ringbuffer[ringbuffer_head].payload + LWS_SEND_BUFFER_PRE_PADDING, command, len);

		HxLOG_Debug("WEB_Write : [%d](%s)\n", len, command);

		if (ringbuffer_head == (MAX_MESSAGE_QUEUE - 1))
			ringbuffer_head = 0;
		else
			ringbuffer_head++;

		libwebsocket_callback_on_writable(_websocket_context, _psd_octo->wsi);
	}
}


/*
 * nl80211 linux driver interface.
 *
 * Copyright (C) 2014, Broadcom Corporation. All Rights Reserved.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * $Id: $
 */
#include <errno.h>
#include <linux/nl80211.h>
#include <wlioctl.h>
#include <brcm_nl80211.h>
#include "wlu_nl80211.h"

static struct nla_policy wl_nl_policy[BCM_NLATTR_MAX + 1] = {
	[BCM_NLATTR_LEN] = { .type = NLA_U16 },
	[BCM_NLATTR_DATA] = { .type = NLA_UNSPEC },
};

/* libnl 1.x compatibility code */
#if !defined(CONFIG_LIBNL20) && !defined(CONFIG_LIBNL30)
static inline struct nl_handle *nl_socket_alloc(void)
{
	return nl_handle_alloc();
}

static inline void nl_socket_free(struct nl_sock *h)
{
	nl_handle_destroy(h);
}
#endif /* CONFIG_LIBNL20 && CONFIG_LIBNL30 */

static int wlnl_error_handler(struct sockaddr_nl * nla,	struct nlmsgerr *err, void *arg)
{
	struct nl_prv_data *prv = arg;
	UNUSED_PARAMETER(nla);

	if (err->error) {
		prv->err = err->error;
		return NL_STOP;
	}
	return NL_SKIP;
}

static int wlnl_finish_handler(struct nl_msg *msg, void *arg)
{
	UNUSED_PARAMETER(msg);
	UNUSED_PARAMETER(arg);
	return NL_SKIP;
}

static int wlnl_ack_handler(struct nl_msg *msg, void *arg)
{
	struct nl_prv_data *prv = arg;
	UNUSED_PARAMETER(msg);

	prv->err = 0;
	return NL_STOP;
}

static int wlnl_valid_handler(struct nl_msg *msg, void *arg)
{
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *attrs[NL80211_ATTR_MAX + 1];
	struct nlattr *bcmnl[BCM_NLATTR_MAX + 1];
	struct nl_prv_data *prv = arg;
	int ret;
	uint payload;

	nla_parse(attrs, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
		genlmsg_attrlen(gnlh, 0), NULL);
	if (!attrs[NL80211_ATTR_VENDOR_DATA]) {
		fprintf(stderr, "Could not find attrs NL80211_ATTR_VENDOR_DATA\n");
		return NL_SKIP;
	}

	ret = nla_parse_nested(bcmnl, BCM_NLATTR_MAX,
		attrs[NL80211_ATTR_VENDOR_DATA], wl_nl_policy);
	if (ret != 0 || !bcmnl[BCM_NLATTR_LEN] || !bcmnl[BCM_NLATTR_DATA]) {
		fprintf(stderr, "nla_parse_nested error\n");
		return NL_SKIP;
	}

	payload = nla_get_u16(bcmnl[BCM_NLATTR_LEN]);
	if (payload > prv->len)
		payload = prv->len;
	memcpy(prv->data, nla_data(bcmnl[BCM_NLATTR_DATA]), payload);
	prv->data += payload;
	prv->len -= payload;

	return NL_SKIP;
}

int wlnl_sock_connect(struct wl_netlink_info *wl_nli)
{
	wl_nli->nl = nl_socket_alloc();
	if (wl_nli->nl == NULL)
		return -1;

	if (genl_connect(wl_nli->nl) < 0) {
		fprintf(stderr, "netlink connection failed\n");
		goto err;
	}

	wl_nli->nl_id = genl_ctrl_resolve(wl_nli->nl, "nl80211");
	if (wl_nli->nl_id < 0) {
		fprintf(stderr, "'nl80211' netlink not found\n");
		goto err;
	}

	wl_nli->cb = nl_cb_alloc(NL_CB_DEFAULT);
	if (wl_nli->cb == NULL)
		goto err;

	nl_socket_set_cb(wl_nli->nl, wl_nli->cb);
	return 0;

err:
	nl_cb_put(wl_nli->cb);
	nl_socket_free(wl_nli->nl);
	fprintf(stderr, "nl80211 connection failed\n");
	return -1;
}

void wlnl_sock_disconnect(struct wl_netlink_info *wl_nli)
{
	nl_cb_put(wl_nli->cb);
	nl_socket_free(wl_nli->nl);
}

int wlnl_do_vndr_cmd(struct wl_netlink_info *wl_nli, wl_ioctl_t *ioc)
{
	struct nl_msg *msg;
	struct nl_prv_data prv_dat;
	struct bcm_nlmsg_hdr *nlioc;
	uint msglen = ioc->len;

	msg = nlmsg_alloc();
	if (msg == NULL)
		return -ENOMEM;

	/* nlmsg_alloc() can only allocate default_pagesize packet, cap
	 * any buffer send down to 1536 bytes
	 * DO NOT switch to nlmsg_alloc_size because Android doesn't support it
	 */
	if (msglen > 0x600)
		msglen = 0x600;
	msglen += sizeof(struct bcm_nlmsg_hdr);
	nlioc = malloc(msglen);
	if (nlioc == NULL) {
		nlmsg_free(msg);
		return -ENOMEM;
	}
	nlioc->cmd = ioc->cmd;
	nlioc->len = ioc->len;
	nlioc->offset = sizeof(struct bcm_nlmsg_hdr);
	nlioc->set = ioc->set;
	nlioc->magic = 0;
	memcpy(((void *)nlioc) + nlioc->offset, ioc->buf, msglen - nlioc->offset);

	/* fill testmode message */
	genlmsg_put(msg, 0, 0, wl_nli->nl_id, 0, 0, NL80211_CMD_VENDOR, 0);
	NLA_PUT_U32(msg, NL80211_ATTR_IFINDEX, wl_nli->ifidx);
	NLA_PUT_U32(msg, NL80211_ATTR_VENDOR_ID, OUI_BRCM);
	NLA_PUT_U32(msg, NL80211_ATTR_VENDOR_SUBCMD, BRCM_VENDOR_SCMD_PRIV_STR);
	NLA_PUT(msg, NL80211_ATTR_VENDOR_DATA, msglen, nlioc);

	prv_dat.err = nl_send_auto_complete(wl_nli->nl, msg);
	if (prv_dat.err < 0) {
		goto out;
	}

	prv_dat.err = 1;
	prv_dat.nlioc = nlioc;
	prv_dat.data = ioc->buf;
	prv_dat.len = ioc->len;
	nl_cb_err(wl_nli->cb, NL_CB_CUSTOM, wlnl_error_handler, &prv_dat);
	nl_cb_set(wl_nli->cb, NL_CB_ACK, NL_CB_CUSTOM,
		wlnl_ack_handler, &prv_dat);
	nl_cb_set(wl_nli->cb, NL_CB_FINISH, NL_CB_CUSTOM,
		wlnl_finish_handler, &prv_dat);
	nl_cb_set(wl_nli->cb, NL_CB_VALID, NL_CB_CUSTOM,
		wlnl_valid_handler, &prv_dat);
	while (prv_dat.err > 0)
		nl_recvmsgs(wl_nli->nl, wl_nli->cb);

out:
	free(nlioc);
	nlmsg_free(msg);
	return prv_dat.err;

nla_put_failure:
	fprintf(stderr, "setting netlink attribute failed\n");
	prv_dat.err = -EFAULT;
	goto out;
}
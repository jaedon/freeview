/*
 * Broadcom Dongle Host Driver (DHD)
 * Broadcom 802.11bang Networking Device Driver
 *
 * Copyright (C) 2011, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 *
 * $Id: dhd_fbsd.c 335620 2012-05-29 19:01:21Z $
 */

#include <typedefs.h>
#include <osl.h>

#include <sys/socket.h>
#include <sys/sockio.h>
#include <sys/proc.h>
#include <sys/kernel.h>
#include <sys/bus.h>
#include <sys/module.h>
#include <sys/kthread.h>
#include <sys/sema.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <net/if_media.h>
#include <net/if_types.h>
#include <dev/usb/usb_freebsd.h>
#include <dev/usb/usb.h>
#include <dev/usb/usbdi.h>
#include <dev/tdfx/tdfx_io.h>

#ifdef DHD_NET80211
#include <net80211/ieee80211_ioctl.h>
#define	WFD_MASK	0x0004

#undef WME_OUI
#undef WME_OUI_TYPE
#undef WPA_OUI
#undef WPA_OUI_TYPE
#undef WPS_OUI
#undef WPS_OUI_TYPE
#undef RSN_CAP_PREAUTH
#endif

#include <epivers.h>
#include <bcmutils.h>
#include <bcmendian.h>
#include <dngl_stats.h>
#include <dhd.h>
#include <dhd_dbg.h>
#include <dhd_proto.h>
#include <dhd_bus.h>
#include <dbus.h>
#include <fbsd_osl.h>
#include <proto/ethernet.h>
#include <proto/bcmevent.h>

#ifdef DHD_NET80211
#include <dhd_net80211.h>
#endif
#ifdef PROP_TXSTATUS
#include <wlfc_proto.h>
#include <dhd_wlfc.h>
/*
 * FreeBsd uses the system header which does not have this definition
 */
#define ETHER_ISMULTI(ea) (((const uint8 *)(ea))[0] & 1)
/*
 * Is this OS agnostic or needs to be tuned per OS
 */
const uint8 wme_fifo2ac[] = { 0, 1, 2, 3, 1, 1 };
const uint8 prio2fifo[8] = { 1, 0, 0, 1, 2, 2, 3, 3 };
#define WME_PRIO2AC(prio)  wme_fifo2ac[prio2fifo[(prio)]]

extern bool dhd_wlfc_skip_fc(void);
extern void dhd_wlfc_plat_init(void *dhd);
extern void dhd_wlfc_plat_deinit(void *dhd);
#endif /* endif PROP_TXSTATUS */

#ifndef ETHER_TYPE_BRCM
#define ETHER_TYPE_BRCM   0x886c
#endif
#ifndef ETHER_TYPE_802_1X
#define ETHER_TYPE_802_1X 0x888e          /* 802.1x */
#endif

#define MAX_WAIT_FOR_8021X_TX	100

/* API's for external use */
void dhd_module_cleanup(void);
void dhd_module_init(void);
int dhd_change_mtu(dhd_pub_t *dhdp, int new_mtu, int ifidx);
/* int dhd_do_driver_init(struct net_device *net); */
/* void dhd_event(struct dhd_info *dhd, char *evpkt, int evlen, int ifidx); */
/* void dhd_sendup_event(dhd_pub_t *dhdp, wl_event_msg_t *event, void *data); */
/* void dhd_bus_dump(dhd_pub_t *dhdp, struct bcmstrbuf *strbuf); */
/* void dhd_bus_clearcounts(dhd_pub_t *dhdp); */
/* bool dhd_bus_dpc(struct dhd_bus *bus); */

#define GET_DHDINFO(xx)               (dhd_info_t *)((xx)->if_softc)
/* #define DBUS_RX_BUFFER_SIZE_DHD(net)  (net->mtu + net->hard_header_len + dhd->pub.hdrlen) */
#define copy_from_user(dst, src, len) copyin(src, dst, len)
#define copy_to_user(dst, src, len)   copyout(src, dst, len)

/* keep these two macro same as the output side, such as dbus_usb_fbsd.c */
#ifndef BWL_RX_LIST_COUNT
#error "BWL_RX_LIST_COUNT not defined"
#define BWL_RX_LIST_COUNT          64
#endif
#ifndef BWL_TX_LIST_COUNT
#error "BWL_TX_LIST_COUNT not defined"
#define BWL_TX_LIST_COUNT          512
#endif

#define DHD_FBSD_NET_INTF_NAME  "wl"

/* Interface control information */
typedef struct dhd_if {
	struct dhd_info  *info;   /* back pointer to dhd_info */
	struct ifnet     *net;    /* OS/stack specifics */

	int    idx;        /* iface idx in dongle */
	int    state;      /* interface state */
	uint   subunit;    /* subunit */
	uint8  mac_addr[ETHER_ADDR_LEN]; /* assigned MAC address */
	bool   attached;            /* Delayed attachment when unset */
	bool   txflowcontrol;       /* Per interface flow control indicator */
	char   name[IFNAMSIZ+1];    /* Interface name */
	uint8  bssidx;              /* bsscfg index for the interface */
} dhd_if_t;

/* Local private structure (extension of pub) */
typedef struct dhd_info {
	dhd_pub_t   pub;       /* pub MUST be the first one */
	void        *softc;

	struct ifmedia  sc_ifmedia;

	dhd_if_t    *iflist[DHD_MAX_IFS];  /* For supporting multiple interfaces */

	struct cv   ioctl_resp_wait;       /* FreeBSD using cv for waiting */
	int         ioctl_resp_wait_chan;  /* FreeBSD cv needs this */

	struct mtx    proto_sem;
	struct mtx    txqlock;

#ifdef PROP_TXSTATUS
	/* Mutex for wl flow control */
	struct mtx    wlfc_mutex;
#endif /* PROP_TXSTATUS */

	struct ether_addr macvalue;
	dhd_attach_states_t    dhd_state;
	int    pend_8021x_cnt;
} dhd_info_t;


/* Static Functions */
static int dhd_netif_media_change(struct ifnet *ifp);
static void dhd_netif_media_status(struct ifnet *ifp, struct ifmediareq *imr);
static int dhd_wait_pend8021x(struct ifnet *ifp);
/* static int dhd_wl_host_event(dhd_info_t *dhd, int *ifidx, void *pktdata,
				wl_event_msg_t *event_ptr, void **data_ptr);
*/
int dhd_net2idx(dhd_info_t *dhd, struct ifnet *net);

#ifdef TOE
#ifndef BDC
#error TOE requires BDC
#endif /* !BDC */
static int dhd_toe_get(dhd_info_t *dhd, int idx, uint32 *toe_ol);
/* static int dhd_toe_set(dhd_info_t *dhd, int idx, uint32 toe_ol); */
#endif /* TOE */
static void dhd_bus_detach(dhd_pub_t *dhdp);
static void dhd_open(void *priv);
static int dhd_stop(struct ifnet *net);
static int dhd_ioctl_entry(struct ifnet *net, u_long cmd, caddr_t data);
static void dhd_start_xmit(struct ifnet *net);
static void dhd_dbus_disconnect_cb(void *arg);
static void *dhd_dbus_probe_cb(void *arg, const char *desc, uint32 bustype, uint32 hdrlen);


/* Needed for dhd.h */
uint dhd_roam_disable = 1;
uint dhd_radio_up = 1;

int dhd_ioctl_timeout_msec = IOCTL_RESP_TIMEOUT;

/* User-specified vid/pid, may be set from environment by using "kenv" command */
static int dhd_vid = 0xa5c;
static int dhd_pid = 0x48f;

static char dhd_version[] = "Dongle Host Driver, version " EPI_VERSION_STR;

/* The USB device structure */
extern device_t bwl_device;

static void dhd_dbus_send_complete(void *handle, void *info, int status);
static void dhd_dbus_recv_buf(void *handle, uint8 *buf, int len);
static void dhd_dbus_recv_pkt(void *handle, void *pkt);
static void dhd_dbus_txflowcontrol(void *handle, bool onoff);
static void dhd_dbus_errhandler(void *handle, int err);
static void dhd_dbus_ctl_complete(void *handle, int type, int status);
static void dhd_dbus_state_change(void *handle, int state);
static void *dhd_dbus_pktget(void *handle, uint len, bool send);
static void dhd_dbus_pktfree(void *handle, void *p, bool send);

static dbus_callbacks_t dhd_dbus_cbs = {
	.send_complete  = dhd_dbus_send_complete,
	.recv_buf       = dhd_dbus_recv_buf,
	.recv_pkt       = dhd_dbus_recv_pkt,
	.txflowcontrol  = dhd_dbus_txflowcontrol,
	.errhandler     = dhd_dbus_errhandler,
	.ctl_complete   = dhd_dbus_ctl_complete,
	.state_change   = dhd_dbus_state_change,
	.pktget         = dhd_dbus_pktget,
	.pktfree        = dhd_dbus_pktfree
};


/*
 * Pubic functions for now. This needs to go in the header file
 */
struct net_device*
dhd_handle_ifadd(dhd_info_t *dhd, int ifidx, char *name,
        uint8 *mac, uint8 bssidx, bool need_rtnl_lock);
int
dhd_handle_ifdel(dhd_info_t *dhd, int ifidx, bool need_rtnl_lock);
/*
*
* Some OS specific functions required to implement DHD driver in OS independent way
*
*/
int
dhd_os_proto_block(dhd_pub_t *pub)
{
	dhd_info_t * dhd = (dhd_info_t *)(pub->info);

	if (dhd) {
		mtx_lock(&dhd->proto_sem);
		return 1;
	}
	return 0;
}

int
dhd_os_proto_unblock(dhd_pub_t *pub)
{
	dhd_info_t * dhd = (dhd_info_t *)(pub->info);

	if (dhd) {
		mtx_unlock(&dhd->proto_sem);
		return 1;
	}
	return 0;
}

unsigned int
dhd_os_get_ioctl_resp_timeout(void)
{
	return ((unsigned int)dhd_ioctl_timeout_msec);
}

void
dhd_os_set_ioctl_resp_timeout(unsigned int timeout_msec)
{
	dhd_ioctl_timeout_msec = (int)timeout_msec;
}

int
dhd_os_ioctl_resp_wait(dhd_pub_t *pub, uint *condition, bool *pending)
{
	dhd_info_t *dhd = pub->info;
	int err = 0;
	struct timeval tv;

	if (!(*condition)) {
		tv.tv_sec  = dhd_ioctl_timeout_msec / 1000;
		tv.tv_usec = (dhd_ioctl_timeout_msec % 1000) * 1000;

		err = tsleep((void*) &dhd->ioctl_resp_wait_chan, 0,
				"ioctl_resp_wait", tvtohz(&tv));
	}
	if (err) {
		printf("Wait ioctl_resp_wait  timeout =%x\n", err);
	}
	return (err == 0);
}

int
dhd_os_ioctl_resp_wake(dhd_pub_t *pub)
{
	dhd_info_t *dhd = (dhd_info_t *) (pub->info);

	wakeup((void*) &dhd->ioctl_resp_wait_chan);
	return 0;
}

void
dhd_os_wd_timer(void *bus, uint wdtick)
{
	DHD_ERROR(("%s(): Not implemented \n", __FUNCTION__));
	return;
}

void *
dhd_os_open_image(char *filename)
{
	DHD_ERROR(("%s(): Not implemented \n", __FUNCTION__));
	return NULL;
}

int
dhd_os_get_image_block(char *buf, int len, void *image)
{
	DHD_ERROR(("%s(): Not implemented \n", __FUNCTION__));
	return -1;
}

void
dhd_os_close_image(void *image)
{
	DHD_ERROR(("%s(): Not implemented \n", __FUNCTION__));
	return;
}

void
dhd_os_sdlock_txq(dhd_pub_t *pub)
{
	dhd_info_t *dhd;

	dhd = (dhd_info_t *)(pub->info);
	mtx_lock(&dhd->txqlock);
}

void
dhd_os_sdunlock_txq(dhd_pub_t *pub)
{
	dhd_info_t *dhd;

	dhd = (dhd_info_t *)(pub->info);

	mtx_unlock(&dhd->txqlock);
}
#ifdef PROP_TXSTATUS
int
dhd_os_wlfc_block(dhd_pub_t *pub)
{
	dhd_info_t *dhd = (dhd_info_t *)(pub->info);
	ASSERT(dhd != NULL);
	mtx_lock(&dhd->wlfc_mutex);
	return 1;
}

int
dhd_os_wlfc_unblock(dhd_pub_t *pub)
{
	dhd_info_t *dhd = (dhd_info_t *)(pub->info);
	ASSERT(dhd != NULL);
	mtx_unlock(&dhd->wlfc_mutex);
	return 1;
}

bool dhd_wlfc_skip_fc(void)
{
	return TRUE;
}
void dhd_wlfc_plat_init(void *dhd)
{
	return;
}
void dhd_wlfc_plat_deinit(void *dhd)
{
	return;
}
#endif /* endif PROP_TXSTATUS */

void
dhd_os_sdlock_rxq(dhd_pub_t *pub)
{
	DHD_ERROR(("%s(): Not implemented \n", __FUNCTION__));
	return;
}

void
dhd_os_sdunlock_rxq(dhd_pub_t *pub)
{
	DHD_ERROR(("%s(): Not implemented \n", __FUNCTION__));
	return;
}

void
dhd_timeout_start(dhd_timeout_t *tmo, uint usec)
{
	DHD_TRACE(("%s(): Enter\n", __FUNCTION__));
	tmo->limit = usec;
	tmo->increment = 0;
	tmo->elapsed = 0;
	/* tmo->tick = 1000000/HZ;  TODO: HOW TO GET THE HZ */
	DHD_ERROR(("%s(): ERROR.. NEED TO know the tick\n", __FUNCTION__));
}

int
dhd_timeout_expired(dhd_timeout_t *tmo)
{
	DHD_ERROR(("%s(): Not implemented \n", __FUNCTION__));
	return -1;
}

int
dhd_net2idx(dhd_info_t *dhd, struct ifnet *net)
{
	int i = 0;

	ASSERT(dhd);
	while (i < DHD_MAX_IFS) {
		/* DHD_TRACE(("%s(): dhd=0x%x, dhd->iflist[%d]=0x%x \n",
			__FUNCTION__, (uint32) dhd, i, (uint32) dhd->iflist[i]));
		*/
		if (dhd->iflist[i] && (dhd->iflist[i]->net == net))
			return i;
		i++;
	}

	DHD_ERROR(("%s(): ERROR: network interface not found! \n", __FUNCTION__));

	return DHD_BAD_IF;
}

struct ifnet *
dhd_idx2net(struct dhd_pub *dhd_pub, int ifidx)
{
	struct dhd_info *dhd_info;

	if (!dhd_pub || ifidx < 0 || ifidx >= DHD_MAX_IFS) {
		if (!dhd_pub)
			DHD_ERROR(("%s(): ERROR: dhd_pub is NULL \n", __FUNCTION__));
		if (ifidx < 0 || ifidx >= DHD_MAX_IFS)
			DHD_ERROR(("%s(): ERROR: ifidx is %d out of range\n", __FUNCTION__, ifidx));
		return NULL;
	}

	dhd_info = dhd_pub->info;
	if (!dhd_info) {
		DHD_ERROR(("%s(): ERROR: dhd_info is NULL \n", __FUNCTION__));
		return NULL;
	}
	if (!dhd_info->iflist[ifidx]) {
		/* DHD_ERROR(("%s(): ERROR: iflist[%d] is NULL \n", __FUNCTION__, ifidx)); */
		return NULL;
	}

	return dhd_info->iflist[ifidx]->net;
}

int
dhd_ifname2idx(dhd_info_t *dhd, char *name)
{
	int i = DHD_MAX_IFS;

	ASSERT(dhd);

	if (name == NULL || *name == '\0') {
		return 0;
	}

	for (i = 0; i < DHD_MAX_IFS; i++) {
		if (dhd->iflist[i] && !strncmp(dhd->iflist[i]->name, name, IFNAMSIZ))
			break;
	}

	if (i == DHD_MAX_IFS) {
		i = 0;  /* default - the primary interface */
		DHD_ERROR(("%s(): ERROR: interface %s not in the list, return default idx 0 \n",
			__FUNCTION__, name));
	}

	DHD_TRACE(("%s(): Return idx %d for interface '%s' \n", __FUNCTION__, i, name));
	return i;
}

int
dhd_ifidx2hostidx(dhd_info_t *dhd, int ifidx)
{
	int i = DHD_MAX_IFS;

	ASSERT(dhd);

	for (i = 0; i < DHD_MAX_IFS; i++) {
		if (dhd->iflist[i] && (dhd->iflist[i]->idx == ifidx))
			break;
	}

	if (i == DHD_MAX_IFS) {
		i = 0;  /* default - the primary interface */
		DHD_ERROR(("%s(): ERROR: ifidx %d not in the list, return default idx 0 \n",
			__FUNCTION__, ifidx));
	}

	DHD_TRACE(("%s(): Return hostidx %d for ifidx %d \n", __FUNCTION__, i, ifidx));
	return i;
}

char *
dhd_ifname(dhd_pub_t *dhdp, int ifidx)
{
	dhd_info_t *dhd = (dhd_info_t *)dhdp->info;

	ASSERT(dhd);

	if (ifidx < 0 || ifidx >= DHD_MAX_IFS) {
		DHD_ERROR(("%s(): ifidx %d out of range\n", __FUNCTION__, ifidx));
		return "<if_bad>";
	}

	if (dhd->iflist[ifidx] == NULL) {
		DHD_ERROR(("%s(): Null i/f %d\n", __FUNCTION__, ifidx));
		return "<if_null>";
	}

	if (dhd->iflist[ifidx]->net == NULL) {
		DHD_ERROR(("%s(): dhd->iflist[%d]->net not allocated\n", __FUNCTION__, ifidx));
		return "<if_none>";
	}

	return (dhd->iflist[ifidx]->name);
}

int dhd_netif_media_change(struct ifnet *ifp)
{
	DHD_ERROR(("%s(): Not implemented \n", __FUNCTION__));
	return BCME_OK;
}

void dhd_netif_media_status(struct ifnet *ifp, struct ifmediareq *imr)
{
	dhd_pub_t *pub = (dhd_pub_t *) ifp->if_softc;
	imr->ifm_active = IFM_ETHER;
	imr->ifm_status = IFM_AVALID;
	if (ifp->if_flags & IFF_UP) {
		imr->ifm_status = IFM_ACTIVE;
		pub->busstate = DBUS_STATE_UP;
		DHD_TRACE(("%s(): set ifm_status active busstate up\n", __FUNCTION__));
	} else {
		pub->busstate = DBUS_STATE_DOWN;
		DHD_TRACE(("%s(): set busstate down\n", __FUNCTION__));
	}
	return;
}

static int
dhd_get_pend_8021x_cnt(dhd_info_t *dhd)
{
	return (atomic_load_acq_int(&dhd->pend_8021x_cnt));
}

int
dhd_wait_pend8021x(struct ifnet *ifp)
{
	dhd_info_t *dhd = (dhd_info_t *)ifp->if_softc;
	int ntimes      = MAX_WAIT_FOR_8021X_TX;
	int pend        = dhd_get_pend_8021x_cnt(dhd);
	struct timeval tv;
	int timeout;

	tv.tv_sec  = 0;
	tv.tv_usec = 10 * 1000;
	timeout = tvtohz(&tv);

	while (ntimes && pend) {
		if (pend) {
			DHD_PERIM_UNLOCK(&dhd->pub);
			pause("wait8021x", timeout);
			DHD_PERIM_LOCK(&dhd->pub);
			ntimes--;
		}
		pend = dhd_get_pend_8021x_cnt(dhd);
	}
	if (ntimes == 0)
	{
		atomic_set_int(&dhd->pend_8021x_cnt, 0);
		DHD_ERROR(("%s: TIMEOUT\n", __FUNCTION__));
	}
	return pend;
}


#ifdef TOE
/* Retrieve current toe component enables, which are kept as a bitmap in toe_ol iovar */
static int
dhd_toe_get(dhd_info_t *dhd, int ifidx, uint32 *toe_ol)
{
	wl_ioctl_t ioc;
	char buf[32];
	int ret;

	memset(&ioc, 0, sizeof(ioc));

	ioc.cmd = WLC_GET_VAR;
	ioc.buf = buf;
	ioc.len = (uint)sizeof(buf);
	ioc.set = WL_IOCTL_ACTION_GET;

	strcpy(buf, "toe_ol");

	ret = dhd_wl_ioctl(&dhd->pub, ifidx, &ioc, ioc.buf, ioc.len);
	if (ret != BCME_OK) {
		/* Check for older dongle image that doesn't support toe_ol */
		if (ret == -EIO) {
			DHD_ERROR(("%s: toe not supported by device\n",
				dhd_ifname(&dhd->pub, ifidx)));
			return BCME_UNSUPPORTED;
		}

		DHD_INFO(("%s: could not get toe_ol: ret=%d\n", dhd_ifname(&dhd->pub, ifidx), ret));
		return ret;
	}

	memcpy(toe_ol, buf, sizeof(uint32));
	return ret;
}

#ifdef NOT_YET
/* Set current toe component enables in toe_ol iovar, and set toe global enable iovar */
static int
dhd_toe_set(dhd_info_t *dhd, int ifidx, uint32 toe_ol)
{
	wl_ioctl_t ioc;
	char buf[32];
	int toe, ret;

	memset(&ioc, 0, sizeof(ioc));

	ioc.cmd = WLC_SET_VAR;
	ioc.buf = buf;
	ioc.len = (uint)sizeof(buf);
	ioc.set = TRUE;

	/* Set toe_ol as requested */

	strcpy(buf, "toe_ol");
	memcpy(&buf[sizeof("toe_ol")], &toe_ol, sizeof(uint32));

	if ((ret = dhd_wl_ioctl(&dhd->pub, ifidx, &ioc, ioc.buf, ioc.len)) < 0) {
		DHD_ERROR(("%s: could not set toe_ol: ret=%d\n",
			dhd_ifname(&dhd->pub, ifidx), ret));
		return ret;
	}

	/* Enable toe globally only if any components are enabled. */

	toe = (toe_ol != 0);

	strcpy(buf, "toe");
	memcpy(&buf[sizeof("toe")], &toe, sizeof(uint32));

	if ((ret = dhd_wl_ioctl(&dhd->pub, ifidx, &ioc, ioc.buf, ioc.len)) < 0) {
		DHD_ERROR(("%s: could not set toe: ret=%d\n", dhd_ifname(&dhd->pub, ifidx), ret));
		return ret;
	}

	return 0;
}
#endif /* #ifdef NOT_YET */

#endif /* TOE */

/*
*
* Some API's needed for external use (for dhd.h)
*
*/
int dhd_change_mtu(dhd_pub_t *dhdp, int new_mtu, int ifidx)
{
	DHD_ERROR(("%s(): Not implemented \n", __FUNCTION__));
	return -1;
}

int dhd_do_driver_init(struct net_device *net)
{
	DHD_ERROR(("%s(): Not implemented \n", __FUNCTION__));
	return -1;
}
void
dhd_event(struct dhd_info *dhd, char *evpkt, int evlen, int ifidx)
{

#ifdef DHD_NET80211
	dhd_if_t *ifp;
	wl_event_msg_t event;

	DHD_ERROR(("%s(): Started.  ifidx = %d, evlen = %d \n", __FUNCTION__, ifidx, evlen));

	memcpy(&event, &(((bcm_event_t *)evpkt)->event), sizeof(wl_event_msg_t));

	ifp = dhd->iflist[ifidx];
	if (ifp == NULL)
		ifp = dhd->iflist[0];

	dhd_net80211_event_proc(&dhd->pub, ifp->net, &event, 0);
#endif
	return;
}

/* send up locally generated event */
void
dhd_sendup_event(dhd_pub_t *dhdp, wl_event_msg_t *event, void *data)
{
#ifdef NOT_YET
	switch (ntoh32(event->event_type)) {
	default:
		break;
	}
#else
	DHD_ERROR(("%s(): Not implemented \n", __FUNCTION__));
	return;
#endif
}

void
dhd_bus_dump(dhd_pub_t *dhdp, struct bcmstrbuf *strbuf)
{
	bcm_bprintf(strbuf, "Bus USB\n");
}

void
dhd_bus_clearcounts(dhd_pub_t *dhdp)
{
	DHD_ERROR(("%s(): Not implemented \n", __FUNCTION__));
	return;
}

bool
dhd_bus_dpc(struct dhd_bus *bus)
{
	return FALSE;
}

int
dhd_dbus_txdata(dhd_pub_t *dhdp, void *pktbuf)
{
	int ret;
	int len;
	caddr_t buf = NULL;

	if (dhdp->txoff)
		return BCME_EPERM;

	len = m_length(pktbuf, NULL);
	buf = MALLOC(dhdp->osh, len);

	if (buf != NULL)
		/*
		 * copy mbuf data to a linear buffer?
		 */
		m_copydata(pktbuf, 0, len, buf);
	else {
		DHD_ERROR(("%s(): Cannot Allocate Memory for copying \n", __FUNCTION__));
		PKTFREE(dhdp->osh, pktbuf, FALSE);
		return -1;
	}
	ret = dbus_send_buf(dhdp->dbus, buf, len, pktbuf);
	/* Done with the buffer */
	MFREE(dhdp->osh, buf, len);

	if (ret)
		PKTFREE(dhdp->osh, pktbuf, TRUE);
	return ret;
}

/*
*
* API's for dhd callback
*
*/
void
dhd_txcomplete(dhd_pub_t *dhdp, void *txp, bool success)
{
	dhd_info_t *dhd = (dhd_info_t *)(dhdp->info);
	struct ether_header *eh;
	uint16 type;

	dhd_prot_hdrpull(dhdp, NULL, txp, NULL, NULL);

	eh = (struct ether_header *)PKTDATA(dhdp->osh, txp);
	type  = ntoh16(eh->ether_type);

	if (type == ETHER_TYPE_802_1X)
		atomic_subtract_int(&dhd->pend_8021x_cnt, 1);
}

int
dhd_sendpkt(dhd_pub_t *dhdp, int ifidx, void *pktbuf)
{
	struct ether_header *eh = NULL;
	dhd_info_t *dhd = (dhd_info_t *)(dhdp->info);
	int ret = 0;

	/* Reject if down */
	if (!dhdp->up || (dhdp->busstate == DHD_BUS_DOWN)) {
		if (!dhdp->up) {
			DHD_ERROR(("%s(): !dhdp->up \n", __FUNCTION__));
		}
		if (dhdp->busstate == DHD_BUS_DOWN) {
			DHD_ERROR(("%s(): (dhdp->busstate == DHD_BUS_DOWN) \n", __FUNCTION__));
		}
		/* free the packet here since the caller won't */
		PKTFREE(dhdp->osh, pktbuf, TRUE);
		return -ENODEV;
	}

	/* Update multicast statistic */
	if (PKTLEN(dhdp->osh, pktbuf) >= ETHER_HDR_LEN) {
		uint8 *pktdata = (uint8 *)PKTDATA(dhdp->osh, pktbuf);
		eh = (struct ether_header *)pktdata;

#ifdef PROP_TXSTATUS
		if (ETHER_ISMULTI(eh->ether_dhost))
			dhdp->tx_multicast++;
#endif
		 if (ntoh16(eh->ether_type) == ETHER_TYPE_802_1X)
			atomic_add_int(&dhd->pend_8021x_cnt, 1);

	} else {
		PKTCFREE(dhdp->osh, pktbuf, TRUE);
		return BCME_ERROR;
	}

	/* Look into the packet and update the packet priority */
	if (PKTPRIO(pktbuf) == 0)
		pktsetprio(pktbuf, FALSE);

#ifdef PROP_TXSTATUS
	if (dhd_wlfc_is_supported(dhdp)) {
		/* store the interface ID */
		DHD_PKTTAG_SETIF(PKTTAG(pktbuf), ifidx);

		/* store destination MAC in the tag as well */
		DHD_PKTTAG_SETDSTN(PKTTAG(pktbuf), eh->ether_dhost);

		/* decide which FIFO this packet belongs to */
		if (ETHER_ISMULTI(eh->ether_dhost))
			/* one additional queue index (highest AC + 1) is used for bc/mc queue */
			DHD_PKTTAG_SETFIFO(PKTTAG(pktbuf), AC_COUNT);
		else
			DHD_PKTTAG_SETFIFO(PKTTAG(pktbuf), WME_PRIO2AC(PKTPRIO(pktbuf)));
	} else
#endif /* PROP_TXSTATUS */
	/* If the protocol uses a data header, apply it */

	dhd_prot_hdrpush(dhdp, ifidx, &pktbuf);

	/* Use bus module to send data frame */
#ifdef WLMEDIA_HTSF
	dhd_htsf_addtxts(dhdp, pktbuf);
#endif

#ifdef PROP_TXSTATUS
	if (dhd_wlfc_commit_packets(dhdp, (f_commitpkt_t)dhd_dbus_txdata,
		dhdp, pktbuf, TRUE) == WLFC_UNSUPPORTED) {
		/* non-proptxstatus way */
		ret = dhd_dbus_txdata(dhdp, pktbuf);
	}
#else
	ret = dhd_dbus_txdata(dhdp, pktbuf);
#endif

	return ret;
}


void
dhd_rx_frame(dhd_pub_t *dhdp, int ifidx, void *pktbuf, int numpkt, uint8 chan)
{
	dhd_info_t *dhd = (dhd_info_t *)dhdp->info;
	struct mbuf *m = NULL;
	void *pnext, *save_pktbuf;
	int i;
	dhd_if_t *ifp;
	struct ether_header *eh;

	DHD_TRACE(("%s(): Enter\n", __FUNCTION__));

	save_pktbuf = pktbuf;

	for (i = 0; pktbuf && i < numpkt; i++, pktbuf = pnext) {

		pnext = PKTNEXT(dhdp->osh, pktbuf);
		PKTSETNEXT(dhdp->osh, pktbuf, NULL);

		m = PKTTONATIVE(dhdp->osh, pktbuf);
		if (m == NULL) {
			return;
		}

		ifp = dhd->iflist[ifidx];
		if (ifp == NULL) {
			ifp = dhd->iflist[0];
			DHD_ERROR(("%s(): dhd->iflist[%d] is NULL, use primary one (0) instead\n",
				__FUNCTION__, ifidx));
		}

		ASSERT(ifp);

		m->m_pkthdr.rcvif = ifp->net;

		eh = mtod(m, struct ether_header *);

#ifdef PROP_TXSTATUS
		if (dhd_wlfc_is_header_only_pkt(dhdp, pktbuf)) {
			/* WLFC may send header only packet when
			 * there is an urgent message but no packet to
			 * piggy-back on
			 */
			PKTCFREE(dhdp->osh, pktbuf, FALSE);
			continue;
		}
#endif
		/* Process special event packets and then discard them */
		if (ntoh16(eh->ether_type) == ETHER_TYPE_BRCM) {
			wl_event_msg_t event;
			void *data;
			memset(&event, 0, sizeof(event));
			DHD_TRACE(("%s(): ETHER_TYPE_BRCM received. \n", __FUNCTION__));
			wl_host_event(dhdp, &ifidx, m->m_data, &event, &data, NULL);
			wl_event_to_host_order(&event);
#ifdef DHD_DONOT_FORWARD_BCMEVENT_AS_NETWORK_PKT
			PKTFREE(dhdp->osh, pktbuf, FALSE);
			continue;
#endif /* DHD_DONOT_FORWARD_BCMEVENT_AS_NETWORK_PKT */
		} else {
#ifdef PROP_TXSTATUS
			dhd_wlfc_save_rxpath_ac_time(dhdp, (uint8)PKTPRIO(pktbuf));
#endif /* PROP_TXSTATUS */
		}

		ASSERT(ifidx < DHD_MAX_IFS && dhd->iflist[ifidx]);
		if (dhd->iflist[ifidx] && !dhd->iflist[ifidx]->state)
			ifp = dhd->iflist[ifidx];

		if (ntoh16(eh->ether_type) != ETHER_TYPE_BRCM) {
			dhdp->dstats.rx_bytes += m->m_len;
			dhdp->rx_packets++; /* Local count */
		}

		(*ifp->net->if_input)(ifp->net, m);

	}
	/*
	// DHD_OS_WAKE_LOCK_TIMEOUT_ENABLE was replaced by DHD_OS_WAKE_LOCK_RX_TIMEOUT_ENABLE
	// and DHD_OS_WAKE_LOCK_CTRL_TIMEOUT_ENABLE, but all these 3 macros in FreeBSD are
	// empty (defined as NULL), so it is safe to just comment it out
	DHD_OS_WAKE_LOCK_TIMEOUT_ENABLE(dhdp, DHD_PACKET_TIMEOUT_MS);
	*/
}


static int
bwl_tx_data(dhd_pub_t *pub, int ifidx, struct mbuf *m)
{
	struct m_tag    *mtag;

	if (m_tag_find(m, FREEBSD_PKTTAG, (struct m_tag *)NULL) == NULL) {
		mtag = m_tag_get(FREEBSD_PKTTAG, OSL_PKTTAG_SZ, M_DONTWAIT);
		if (!mtag) {
			DHD_ERROR(("%s(): FREEBSD_PKTTAG not found \n", __FUNCTION__));
			m_freem(m);
			return -1;
		}
		bzero((void *)(mtag + 1), OSL_PKTTAG_SZ);
	        m_tag_prepend(m, mtag);
	}

	if (m_tag_find(m, FREEBSD_PKTPRIO, (struct m_tag *)NULL) == NULL) {
		mtag = m_tag_get(FREEBSD_PKTPRIO, OSL_PKTPRIO_SZ, M_DONTWAIT);
		if (!mtag) {
			DHD_ERROR(("%s(): FREEBSD_PKTPRIO not found \n", __FUNCTION__));
			m_freem(m);
			return -1;
		}
		bzero((void *)(mtag + 1), OSL_PKTPRIO_SZ);
		m_tag_prepend(m, mtag);
	}
	pktsetprio((void *)m, FALSE);

	dhd_sendpkt(pub, ifidx, m);

	return 0;
}


void
dhd_txflowcontrol(dhd_pub_t *dhdp, int ifidx, bool state)
{
	struct ifnet *net;
	int i;

	if (ifidx == ALL_INTERFACES) {
		for (i = 0; i < DHD_MAX_IFS; i++) {
			net = dhd_idx2net(dhdp, i);
			if (net != NULL) {
				/* DHD_ERROR(("Toggle flow control state = %d\n", state)); */
				if (state == OFF) {
					net->if_drv_flags &= ~IFF_DRV_OACTIVE;
						dhd_start_xmit(net);
				} else {
					net->if_drv_flags |= IFF_DRV_OACTIVE;
				}
			}
		}
	}
	else {
		net = dhd_idx2net(dhdp, ifidx);
		if (net != NULL) {
			if (state == OFF)
				net->if_drv_flags &= ~IFF_DRV_OACTIVE;
			else
				net->if_drv_flags |= IFF_DRV_OACTIVE;
		}
	}
}


static void
dhd_op_if(dhd_if_t *ifp)
{
	dhd_info_t *dhd;
	int ret = 0, err = 0;

	ASSERT(ifp && ifp->info && ifp->idx);	/* Virtual interfaces only */

	dhd = ifp->info;

	DHD_TRACE(("%s(): idx %d, state %d\n", __FUNCTION__, ifp->idx, ifp->state));

	switch (ifp->state) {
		case WLC_E_IF_ADD:
			/* Delete the existing interface before overwriting it,
			* in case if we missed the WLC_E_IF_DEL event.
			*/
			DHD_TRACE(("%s(): ifp->state = WLC_E_IF_ADD \n", __FUNCTION__));
			if (ifp->net != NULL) {
				DHD_ERROR(("%s(): ERROR, net device is NULL\n", __FUNCTION__));
				ifmedia_removeall(&dhd->sc_ifmedia);
				ether_ifdetach(ifp->net);
				if_free(ifp->net);
			}
			/* Allocate etherdev, including space for private structure */
			if (!(ifp->net = if_alloc(IFT_ETHER))) {
				DHD_ERROR(("%s(): ERROR.. if_alloc() failed \n", __FUNCTION__));
				ret = -ENOMEM;
			}
			if (ret == 0) {
				if_initname(ifp->net, ifp->name, device_get_unit(bwl_device));
				ifp->net->if_softc = dhd;
				if ((err = dhd_register_if(&dhd->pub, ifp->idx, TRUE)) != 0) {
					DHD_ERROR(("%s(): ERROR.. if_alloc() failed. err = %d \n",
						__FUNCTION__, err));
					ret = -EOPNOTSUPP;
				} else {
					ifp->state = 0;
				}
			}
			break;
		case WLC_E_IF_DEL:
			DHD_TRACE(("%s(): ifp->state = WLC_E_IF_ADD \n", __FUNCTION__));
			if (ifp->net != NULL) {
				DHD_TRACE(("\n%s: got 'WLC_E_IF_DEL' state\n", __FUNCTION__));
				ret = DHD_DEL_IF;	/* Make sure the free_netdev() is called */
			}
			break;
		default:
			DHD_TRACE(("%s(): ifp->state = %d.  Not handled \n",
				__FUNCTION__, ifp->state));
			ASSERT(!ifp->state);
			break;
	}

	if (ret < 0) {
		if (ifp->net) {
			if_free(ifp->net);
		}
		dhd->iflist[ifp->idx] = NULL;
		MFREE(dhd->pub.osh, ifp, sizeof(*ifp));
	}
}


static void
dhd_dbus_send_complete(void *handle, void *info, int status)
{
	dhd_info_t *dhd = (dhd_info_t *)handle;
	void *pkt = info;

	DHD_TRACE(("%s(): Started. status = %d \n", __FUNCTION__, status));

	if (pkt == NULL)
		return;
	if (dhd == NULL) {
		DHD_ERROR(("%s(): ERROR.. dhd = NULL \n", __FUNCTION__));
		return;
	}

	if (status == DBUS_OK) {
		dhd->pub.dstats.tx_packets++;
	} else {
		DHD_ERROR(("%s(): ERROR.. status (%d) != DBUS_OK \n",
			__FUNCTION__, status));
		dhd->pub.dstats.tx_errors++;
	}
#ifdef PROP_TXSTATUS
	if (DHD_PKTTAG_WLFCPKT(PKTTAG(pkt)) &&
		(dhd_wlfc_txcomplete(&dhd->pub, pkt, status == 0) != WLFC_UNSUPPORTED)) {
		return;
	}
#endif /* PROP_TXSTATUS */
	PKTFREE(dhd->pub.osh, pkt, TRUE);

}

static void
dhd_dbus_recv_buf(void *handle, uint8 *buf, int len)
{
	dhd_info_t *dhd = (dhd_info_t *)handle;
	void *pkt;

	DHD_TRACE(("%s(): Started. len = %d \n", __FUNCTION__, len));

	if (dhd == NULL) {
		DHD_ERROR(("%s(): ERROR.. dhd = NULL \n", __FUNCTION__));
		return;
	}

	/* It looks fine for FreeBSD to use this */
	if ((pkt = PKTGET(dhd->pub.osh, len, FALSE)) == NULL) {
		DHD_ERROR(("%s(): ERROR.. PKTGET failed \n", __FUNCTION__));
		return;
	}

	bcopy(buf, PKTDATA(dhd->pub.osh, pkt), len);
	dhd_dbus_recv_pkt(dhd, pkt);
}

static void
dhd_dbus_recv_pkt(void *handle, void *pkt)
{
	uchar reorder_info_buf[WLHOST_REORDERDATA_TOTLEN];
	uint reorder_info_len;
	uint pkt_count;
	dhd_info_t *dhd = (dhd_info_t *)handle;
	int ifidx = 0;

	if (dhd == NULL) {
		DHD_ERROR(("%s(): ERROR.. dhd = NULL \n", __FUNCTION__));
		return;
	}

	/* If the protocol uses a data header, check and remove it */
	if (dhd_prot_hdrpull(&dhd->pub, &ifidx, pkt, reorder_info_buf,
		&reorder_info_len) != 0) {
		DHD_ERROR(("%s() rx protocol error\n", __FUNCTION__));
		PKTFREE(dhd->pub.osh, pkt, FALSE);
		dhd->pub.rx_errors++;
		return;
	}

	if (reorder_info_len) {
		/* Reordering info from the firmware */
		dhd_process_pkt_reorder_info(&dhd->pub, reorder_info_buf, reorder_info_len,
			&pkt, &pkt_count);
		if (pkt_count == 0)
			return;
	}
	else
		pkt_count = 1;

	dhd_rx_frame(&dhd->pub, ifidx, pkt, pkt_count, 0);
}

static void
dhd_dbus_txflowcontrol(void *handle, bool onoff)
{
	dhd_info_t *dhd = (dhd_info_t *)handle;
	bool wlfc_enabled = FALSE;

	DHD_TRACE(("%s(): Started. \n", __FUNCTION__));

	if (dhd == NULL) {
		DHD_ERROR(("%s(): ERROR.. dhd = NULL \n", __FUNCTION__));
		return;
	}

#ifdef PROP_TXSTATUS
	wlfc_enabled = (dhd_wlfc_flowcontrol(&dhd->pub, onoff, !onoff) != WLFC_UNSUPPORTED);
#endif

	if (!wlfc_enabled) {
		dhd_txflowcontrol(&dhd->pub, ALL_INTERFACES, onoff);
	}
}
static void
dhd_dbus_errhandler(void *handle, int err)
{
	DHD_ERROR(("%s(): Not implemented \n", __FUNCTION__));
	return;
}

static void
dhd_dbus_ctl_complete(void *handle, int type, int status)
{
	dhd_info_t *dhd = (dhd_info_t *)handle;

	DHD_TRACE(("%s(): Started. type = %d, status = %d \n",
		__FUNCTION__, type, status));

	if (dhd == NULL) {
		DHD_ERROR(("%s(): ERROR.. dhd = NULL \n", __FUNCTION__));
		return;
	}

	if (type == DBUS_CBCTL_READ) {
		if (status == DBUS_OK)
			dhd->pub.rx_ctlpkts++;
		else
			dhd->pub.rx_ctlerrs++;
	} else if (type == DBUS_CBCTL_WRITE) {
		if (status == DBUS_OK)
			dhd->pub.tx_ctlpkts++;
		else
			dhd->pub.tx_ctlerrs++;
	}

	dhd_prot_ctl_complete(&dhd->pub);
}

static void
dhd_dbus_state_change(void *handle, int state)
{
	dhd_info_t *dhd = (dhd_info_t *)handle;

	DHD_TRACE(("%s(): Started. state = %d \n", __FUNCTION__, state));

	if (dhd == NULL) {
		DHD_ERROR(("%s(): ERROR.. dhd = NULL \n", __FUNCTION__));
		return;
	}

	if (state == DBUS_STATE_DOWN) {
		DHD_TRACE(("%s(): DBUS is down \n", __FUNCTION__));
		dhd->pub.busstate = DHD_BUS_DOWN;
	} else {
		DHD_TRACE(("%s(): DBUS current state=%d\n", __FUNCTION__, state));
	}
}

static void *
dhd_dbus_pktget(void *handle, uint len, bool send)
{
	dhd_info_t *dhd = (dhd_info_t *)handle;
	void *p = NULL;

	DHD_TRACE(("%s(): Started. \n", __FUNCTION__));

	if (dhd == NULL) {
		DHD_ERROR(("%s(): ERROR.. dhd = NULL \n", __FUNCTION__));
		return NULL;
	}

	if (send == TRUE) {
		dhd_os_sdlock_txq(&dhd->pub);
		p = PKTGET(dhd->pub.osh, len, TRUE);
		dhd_os_sdunlock_txq(&dhd->pub);
	} else {
		dhd_os_sdlock_rxq(&dhd->pub);
		p = PKTGET(dhd->pub.osh, len, FALSE);
		dhd_os_sdunlock_rxq(&dhd->pub);
	}

	return p;
}

static void
dhd_dbus_pktfree(void *handle, void *p, bool send)
{
	dhd_info_t *dhd = (dhd_info_t *)handle;

	DHD_TRACE(("%s(): Started. \n", __FUNCTION__));

	if (dhd == NULL) {
		DHD_ERROR(("%s(): ERROR.. dhd = NULL \n", __FUNCTION__));
		return;
	}

	if (send == TRUE) {
#ifdef PROP_TXSTATUS
		if (DHD_PKTTAG_WLFCPKT(PKTTAG(p)) &&
			(dhd_wlfc_txcomplete(&dhd->pub, p, FALSE) != WLFC_UNSUPPORTED)) {
			return;
		}
#endif /* PROP_TXSTATUS */

		dhd_os_sdlock_txq(&dhd->pub);
		PKTFREE(dhd->pub.osh, p, TRUE);
		dhd_os_sdunlock_txq(&dhd->pub);
	} else {
		dhd_os_sdlock_rxq(&dhd->pub);
		PKTFREE(dhd->pub.osh, p, FALSE);
		dhd_os_sdunlock_rxq(&dhd->pub);
	}
}

/*
*
* Main API's used in dhd driver
*
*/
dhd_pub_t *
dhd_attach(osl_t *osh, struct dhd_bus *bus, uint bus_hdrlen)
{
	dhd_info_t *dhd = NULL;
	struct ifnet *ifp = NULL;
	dhd_attach_states_t dhd_state = DHD_ATTACH_STATE_INIT;

	DHD_TRACE(("%s(): Enter\n", __FUNCTION__));

	/* Allocate primary dhd_info */
	DHD_TRACE(("%s(): Allocate primary dhd_info \n", __FUNCTION__));
	if (!(dhd = MALLOC(osh, sizeof(dhd_info_t)))) {
		DHD_ERROR(("%s(): ERROR.. MALLOC() failed \n", __FUNCTION__));
		goto fail;
	}
	memset(dhd, 0, sizeof(dhd_info_t));
	dhd_state |= DHD_ATTACH_STATE_DHD_ALLOC;

	dhd->pub.osh = osh;
	dhd->pub.info = dhd;
	dhd->pub.bus = bus;
	dhd->pub.hdrlen = bus_hdrlen;

	/* facility initialization */
	cv_init(&dhd->ioctl_resp_wait, "ioctl_resp_wait");
	mtx_init(&dhd->txqlock, "txqlock", NULL, MTX_DEF);

	DHD_TRACE(("%s(): Calling dhd_handle_ifadd()\n", __FUNCTION__));
	if (!(ifp = dhd_handle_ifadd(dhd, 0, NULL, NULL, 0, TRUE))) {
		DHD_ERROR(("%s(): ERROR.. dhd_handle_ifadd() failed \n", __FUNCTION__));
		goto fail;
	}
	dhd_state |= DHD_ATTACH_STATE_ADD_IF;

	mtx_init(&dhd->proto_sem, "proto_sem", NULL, MTX_DEF);

#ifdef PROP_TXSTATUS
	mtx_init(&dhd->wlfc_mutex, "dhdwlfc", NULL, MTX_DEF);

	dhd->pub.skip_fc = dhd_wlfc_skip_fc;
	dhd->pub.plat_init = dhd_wlfc_plat_init;
	dhd->pub.plat_deinit = dhd_wlfc_plat_deinit;

#endif

	dhd_state |= DHD_ATTACH_STATE_WAKELOCKS_INIT;

	/* Attach and link in the protocol */
	DHD_TRACE(("%s(): Calling dhd_prot_attach() \n", __FUNCTION__));
	if (dhd_prot_attach(&dhd->pub) != 0) {
		DHD_ERROR(("%s(): ERROR.. dhd_prot_attach() failed \n", __FUNCTION__));
		goto fail;
	}
	dhd_state |= DHD_ATTACH_STATE_PROT_ATTACH;


	/* Not creating any threads now */
	dhd_state |= DHD_ATTACH_STATE_THREADS_CREATED;

	dhd_state |= DHD_ATTACH_STATE_DONE;

	/* Save the dhd_info into the priv */
	dhd->dhd_state = dhd_state;

	DHD_TRACE(("%s(): Returns SUCCESS \n", __FUNCTION__));

	return &dhd->pub;

fail:
	DHD_ERROR(("%s(): ERROR.. Doing cleanup \n", __FUNCTION__));
	DHD_TRACE(("%s(): Calling dhd_detach() dhd_state 0x%x &dhd->pub %p \n",
		__FUNCTION__, dhd_state, &dhd->pub));
	dhd->dhd_state = dhd_state;
	dhd_detach(&dhd->pub);
	dhd_free(&dhd->pub);
	return NULL;
}

int
dhd_register_if(dhd_pub_t *dhdp, int ifidx, bool need_rtnl_lock)
{
	dhd_info_t *dhd = (dhd_info_t *)dhdp->info;
	struct ifnet *net = NULL;

	DHD_TRACE(("%s(): ifidx %d \n", __FUNCTION__, ifidx));

	ASSERT(dhd && dhd->iflist[ifidx]);

	net = dhd->iflist[ifidx]->net;
	ASSERT(net);

	ifmedia_init(&dhd->sc_ifmedia, 0, dhd_netif_media_change, dhd_netif_media_status);
	ifmedia_add(&dhd->sc_ifmedia, IFM_ETHER|IFM_AUTO, 0, NULL);
	ifmedia_set(&dhd->sc_ifmedia, IFM_ETHER|IFM_AUTO);

	if_initname(net, DHD_FBSD_NET_INTF_NAME, device_get_unit(bwl_device));
	net->if_flags = IFF_BROADCAST | IFF_SIMPLEX | IFF_MULTICAST;
	net->if_drv_flags &= ~(IFF_DRV_RUNNING | IFF_DRV_OACTIVE);

	net->if_init = dhd_open;
	net->if_ioctl = dhd_ioctl_entry;
	net->if_start = dhd_start_xmit;
	net->if_mtu = 1500;

	IFQ_SET_MAXLEN(&net->if_snd, BWL_TX_LIST_COUNT);
	net->if_snd.ifq_drv_maxlen = BWL_TX_LIST_COUNT;
	IFQ_SET_READY(&net->if_snd);

	DHD_TRACE(("%s(): net=0x%x, net->if_softc=0x%x\n", __FUNCTION__,
		(uint32)net, (uint32)(net->if_softc)));

	dhd->pub.rxsz = BWL_RX_BUFFER_SIZE;
	memcpy(&dhd->macvalue, dhdp->mac.octet, ETHER_ADDR_LEN);

	DHD_TRACE(("%s: Calling ether_ifattach() \n", __FUNCTION__));
	ether_ifattach(net, dhd->macvalue.octet);

#ifdef DHD_NET80211
	{
		int ret;
		DHD_TRACE(("%s(): Calling dhd_net80211_attach() \n", __FUNCTION__));
		ret = dhd_net80211_attach(dhdp, net);
		if (ret) {
			DHD_ERROR(("%s(): ERROR.. dhd_net80211_attach() failed\n", __FUNCTION__));
		}
	}
#endif /* #ifdef DHD_NET80211 */

	return 0;

}


void
dhd_detach(dhd_pub_t *dhdp)
{
	dhd_info_t *dhd;

	DHD_TRACE(("%s(): Enter\n", __FUNCTION__));

	if (!dhdp) {
		DHD_ERROR(("%s(): ERROR.. dhdp is NULL \n", __FUNCTION__));
		return;
	}

	dhd = (dhd_info_t *)dhdp->info;

	if (!dhd) {
		DHD_ERROR(("%s(): ERROR.. dhd is NULL \n", __FUNCTION__));
		return;
	}

	DHD_TRACE(("%s(): Enter state 0x%x \n", __FUNCTION__, dhd->dhd_state));

	if (!(dhd->dhd_state & DHD_ATTACH_STATE_DONE)) {
		/* Give some delay for threads to start running
		* in case dhd_attach() has failed
		*/
		osl_delay(1000*100);
	}

	/* delete all interfaces, start with virtual  */
	DHD_TRACE(("%s(): Deleting all the interfaces \n", __FUNCTION__));
	if (dhd->dhd_state & DHD_ATTACH_STATE_ADD_IF) {
		int i = 1;
		dhd_if_t *ifp;

		for (i = 1; i < DHD_MAX_IFS; i++) {
			if (dhd->iflist[i]) {
				dhd->iflist[i]->state = WLC_E_IF_DEL;
				dhd->iflist[i]->idx = i;
				dhd_op_if(dhd->iflist[i]);
			}
		}

		/*  delete primary interface 0 */
		ifp = dhd->iflist[0];
		ASSERT(ifp);
		if (ifp->net) {
			dhd_stop(ifp->net);
			ifmedia_removeall(&dhd->sc_ifmedia);
			ether_ifdetach(ifp->net);
			if_free(ifp->net);
			MFREE(dhd->pub.osh, ifp, sizeof(*ifp));

		}
	}

	if (dhd->dhd_state & DHD_ATTACH_STATE_PROT_ATTACH) {
		DHD_TRACE(("%s(): Calling dhd_bus_detach() \n", __FUNCTION__));
		dhd_bus_detach(dhdp);
		if (dhdp->prot) {
			DHD_TRACE(("%s(): Calling dhd_prot_detach() \n", __FUNCTION__));
			dhd_prot_detach(dhdp);
		}
	}

	if (dhdp->dbus) {
		DHD_TRACE(("%s(): Calling dbus_detach() \n", __FUNCTION__));
		dbus_detach(dhdp->dbus);
		dhdp->dbus = NULL;
	}

#ifdef NOT_YET
	if (dhd->dhd_state & DHD_ATTACH_STATE_WAKELOCKS_INIT) {
	/* && defined(CONFIG_PM_SLEEP) */
	}
#endif

	cv_destroy(&dhd->ioctl_resp_wait);

	mtx_destroy(&dhd->txqlock);
	mtx_destroy(&dhd->proto_sem);

#ifdef PROP_TXSTATUS
	mtx_destroy(&dhd->wlfc_mutex);
#endif

	DHD_TRACE(("%s(): Returns SUCCESS \n", __FUNCTION__));
	return;

}


static void
dhd_bus_detach(dhd_pub_t *dhdp)
{
	dhd_info_t *dhd;

	DHD_TRACE(("%s(): Enter\n", __FUNCTION__));

	if (dhdp) {
		dhd = (dhd_info_t *)dhdp->info;
		if (dhd) {
			/* Stop the protocol module */
			dhd_prot_stop(&dhd->pub);

			/* Force Dongle terminated */
			DHD_ERROR(("%s send WLC_TERMINATED\n", __FUNCTION__));
			if (dhd_wl_ioctl_cmd(dhdp, WLC_TERMINATED, NULL, 0, TRUE, 0) < 0)
				DHD_ERROR(("%s Setting WLC_TERMINATED failed\n",
					__FUNCTION__));

			/* Stop the bus module */
			DHD_TRACE(("%s(): Calling dbus_stop() \n", __FUNCTION__));
			dbus_stop(dhd->pub.dbus);
			dhd->pub.busstate = DHD_BUS_DOWN;
#if defined(OOB_INTR_ONLY)
			bcmsdh_unregister_oob_intr();
#endif /* defined(OOB_INTR_ONLY) */
		}
	}
}


static void
dhd_open(void *priv)
{
	dhd_info_t *dhd = (dhd_info_t *) priv;
	dhd_if_t *ifp;
	int ifidx;

	DHD_TRACE(("%s(): Started dhd = 0x%x \n", __FUNCTION__, (unsigned int)dhd));

	if (dhd == NULL) {
		DHD_ERROR(("%s(): ERROR..  dhd == NULL. Return -1 \n", __FUNCTION__));
		return;
	}

	atomic_set_int(&dhd->pend_8021x_cnt, 0);

	/* ifidx = dhd_net2idx2(dhd, net); */
	ifidx = 0;
	ifp = dhd->iflist[ifidx];
	DHD_TRACE(("%s: ifidx %d \n", __FUNCTION__, ifidx));
	if ((ifp) && (ifp->state == WLC_E_IF_DEL)) {
		DHD_ERROR(("%s(): ERROR..  IF already deleted. Return -1 \n", __FUNCTION__));
		return;
	}

	if (ifp->name[0] == '\0') {
		strncpy(ifp->name, ifp->net->if_xname, IFNAMSIZ);
	}

	/* Allow transmit calls */
	IFQ_SET_READY(&dhd->iflist[0]->net->if_snd);
	DHD_TRACE(("%s(): Seting pub.up to 1 \n", __FUNCTION__));
	dhd->pub.up = 1;

	return;
}


static int
dhd_up_down(struct ifnet *net, bool up)
{
	dhd_info_t *dhd = (dhd_info_t *)(net->if_softc);
	dhd_if_t *ifp;
#ifdef TOE
	uint32 toe_ol;
#endif
	int ifidx;
	int ret = BCME_OK;

	DHD_TRACE(("%s(): Started dhd = 0x%x \n", __FUNCTION__, (unsigned int)dhd));

	if (dhd == NULL) {
		DHD_ERROR(("%s(): ERROR..  dhd == NULL. Return -1 \n", __FUNCTION__));
		ret = BCME_BADARG;
		goto done;
	}

	ifidx = dhd_net2idx(dhd, net);
	ifp   = dhd->iflist[ifidx];
	DHD_TRACE(("%s: ifidx %d \n", __FUNCTION__, ifidx));
	if ((ifp) && (ifp->state == WLC_E_IF_DEL)) {
		DHD_ERROR(("%s(): ERROR..  IF already deleted. Return -1 \n", __FUNCTION__));
		ret = BCME_BADARG;
		goto done;
	}

	if (ifp->name[0] == '\0') {
		strncpy(ifp->name, ifp->net->if_xname, IFNAMSIZ);
	}

	if (ifidx == 0) { /* do it only for primary eth0 */

#ifdef TOE
		/* Get current TOE mode from dongle */
		if (dhd_toe_get(dhd, ifidx, &toe_ol) >= 0 && (toe_ol & TOE_TX_CSUM_OL) != 0) {
			/* dhd->iflist[ifidx]->net->features |= NETIF_F_IP_CSUM; */
			dhd->iflist[ifidx]->net->if_capenable |= IFCAP_TXCSUM;
			/* dhd->iflist[ifidx]->net->if_capenable |= IFCAP_HWCSUM; */
		} else {
			/* dhd->iflist[ifidx]->net->features &= ~NETIF_F_IP_CSUM; */
			dhd->iflist[ifidx]->net->if_capenable &= ~IFCAP_TXCSUM;
			/* dhd->iflist[ifidx]->net->if_capenable &= ~IFCAP_HWCSUM; */
		}
		dhd->iflist[ifidx]->net->if_capenable |= IFCAP_TOE4;
#endif
#if defined(DHD_NET80211)
		{
			wl_ioctl_t ioc;
			memset(&ioc, 0, sizeof(ioc));
			if (up == TRUE)
				ioc.cmd = WLC_UP;
			else
				ioc.cmd = WLC_DOWN;
			ioc.set = WL_IOCTL_ACTION_SET;

			DHD_TRACE(("%s(): Calling dhd_wl_ioctl() with cmd = %d \n",
				__FUNCTION__, ioc.cmd));
			ret = dhd_wl_ioctl(&dhd->pub, ifidx, &ioc, NULL, 0);
			osl_delay(3000);
			if (ret != BCME_OK) {
				DHD_ERROR(("%s(): ERROR... while doing WLC_UP/WLC_DOWN \n",
					__FUNCTION__));
				goto done;
			}
		}
#endif /* #if defined(DHD_NET80211) */

	}

done:
	return ret;
}

static int
dhd_stop(struct ifnet *net)
{
	dhd_info_t *dhd = (dhd_info_t *) net->if_softc;

	DHD_TRACE(("%s(): Srtarted. net = 0x%x \n", __FUNCTION__, (unsigned int)net));
	if (dhd->pub.up == 0) {
		return 0;
	}

	/* Set state and stop OS transmissions */
	dhd->pub.up = 0;
	if (net) {
		net->if_drv_flags &= ~IFF_DRV_RUNNING;
	}

#ifdef PROP_TXSTATUS
	dhd_wlfc_cleanup(&dhd->pub, NULL, 0);
#endif
	/* Stop the protocol module */
	dhd_prot_stop(&dhd->pub);
	return 0;
}

void
dhd_free(dhd_pub_t *dhdp)
{
	dhd_info_t *dhd;

	DHD_TRACE(("%s(): Enter\n", __FUNCTION__));

	if (dhdp) {
		dhd = (dhd_info_t *)dhdp->info;
		if (dhd) {
			MFREE(dhd->pub.osh, dhd, sizeof(*dhd));
		}
	}
	DHD_TRACE(("%s(): Returns \n", __FUNCTION__));
}

int
dhd_event_ifadd(dhd_info_t *dhd, wl_event_data_if_t *ifevent, char *name, uint8 *mac)
{
	int ifidx = ifevent->ifidx;
	int bssidx = ifevent->bssidx;
	struct net_device* ndev;

	BCM_REFERENCE(ndev);
#ifdef WL_CFG80211
	if (wl_cfg80211_notify_ifadd(ifidx, name, mac, bssidx) == BCME_OK)
		return BCME_OK;
#endif

	/* handle IF event caused by wl commands, SoftAP, WEXT and
	 * anything else
	 */
	ndev = dhd_handle_ifadd(dhd, ifidx, name, mac, bssidx, TRUE);

	return BCME_OK;
}

int
dhd_event_ifdel(dhd_info_t *dhd, wl_event_data_if_t *ifevent, char *name, uint8 *mac)
{
	int ifidx = ifevent->ifidx;

#ifdef WL_CFG80211
	if (wl_cfg80211_notify_ifdel(ifidx, name, mac, ifevent->bssidx) == BCME_OK)
		return BCME_OK;
#endif

	/* handle IF event caused by wl commands, SoftAP, WEXT and
	 * anything else
	 */
	dhd_handle_ifdel(dhd, ifidx, TRUE);

	return BCME_OK;
}

struct net_device*
dhd_handle_ifadd(dhd_info_t *dhd, int ifidx, char *name,
	uint8 *mac, uint8 bssidx, bool need_rtnl_lock)
{
	dhd_if_t *ifp;
	struct ifnet *ifnetp = NULL;
	int32 timeout =  2000; /* 2 secs */

	DHD_TRACE(("%s(): Calling if_alloc() \n", __FUNCTION__));
	if (!(ifnetp = if_alloc(IFT_ETHER))) {
		DHD_ERROR(("%s(): ERROR.. if_alloc() failed \n", __FUNCTION__));
		return NULL;
	}
	/* Save the dhd_info into the priv */
	ifnetp->if_softc = dhd;

	DHD_TRACE(("%s(): idx %d, ifnetp->%p\n", __FUNCTION__, ifidx, ifnetp));
	ASSERT(dhd && (ifidx < DHD_MAX_IFS));

	ifp = dhd->iflist[ifidx];
	if (ifp != NULL) {
		DHD_TRACE(("%s(): ERROR.. ifp not null, stop it\n", __FUNCTION__));
		if (ifp->net != NULL) {
			ifp->net->if_drv_flags &= ~(IFF_DRV_RUNNING | IFF_DRV_OACTIVE);
			if_detach(ifp->net);
			if_free(ifp->net);
		}
	} else {
		DHD_TRACE(("%s(): ifp null, allocate it\n", __FUNCTION__));
		if ((ifp = MALLOC(dhd->pub.osh, sizeof(dhd_if_t))) == NULL) {
			DHD_ERROR(("%s(): ERROR.. MALLOC() failed \n", __FUNCTION__));
			goto FAIL;
		}
		dhd->iflist[ifidx] = ifp;
	}
	memset(ifp, 0, sizeof(dhd_if_t));
	ifp->info = dhd;
	if (name)
		strncpy(ifp->name, name, IFNAMSIZ);
	ifp->name[IFNAMSIZ] = '\0';
	if (mac != NULL) {
		DHD_TRACE(("%s()(): set the mac_addr to ifp: %x:%x:%x:%x:%x:%x\n", __FUNCTION__,
			mac[0], mac[1], mac[2],
			mac[3], mac[4], mac[5]));
		memcpy(&ifp->mac_addr, mac, ETHER_ADDR_LEN);
	}

	if (ifnetp == NULL) {
		ifp->state = WLC_E_IF_ADD;
		ifp->idx = ifidx;

#ifdef NOT_YET
		/* Wait till ifnet pointer is populated */
		while ((ifp->net == NULL) && (timeout > 0)) {
			/* timeout = schedule_timeout(timeout); */
		}
#endif
		if (!timeout) {
			DHD_ERROR(("%s(): net_dev alloc failed for idx %d \n",
				__FUNCTION__, ifidx));
			goto FAIL;
		}
	} else {
		ifp->net = ifnetp;
	}

	return ifp->net;

FAIL:
	if (ifnetp)
		if_free(ifnetp);

	return NULL;
}

int
dhd_handle_ifdel(dhd_info_t *dhd, int ifidx, bool need_rtnl_lock)
{
	dhd_if_t *ifp;

	DHD_TRACE(("%s(): idx %d\n", __FUNCTION__, ifidx));

	ASSERT(dhd && ifidx && (ifidx < DHD_MAX_IFS));
	ifp = dhd->iflist[ifidx];
	if (!ifp) {
		DHD_ERROR(("%s(): ERROR.. Null interface\n", __FUNCTION__));
		return BCME_OK;
	}

	ifp->state = WLC_E_IF_DEL;
	ifp->idx = ifidx;
	return BCME_OK;
}

static int
dhd_ioctl_entry(struct ifnet *net, u_long cmd, caddr_t data)
{
	dhd_info_t *dhd = (dhd_info_t *)net->if_softc;
	/* dhd_ioctl_t dhd_ioc; */
	int bcmerror = BCME_OK;
	int buflen = 0;
	uint driver = 0;
	int ifidx;
	int error = 0;
	struct ifreq *ifr = (struct ifreq *) data;

	DHD_OS_WAKE_LOCK(&dhd->pub);

	ifidx = dhd_net2idx(dhd, net);
	DHD_TRACE(("%s(): Started... ifidx %d, cmd 0x%lx \n", __FUNCTION__, ifidx, cmd));

	if (ifidx == DHD_BAD_IF) {
		DHD_TRACE(("%s: ERROR: ifidx %d, get DHD_BAD_IF\n", __FUNCTION__, ifidx));
		DHD_OS_WAKE_UNLOCK(&dhd->pub);
		return (OSL_ERROR(BCME_NODEVICE));
	}

	/* memset(&dhd_ioc, 0, sizeof(dhd_ioctl_t)); */

	switch (cmd) {
		case SIOCSIFFLAGS:
			DHD_TRACE(("%s(): SIOCSIFFLAGS \n", __FUNCTION__));
			/* MUTEX_LOCK(sc); */
			if (net->if_flags & IFF_UP) {
				DHD_TRACE(("%s(): (if_flags & IFF_UP) = TRUE \n", __FUNCTION__));
				if ((net->if_drv_flags & IFF_DRV_RUNNING) == 0) {
					error = dhd_up_down(net, TRUE);
					net->if_drv_flags &= ~IFF_DRV_OACTIVE;
					net->if_drv_flags |= IFF_DRV_RUNNING;
				}
			} else {
				DHD_TRACE(("%s(): (if_flags & IFF_UP) = FALSE \n", __FUNCTION__));
				if (net->if_drv_flags & IFF_DRV_RUNNING) {
					error = dhd_up_down(net, FALSE);
					net->if_drv_flags &= ~IFF_DRV_RUNNING;
				}
			}
			/* MUTEX_UNLOCK(sc); */
			break;

		case SIOCGIFMEDIA:
			DHD_TRACE(("%s(): SIOCGIFMEDIA: ifr->ifm_count = %d \n",
				__FUNCTION__, ((struct ifmediareq *)ifr)->ifm_count));
			error = ifmedia_ioctl(net, ifr, &dhd->sc_ifmedia, cmd);
			DHD_TRACE(("%s(): New ifr->ifm_count = %d \n", 	__FUNCTION__,
				((struct ifmediareq *)ifr)->ifm_count));
			break;

		case SIOCSIFMEDIA:
			DHD_TRACE(("%s(): SIOCSIFMEDIA: ifr->ifm_count = %d \n",
				__FUNCTION__, ((struct ifmediareq *)ifr)->ifm_count));
			error = ifmedia_ioctl(net, ifr, &dhd->sc_ifmedia, cmd);
			DHD_TRACE(("%s(): New ifr->ifm_count = %d \n", __FUNCTION__,
				((struct ifmediareq *)ifr)->ifm_count));
			break;

#ifdef NOT_YET
		case SIOCGIFADDR:
			DHD_TRACE(("%s(): SIOCGIFADDR \n", __FUNCTION__));
			error = ifmedia_ioctl(net, ifr, &dhd->sc_ifmedia, cmd);
			break;
#endif
		case SIOCDEVPRIVATE:
		{
			wl_ioctl_t wl_ioc;
			void *buf = NULL;

			DHD_TRACE(("%s(): SIOCDEVPRIVATE \n", __FUNCTION__));
			if (copy_from_user (&wl_ioc, ifr->ifr_data, sizeof(wl_ioctl_t))) {
				DHD_ERROR(("%s(): ERROR.. copy_from_user() error \n",
					__FUNCTION__));
				bcmerror = BCME_BADADDR;
				goto done;
			}
			if (wl_ioc.buf) {
				buflen = MIN(wl_ioc.len, DHD_IOCTL_MAXLEN);
				if (!(buf = (void *) MALLOC(dhd->pub.osh, buflen))) {
					DHD_ERROR(("%s(): ERROR.. MALLOC() failed \n",
						__FUNCTION__));
					bcmerror = BCME_NOMEM;
					goto done;
				}
				if (copy_from_user (buf, wl_ioc.buf, buflen)) {
					DHD_ERROR(("%s(): ERROR.. copy_from_user() error \n",
						__FUNCTION__));
					bcmerror = BCME_BADADDR;
					goto done;
				}
			}

			/* To differentiate between wl and dhd read 4 more byes */
			if ((copy_from_user(&driver, (char *)ifr->ifr_data + sizeof(wl_ioctl_t),
				sizeof(uint)) != 0)) {
				DHD_ERROR(("%s(): ERROR.. copy_from_user() error \n",
					__FUNCTION__));
				bcmerror = BCME_BADADDR;
				goto done;
			}


			/* check for local dhd ioctl and handle it */
			if (driver == DHD_IOCTL_MAGIC) {
				DHD_TRACE(("%s(): Calling dhd_ioctl() \n",
					__FUNCTION__));
				bcmerror = dhd_ioctl(&dhd->pub,
					(dhd_ioctl_t *)(&wl_ioc), buf, buflen);
				if (bcmerror)
					dhd->pub.bcmerror = bcmerror;
				goto done;
			}

			/*
			 * Flush the TX queue if required for proper message serialization:
			 * Intercept WLC_SET_KEY IOCTL - serialize M4 send and set key IOCTL to
			 * prevent M4 encryption and intercept WLC_DISASSOC IOCTL - serialize
			 * WPS-DONE and WLC_DISASSOC IOCTL to prevent disassoc frame being
			 * sent before WPS-DONE frame.
			 */
			if (wl_ioc.cmd == WLC_SET_KEY ||
				(wl_ioc.cmd == WLC_SET_VAR && wl_ioc.buf != NULL &&
				strncmp("wsec_key", wl_ioc.buf, 9) == 0) ||
				(wl_ioc.cmd == WLC_SET_VAR && wl_ioc.buf != NULL &&
				strncmp("bsscfg:wsec_key", wl_ioc.buf, 15) == 0) ||
				(wl_ioc.cmd == WLC_DISASSOC)) {
				dhd_wait_pend8021x(net);
			}

			DHD_TRACE(("%s(): Calling dhd_wl_ioctl() with cmd = %d, buflen = %d \n",
				__FUNCTION__, wl_ioc.cmd, buflen));
			bcmerror = dhd_wl_ioctl(&dhd->pub, ifidx,
				(wl_ioctl_t *)&wl_ioc, buf, buflen);

done:
			if (!bcmerror && buf && wl_ioc.buf) {
				if (copy_to_user(wl_ioc.buf, buf, buflen))
					bcmerror = -EFAULT;
			}

			error = OSL_ERROR(bcmerror);

			if (buf) {
				MFREE(dhd->pub.osh, buf, buflen);
			}
			break;
		}

#ifdef DHD_NET80211
		case SIOCG80211:
			DHD_TRACE(("%s(): Calling dhd_net80211_ioctl_get() \n", __FUNCTION__));
			bcmerror = dhd_net80211_ioctl_get(&dhd->pub, net, cmd, data);
			error = OSL_ERROR(bcmerror);
			break;

		case SIOCS80211:
			DHD_TRACE(("%s(): Calling dhd_net80211_ioctl_set() \n", __FUNCTION__));
			bcmerror = dhd_net80211_ioctl_set(&dhd->pub, net, cmd, data);
			error = OSL_ERROR(bcmerror);
			break;

		case SIOCG80211STATS:
			DHD_TRACE(("%s(): Calling dhd_net80211_ioctl_stats() \n", __FUNCTION__));
			bcmerror = dhd_net80211_ioctl_stats(&dhd->pub, net, cmd, data);
			error = OSL_ERROR(bcmerror);
			break;
#endif  /* #if defined(DHD_NET80211) */

		case SIOCSIFADDR:
		case SIOCGIFADDR:
		case SIOCSIFMTU:
			DHD_TRACE(("%s(): Calling ether_ioctl() \n", __FUNCTION__));
			error = ether_ioctl(net, cmd, data);
			break;

		default:
			DHD_TRACE(("%s(): Unknown Command \n", __FUNCTION__));
			error = OSL_ERROR(BCME_UNSUPPORTED);
			break;

	}

	DHD_OS_WAKE_UNLOCK(&dhd->pub);
	return (error);
}


int
dhd_preinit_ioctls(dhd_pub_t *dhd)
{
	int ret = 0;
	char eventmask[WL_EVENTING_MASK_LEN];
	char iovbuf[WL_EVENTING_MASK_LEN + 12]; /*  Room for "event_msgs" + '\0' + bitvec  */
	char buf[WLC_IOCTL_SMLEN];

	DHD_TRACE(("%s(): Srtarted. \n", __FUNCTION__));

	/* Get the default device MAC address directly from firmware */
	memset(buf, 0, sizeof(buf));
	bcm_mkiovar("cur_etheraddr", 0, 0, buf, sizeof(buf));
	ret = dhd_wl_ioctl_cmd(dhd, WLC_GET_VAR, buf, sizeof(buf), FALSE, 0);
	if (ret < 0) {
		DHD_ERROR(("%s: can't get MAC address , error=%d\n", __FUNCTION__, ret));
		return BCME_NOTUP;
	}
	/* Update public MAC address after reading from Firmware */
	memcpy(dhd->mac.octet, buf, ETHER_ADDR_LEN);

	DHD_INFO(("%s(): mac=%.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n", __FUNCTION__,
		dhd->mac.octet[0], dhd->mac.octet[1], dhd->mac.octet[2],
		dhd->mac.octet[3], dhd->mac.octet[4], dhd->mac.octet[5]));

	/* Read event_msgs mask */
	bcm_mkiovar("event_msgs", eventmask, WL_EVENTING_MASK_LEN, iovbuf, sizeof(iovbuf));
	ret  = dhd_wl_ioctl_cmd(dhd, WLC_GET_VAR, iovbuf, sizeof(iovbuf), WL_IOCTL_ACTION_GET, 0);
	if (ret < 0) {
		DHD_ERROR(("%s(): read Event mask failed %d\n", __FUNCTION__, ret));
		goto done;
	}
	bcopy(iovbuf, eventmask, WL_EVENTING_MASK_LEN);

	/* Setup event_msgs */
	setbit(eventmask, WLC_E_SET_SSID);
	setbit(eventmask, WLC_E_PRUNE);
	setbit(eventmask, WLC_E_AUTH);
	setbit(eventmask, WLC_E_REASSOC);
	setbit(eventmask, WLC_E_REASSOC_IND);
	setbit(eventmask, WLC_E_DEAUTH);
	setbit(eventmask, WLC_E_DEAUTH_IND);
	setbit(eventmask, WLC_E_DISASSOC_IND);
	setbit(eventmask, WLC_E_DISASSOC);
	setbit(eventmask, WLC_E_JOIN);
	setbit(eventmask, WLC_E_ASSOC_IND);
	setbit(eventmask, WLC_E_PSK_SUP);
	setbit(eventmask, WLC_E_LINK);
	setbit(eventmask, WLC_E_NDIS_LINK);
	setbit(eventmask, WLC_E_MIC_ERROR);
	setbit(eventmask, WLC_E_ASSOC_REQ_IE);
	setbit(eventmask, WLC_E_ASSOC_RESP_IE);
	setbit(eventmask, WLC_E_PMKID_CACHE);
	setbit(eventmask, WLC_E_TXFAIL);
	setbit(eventmask, WLC_E_JOIN_START);
	setbit(eventmask, WLC_E_SCAN_COMPLETE);
#ifdef WLMEDIA_HTSF
	setbit(eventmask, WLC_E_HTSFSYNC);
#endif /* WLMEDIA_HTSF */
#ifdef PNO_SUPPORT
	setbit(eventmask, WLC_E_PFN_NET_FOUND);
#endif /* PNO_SUPPORT */
	/* enable dongle roaming event */

#ifdef DHD_NET80211
	setbit(eventmask, WLC_E_ESCAN_RESULT);
	if ((dhd->op_mode & WFD_MASK) == WFD_MASK) {
		setbit(eventmask, WLC_E_ACTION_FRAME_RX);
		setbit(eventmask, WLC_E_ACTION_FRAME_COMPLETE);
		setbit(eventmask, WLC_E_ACTION_FRAME_OFF_CHAN_COMPLETE);
		setbit(eventmask, WLC_E_P2P_PROBREQ_MSG);
		setbit(eventmask, WLC_E_P2P_DISC_LISTEN_COMPLETE);
	}
#endif /* DHD_NET80211 */

	/* Write updated Event mask */
	bcm_mkiovar("event_msgs", eventmask, WL_EVENTING_MASK_LEN, iovbuf, sizeof(iovbuf));
	ret = dhd_wl_ioctl_cmd(dhd, WLC_SET_VAR, iovbuf, sizeof(iovbuf), WL_IOCTL_ACTION_SET, 0);
	if (ret < 0) {
		DHD_ERROR(("%s(): Set Event mask failed %d\n", __FUNCTION__, ret));
		goto done;
	}

#ifdef PROP_TXSTATUS
	dhd_wlfc_init(dhd);
#endif

done:
	return ret;
}


static void
dhd_start_xmit(struct ifnet *net)
{
	dhd_info_t *dhd = net->if_softc;
	struct mbuf *m;
	int ifidx;

	DHD_TRACE(("%s(): Started \n", __FUNCTION__));

	DHD_OS_WAKE_LOCK(&dhd->pub);

	if (!dhd->pub.up || (dhd->pub.busstate == DHD_BUS_DOWN)) {
		DHD_ERROR(("%s(): xmit rejected pub.up=%d busstate=%d \n",
			__FUNCTION__, dhd->pub.up, dhd->pub.busstate));
		goto xmit_done;
	}

	/* Net if status check  */
	if ((net->if_flags & (IFF_DRV_RUNNING|IFF_UP)) == 0) {
		DHD_ERROR(("%s(): Interface not up !!\n", __FUNCTION__));
		goto xmit_done;
	}

	ifidx = dhd_net2idx(dhd, net);
	if (ifidx == DHD_BAD_IF) {
		DHD_ERROR(("%s(): Bad Interface. ifidx = 0x%x !! \n", __FUNCTION__, ifidx));
		goto xmit_done;
	}

	dhd_os_sdlock_txq(&dhd->pub);

	if ((net->if_drv_flags & (IFF_DRV_RUNNING|IFF_DRV_OACTIVE)) !=
		IFF_DRV_RUNNING) {
		dhd_os_sdunlock_txq(&dhd->pub);
		goto xmit_done;
	}

	while (!IFQ_DRV_IS_EMPTY(&net->if_snd)) {

		IFQ_DRV_DEQUEUE(&net->if_snd, m);
		if (m == NULL) {
			/* DHD_ERROR(("%s(): ERROR.. m is NULL !! \n", __FUNCTION__)); */
			break;
		}
		bwl_tx_data(&dhd->pub, ifidx, m);
	}

	dhd_os_sdunlock_txq(&dhd->pub);

xmit_done:
	/* dhd->pub.dstats.tx_dropped++; */
	dhd->pub.tx_packets++;

	DHD_OS_WAKE_UNLOCK(&dhd->pub);

	DHD_TRACE(("%s(): finished \n", __FUNCTION__));

	return;
}


static void
dhd_dbus_disconnect_cb(void *arg)
{
	dhd_info_t *dhd = (dhd_info_t *)arg;
	dhd_pub_t *pub;
	osl_t *osh;

	DHD_TRACE(("%s(): Enter\n", __FUNCTION__));

	if (dhd == NULL) {
		DHD_ERROR(("%s(): ERROR.. arg is NULL. Returning \n", __FUNCTION__));
		return;
	}

	pub = &dhd->pub;
	osh = pub->osh;

#ifdef DHD_NET80211
	DHD_TRACE(("%s(): Calling dhd_net80211_detach() \n", __FUNCTION__));
	dhd_net80211_detach(pub);
#endif

	DHD_TRACE(("%s(): Calling dhd_detach() \n", __FUNCTION__));
	dhd_detach(pub);
	DHD_TRACE(("%s(): Calling dhd_free() \n", __FUNCTION__));
	dhd_free(pub);

#ifdef NOT_YET
	if (MALLOCED(osh)) {
		DHD_ERROR(("%s(): MEMORY LEAK %d bytes\n", __FUNCTION__, MALLOCED(osh)));
	}
#endif
	DHD_TRACE(("%s(): Calling osl_detach() \n", __FUNCTION__));
	osl_detach(osh);

	DHD_TRACE(("%s(): Returns \n", __FUNCTION__));
	return;

}


static void *
dhd_dbus_probe_cb(void *arg, const char *desc, uint32 bustype, uint32 hdrlen)
{
	osl_t *osh = NULL;
	dbus_attrib_t attrib;
	dhd_pub_t *pub = NULL;
	int ret;

	DHD_TRACE(("%s(): Enter\n", __FUNCTION__));

	/* Ask the OS interface part for an OSL handle */
	DHD_TRACE(("%s(): Calling dhd_osl_attach() \n", __FUNCTION__));
	osh = osl_attach(NULL, "dhd", 0, 0, NULL);
	if (!osh) {
		DHD_ERROR(("%s(): ERROR.. osl_attach() failed\n", __FUNCTION__));
		goto fail;
	}

	/* Attach to the dhd/OS interface */
	DHD_TRACE(("%s(): Calling dhd_attach() \n", __FUNCTION__));
	pub = dhd_attach(osh, NULL /* bus */, hdrlen);
	if (!pub) {
		DHD_ERROR(("%s(): ERROR.. dhd_attach() failed\n", __FUNCTION__));
		goto fail;
	}

	/* TODO: Set the NRXQ to 1, increase it later */
	if (pub->rxsz == 0)
		pub->rxsz = BWL_RX_BUFFER_SIZE;

	DHD_TRACE(("%s(): Calling dbus_attach() \n", __FUNCTION__));
	pub->dbus = dbus_attach(osh, pub->rxsz, BWL_RX_LIST_COUNT, BWL_TX_LIST_COUNT,
		pub->info, &dhd_dbus_cbs, NULL, NULL);
	if (pub->dbus) {
		DHD_TRACE(("%s(): Calling dbus_get_attrib() \n", __FUNCTION__));
		dbus_get_attrib(pub->dbus, &attrib);
		DHD_INFO(("%s(): DBUS: vid=0x%x pid=0x%x devid=0x%x bustype=0x%x mtu=%d \n",
			__FUNCTION__, attrib.vid, attrib.pid, attrib.devid,
			attrib.bustype, attrib.mtu));
	} else {
		DHD_ERROR(("%s(): dbus_attach failed \n", __FUNCTION__));
		goto fail;
	}

	if (pub->busstate != DHD_BUS_DATA) {
		DHD_TRACE(("%s(): Calling dbus_up() \n", __FUNCTION__));
		ret = dbus_up(pub->dbus);
		if (ret != BCME_OK) {
			DHD_ERROR(("%s(): ERROR..  dbus_up() return %d \n", __FUNCTION__, ret));
			goto fail;
		}
		pub->busstate = DHD_BUS_DATA;
	}

	/* Bus is ready, query any dongle information */
	DHD_TRACE(("%s(): Calling dhd_sync_with_dongle() \n", __FUNCTION__));
	ret = dhd_sync_with_dongle(pub);
	if (ret != BCME_OK) {
		DHD_ERROR(("%s(): ERROR. dhd_sync_with_dongle() return %d \n", __FUNCTION__, ret));
		goto fail;
	}

	/* Attach to the OS network interface */
	DHD_TRACE(("%s(): Calling dhd_register_if() \n", __FUNCTION__));
	ret = dhd_register_if(pub, 0, TRUE);
	if (ret) {
		DHD_ERROR(("%s(): ERROR.. dhd_register_if() failed\n", __FUNCTION__));
		goto fail;
	}

	DHD_TRACE(("%s(): Returns SUCCESS \n", __FUNCTION__));
	/* This is passed to dhd_dbus_disconnect_cb */
	return (pub->info);

fail:
	DHD_ERROR(("%s(): ERROR.. Doing cleanup \n", __FUNCTION__));
	/* Release resources in reverse order */
	if (osh) {
		if (pub) {
			dhd_detach(pub);
			dhd_free(pub);
		}
		/* dhd_osl_detach(osh); */
	}
	DHD_ERROR(("%s(): ERROR.. cleanup done\n", __FUNCTION__));
	return NULL;
}

void
dhd_module_cleanup(void)
{
	DHD_TRACE(("%s(): Enter\n", __FUNCTION__));
	dbus_deregister();
	DHD_TRACE(("%s(): Exit \n", __FUNCTION__));
	return;
}

void
dhd_module_init(void)
{
	int error;

	DHD_TRACE(("%s(): Enter\n", __FUNCTION__));

	TUNABLE_INT("dhd_vid", &dhd_vid);
	TUNABLE_INT("dhd_pid", &dhd_pid);

	error = dbus_register(dhd_vid, dhd_pid, dhd_dbus_probe_cb,
		dhd_dbus_disconnect_cb, NULL /* arg */, NULL, NULL);
	if (error == DBUS_OK) {
		printf("%s \n", dhd_version);
	}
	else {
		printf("%s(): ERROR..  dbus_register() returns 0x%x \n", __FUNCTION__, error);
	}
	DHD_TRACE(("%s(): Exit \n", __FUNCTION__));
	return;
}

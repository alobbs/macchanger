/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/* MAC Changer
 *
 * Authors:
 *      Alvaro Lopez Ortega <alvaro@alobbs.com>
 *
 * Copyright (C) 2002,2015 Alvaro Lopez Ortega
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <linux/ethtool.h>
#include <linux/sockios.h>

#include "netinfo.h"

ret_t
mc_net_info_new (net_info_t **net, const char *device)
{
    int re;
    CHULA_GEN_NEW_STRUCT (net,n,info);

	n->sock = socket (AF_INET, SOCK_DGRAM, 0);
	if (n->sock<0) {
		perror ("[ERROR] Socket");
        goto error;
	}

	strncpy (n->dev.ifr_name, device, sizeof(n->dev.ifr_name));
	n->dev.ifr_name[sizeof(n->dev.ifr_name)-1] = '\0';

    re = ioctl(n->sock, SIOCGIFHWADDR, &n->dev);
	if (re < 0) {
		perror ("[ERROR] Set device name");
        goto error;
	}

    *net = n;
    return ret_ok;

 error:
    free (n);
    return ret_error;
}


void
mc_net_info_free (net_info_t *net)
{
	close(net->sock);
	free(net);
}


ret_t
mc_net_info_set_mac (net_info_t *net, mac_t *mac)
{
	int re;

	for (int i=0; i<6; i++) {
		net->dev.ifr_hwaddr.sa_data[i] = mac->byte[i];
	}

    re = ioctl(net->sock, SIOCSIFHWADDR, &net->dev);
	if (re < 0) {
		perror ("[ERROR] Could not change MAC: interface up or insufficient permissions");
		return ret_error;
	}

	return ret_ok;
}


ret_t
mc_net_info_get_mac (net_info_t *net, mac_t **mac)
{
	*mac = (mac_t *) calloc (1, sizeof(mac_t));
    if (unlikely (*mac == NULL)) return ret_nomem;

	for (int i=0; i<6; i++) {
		(*mac)->byte[i] = net->dev.ifr_hwaddr.sa_data[i] & 0xFF;
	}

	return ret_ok;
}


ret_t
mc_net_info_get_perm_mac (net_info_t *net, mac_t **mac)
{
	int                       re;
	struct ifreq              req;
	struct ethtool_perm_addr *epa;

    /* Memory allocation */
    epa = (struct ethtool_perm_addr *) calloc(1, sizeof(struct ethtool_perm_addr) + IFHWADDRLEN);
    if (unlikely (epa == NULL)) {
        return ret_nomem;
    }

	*mac = (mac_t *) calloc (1, sizeof(mac_t));
    if (unlikely (*mac == NULL)) {
        free (epa);
        return ret_nomem;
    }

    /* Data structures set up */
	epa->cmd  = ETHTOOL_GPERMADDR;
	epa->size = IFHWADDRLEN;

	memcpy (&req, &net->dev, sizeof(struct ifreq));
	req.ifr_data = (caddr_t)epa;

    /* Syscall */
    re = ioctl (net->sock, SIOCETHTOOL, &req);
	if (re < 0) {
		perror ("[ERROR] Could not read permanent MAC");
        goto error;
	}

    /* Copy MAC address */
    for (int i=0; i<6; i++) {
        (*mac)->byte[i] = epa->data[i];
    }

	free(epa);
	return ret_ok;

 error:
    free (epa);
    return ret_error;
}

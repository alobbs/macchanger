/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/* MAC Changer
 *
 * Authors:
 *      Alvaro Lopez Ortega <alvaro@alobbs.com>
 *
 * Copyright (C) 2002 Alvaro Lopez Ortega
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

#include "netinfo.h"


net_info_t *
mc_net_info_new (const char *device)
{
	net_info_t *new = (net_info_t *) malloc (sizeof(net_info_t));

	new->sock = socket (AF_INET, SOCK_DGRAM, 0);
	if (new->sock<0) {
		perror ("socket");
		free(new);
		return NULL;
	}

	strcpy (new->dev.ifr_name, device);
	if (ioctl(new->sock, SIOCGIFHWADDR, &new->dev) < 0) {
		perror ("set device name");
		free(new);
		return NULL;
	}
	   
	return new;
}


void
mc_net_info_free (net_info_t *net)
{
	close(net->sock);
	free(net);
}


mac_t *
mc_net_info_get_mac (const net_info_t *net)
{
	int i;
	mac_t *new = (mac_t *) malloc (sizeof(mac_t));

	for (i=0; i<6; i++) {
		new->byte[i] = net->dev.ifr_hwaddr.sa_data[i] & 0xFF;
	}
	   
	return new;
}


int
mc_net_info_set_mac (net_info_t *net, const mac_t *mac)
{
	int i;

	for (i=0; i<6; i++) {
		net->dev.ifr_hwaddr.sa_data[i] = mac->byte[i];
	}
	
	if (ioctl(net->sock, SIOCSIFHWADDR, &net->dev) < 0) {
		perror ("ERROR: Can't change MAC: interface up or not permission");
		return -1;
	}
	
	return 0;
}

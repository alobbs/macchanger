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

#ifndef __MAC_CHANGER_NETINFO_H__
#define __MAC_CHANGER_NETINFO_H__

#include <net/if.h>
#include "mac.h"


typedef struct {
	   int sock;
	   struct ifreq dev;
} net_info_t;



net_info_t *mc_net_info_new     (const char *device);
void        mc_net_info_free    (net_info_t *);

mac_t      *mc_net_info_get_mac (const net_info_t *);
int         mc_net_info_set_mac (net_info_t *, const mac_t *);


#endif /* __MAC_CHANGER_NETINFO_H__ */

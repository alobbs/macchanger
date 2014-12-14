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

#ifndef MAC_CHANGER_NETINFO_H
#define MAC_CHANGER_NETINFO_H

#include <libchula/libchula.h>

#include <sys/socket.h>
#include <net/if.h>
#include "mac.h"

typedef struct {
    int          sock;
    struct ifreq dev;
} net_info_t;

ret_t mc_net_info_new  (net_info_t **net, const char *device);
void  mc_net_info_free (net_info_t  *net);

ret_t mc_net_info_set_mac      (net_info_t *net, mac_t  *mac);
ret_t mc_net_info_get_mac      (net_info_t *net, mac_t **mac);
ret_t mc_net_info_get_perm_mac (net_info_t *net, mac_t **mac);

#endif /* MAC_CHANGER_NETINFO_H */

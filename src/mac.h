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

#ifndef MAC_CHANGER_MAC_H
#define MAC_CHANGER_MAC_H

#include <libchula/libchula.h>

typedef struct {
	unsigned char byte[6];
} mac_t;

typedef enum {
	mac_is_anykind,
	mac_is_wireless,
	mac_is_others
} mac_type_t;

ret_t mc_mac_dup    (mac_t *mac, mac_t **copy);
void  mc_mac_free   (mac_t *mac);

ret_t mc_mac_read   (mac_t *mac, chula_buffer_t *buf);
ret_t mc_mac_to_buf (mac_t *mac, chula_buffer_t *buf);

bool  mc_mac_equal  (mac_t *mac, mac_t *other);
void  mc_mac_random (mac_t *mac, unsigned char last_n_bytes, char set_bia);

#endif /* MAC_CHANGER_MAC_H */

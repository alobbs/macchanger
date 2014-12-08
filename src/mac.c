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

#include "mac.h"


ret_t
mc_mac_dup (mac_t *src, mac_t **copy)
{
	*copy = (mac_t *)malloc(sizeof(mac_t));
    if (unlikely (*copy == NULL)) return ret_nomem;

    memcpy (*copy, src, sizeof(mac_t));
    return ret_ok;
}


void
mc_mac_free (mac_t *mac)
{
	free (mac);
}


ret_t
mc_mac_to_buf (mac_t          *mac,
               chula_buffer_t *buf)
{
    ret_t ret;

	for (int i=0; i<6; i++) {
        ret = chula_buffer_add_va (buf, "%02x%s", mac->byte[i], i<5?":":"");
        if (unlikely (ret != ret_ok)) return ret;
	}

    return ret_ok;
}


void
mc_mac_random (mac_t *mac, unsigned char last_n_bytes, char set_bia)
{
	/* The LSB of first octet can not be set.  Those are musticast
	 * MAC addresses and not allowed for network device:
	 * x1:, x3:, x5:, x7:, x9:, xB:, xD: and xF:
	 */

	switch (last_n_bytes) {
	case 6:
		/* 8th bit: Unicast / Multicast address
		 * 7th bit: BIA (burned-in-address) / locally-administered
		 */
		mac->byte[0] = (random()%255) & 0xFC;
		mac->byte[1] = random()%255;
		mac->byte[2] = random()%255;
	case 3:
		mac->byte[3] = random()%255;
		mac->byte[4] = random()%255;
		mac->byte[5] = random()%255;
	}

	/* Handle the burned-in-address bit
	 */
	if (set_bia) {
		mac->byte[0] &= ~2;
	} else {
		mac->byte[0] |= 2;
	}
}


bool
mc_mac_equal (mac_t *mac1, mac_t *mac2)
{
	for (int i=0; i<6; i++) {
		if (mac1->byte[i] != mac2->byte[i]) {
			return false;
		}
	}

	return true;
}


ret_t
mc_mac_read (mac_t          *mac,
             chula_buffer_t *buf)
{
    ret_t ret;
	int   nbyte = 5;

	/* Check the format */
	if (buf->len != 17) {
		fprintf (stderr, "[ERROR] Incorrect format: MAC length should be 17. %s(%u)\n", buf->buf, buf->len);
        return ret_error;
	}

	for (nbyte=2; nbyte<16; nbyte+=3) {
		if (buf->buf[nbyte] != ':') {
			fprintf (stderr, "[ERROR] Incorrect format: %s\n", buf->buf);
            return ret_error;
		}
	}

	/* Read the values */
	for (nbyte=0; nbyte<6; nbyte++) {
        long val = 0;

        ret = chula_ahextol((const char*)&buf->buf[nbyte*3], &val);
        if (unlikely (ret != ret_ok)) return ret;

        mac->byte[nbyte] = (char)(val & 0xFF);
	}

    return ret_ok;
}

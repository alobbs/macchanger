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

#ifndef MAC_CHANGER_LIST_H
#define MAC_CHANGER_LIST_H

#include <libchula/libchula.h>

#include "mac.h"

typedef struct {
    chula_list_t   list;
    chula_buffer_t name;
	unsigned char  byte[3];
} mac_list_item_t;

#define LIST_MAC_ITEM(l) ((mac_list_item_t *)(l))
#define CARD_NAME(x)     (mc_maclist_get_cardname_with_default(x, "unknown"))

/* Global init */
ret_t mc_maclists_init     (void);
ret_t mc_maclists_mrproper (void);

/* MAC List */
const char *mc_maclist_get_cardname_with_default (mac_t *mac, const char *);
void        mc_maclist_set_random_vendor         (mac_t *mac, mac_type_t);
bool        mc_maclist_is_wireless               (mac_t *mac);
void        mc_maclist_print                     (const char *keyword);

#endif /* MAC_CHANGER_LIST_H */

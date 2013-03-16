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

#ifndef __MAC_CHANGER_LIST_H__
#define __MAC_CHANGER_LIST_H__

#include "mac.h"

typedef struct {
	char  *name;
	unsigned char byte[3];
} card_mac_list_item_t;

#define LIST_LENGHT(l)   ((sizeof(l) / sizeof(card_mac_list_item_t))-1)
#define CARD_NAME(x)     mc_maclist_get_cardname_with_default(x, "unknown")

int    mc_maclist_init  (void);
void   mc_maclist_free  (void);

const char * mc_maclist_get_cardname_with_default (const mac_t *, const char *);
void         mc_maclist_set_random_vendor         (mac_t *, mac_type_t);
int          mc_maclist_is_wireless               (const mac_t *);
void         mc_maclist_print                     (const char *keyword);

#endif /* __MAC_CHANGER_LIST_H__ */

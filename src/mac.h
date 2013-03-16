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

#ifndef __MAC_CHANGER_MAC_H__
#define __MAC_CHANGER_MAC_H__


typedef struct {
	unsigned char byte[6];
} mac_t;

typedef enum {
	mac_is_anykind,
	mac_is_wireless,
	mac_is_others
} mac_type_t;



int     mc_mac_read_string (mac_t *, char *);
void    mc_mac_into_string (const mac_t *, char *);

int     mc_mac_equal       (const mac_t *, const mac_t *);
mac_t  *mc_mac_dup         (const mac_t *);
void    mc_mac_free        (mac_t *);
void    mc_mac_random      (mac_t *, unsigned char last_n_bytes);
void    mc_mac_next        (mac_t *);

#endif /* __MAC_CHANGER_LISTA_H__ */

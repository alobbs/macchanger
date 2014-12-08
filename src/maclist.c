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

#include "maclist.h"

chula_list_t list_others;   /* IEEE OUI */
chula_list_t list_wireless; /* Wireless cards */

/* List entry
 */
static ret_t
list_item_new (mac_list_item_t **mac_entry)
{
    CHULA_GEN_NEW_STRUCT (mac,n,list_item);

    chula_list_init (&n->list);
    chula_buffer_init (&n->name);
    memset (&n->byte, 0, 3);

    *mac_entry = n;
    return ret_ok;
}

static ret_t
list_item_free (mac_list_item_t *mac_entry)
{
    chula_buffer_mrproper (&mac_entry->name);
    free (mac_entry);
    return ret_ok;
}

static chula_buffer_t *
get_mac_name (mac_t *mac_in, chula_list_t *list)
{
    chula_list_t *i;

    list_for_each (i,list) {
        mac_list_item_t *mac = (mac_list_item_t *)i;

		if ((mac_in->byte[0] == mac->byte[0]) &&
		    (mac_in->byte[1] == mac->byte[1]) &&
		    (mac_in->byte[2] == mac->byte[2])) {
			return &mac->name;
		}
    }

    return NULL;
}

static chula_buffer_t *
mc_maclist_get_cardname (mac_t *mac)
{
	chula_buffer_t *name;

	name = get_mac_name (mac, &list_wireless);
	if (name) {
		return name;
	}

	return get_mac_name (mac, &list_others);
}


const char *
mc_maclist_get_cardname_with_default (mac_t *mac, const char *def)
{
	chula_buffer_t *name;
	name = mc_maclist_get_cardname(mac);
	return name ? (const char *)name->buf : def;
}


static ret_t
mc_maclist_set_random_vendor_from_list (mac_t *mac, chula_list_t *list)
{
    ret_t  ret;
    size_t len = 0;

    /* Choose a random entry */
    ret = chula_list_get_len (list, &len);
    if (ret != ret_ok) return ret_error;

	uint32_t num = chula_random() % len;

	/* Copy the vendor MAC range */
    while (num > 0) {
        list = list->next;
    }

	for (int i=0; i<3; i++) {
		mac->byte[i] = LIST_MAC_ITEM(list)->byte[i];
	}

    return ret_ok;
}


void
mc_maclist_set_random_vendor (mac_t *mac, mac_type_t type)
{
	int    num;
    ret_t  ret;
    size_t len_wireless = 0;
    size_t len_others   = 0;

	switch (type) {
	case mac_is_anykind:
        ret = chula_list_get_len (&list_others, &len_others);
        if (ret != ret_ok) return;

        ret = chula_list_get_len (&list_wireless, &len_wireless);
        if (ret != ret_ok) return;

        num = chula_random() % (len_others + len_wireless);
        if (num < len_others) {
			mc_maclist_set_random_vendor_from_list (mac, &list_others);
		} else {
			mc_maclist_set_random_vendor_from_list (mac, &list_wireless);
		}
		break;
	case mac_is_wireless:
		mc_maclist_set_random_vendor_from_list (mac, &list_wireless);
		break;
	case mac_is_others:
		mc_maclist_set_random_vendor_from_list (mac, &list_others);
		break;
	}
}


bool
mc_maclist_is_wireless (mac_t *mac)
{
    chula_buffer_t *name;
    name = get_mac_name(mac, &list_wireless);
    return (name != NULL);
}


static void
mc_maclist_print_from_list (chula_list_t *list, const char *keyword)
{
    chula_list_t *i;
    uint32_t      founds = 0;

    list_for_each (i, list) {
        mac_list_item_t *mac     = LIST_MAC_ITEM(i);
        bool             matched = (keyword == NULL);

        if (! matched) {
            matched = (chula_strncasestr((const char *)mac->name.buf, keyword, mac->name.len) != NULL);
            if (matched) {
                founds++;
                printf ("%04i - %02x:%02x:%02x - %s\n", founds,
                        mac->byte[0], mac->byte[1], mac->byte[2], mac->name.buf);
            }
        }
    }
}


void
mc_maclist_print (const char *keyword)
{
	printf ("Misc MACs:\n"
		"Num    MAC        Vendor\n"
		"---    ---        ------\n");
	mc_maclist_print_from_list (&list_others, keyword);

	printf ("\n"
		"Wireless MACs:\n"
		"Num    MAC        Vendor\n"
		"---    ---        ------\n");
	mc_maclist_print_from_list (&list_wireless, keyword);
}


static ret_t
read_maclist_file (char *path, chula_list_t *list)
{
    ret_t          ret;
    chula_buffer_t raw = CHULA_BUF_INIT;

    /* Read file */
    ret = chula_buffer_read_file (&raw, path);
    if (ret != ret_ok) {
		fprintf (stderr, "[ERROR] Could not read data file: %s\n", path);
        return ret_error;
    }

    /* Parse */
    while (! chula_buffer_is_empty(&raw)) {
        char            *EOL;
        mac_list_item_t *mac  = NULL;
        const char      *line = (const char *)raw.buf;

        EOL = chula_strchrnul (line, '\n');
        *EOL = '\0';

        ret = list_item_new (&mac);
        if (unlikely (ret != ret_ok)) return ret;

        mac->byte[0] = (char)(strtoul (line,   NULL, 16) & 0xFF);
		mac->byte[1] = (char)(strtoul (line+3, NULL, 16) & 0xFF);
		mac->byte[2] = (char)(strtoul (line+6, NULL, 16) & 0xFF);

        ret = chula_buffer_add (&mac->name, line+9, strlen(line+9));
        if (unlikely (ret != ret_ok)) {
            list_item_free(mac);
            goto error;
        }

        ret = chula_buffer_move_to_begin (&mac->name, strlen(line)+1);
        if (unlikely (ret != ret_ok)) {
            list_item_free(mac);
            goto error;
        }

        chula_list_add_tail (&mac->list, list);
    }

    chula_buffer_mrproper (&raw);
    return ret_ok;

 error:
    chula_buffer_mrproper (&raw);
    return ret_error;
}


ret_t
mc_maclists_init (void)
{
    ret_t ret;

    ret = read_maclist_file (LISTDIR "/OUI.list", &list_others);
    if (ret != ret_ok) return ret_error;

    ret = read_maclist_file (LISTDIR "/wireless.list", &list_wireless);
    if (ret != ret_ok) return ret_error;

    return ret_ok;
}

void
mc_maclist_free (chula_list_t *list)
{
    chula_list_t *i, *tmp;

    list_for_each_safe (i, tmp, list) {
        chula_list_del(i);
        list_item_free((mac_list_item_t *)i);
    }
}

ret_t
mc_maclists_mrproper (void)
{
    mc_maclist_free (&list_others);
    mc_maclist_free (&list_wireless);
    return ret_ok;
}

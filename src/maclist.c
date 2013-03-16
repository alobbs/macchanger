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

#include "maclist.h"

card_mac_list_item_t *list_others   = NULL; /* IEEE OUI */
card_mac_list_item_t *list_wireless = NULL; /* Wireless cards */


static char *
mc_maclist_get_cardname_from_list (const mac_t *mac, card_mac_list_item_t *list)
{
	   int i = 0;

	   while (list[i].name) {
			 if ((mac->byte[0] == list[i].byte[0]) &&
				(mac->byte[1] == list[i].byte[1]) &&
				(mac->byte[2] == list[i].byte[2]))
			 {
				    return list[i].name;
			 }
			 i++;
	   }
	   
	   return NULL;
}


static char *
mc_maclist_get_cardname (const mac_t *mac)
{
	   char *name;

	   name = mc_maclist_get_cardname_from_list (mac, list_wireless);
	   if (name) {
			 return name;
	   }

	   name = mc_maclist_get_cardname_from_list (mac, list_others);
	   return name;
}


const char *
mc_maclist_get_cardname_with_default (const mac_t *mac, const char *def)
{
	char *name;
	name = mc_maclist_get_cardname (mac);
	return name ? name : def;
}

static void
mc_maclist_set_random_vendor_from_list (mac_t *mac, card_mac_list_item_t *list)
{
	   int i, num = 0;

	   /* Count */
	   while (list[++num].name);

	   /* Choose one randomly */
	   num = random()%num;
	   
	   /* Copy the vender MAC range */
	   for (i=0; i<3; i++) {
			 mac->byte[i] = list[num].byte[i];
	   }
}


void
mc_maclist_set_random_vendor (mac_t *mac, mac_type_t type)
{
	   int total, num;
	   total = LIST_LENGHT (list_others) +
		   LIST_LENGHT (list_wireless);

	   num = random() % total;
	   
	   switch (type) {
	   case mac_is_anykind:
			 if (num < LIST_LENGHT(list_others)) {
				    mc_maclist_set_random_vendor_from_list (mac, list_others);
			 } else {
				    mc_maclist_set_random_vendor_from_list (mac, list_wireless);
			 }
			 break;
	   case mac_is_wireless:
			 mc_maclist_set_random_vendor_from_list (mac, list_wireless);
			 break;
	   case mac_is_others:
			 mc_maclist_set_random_vendor_from_list (mac, list_others);
			 break;
	   }
}


int
mc_maclist_is_wireless (const mac_t *mac)
{
	   return (mc_maclist_get_cardname_from_list (mac, list_wireless) != NULL);
}


static void
mc_maclist_print_from_list (card_mac_list_item_t *list, const char *keyword)
{
	   int i = 0;

	   while (list[i].name) {
		   if (!keyword || (keyword && strstr(list[i].name, keyword))) {
			 printf ("%04i - %02x:%02x:%02x - %s\n", i,
				    list[i].byte[0], list[i].byte[1],list[i].byte[2],
				    list[i].name);
		   }
			 i++;
	   }
}


void
mc_maclist_print (const char *keyword)
{
	   printf ("Misc MACs:\n"
			 "Num    MAC        Vendor\n"
			 "---    ---        ------\n");
	   mc_maclist_print_from_list (list_others, keyword);

	   printf ("\n"
			 "Wireless MACs:\n"
			 "Num    MAC        Vendor\n"
			 "---    ---        ------\n");
	   mc_maclist_print_from_list (list_wireless, keyword);
}


static card_mac_list_item_t *
mc_maclist_read_from_file (const char *fullpath)
{
	FILE *f;
	char *line;
	char  tmp[512];
	int   num =0;
	card_mac_list_item_t *list;

	if ((f = fopen(fullpath, "r")) == NULL) {
		fprintf (stderr, "ERROR: Can't read MAC list file \"%s\", It looks like a bad installation\n", fullpath);
		return NULL;
	}

	/* Count lines */
	while ((line = fgets (tmp, 511, f)) != NULL) num++;	
	rewind (f);
	
	/* Get mem */
	list = (card_mac_list_item_t *) malloc (sizeof(card_mac_list_item_t) * (num+1));

	/* Parse it */
	num = 0;
	while ((line = fgets (tmp, 511, f)) != NULL) {
		list[num].byte[0] = (char) (strtoul (line, NULL, 16) & 0xFF);
		list[num].byte[1] = (char) (strtoul (line+3, NULL, 16) & 0xFF);
		list[num].byte[2] = (char) (strtoul (line+6, NULL, 16) & 0xFF);

		line[strlen(line)-1] = '\0';
		list[num].name = (char*)(strdup(line+9));
		
		num ++;
	}

	/* End of list */
	list[num].byte[0] = list[num].byte[1] = list[num].byte[2] = 0;
	list[num].name = NULL;

	fclose (f);

	return list;
}


int
mc_maclist_init (void)
{
	list_others = mc_maclist_read_from_file(LISTDIR "/OUI.list");
	list_wireless = mc_maclist_read_from_file(LISTDIR "/wireless.list");

	return (list_others && list_wireless)? 0 : -1;
}


static void
free_list (card_mac_list_item_t *list)
{
	int i = 0;
	while (list[i].name) {
		free(list[i].name);
		i++;
	}
	free(list);
}


void
mc_maclist_free (void)
{
	free_list (list_others);
	free_list (list_wireless);
}


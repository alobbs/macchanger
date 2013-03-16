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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <time.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>

#include "mac.h"
#include "maclist.h"
#include "netinfo.h"


static void
print_help (void)
{
	printf ("GNU MAC Changer\n"
		"Usage: macchanger [options] device\n\n"
		"  -h,  --help                   Print this help\n"
		"  -V,  --version                Print version and exit\n"
		"  -s,  --show                   Print the MAC address and exit\n"
		"  -e,  --endding                Don't change the vendor bytes\n"
		"  -a,  --another                Set random vendor MAC of the same kind\n"
		"  -A                            Set random vendor MAC of any kind\n"
		"  -r,  --random                 Set fully random MAC\n"
		"  -l,  --list[=keyword]         Print known vendors\n"
		"  -m,  --mac=XX:XX:XX:XX:XX:XX  Set the MAC XX:XX:XX:XX:XX:XX\n\n"
		"Report bugs to alvaro@gnu.org\n");
}


static void
print_usage (void)
{
	printf ("GNU MAC Changer\n"
		"Usage: macchanger [options] device\n\n"
		"Try `macchanger --help' for more options.\n");
}


static void
print_mac (char *s, mac_t *mac)
{
	char string[18];
	int  is_wireless;

	is_wireless = mc_maclist_is_wireless(mac);
	mc_mac_into_string (mac, string);
	printf ("%s%s%s (%s)\n", s,
		string,
		is_wireless ? " [wireless]": "",
		CARD_NAME(mac));
}


int
main (int argc, char *argv[])
{
	char random       = 0;
	char endding      = 0;
	char another_any  = 0;
	char another_same = 0;
	char print_list   = 0;
	char show         = 0;
	char *set_mac     = NULL;
	char *search_word = NULL;
	
	struct option long_options[] = {
		/* Options without arguments */
		{"help",        no_argument,       NULL, 'h'},
		{"version",     no_argument,       NULL, 'V'},
		{"random",      no_argument,       NULL, 'r'},
		{"endding",     no_argument,       NULL, 'e'},
		{"another",     no_argument,       NULL, 'a'},
		{"show",        no_argument,       NULL, 's'},
		{"another_any", no_argument,       NULL, 'A'},
		{"list",        optional_argument, NULL, 'l'},
		{"mac",         required_argument, NULL, 'm'},
		{NULL, 0, NULL, 0}
	};

	
	net_info_t *net;
	mac_t      *mac;
	mac_t      *mac_faked;
	char       *device_name;

	int val;

	
	/* Read the parameters */
	while ((val = getopt_long (argc, argv, "VasArehlm:", long_options, NULL)) != -1) {
		switch (val) {
		case 'V':
			printf ("GNU MAC changer %s\n"
				"Written by Alvaro Lopez Ortega <alvaro@gnu.org>\n\n"
				"Copyright (C) 2003 Free Software Foundation, Inc.\n"
				"This is free software; see the source for copying conditions.  There is NO\n"
				"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n",
				VERSION);
			exit(0);
			break;
		case 'l':
			print_list = 1;
			search_word = optarg;
			break;
		case 'r':
			random = 1;
			break;
		case 'e':
			endding = 1;
			break;
		case 'a':
			another_same = 1;
			break;
		case 's':
			show = 1;
			break;
		case 'A':
			another_any = 1;
			break;
		case 'm':
			set_mac = optarg;
			break;
		case 'h':
		case '?':
		default:
			print_help();
			exit(0);
			break;
		}
	}

	/* Read the MAC lists */
	if (mc_maclist_init() < 0) exit(1);
	
	/* Print list? */
	if (print_list) {
		mc_maclist_print(search_word);
		exit(0);
	}

	/* Get device name argument */
	if (optind >= argc) {
		print_usage();
		exit(0);
	}
	device_name = argv[optind];
	
	
	srandom(time(NULL));

        /* Read the MAC */
	if ((net = mc_net_info_new(device_name)) == NULL) exit(1);
	mac = mc_net_info_get_mac(net);

	/* Print the current MAC info */
	print_mac ("Current MAC: ", mac);
	
	/* Change the MAC */
	mac_faked = mc_mac_dup (mac);

	if (show) {
		exit (0);
	} else if (set_mac) {
		if (mc_mac_read_string (mac_faked, set_mac) < 0) exit(1);
	} else if (random) {
		mc_mac_random (mac_faked, 6);
	} else if (endding) {
		mc_mac_random (mac_faked, 3);
	} else if (another_same) {
		int wl = mc_maclist_is_wireless(mac);
		mc_maclist_set_random_vendor(mac_faked, wl);
		mc_mac_random (mac_faked, 3);
	} else if (another_any) {
		mc_maclist_set_random_vendor(mac_faked, mac_is_anykind);
		mc_mac_random (mac_faked, 3);
	} else {
		mc_mac_next (mac_faked);
	}

	/* Set the new MAC */
	if (mc_net_info_set_mac (net, mac_faked) >= 0) {
		/* Re-read the MAC */
		mc_mac_free (mac_faked);
		mac_faked = mc_net_info_get_mac(net);

		/* Print it */
		print_mac ("Faked MAC:   ", mac_faked);
		
		/* Is the same MAC? */
		if (mc_mac_equal (mac, mac_faked)) {
			printf ("It's the same MAC!!\n");
		}
	}
	
	/* Memory free */
	mc_mac_free (mac);
	mc_mac_free (mac_faked);
	mc_net_info_free (net);
	mc_maclist_free();
	
	return 0;
}

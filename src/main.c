/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/* MAC Changer
 *
 * Authors:
 *      Alvaro Lopez Ortega <alvaro@alobbs.com>
 *
 * Copyright (C) 2002,2013 Alvaro Lopez Ortega
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
# include <config.h>
#endif

#include <sys/types.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "mac.h"
#include "maclist.h"
#include "netinfo.h"

#define EXIT_OK    0
#define EXIT_ERROR 1

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
		"  -p,  --permanent              Reset to original, permanent hardware MAC\n"
		"  -r,  --random                 Set fully random MAC\n"
		"  -l,  --list[=keyword]         Print known vendors\n"
		"  -b,  --bia                    Pretend to be a burned-in-address\n"
		"  -m,  --mac=XX:XX:XX:XX:XX:XX  Set the MAC XX:XX:XX:XX:XX:XX\n\n"
		"Report bugs to https://github.com/alobbs/macchanger/issues\n");
}


static void
print_usage (void)
{
	printf ("GNU MAC Changer\n"
		"Usage: macchanger [options] device\n\n"
		"Try `macchanger --help' for more options.\n");
}


static void
print_mac (const char *s, const mac_t *mac)
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


static void
random_seed (void)
{
	int            fd;
	struct timeval tv;
	unsigned int   seed;

	if ((fd = open("/dev/urandom", O_RDONLY)) >= 0 ||
	    (fd = open("/dev/random", O_RDONLY)) >= 0)
	{
		read (fd, &seed, sizeof(seed));
		close (fd);
	} else {
		gettimeofday (&tv, NULL);
		seed = (getpid() << 16) ^ tv.tv_sec ^ tv.tv_usec;
	}

	srandom(seed);
}


int
main (int argc, char *argv[])
{
	char random       = 0;
	char endding      = 0;
	char another_any  = 0;
	char another_same = 0;
	char permanent    = 0;
	char print_list   = 0;
	char show         = 0;
	char set_bia      = 0;
	char *set_mac     = NULL;
	char *search_word = NULL;

	struct option long_options[] = {
		/* Options without arguments */
		{"help",        no_argument,       NULL, 'h'},
		{"version",     no_argument,       NULL, 'V'},
		{"random",      no_argument,       NULL, 'r'},
		{"endding",     no_argument,       NULL, 'e'},
		{"another",     no_argument,       NULL, 'a'},
		{"permanent",   no_argument,       NULL, 'p'},
		{"show",        no_argument,       NULL, 's'},
		{"another_any", no_argument,       NULL, 'A'},
		{"bia",         no_argument,       NULL, 'b'},
		{"list",        optional_argument, NULL, 'l'},
		{"mac",         required_argument, NULL, 'm'},
		{NULL, 0, NULL, 0}
	};

	net_info_t *net;
	mac_t      *mac;
	mac_t      *mac_permanent;
	mac_t      *mac_faked;
	char       *device_name;
	int         val;
	int         ret;

	/* Read the parameters */
	while ((val = getopt_long (argc, argv, "VasAbrephlm:", long_options, NULL)) != -1) {
		switch (val) {
		case 'V':
			printf ("GNU MAC changer %s\n"
				"Written by Alvaro Lopez Ortega <alvaro@gnu.org>\n\n"
				"Copyright (C) 2003,2013 Alvaro Lopez Ortega <alvaro@gnu.org>.\n"
				"This is free software; see the source for copying conditions.  There is NO\n"
				"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n",
				VERSION);
			exit (EXIT_OK);
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
		case 'b':
			set_bia = 1;
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
		case 'p':
			permanent = 1;
			break;
		case 'm':
			set_mac = optarg;
			break;
		case 'h':
		case '?':
		default:
			print_help();
			exit (EXIT_OK);
			break;
		}
	}

	/* Read the MAC lists */
	if (mc_maclist_init() < 0) {
		exit (EXIT_ERROR);
	}

	/* Print list? */
	if (print_list) {
		mc_maclist_print(search_word);
		exit (EXIT_OK);
	}

	/* Get device name argument */
	if (optind >= argc) {
		print_usage();
		exit (EXIT_OK);
	}
	device_name = argv[optind];

	/* Seed a random number generator */
	random_seed();

        /* Read the MAC */
	if ((net = mc_net_info_new(device_name)) == NULL) {
		exit (EXIT_ERROR);
	}
	mac = mc_net_info_get_mac(net);
	mac_permanent = mc_net_info_get_permanent_mac(net);

	/* Print the current MAC info */
	print_mac ("Current MAC:   ", mac);
	print_mac ("Permanent MAC: ", mac_permanent);

	/* Change the MAC */
	mac_faked = mc_mac_dup (mac);

	if (show) {
		exit (EXIT_OK);
	} else if (set_mac) {
		if (mc_mac_read_string (mac_faked, set_mac) < 0) {
			exit (EXIT_ERROR);
		}
	} else if (random) {
		mc_mac_random (mac_faked, 6, set_bia);
	} else if (endding) {
		mc_mac_random (mac_faked, 3, set_bia);
	} else if (another_same) {
		val = mc_maclist_is_wireless (mac);
		mc_maclist_set_random_vendor (mac_faked, val);
		mc_mac_random (mac_faked, 3, set_bia);
	} else if (another_any) {
		mc_maclist_set_random_vendor(mac_faked, mac_is_anykind);
		mc_mac_random (mac_faked, 3, set_bia);
	} else if (permanent) {
		mac_faked = mc_mac_dup (mac_permanent);
	} else {
		mc_mac_next (mac_faked);
	}

	/* Set the new MAC */
	ret = mc_net_info_set_mac (net, mac_faked);
	if (ret == 0) {
		/* Re-read the MAC */
		mc_mac_free (mac_faked);
		mac_faked = mc_net_info_get_mac(net);

		/* Print it */
		print_mac ("New MAC:       ", mac_faked);

		/* Is the same MAC? */
		if (mc_mac_equal (mac, mac_faked)) {
			printf ("It's the same MAC!!\n");
		}
	}

	/* Memory free */
	mc_mac_free (mac);
	mc_mac_free (mac_faked);
	mc_mac_free (mac_permanent);
	mc_net_info_free (net);
	mc_maclist_free();

	return (ret == 0) ? EXIT_OK : EXIT_ERROR;
}

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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <libchula/libchula.h>

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
		"  -e,  --ending                 Don't change the vendor bytes\n"
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


static ret_t
print_mac (const char *title, mac_t *mac)
{
    ret_t          ret;
    chula_buffer_t tmp = CHULA_BUF_INIT;

    ret = mc_mac_to_buf (mac, &tmp);
    if (ret != ret_ok) return ret;

    bool is_wireless = mc_maclist_is_wireless(mac);
	printf ("%s%s%s (%s)\n", title, tmp.buf,
            is_wireless ? " [wireless]": "",
            CARD_NAME(mac));

    chula_buffer_mrproper (&tmp);
    return ret_ok;
}


int
main (int argc, char *argv[])
{
	char random       = 0;
	char ending       = 0;
	char another_any  = 0;
	char another_same = 0;
	char permanent    = 0;
	char print_list   = 0;
	char show         = 0;
	char set_bia      = 0;
	char *search_word = NULL;
    chula_buffer_t mac_to_set = CHULA_BUF_INIT;

	struct option long_options[] = {
		/* Options without arguments */
		{"help",        no_argument,       NULL, 'h'},
		{"version",     no_argument,       NULL, 'V'},
		{"random",      no_argument,       NULL, 'r'},
		{"ending",      no_argument,       NULL, 'e'},
		{"endding",     no_argument,       NULL, 'e'}, /* kept for backwards compatibility */
		{"another",     no_argument,       NULL, 'a'},
		{"permanent",   no_argument,       NULL, 'p'},
		{"show",        no_argument,       NULL, 's'},
		{"another_any", no_argument,       NULL, 'A'},
		{"bia",         no_argument,       NULL, 'b'},
		{"list",        optional_argument, NULL, 'l'},
		{"mac",         required_argument, NULL, 'm'},
		{NULL, 0, NULL, 0}
	};

	int         re;
	ret_t       ret;
	int         val;
	net_info_t *net           = NULL;
	mac_t      *mac           = NULL;
    mac_t      *mac_permanent = NULL;
	mac_t      *mac_faked     = NULL;
	char       *device_name   = NULL;

	/* Read the parameters */
	while ((val = getopt_long (argc, argv, "VasAbrephlm:", long_options, NULL)) != -1) {
		switch (val) {
		case 'V':
			printf ("GNU MAC changer %s\n"
				"Written by Alvaro Lopez Ortega <alvaro@gnu.org>\n\n"
				"Copyright (C) 2003,2015 Alvaro Lopez Ortega <alvaro@gnu.org>.\n"
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
			ending = 1;
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
            if (optarg) {
                chula_buffer_add (&mac_to_set, optarg, strlen(optarg));
            }
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
    ret = mc_maclists_init();
    if (ret != ret_ok) {
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
	chula_random_seed();

	/* Read the MAC */
    ret = mc_net_info_new (&net, device_name);
    if (ret != ret_ok) {
		exit (EXIT_ERROR);
	}

    ret  = mc_net_info_get_mac (net, &mac);
    ret |= mc_net_info_get_perm_mac (net, &mac_permanent);
    if (ret != ret_ok) {
		exit (EXIT_ERROR);
    }

	/* --bia can only be used with --random */
	if (set_bia  &&  !random) {
		fprintf (stderr, "[WARNING] Ignoring --bia option that can only be used with --random\n");
	}

	/* Print the current MAC info */
	print_mac ("Current MAC:   ", mac);
	print_mac ("Permanent MAC: ", mac_permanent);

	/* Change the MAC */
    ret = mc_mac_dup (mac, &mac_faked);
    if (ret != ret_ok) {
        exit (EXIT_ERROR);
    }

	if (show) {
		exit (EXIT_OK);
	} else if (! chula_buffer_is_empty(&mac_to_set)) {
        ret = mc_mac_read (mac_faked, &mac_to_set);
        if (ret != ret_ok) {
			exit (EXIT_ERROR);
        }
	} else if (random) {
		mc_mac_random (mac_faked, 6, set_bia);
	} else if (ending) {
		mc_mac_random (mac_faked, 3, 1);
	} else if (another_same) {
		val = mc_maclist_is_wireless (mac);
		mc_maclist_set_random_vendor (mac_faked, val);
		mc_mac_random (mac_faked, 3, 1);
	} else if (another_any) {
		mc_maclist_set_random_vendor(mac_faked, mac_is_anykind);
		mc_mac_random (mac_faked, 3, 1);
	} else if (permanent) {
        ret = mc_mac_dup (mac_permanent, &mac_faked);
        if (ret != ret_ok) {
            exit (EXIT_ERROR);
        }
	} else {
		exit (EXIT_OK); /* default to show */
	}

	/* Set the new MAC */
	re = mc_net_info_set_mac (net, mac_faked);
	if (re == 0) {
		/* Re-read the MAC */
		mc_mac_free (mac_faked);

        ret = mc_net_info_get_mac (net, &mac_faked);
        if (ret != ret_ok) exit (EXIT_ERROR);

		/* Print it */
		print_mac ("New MAC:       ", mac_faked);

		/* Is the same MAC? */
		if (mc_mac_equal (mac, mac_faked)) {
			printf ("[WARNING] It is the same MAC\n");
		}
	}

	/* Memory free */
	mc_mac_free (mac);
	mc_mac_free (mac_faked);
	mc_mac_free (mac_permanent);
	mc_net_info_free (net);
	mc_maclists_mrproper();

	return (re == 0) ? EXIT_OK : EXIT_ERROR;
}

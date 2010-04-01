/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Open Source Development Lab, Inc.
 *
 * History:
 * Jan-30-2002  Created by Mark Wong & Jenny Zhang 
 * Aug-20-2003  Rewrote parsing command line part
 */

#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <time.h>

#include <common.h>
#include <eu.h>

int eus, rampuprate, duration;
double think_time;
char sname[32];
int help;
int mode_access, mode_cache, mode_clean;

sem_t running_eu_count;
sem_t running_interactions[INTERACTION_TOTAL];


/* used to be for PHASE1, now just declare it but might not be used */
char uname[32], auth[32], dbname[32];
char cache_host[32];
int cache_port;
/* used to be for PHASE2, now just declare it but might not be used */
int port;

int usage(char *);

/*
 * main() just takes arguments off the command line, starts up the rest of
 * the program and waits for the application to finish.
 */
int main(int argc, char *argv[])
{
	int running_eus;
	time_t stop_time;
	int c;

	/* setup default value in case the user forgets, but in most
	   cases the user should set it */
#ifdef ODBC
	strcpy(sname, "DBT1");
	strcpy(uname, "pgsql");
	strcpy(auth, "pgsql");
#endif
#ifdef LIBPQ
	strcpy(sname, "localhost");
	strcpy(dbname, "DBT1");
	strcpy(uname, "pgsql");
	strcpy(auth, "pgsql");
#endif
	strcpy(cache_host, "localhost");
	strcpy(dbport, "5432");
	cache_port = 9999;
	port = 9992;
	item_count = 1000;
	customers = 28800;
	eus = 10;
	rampuprate = 10;
	think_time = 7.2;
	duration = 300;
	help = 0;
	altered = 0;
	mode_access = MODE_APPSERVER;
	mode_cache = MODE_CACHE_OFF;
	mode_clean = MODE_CLEAN_OFF;

	while (1)
	{
		static struct option long_options[] = {
			{ "access_cache", no_argument, &mode_cache, MODE_CACHE_ON },
			{ "access_direct", no_argument, &mode_access, MODE_DIRECT },
			{ "access_appServer", no_argument, &mode_access, MODE_APPSERVER },
			{ "access_clean", no_argument, &mode_clean, MODE_CLEAN_ON },
			{ "dbhost", required_argument, 0, 0 },
			{ "dbname", required_argument, 0, 0 },
			{ "dbport", required_argument, 0, 0 },
			{ "dbnodename", required_argument, 0, 0 },
			{ "username", required_argument, 0, 0 },
			{ "password", required_argument, 0, 0 },
			{ "server_name", required_argument, 0, 0 },
			{ "port", required_argument, 0, 0 },
			{ "item_count", required_argument, 0, 0 },
			{ "customer_count", required_argument, 0, 0 },
			{ "emulated_users", required_argument, 0, 0 },
			{ "rampup_rate", required_argument, 0, 0 },
			{ "think_time", required_argument, 0, 0 },
			{ "duration", required_argument, 0, 0 },
			{ "cache_host", required_argument, 0, 0 },
			{ "cache_port", required_argument, 0, 0 },
			{ "output_path", required_argument, 0, 0 },
			{ "help", no_argument, &help, 1 },
			{ "altered", no_argument, &altered, 1 },
			{ 0, 0, 0, 0 }
		};
		int option_index = 0;
		c = getopt_long_only(argc, argv, "", long_options, &option_index);
		if (c == -1)
		{
			break;
		}
		switch (c)
		{
			case 0:
				if (long_options[option_index].flag != 0)
				{
					break;
				}
#ifdef LIBPQ
				if (strcmp(long_options[option_index].name, "dbport") == 0)
				{
					strcpy(dbport, optarg);
				}
				if (strcmp(long_options[option_index].name, "dbhost") == 0)
				{
					strcpy(sname, optarg);
				}
				if (strcmp(long_options[option_index].name, "dbname") == 0)
				{
					strcpy(dbname, optarg);
				}
#endif
#ifdef ODBC
				if (strcmp(long_options[option_index].name, "dbnodename") == 0)
				{
					strcpy(sname, optarg);
				}
#endif
				if (strcmp(long_options[option_index].name, "username") == 0)
				{
					strcpy(uname, optarg);
				}
				if (strcmp(long_options[option_index].name, "password") == 0)
				{
					strcpy(auth, optarg);
				}
				if (strcmp(long_options[option_index].name, "cache_host") == 0)
				{
					strcpy(cache_host, optarg);
				}
				if (strcmp(long_options[option_index].name, "cache_port") == 0)
				{
					cache_port = atoi(optarg);
				}
				if (strcmp(long_options[option_index].name, "server_name") == 0)
				{
					strcpy(sname, optarg);
				}
				if (strcmp(long_options[option_index].name, "port") == 0)
				{
					port = atoi(optarg);
				}
				if (strcmp(long_options[option_index].name, "item_count") == 0)
				{
					item_count = atoi(optarg);
				}
				if (strcmp(long_options[option_index].name, "customer_count") == 0)
				{
					customers = atoi(optarg);
				}
				if (strcmp(long_options[option_index].name, "emulated_users") == 0)
				{
					eus = atoi(optarg);
				}
				if (strcmp(long_options[option_index].name, "output_path") == 0)
				{
					strcpy(output_path, optarg);
				}
				if (strcmp(long_options[option_index].name, "rampup_rate") == 0)
				{
					rampuprate = atoi(optarg);
				}
				if (strcmp(long_options[option_index].name, "think_time") == 0)
				{
					think_time = atof(optarg);
				}
				if (strcmp(long_options[option_index].name, "duration") == 0)
				{
					duration = atoi(optarg);
				}
			break;
		default:
			printf ("?? getopt returned character code 0%o ??\n", c);
			exit(1);
		}
	}

	if ( help == 1)
	{
		return usage(argv[0]);
	}

	/* Initialize global variables. */
	init_common();

	/*
	 * Calculate the stop time before the users are started and factor in
	 * the rampup time.
	 */
	stop_time = time(NULL) + duration + (int) ((eus / rampuprate) * 60);

	/* Start the user threads. */
	if ( init_eus(eus, MIX_SHOPPING, rampuprate, duration, think_time, item_count ) != OK)
	{
		printf("dbDriver init_eus failed\n") ;
		return 1;
	}
	
	/*
	 * Sit in an infinite loop until the run duration expires.  This is a good
	 * place to implement some real-time statistics reporting, if someone is so
	 * inclined.
	 */
	mark_logs(RUN_START);

	int i,sem_val;
	printf("INTERACTION_NAME\n");
	for (i = 0; i < INTERACTION_TOTAL; i++)
	{
		printf("  %s ", interaction_short_name[i]);
	}
	printf("\n");
	do
	{
		for (i = 0; i < INTERACTION_TOTAL; i++)
		{
			sem_getvalue(&running_interactions[i], &sem_val);
			printf(" %3d ",sem_val);
		}
		printf("\n");

		sem_getvalue(&running_eu_count, &running_eus);
		sleep(10);
	}
	while (time(NULL) <= stop_time);
	printf ("Dbdriver ended\n");
	fflush(stdout);

	return 0;
}

int usage(char *name)
{
	printf("run without the middle tier and search_results_cache: \n");
#ifdef LIBPQ
	printf("usage: %s --access_direct --dbhost <dbhost> --dbname <dbname>\n", name);
#endif
#ifdef ODBC
	printf("usage: %s --access_direct --dbnodehost <dbnodehost>\n", name);
#endif
	printf("--username <username> --password <password>\n");
	printf("--item_count <item_count> --coustomer_count <customer_count>\n");
	printf("--emulated_users <emulated_users> --rampup_rate <eu/min>\n");
	printf("--think_time <think_time> --duration <duration>\n\n");


	printf("run without the middle tier but with search_results_cache: \n");
#ifdef LIBPQ
	printf("usage: %s --access_direct --dbhost <dbhost> --dbname <dbname>\n", name);
#endif
#ifdef ODBC
	printf("usage: %s --access_direct --dbnodehost <dbnodehost>\n", name);
#endif
	printf("--access_cache --cache_host <cache_host> --cache_port <cache_port>\n");
	printf("--username <username> --password <password>\n");
	printf("--item_count <item_count> --coustomer_count <customer_count>\n");
	printf("--emulated_users <emulated_users> --rampup_rate <eu/min>\n");
	printf("--think_time <think_time> --duration <duration>\n\n");

	printf("run with the middle tier: \n");
	printf("usage: %s --server_name <server_name> --port <port>\n", name);

	printf("The default values if not defined\n");
#ifdef LIBPQ
	printf("--dbhost %s --dbname %s\n", sname, dbname);
#endif
#ifdef ODBC
	printf("--dbnodehost %s\n", sname);
#endif
	printf("--username %s --password %s\n", uname, auth);
	printf("--cache_host %s --cache_port %d\n", cache_host, cache_port);
	printf("--server_name %s --port %d\n", sname, port);
	printf("--item_count %d --coustomer_count %d\n", item_count, customers);
	printf("--emulated_users %d --rampup_rate %d\n", eus, rampuprate);
	printf("--think_time %f --duration %d\n", think_time, duration);
	printf("--output_path dbdriver_dir\n");

	return 1;
}

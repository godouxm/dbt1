/*
 * results.c
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 *
 * 9 april 2002
 */

#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <common.h>
#include <getopt.h>

int usage(char *name);
int help = 0;

int main(int argc, char *argv[])
{
	int i;
	FILE *log_mix;
	FILE *plot_ips;
	FILE *BT;
	time_t current_time;
	char interaction[3];
	float response_time;
	int tid;
	time_t start_time = -1;
	float total_response_time = 0;
	float ips; /* bogotransactions per second */
	long long total_interaction_count = 0;
	long long interaction_count[INTERACTION_TOTAL];
	float interaction_response_time[INTERACTION_TOTAL];
	int error_count, rc;
	char mix_filename[256];
	char outputdir[256];
	char filename[256];

	time_t previous_time;
	int elapsed_time = 0;
	int current_interation_count = 0;

	char marker[128];

	int c;

	if ( argc < 1)
	{
		return usage(argv[0]);
	}

	while (1)
	{
		static struct option long_options[] = {
			{ "mixfile", required_argument, 0, 0 },
			{ "outputdir", required_argument, 0, 0 },
			{ "help", no_argument, &help, 1},
			{0, 0, 0, 0}
		};

		int option_index = 0;
		c = getopt_long_only(argc, argv, "", long_options, &option_index);
		
		if (c == -1)
			 break;

		switch (c)
		{
		case 0:
			if (long_options[option_index].flag != 0)
			{
				break;
			}
			if (strcmp(long_options[option_index].name, "outputdir") == 0)
			{
				strcpy(outputdir, optarg);
			}
			if (strcmp(long_options[option_index].name, "mixfile") == 0)
			{
				strcpy(mix_filename, optarg);
			}
			if (strcmp(long_options[option_index].name, "help") == 0)
			{
				break;
			}
			break;
		default:
			printf ("?? getopt returned character code 0%o ??\n", c);
			return 1;
		}
	}

	if ( help == 1)
		return usage(argv[0]);

	error_count = 0;
	previous_time=0;

	log_mix = fopen(mix_filename, "r");
	if (log_mix == NULL)
	{
		printf("cannot open %s for reading\n", mix_filename);
		return 2;
	}

	sprintf(filename, "%s/%s", outputdir, "ips.csv");
	plot_ips = fopen(filename, "w");
	if (plot_ips == NULL)
	{
		printf("cannot open outputdir/ips.csv for writing\n");
		return 3;
	}

	sprintf(filename, "%s/%s", outputdir, "BT");
	BT = fopen(filename, "w");
	if (BT == NULL)
	{
		printf("cannot open outputdir/BT for writing\n");
		return 3;
	}

	/*
	 * Initialize the counters for the number of each interaction to 0
	 * as well as the total response time.
	 */
	for (i = 0; i < INTERACTION_TOTAL; i++)
	{
		interaction_count[i] = 0;
		interaction_response_time[i] = 0;
	}

	/*
	 * Set the last character to null, in case we ever want to print out
	 * the data captured in interaction[].
	 */
	interaction[2] = '\0';

	/* Keep reading the file until we hit the end. */
	while (1)
	{
		rc = fscanf(log_mix, "%d,%c%c,%f,%d", (int *) &current_time,
			&interaction[0], &interaction[1], &response_time, &tid);
		if (rc == 5)
		{
			if (start_time == -1)
				start_time = current_time;

			/*
			 * Note that we're factoring in Customer Registration even though
			 * it isn't a database interaction.
			 */
			/* only count the successful interactions */
			if (strcmp(interaction, "ER") != 0)
			{
				total_response_time += response_time;
				++total_interaction_count;
				if (strcmp(interaction, interaction_short_name[ADMIN_CONFIRM]) == 0)
				{
					++interaction_count[ADMIN_CONFIRM];
					interaction_response_time[ADMIN_CONFIRM] += response_time;
				}
				else if (strcmp(interaction,
					interaction_short_name[ADMIN_REQUEST]) == 0)
				{
					++interaction_count[ADMIN_REQUEST];
					interaction_response_time[ADMIN_REQUEST] += response_time;
				}
				else if (strcmp(interaction,
					interaction_short_name[BEST_SELLERS]) == 0)
				{
					++interaction_count[BEST_SELLERS];
					interaction_response_time[BEST_SELLERS] += response_time;
				}
				else if (strcmp(interaction,
					interaction_short_name[BUY_CONFIRM]) == 0)
				{
					++interaction_count[BUY_CONFIRM];
					interaction_response_time[BUY_CONFIRM] += response_time;
				}
				else if (strcmp(interaction, interaction_short_name[BUY_REQUEST]) == 0)
				{
					++interaction_count[BUY_REQUEST];
					interaction_response_time[BUY_REQUEST] += response_time;
				}
				else if (strcmp(interaction,
					interaction_short_name[CUSTOMER_REGISTRATION]) == 0)
				{
					++interaction_count[CUSTOMER_REGISTRATION];
					interaction_response_time[CUSTOMER_REGISTRATION] += response_time;
				}
				else if (strcmp(interaction, interaction_short_name[HOME]) == 0)
				{
					++interaction_count[HOME];
					interaction_response_time[HOME] += response_time;
				}
				else if (strcmp(interaction, interaction_short_name[NEW_PRODUCTS]) == 0)
				{
					++interaction_count[NEW_PRODUCTS];
					interaction_response_time[NEW_PRODUCTS] += response_time;
				}
				else if (strcmp(interaction,
					interaction_short_name[ORDER_DISPLAY]) == 0)
				{
					++interaction_count[ORDER_DISPLAY];
					interaction_response_time[ORDER_DISPLAY] += response_time;
				}
				else if (strcmp(interaction,
					interaction_short_name[ORDER_INQUIRY]) == 0)
				{
					++interaction_count[ORDER_INQUIRY];
					interaction_response_time[ORDER_INQUIRY] += response_time;
				}
				else if (strcmp(interaction,
					interaction_short_name[PRODUCT_DETAIL]) == 0)
				{
					++interaction_count[PRODUCT_DETAIL];
					interaction_response_time[PRODUCT_DETAIL] += response_time;
				}
				else if (strcmp(interaction,
					interaction_short_name[SEARCH_REQUEST]) == 0)
				{
					++interaction_count[SEARCH_REQUEST];
					interaction_response_time[SEARCH_REQUEST] += response_time;
				}
				else if (strcmp(interaction,
					interaction_short_name[SEARCH_RESULTS]) == 0)
				{
					++interaction_count[SEARCH_RESULTS];
					interaction_response_time[SEARCH_RESULTS] += response_time;
				}
				else if (strcmp(interaction,
					interaction_short_name[SHOPPING_CART]) == 0)
				{
					++interaction_count[SHOPPING_CART];
					interaction_response_time[SHOPPING_CART] += response_time;
				}

				/* Output data to graph for interactions per second. */
				if (current_time <= previous_time + 30)
				{
					++current_interation_count;
				}
				else
				{
					fprintf(plot_ips, "%d,%f\n", elapsed_time,
						(double) current_interation_count / 30.0);
					++elapsed_time;

					previous_time = current_time;
					current_interation_count = 1;
				}
			}
			else
			{
				++error_count;
			}
		}
		/* if START mark is in mix.log, use START time as start_time */
		else if (rc == 3 && (strcmp(interaction, "ST") == 0) )
		{
			/* finish reading this line */
			fscanf(log_mix, "%s", marker);
			start_time = current_time;
			/* reset everything */
			total_response_time = 0;
			total_interaction_count = 0;
			for (i = 0; i < INTERACTION_TOTAL; i++)
			{
				interaction_count[i] = 0;
				interaction_response_time[i] = 0;
			}
			fclose(plot_ips);
			sprintf(filename, "%s/%s", outputdir, "ips.csv");
			plot_ips = fopen(filename, "w");
		}
		else break;
	}
	fclose(log_mix);
	fclose(plot_ips);

	/* Calculate the actual mix of interactions. */
	fprintf(BT, "%s\n", "Interaction                 %%  Avg. Response Time (s)");
	for (i = 0; i < INTERACTION_TOTAL; i++)
	{
		fprintf(BT, "%-21s  %6.2f  %22.3f\n", interaction_name[i],
			(double) interaction_count[i] / (double) total_interaction_count * 100.0,
			interaction_response_time[i] / (double) interaction_count[i]);
	}
	
	/* Calculated the number of interactions per second. */
	ips = (double) total_interaction_count / difftime(current_time, start_time);
	fprintf(BT, "\n%0.1f bogotransactions per second\n", ips);

	fprintf(BT, "%0.1f minute duration\n",
		difftime(current_time, start_time) / 60.0);
	fprintf(BT, "total bogotransactions %ld\n", (long) total_interaction_count);
	fprintf(BT, "total errors %d\n", error_count);
	fprintf(BT, "\n");
	fclose(BT);

	return 0;
}

int usage(char *name)
{
	printf("usage: %s --mixfile <filename> --outputdir <outputdir>\n", name);
	return 1;
}

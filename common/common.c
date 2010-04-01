/* common.c
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 *
 * 10 january 2001
 * Based on TPC-W v1.6 specification.
 */


#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <common.h>
#include <stdarg.h>
#include <sys/time.h>

#define ERROR_LOG_NAME "error.log"
#define DEBUG_LOG_NAME "debug.log"


/* Clause 2.6.2.2 */
const char *a_string_char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789DEADBEEFDEADBEEFDEADBEEFXXX";
const char *n_string_char = "0123456789";
int item_count;


/* Clause 4.7.1 */
const char *cx_type[CX_TYPE_MAX] =
{
	"VISA", "MASTERCARD", "DISCOVER", "AMEX", "DINERS"
};


/* Clause 4.6.2.16 */
const char *i_backing[I_BACKING_MAX] =
{
	"HARDBACK", "PAPERBACK", "USED", "AUDIO", "LIMITED-EDITION"
};


/* Clause 4.6.2.12 */
const char *i_subject[I_SUBJECT_MAX] =
{
	"ARTS",			"BIOGRAPHIES",		"BUSINESS",
	"CHILDREN",		"COMPUTERS",		"COOKING",
	"HEALTH",		"HISTORY",			"HOME",
	"HUMOR",		"LITERATURE",		"MYSTERY",
	"NON-FICTION",	"PARENTING",		"POLITICS",
	"REFERENCE",	"REGLIGION",		"ROMANCE",
	"SELF-HELP",	"SCIENCE-NATURE",	"SCIENCE-FICTION",
	"SPORTS",		"YOUTH",			"TRAVEL"
};


/* Clause 4.7.1 */
const char *o_ship_type[O_SHIP_TYPE_MAX] =
{
	"AIR", "UPS", "FEDEX", "SHIP", "COURIER", "MAIL"
};


/* Clause 4.7.1 */
const char *o_status[O_STATUS_MAX] =
{
	"PROCESSING", "SHIPPED", "PENDING", "DENIED"
};


/* Clause 4.6.2.9 */
const char *syl[10] =
{
	"BA", "OG", "AL", "RI", "RE", "SE", "AT", "UL", "IN", "NG"
};

/* Clause 4.6.2.15 */
const char *co_name[CO_ID_MAX] =
{
	"Algeria", "Argentina", "Armenia", "Australia", "Austria", "Azerbaijan",
	"Bahamas", "Bahrain", "Bangla Desh", "Barbados", "Belarus", "Belgium",
	"Bermuda", "Bolivia", "Botswana", "Brazil", "Bulgaria", "Canada",
	"Cayman Islands", "Chad", "Chile", "China", "Christmas Island", "Colombia",
	"Croatia", "Cuba", "Cyprus", "Czech Republic", "Denmark",
	"Dominican Republic", "Eastern Caribbean", "Ecuador", "Egypt",
	"El Salvador", "Estonia", "Ethiopia", "Falkland Island", "Faroe Island",
	"Fiji", "Finland", "France", "Gabon", "Germany", "Gibraltar", "Greece",
	"Guam", "Hong Kong", "Hungary", "Iceland", "India", "Indonesia", "Iran",
	"Iraq", "Ireland", "Israel", "Italy", "Jamaica", "Japan", "Jordan",
	"Kazakhstan", "Kuwait", "Lebanon", "Luxembourg", "Malaysia", "Mauritius",
	"Mexico", "Netherlands", "New Zealand", "Norway", "Pakistan",
	"Philippines", "Poland", "Portugal", "Romania", "Russia", "Saudi Arabia",
	"Singapore", "Slovakia", "South Africa", "South Korea", "Spain", "Sudan",
	"Sweden", "Switzerland", "Taiwan", "Thailand", "Trinidad", "Turkey",
	"United Kingdom", "United States", "Venezuela", "Zambia"
};


/* Clause 2.10.5.1 */
const char *search_type[SEARCH_TYPE_MAX] =
{
	"Author", "Title", "Subject"
};

const char *interaction_short_name[INTERACTION_TOTAL] =
{
	"AC", "AR", "BS", "BC", "BR", "CR", "HO", "NP", "OD", "OI", "PD", "SR",
	"SU", "SC"
};

const char *interaction_name[INTERACTION_TOTAL] =
{
	"Admin Confirm", "Admin Request", "Best Sellers", "Buy Confirm",
	"Buy Request", "Customer Registration", "Home", "New Products",
	"Order Display", "Order Inquiry", "Product Detail", "Search Request",
	"Search Results", "Shopping Cart"
};


char output_path[256] = "";
FILE *log_error;
#ifdef DEBUG
FILE *log_debug;
#endif /* DEBUG */
pthread_mutex_t mutex_error_log = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_debug_log = PTHREAD_MUTEX_INITIALIZER;

int init_common()
{
	char path[512];
	/* Use the same default seed so the same data is always generated
	 * by datagen.  For the multi-threaded application, it appears that
	 * each thread needs to seed itself.
	 */
	srand(1);

	/* Open a file to log errors to. */
	if (output_path[0] != '\0')
	{
		strcat(output_path, "/");
	}
	sprintf(path, "%s%s", output_path, ERROR_LOG_NAME);
	log_error = fopen(path, "w");
	if (log_error == NULL)
	{
		fprintf(stderr, "cannot open %s\n", ERROR_LOG_NAME);
		return ERROR;
	}

#ifdef DEBUG
	sprintf(path, "%s%s", output_path, DEBUG_LOG_NAME);
	log_debug = fopen(path, "w");
	if (log_debug == NULL)
	{
		fprintf(stderr, "cannot open %s\n", DEBUG_LOG_NAME);
		return ERROR;
	}
#endif /* DEBUG */

	return OK;
}


/* Clause 4.6.2.9 */
void digsyl2(char *string, long long d, long long n)
{
	char dig[32];
	long long pad;
	long long i, length;

	if (d < 0)
	{
		string = '\0';
		return; 
	}

	sprintf(dig, "%lld", d);
	length = strlen(dig);
	pad = n - length;
	if (pad < 0)
	{
		pad = 0;
	}
	string[0] = '\0';
	for (i = 0; i < pad; i++)
	{
		strcat(string, syl[0]);
	}
	for (i = 0; i < length; i++)
	{
		strcat(string, syl[dig[i] - '0']);
	}

	return;
}


double get_percentage()
{
	return (double) rand() / (double) RAND_MAX;
}


/* Clause 4.6.2.2 */
void get_a_string(char *a_string, int x, int y)
{
	int length;
	int i;

	length = x + get_random_int(y - x + 1) + 1;
	a_string[length - 1] = '\0';

	for (i = 0; i < length - 1; i++)
	{
		a_string[i] = a_string_char[get_random_int(A_STRING_CHAR_LEN)];
	}

	return;
}


/* Clause 4.6.2.3 */
void get_n_string(char *n_string, int x, int y)
{
	int length;
	int i;

	length = x + get_random_int(y - x + 1) + 1;
	n_string[length - 1] = '\0';

	for (i = 0; i < length - 1; i++)
	{
		n_string[i] = n_string_char[get_random_int(N_STRING_CHAR_LEN)];
	}

	return;
}


/* Clause 2.1.13 */
int get_nu_rand(int a, int x, int y)
{
	return ((get_random_int((a + 1)) | (x + (int) get_random_int((y + 1)))) % (y - x + 1)) + x;
}


/* Return a number from 0 to max - 1. */
int get_random_int(int max)
{
	return rand() %  max;
}

int log_error_message(char *filename, int line, const char *fmt, ...)
{
	va_list fmtargs;
	time_t t;
	FILE *of = (log_error) ? log_error: stderr;

	/* Print the error message(s) */
	t = time(NULL);
	va_start(fmtargs, fmt);

	pthread_mutex_lock(&mutex_error_log);
	fprintf(of, "%s%d\n%s:%d\n", ctime(&t), (int) pthread_self(), filename,
		line);
	va_start(fmtargs, fmt);
	vfprintf(of, fmt, fmtargs);
	va_end(fmtargs);
	fprintf(of, "\n");
	fflush(log_error);
	pthread_mutex_unlock(&mutex_error_log);

	return OK;
}

#ifdef DEBUG
int log_debug_message(char *filename, int line, const char *fmt, ...)
{
	va_list fmtargs;
	time_t t;
	FILE *of = (log_debug) ? log_debug: stderr;

	/* Print the error message(s) */
	t = time(NULL);
	va_start(fmtargs, fmt);

	pthread_mutex_lock(&mutex_debug_log);
	fprintf(of, "%s%s:%d\n", ctime(&t), filename, line);
	va_start(fmtargs, fmt);
	vfprintf(of, fmt, fmtargs);
	va_end(fmtargs);
	fprintf(of, "\n");
	fflush(log_debug);
	pthread_mutex_unlock(&mutex_debug_log);

	return OK;
}
#endif /* DEBUG */

double time_diff(struct timeval start_time, struct timeval end_time)
{
	return (end_time.tv_sec-start_time.tv_sec)+(double)(end_time.tv_usec-start_time.tv_usec)/1000000.00;
}

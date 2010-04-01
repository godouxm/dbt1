/*
 * common.h
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 *
 * 10 janurary 2002
 */

#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <sys/time.h>

#define ADMIN_CONFIRM 0
#define ADMIN_REQUEST 1
#define BEST_SELLERS 2
#define BUY_CONFIRM 3
#define BUY_REQUEST 4
#define CUSTOMER_REGISTRATION 5
#define HOME 6
#define NEW_PRODUCTS 7
#define ORDER_DISPLAY 8
#define ORDER_INQUIRY 9
#define PRODUCT_DETAIL 10
#define SEARCH_REQUEST 11
#define SEARCH_RESULTS 12
#define SHOPPING_CART 13
#define INTERACTION_TOTAL 14

#define SOCKET_CLOSE 0
#define ERROR -1
#define W_ERROR -1
#define OK 1
#define W_ZERO_ITEMS 2


#define FALSE 0
#define TRUE 1


#define UNKNOWN_CUSTOMER 0
#define UNKNOWN_SHOPPING_CART 0

#define RETRY 3

#define DATE_LEN 10 /* 'YYYY-MM-DD' */
#define TIMESTAMP_LEN 28 /* 'YYYY-MM-DD HH:MM:SS.SSSSSS' */

#define A_STRING_CHAR_LEN 89
#define N_STRING_CHAR_LEN 10

/* Author */
#define A_BIO_LEN 500
#define A_FNAME_LEN 20
#define A_LNAME_LEN 20
#define A_MNAME_LEN 20

/* Address */
#define ADDR_CITY_LEN 30
#define ADDR_STATE_LEN 20
#define ADDR_STREET1_LEN 40
#define ADDR_STREET2_LEN 40
#define ADDR_ZIP_LEN 10

/* County */
#define CO_ID_MAX 92
#define CO_NAME_LEN 50

/* Credit Card Transactions */
#define CX_TYPE_MAX 5
#define CX_AUTH_ID_LEN 15
#define CX_EXPIRY_LEN DATE_LEN
#define CX_NAME_LEN 31
#define CX_NUM_LEN 16
#define CX_TYPE_LEN 10

/* Customer */
#define C_BIRTHDATE_LEN DATE_LEN
#define C_DATA_LEN 500
#define C_EMAIL_LEN 50
#define C_FNAME_LEN 15
#define C_LNAME_LEN 15
#define C_PASSWD_LEN 20
#define C_PHONE_LEN 16
#define C_UNAME_LEN 20

/* Item */
#define I_BACKING_MAX 5
#define I_SUBJECT_MAX 24
#define I_AVAIL_LEN DATE_LEN
#define I_BACKING_LEN 15
#define I_DESC_LEN 500
#define I_DIMENSIONS_LEN 25
#define I_ISBN_LEN 13
#define I_SUBJECT_LEN 60
#define I_TITLE_LEN 60
#define I_PUB_DATE_LEN DATE_LEN
#define I_PUBLISHER_LEN 60

/* Order Line */
#define OL_COMMENT_LEN 100

/* Orders */
#define O_SHIP_TYPE_MAX 6
#define O_STATUS_MAX 4
#define O_DATE_LEN TIMESTAMP_LEN
#define O_SHIP_DATE_LEN TIMESTAMP_LEN
#define O_SHIP_TYPE_LEN 10
#define O_STATUS_LEN 15

#define SEARCH_RESULT_ITEMS_MAX 50
#define SEARCH_TYPE_MAX 3
#define SEARCH_AUTHOR 0
#define SEARCH_SUBJECT 1
#define SEARCH_TITLE 2
#define SEARCH_STRING_LEN I_TITLE_LEN
#define SEARCH_TYPE_LEN 7

#define SHOPPING_CART_ITEMS_MAX 20

#define PROMOTIONAL_ITEMS_MAX 5

#define LOG_ERROR_MESSAGE(arg...) log_error_message(__FILE__, __LINE__, ## arg)

#ifdef DEBUG
#define DEBUGMSG(arg...) log_debug_message(__FILE__, __LINE__, ## arg)
#endif

/* Macros for marking the log files. */
#define RUN_START "START"
#define RUN_END "END"

/* Prototypes */

int init_common();
void digsyl2(char *string, long long d, long long n);
double get_percentage();
void get_a_string(char *a_string, int x, int y);
void get_n_string(char *n_string, int x, int y);
int get_nu_rand(int a, int x, int y);
int get_random_int(int max);
int log_error_message(char *filename, int line, const char *fmt, ...);
int log_debug_message(char *filename, int line, const char *fmt, ...);
double time_diff(struct timeval start_time, struct timeval end_time);

/* Global Variables */

extern const char *a_string_char;
extern const char *n_string_char;
extern const char *co_name[CO_ID_MAX];
extern const char *cx_type[CX_TYPE_MAX];
extern const char *i_backing[I_BACKING_MAX];
extern const char *i_subject[I_SUBJECT_MAX];
extern const char *o_ship_type[O_SHIP_TYPE_MAX];
extern const char *o_status[O_STATUS_MAX];
extern const char *search_type[SEARCH_TYPE_MAX];
extern const char *syl[10];
extern int item_count;
extern const char *interaction_short_name[INTERACTION_TOTAL];
extern const char *interaction_name[INTERACTION_TOTAL];
extern char output_path[256];

#endif /* _COMMON_H_ */

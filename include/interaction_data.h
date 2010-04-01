/*
 * interaction_data.h
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 *
 */

#ifndef _INTERACTION_DATA_H_
#define _INTERACTION_DATA_H_

#include <common.h>

struct promotional_processing_t
{
	long long i_id;
	long long i_related[PROMOTIONAL_ITEMS_MAX];
	long long i_thumbnail[PROMOTIONAL_ITEMS_MAX];
};

struct shopping_cart_refresh_t
{
	long long scl_i_id;
	short int scl_qty;
};

struct shopping_cart_line_t
{
	long long scl_i_id;
	short int scl_qty;
	double scl_cost;
	double scl_srp;
	char i_title[I_TITLE_LEN + 1];
	char i_backing[I_BACKING_LEN + 1];
};

struct address_t
{
	char addr_street1[ADDR_STREET1_LEN + 1];
	char addr_street2[ADDR_STREET2_LEN + 1];
	char addr_city[ADDR_CITY_LEN + 1];
	char addr_state[ADDR_STATE_LEN + 1];
	char addr_zip[ADDR_ZIP_LEN + 1];
	char co_name[CO_NAME_LEN + 1];
};

struct search_results_line_t
{
	long long i_id;
	char i_title[I_TITLE_LEN + 1];
	char a_fname[A_FNAME_LEN + 1];
	char a_lname[A_LNAME_LEN + 1];
};

/* Clause 2.16 */
struct admin_confirm_t
{
	/* Input data. */
	long long i_id;

	/* Input and output data. */
	long long i_image;
	long long i_thumbnail;
	double i_cost;

	/* Output data. */
	char i_title[I_TITLE_LEN + 1];
	char a_fname[A_FNAME_LEN + 1];
	char a_lname[A_LNAME_LEN + 1];
	char i_subject[I_SUBJECT_LEN + 1];
	char i_desc[I_DESC_LEN + 1];
	double i_srp;
	char i_backing[I_BACKING_LEN + 1];
	short int i_page;
	char i_publisher[I_PUBLISHER_LEN + 1];
	char i_pub_date[I_PUB_DATE_LEN + 1];
	char i_dimensions[I_DIMENSIONS_LEN + 1];
	char i_isbn[I_ISBN_LEN + 1];
};

/* Clause 2.15 */
struct admin_request_t
{
	/* Input data. */
	long long i_id;

	/* Output data. */
	double i_srp;
	double i_cost;
	char i_title[I_TITLE_LEN + 1];
	long long i_image;
	long long i_thumbnail;
	char a_fname[A_FNAME_LEN + 1];
	char a_lname[A_LNAME_LEN + 1];
};

/* Clause 2.13 */
struct best_sellers_t
{
	/* Input data. */
	char i_subject[I_SUBJECT_LEN + 1];

	/* Output data. */
	short int items;
	struct search_results_line_t results_data[SEARCH_RESULT_ITEMS_MAX];
	struct promotional_processing_t pp_data;
};

/* Clause 2.7 */
struct buy_confirm_t
{
	/* Input data. */
	long long sc_id;
	long long c_id;
	char cx_type[CX_TYPE_LEN + 1];
	char cx_num[CX_NUM_LEN + 1];
	char cx_name[CX_NAME_LEN + 1];
	char cx_expiry[CX_EXPIRY_LEN + 1];
	char o_ship_type[O_SHIP_TYPE_LEN + 1];
	short int update_address;
	struct address_t shipping;

	/* Output data. */
	long long o_id;
	short int sc_size;
	struct shopping_cart_line_t scl_data[SHOPPING_CART_ITEMS_MAX];
	double c_discount;
	double sc_sub_total;
	double sc_tax;
	double sc_ship_cost;
	double sc_total;
};

/* Clause 2.6 */
struct buy_request_t
{
	/* Input data. */
	short int returning_flag;
	char c_uname[C_UNAME_LEN + 1];
	char c_passwd[C_PASSWD_LEN + 1];
	long long sc_id;
	double c_discount;
	char c_fname[C_FNAME_LEN + 1];
	char c_lname[C_LNAME_LEN + 1];
	struct address_t address;
	char c_phone[C_PHONE_LEN + 1];
	char c_email[C_EMAIL_LEN + 1];
	char c_birthdate[C_BIRTHDATE_LEN + 1];
	char c_data[C_DATA_LEN + 1];

	/* Output data. */
	short int sc_size;
	struct shopping_cart_line_t scl_data[SHOPPING_CART_ITEMS_MAX];
	double sc_sub_total;
	double sc_tax;
	double sc_ship_cost;
	double sc_total;
	long long c_id;
};

/* Clause 2.3 */
struct home_t
{
	/* Input data. */
	long long c_id;

	/* Output data. */
	char c_fname[C_FNAME_LEN + 1];
	char c_lname[C_LNAME_LEN + 1];
	struct promotional_processing_t pp_data;
};

/* Clause 2.12 */
struct new_products_t
{
	/* Input data. */
	char i_subject[I_SUBJECT_LEN + 1];

	/* Output data. */
	short int items;
	struct search_results_line_t results_data[SEARCH_RESULT_ITEMS_MAX];
	struct promotional_processing_t pp_data;
};

struct order_display_line_t
{
	long long ol_i_id;
	char i_title[I_TITLE_LEN + 1];
	char i_publisher[I_PUBLISHER_LEN + 1];
	double i_cost;
	short int ol_qty;
	double ol_discount;
	char ol_comment[OL_COMMENT_LEN + 1];
};

/* Clause 2.9 */
struct order_display_t
{
	/* Input data. */
	long long clean_order;
	char c_uname[C_UNAME_LEN + 1];
	char c_passwd[C_UNAME_LEN + 1];

	/* Output data. */
	short int items;
	long long o_id;
	long long c_id; /* pgxc additional */
	char c_fname[C_FNAME_LEN + 1];
	char c_lname[C_LNAME_LEN + 1];
	char c_phone[C_PHONE_LEN + 1];
	char c_email[C_EMAIL_LEN + 1];
	char o_date[O_DATE_LEN + 1];
	double o_sub_total;
	double o_tax;
	double o_total;
	char o_ship_type[O_SHIP_TYPE_LEN + 1];
	char o_ship_date[O_SHIP_DATE_LEN + 1];
	char o_status[O_STATUS_LEN + 1];
	struct address_t billing;
	struct address_t shipping;
	struct order_display_line_t odl_data[SHOPPING_CART_ITEMS_MAX];
	char cx_type[CX_TYPE_LEN + 1];
	char cx_auth_id[CX_AUTH_ID_LEN + 1];
};

/* Clause 2.8 */
struct order_inquiry_t
{
	/* Input data. */
	long long c_id;

	/* Output data. */
	char c_uname[C_UNAME_LEN + 1];
};

/* Clause 2.14 */
struct product_detail_t
{
	/* Input data. */
	long long i_id;

	/* Output data. */
	char i_title[I_TITLE_LEN + 1];
	char a_fname[A_FNAME_LEN + 1];
	char a_lname[A_LNAME_LEN + 1];
	char i_pub_date[I_PUB_DATE_LEN + 1];
	char i_publisher[I_PUBLISHER_LEN + 1];
	char i_subject[I_SUBJECT_LEN + 1];
	char i_desc[I_DESC_LEN + 1];
	long long i_image;
	double i_cost;
	double i_srp;
	char i_avail[I_AVAIL_LEN + 1];
	char i_isbn[I_ISBN_LEN + 1];
	short int i_page;
	char i_backing[I_BACKING_LEN + 1];
	char i_dimensions[I_DIMENSIONS_LEN + 1];
};

/* Clause 2.10 */
struct search_request_t
{
	/* Output data. */
	struct promotional_processing_t pp_data;
};

/* Clause 2.11 */
struct search_results_t
{
	/* Input data. */
	short int search_type;
	char search_string[SEARCH_STRING_LEN + 1];

	/* Output data. */
	short int items;
	struct search_results_line_t results_data[SEARCH_RESULT_ITEMS_MAX];
	struct promotional_processing_t pp_data;
};

/* Clause 2.4 */
struct shopping_cart_t
{
	/* Input data. */
	short int add_flag;
	long long i_id;
	long long c_id;
	struct shopping_cart_refresh_t sc_refresh[SHOPPING_CART_ITEMS_MAX];

	/* Input and output data. */
	long long sc_id;
	short int sc_size;
	struct shopping_cart_line_t scl_data[SHOPPING_CART_ITEMS_MAX];
	struct promotional_processing_t pp_data;
};

union interaction_data_t
{
	struct admin_confirm_t admin_confirm_data;
	struct admin_request_t admin_request_data;
	struct best_sellers_t best_sellers_data;
	struct buy_confirm_t buy_confirm_data;
	struct buy_request_t buy_request_data;
	struct home_t home_data;
	struct new_products_t new_products_data;
	struct order_display_t order_display_data;
	struct order_inquiry_t order_inquiry_data;
	struct product_detail_t product_detail_data;
	struct search_request_t search_request_data;
	struct search_results_t search_results_data;
	struct shopping_cart_t shopping_cart_data;
};

#endif /* _INTERACTION_DATA_H_ */

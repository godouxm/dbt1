/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2004 Mark Wong & Open Source Development Labs, Inc.
 *
 * 8 May 2004
 */

#ifndef _INTERACTION_H_
#define _INTERACTION_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>
#include <cstdlib>

using namespace std;

class Interaction
{
public:
    Interaction(int seed = 0);
    int nextInteraction(int previousInteraction);

    // Constants
    static const int ADMIN_CONFIRM = 0;
    static const int ADMIN_REQUEST = 1;
    static const int BEST_SELLERS = 2;
    static const int BUY_CONFIRM = 3;
    static const int BUY_REQUEST = 4;
    static const int CUSTOMER_REGISTRATION = 5;
    static const int HOME = 6;
    static const int NEW_PRODUCTS = 7;
    static const int ORDER_DISPLAY = 8;
    static const int ORDER_INQUIRY = 9;
    static const int PRODUCT_DETAIL = 10;
    static const int SEARCH_REQUEST = 11;
    static const int SEARCH_RESULT = 12;
    static const int SHOPPING_CART = 13;

    static const int SEARCH_AUTHOR = 0;
    static const int SEARCH_SUBJECT = 1;
    static const int SEARCH_TITLE = 2;

    static const int ADDR_CITY_LEN = 30;
    static const int ADDR_STATE_LEN = 20;
    static const int ADDR_STREET1_LEN = 40;
    static const int ADDR_STREET2_LEN = 40;
    static const int ADDR_ZIP_LEN = 10;
    static const int C_DATA_LEN = 500;
    static const int C_EMAIL_LEN = 50;
    static const int C_FNAME_LEN = 15;
    static const int C_LNAME_LEN = 15;
    static const int C_PHONE_LEN = 16;

    string co_name[92];
    string cx_type[5];
    string interactionName[14];
    string i_subject[24];
    string o_ship_type[5];
    string searchType[3];

private:
    int mixMatrix[14][14];
};

#endif // _INTERACTION_H_

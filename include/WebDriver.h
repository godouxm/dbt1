/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2004 Mark Wong & Open Source Development Labs, Inc.
 *
 * 9 May 2004
 */

#ifndef _WEBDRIVER_H_
#define _WEBDRIVER_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "WebClient.h"
#include "Interaction.h"

class WebDriver : public WebClient
{
public:
	static const int retryMax = 10;

	WebDriver(int id, int items, int customers, string url, time_t stopTime,
		int usmd, int thinktime);
	~WebDriver();
	void start();

private:
string astring(int min, int max);
double difftimeval(struct timeval *end, struct timeval *start);
	int doAdminConfirm();
	int doAdminRequest();
	int doBestSellers();
	int doBuyConfirm();
	int doBuyRequest();
	int doCustomerRegistration();
	int doHome();
	int doNewProducts();
	int doOrderDisplay();
	int doOrderInquiry();
	int doProductDetail();
	int doSearchRequest();
	int doSearchResult();
	int doShoppingCart();
	string digsyl(long long d, long long n);
	string escape(string html);
	int getCid();
	string getProductDetailIid();
	void getProductLinks();
	string getRefreshRequest();
	int getScid();
	double getThinktime();
	double getUsmd();
	string nstring(int min, int max);
	int nuRand(int a, int x, int y);

	int a;
	int items;
	int customers;

	Interaction *interaction;

	time_t stopTime;
	int averageThinktime;
	int averageUsmd;

	int (WebDriver::*interactionFunction[14])();

	string baseUrl;
	string request;
	int status;

	int lastInteraction;
	int currentInteraction;

	string c_id;
	string sc_id;

	vector<string> productLinks;

	string syllable[10];
	string aStringChar;
	string nStringChar;
};

#endif // _WEBDRIVER_H_

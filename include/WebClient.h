/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2004 Mark Wong & Open Source Development Labs, Inc.
 *
 * 7 May 2004
 */

////
// WebClient.h
//
// 5 October 2000
// Mark Wong (mwong@cse.ogi.edu)
////

#ifndef _WEBCLIENT_H
#define _WEBCLIENT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

#include "SocketTable.h"

using namespace std;

#define URL_PREFIX "http://"
#define CR "\r"
#define LF "\n"
#define CRLF "\r\n"
#define CRLFCRLF "\r\n\r\n"
#define SP " "

class WebClient
{
public:
	// Contructors.
	WebClient(int id);

	// Method prototypes.
	string getHttpBody();
	string getHttpHeader();
	int httpGet(string url);
	void verbose(int level);

protected:
	int verboseFlag;
	ofstream logfile;    
	string httpHeader;
	string httpBody;
	vector<string> links;

	vector<string> *getImgLinks(string httpBody);
	void getLinks();

private:
    // Constants.
	static const int HOSTNAME_LEN = 64;
	static const int MSS = 1460;

	SocketTable socketTable;
	char hostname[HOSTNAME_LEN];
	string currentServerAddress;
	int *currentSocket;
	string httpReferrer;

	int imageFlag;
};

#endif // _WEBCLIENT_H

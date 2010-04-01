/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2004 Mark Wong & Open Source Development Labs, Inc.
 *
 * 7 May 2004
 */

////
// SocketTable.h
//
// 12 November 2000
// Mark Wong (mwong@cse.ogi.edu)
////

#ifndef _SOCKETTABLE_H
#define _SOCKETTABLE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <string>
#include <map>

#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

class SocketTable
{
public:
	int closeSocket(string server_addr);
	int *getSocket(string server_addr);

private:
	int connectSocket(string server_addr);

	map<string, int> socketTable;
};

#endif // _SOCKETTABLE_H

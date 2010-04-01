/*
 * _socket.c
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 *
 * 20 march 2002
 */

#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <_socket.h>

pthread_mutex_t mutex_hostent = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_protoent = PTHREAD_MUTEX_INITIALIZER;

int _connect(char *address, unsigned short port)
{
	int sockfd = 0;
	struct sockaddr_in sa;
	struct hostent *he;
	in_addr_t addr;

	bzero(&sa, sizeof(struct sockaddr_in));

	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	if (sa.sin_port == 0)
	{
		return -1;
	}

	/* Assume that an IP address is used. */
	pthread_mutex_lock(&mutex_hostent);
	addr = inet_addr(address);
	if (addr == INADDR_NONE)
	{
		/* If it is not an IP address, assume it is a hostname. */
		if ((he = gethostbyname(address)) == NULL)
		{
			close(sockfd);
			pthread_mutex_unlock(&mutex_hostent);
			return -1;
		}
	}
	else
	{
		/* Continue the assumption that an IP address is used. */
		if ((he = gethostbyaddr((char *) (&addr), sizeof(addr), AF_INET)) == NULL)
		{
			close(sockfd);
			pthread_mutex_unlock(&mutex_hostent);
			return -1;
		}
		memcpy(&sa.sin_addr, &addr, sizeof(addr));
	}
	memcpy(&sa.sin_addr, he->h_addr_list[0], he->h_length);
	pthread_mutex_unlock(&mutex_hostent);

	pthread_mutex_lock(&mutex_protoent);
	sockfd = socket(PF_INET, SOCK_STREAM, resolveproto("tcp"));
	pthread_mutex_unlock(&mutex_protoent);
	if (sockfd == -1)
	{
		return sockfd;
	}

	if (connect(sockfd, (struct sockaddr *) &sa, sizeof(struct sockaddr_in)) <0)
		return -1;

	return sockfd;
}

int _receive(int s, void *data, int length)
{
	int received, total, remaining;
	remaining = length;
	total = 0;
	do
	{
		received = recv(s, data, remaining, 0);
		if (received == -1)
		{
			return -1;
		}
		if (received == 0)
		{
			return 0;
		}
		total += received;
		data += received;
		remaining -= received;
	} while (total != length);
	return total;
}

int _send(int s, void *data, int length)
{
	int sent = 0;
	int remaining = length;
	do
	{
		sent = send(s, (void *) data, remaining, 0);
		if (sent == -1)
		{
			return -1;
		}
		data += sent;
		remaining -= sent;
	} while (sent != length);
	return sent;
}

int _server_init_socket(int port)
{
	struct sockaddr_in sa;
	int mastersock;

	bzero(&sa, sizeof(struct sockaddr_in));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = INADDR_ANY;
	sa.sin_port=htons((u_short)port);

	pthread_mutex_lock(&mutex_protoent);
	mastersock = socket(PF_INET, SOCK_STREAM, resolveproto("TCP"));
	pthread_mutex_unlock(&mutex_protoent);

	if (mastersock < 0) 
	{
		return ERR_SOCKET_CREATE;
	}

	if (bind(mastersock, (struct sockaddr *)&sa, sizeof(struct sockaddr_in)) <0)
	{
		return ERR_SOCKET_BIND;
	}

	if (listen(mastersock, 1) < 0)
	{
		return ERR_SOCKET_LISTEN;
	}
	return mastersock;
}

int resolveproto(const char *proto)
{
	struct protoent *protocol;
	protocol = getprotobyname(proto);
	if (!protocol) {
		return ERR_SOCKET_RESOLVPROTO;
	}

	return protocol->p_proto;
}

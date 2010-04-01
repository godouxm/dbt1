/*
 * DigSyl.c
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 * Copyright (C) 2005 Koji Fukui
 *                    Sumisho Computer Systems Corporation
 * Copyright (C) 2005 Masaki.Kuroda & Shuuichi Suzuki
 *                    UNIADEX, Ltd. 
 *
 */

#include <string.h>
#include <interaction.h>
#include <DigSyl.h>

int DigSyl(int D,int N,char *resstr)
{
	int digits = 0;
	int base = D;
	int ix;
	int start;
	char *basestr = "BAOGALRIRESEATULINNG";

	while (base > 0)
	{
		digits++;
		base /= 10;
	}

	if (digits > N)
	{
		N = digits;
	}

	base = 1;
	ix = 0;
	while (ix < N-1)
	{
		base *= 10;
		ix++;
	}
	ix = 0;

	while (ix < N)
	{
		digits = D / base;
		start = digits*2 + 1;
		strncat(resstr,(char *)(basestr + start - 1),2);
		D = D % base;
		base /= 10;
		ix++;
	}

	return OK;
}

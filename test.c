/*
 * mrbuffer.c	magic ring buffer
 *
 * Copyright (C) 2016 Seongmyun Cho <highsky@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2 of the License.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <stdio.h>
#include <string.h>
#include "mrbuffer.h"

#define LEN	(1024*3-256)
char data[LEN];

int main(int argc, char *argv[])
{
	struct mrbuffer *mrbuf;
	size_t len;
	int i;

	for (i = 0; i < LEN; i++)
		data[i] = i & 0xff ;

	mrbuf = mrbuffer_alloc(MRBUF_FLAG_SHMAT, 0);
//	mrbuf = mrbuffer_alloc(MRBUF_FLAG_MMAP, 0);

	len = mrbuffer_write(mrbuf, LEN, data);
	printf("h:%04lu t:%04lu %04lu\n", \
			MRBUF_WRITE_OFFSET(mrbuf), \
			MRBUF_READ_OFFSET(mrbuf), \
			MRBUF_FREE_LEN(mrbuf));

	len = mrbuffer_read(mrbuf, LEN, data);
	len = mrbuffer_write(mrbuf, LEN, data);

	printf("h:%04lu t:%04lu %04lu\n", \
			MRBUF_WRITE_OFFSET(mrbuf), \
			MRBUF_READ_OFFSET(mrbuf), \
			MRBUF_FREE_LEN(mrbuf));

	len = mrbuffer_read(mrbuf, LEN, data);
	len = mrbuffer_write(mrbuf, LEN, data);

	printf("h:%04lu t:%04lu %04lu\n", \
			MRBUF_WRITE_OFFSET(mrbuf), \
			MRBUF_READ_OFFSET(mrbuf), \
			MRBUF_FREE_LEN(mrbuf));

	len = mrbuffer_read(mrbuf, LEN, data);
	len = mrbuffer_write(mrbuf, LEN, data);

	printf("h:%04lu t:%04lu %04lu\n", \
			MRBUF_WRITE_OFFSET(mrbuf), \
			MRBUF_READ_OFFSET(mrbuf), \
			MRBUF_FREE_LEN(mrbuf));

	len = mrbuffer_read(mrbuf, LEN, data);
	len = mrbuffer_write(mrbuf, LEN, data);

	printf("h:%04lu t:%04lu %04lu\n", \
			MRBUF_WRITE_OFFSET(mrbuf), \
			MRBUF_READ_OFFSET(mrbuf), \
			MRBUF_FREE_LEN(mrbuf));

	len = mrbuffer_write(mrbuf, LEN, data);
	printf("h:%04lu t:%04lu %04lu\n", \
			MRBUF_WRITE_OFFSET(mrbuf), \
			MRBUF_READ_OFFSET(mrbuf), \
			MRBUF_FREE_LEN(mrbuf));

	len = mrbuffer_write(mrbuf, LEN, data);
	printf("h:%04lu t:%04lu %04lu\n", \
			MRBUF_WRITE_OFFSET(mrbuf), \
			MRBUF_READ_OFFSET(mrbuf), \
			MRBUF_FREE_LEN(mrbuf));

	if (MRBUF_FULL(mrbuf))
		printf("buffer full(data len): %lu\n", MRBUF_DATA_LEN(mrbuf));

	len = mrbuffer_read(mrbuf, LEN, data);
	printf("h:%04lu t:%04lu %04lu\n", \
			MRBUF_WRITE_OFFSET(mrbuf), MRBUF_READ_OFFSET(mrbuf), len);
	len = mrbuffer_read(mrbuf, LEN, data);
	printf("h:%04lu t:%04lu %04lu\n", \
			MRBUF_WRITE_OFFSET(mrbuf), MRBUF_READ_OFFSET(mrbuf), len);
	len = mrbuffer_read(mrbuf, LEN, data);
	printf("h:%04lu t:%04lu %04lu\n", \
			MRBUF_WRITE_OFFSET(mrbuf), MRBUF_READ_OFFSET(mrbuf), len);

	if (MRBUF_EMPTY(mrbuf))
		printf("buffer empty(free len): %lu\n", MRBUF_FREE_LEN(mrbuf));

	if (MRBUF_FREE_LEN(mrbuf) >= LEN) {
		printf("write mrbuf using memcpy\n");
		memcpy(MRBUF_WRITE_ADDR(mrbuf), data, LEN);
		/* this must be done to update write pointer after every writes */
		len = mrbuffer_give(mrbuf, LEN);
		printf("h:%04lu t:%04lu %04lu\n", \
			MRBUF_WRITE_OFFSET(mrbuf), \
			MRBUF_READ_OFFSET(mrbuf), \
		   	len);
	}

	if (!memcmp(mrbuf->vaddr, mrbuf->vaddr + mrbuf->size, mrbuf->size))
		printf("mrbuffer is working\n");


	mrbuffer_free(mrbuf);

	return 0;
}

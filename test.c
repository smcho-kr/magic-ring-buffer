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

#define LEN	2123
char data[LEN];

int main(int argc, char *argv[])
{
	struct mrbuffer *mrbuf;
	size_t len;
	int i;

	for (i = 0; i < LEN; i++)
		data[i] = i & 0xff ;

	mrbuf = mrbuffer_alloc(0);

	len = mrbuffer_write(mrbuf, LEN, data);

	len = mrbuffer_bytes_to_write(mrbuf);
	printf("h:%04lu t:%04lu %04lu\n", mrbuf->head, mrbuf->tail, len);

	len = mrbuffer_read(mrbuf, LEN, data);
	len = mrbuffer_write(mrbuf, LEN, data);

	len = mrbuffer_bytes_to_write(mrbuf);
	printf("h:%04lu t:%04lu %04lu\n", mrbuf->head, mrbuf->tail, len);

	len = mrbuffer_read(mrbuf, LEN, data);
	len = mrbuffer_write(mrbuf, LEN, data);

	len = mrbuffer_bytes_to_write(mrbuf);
	printf("h:%04lu t:%04lu %04lu\n", mrbuf->head, mrbuf->tail, len);

	len = mrbuffer_read(mrbuf, LEN, data);
	len = mrbuffer_write(mrbuf, LEN, data);

	len = mrbuffer_bytes_to_write(mrbuf);
	printf("h:%04lu t:%04lu %04lu\n", mrbuf->head, mrbuf->tail, len);

	len = mrbuffer_read(mrbuf, LEN, data);
	len = mrbuffer_write(mrbuf, LEN, data);

	len = mrbuffer_bytes_to_write(mrbuf);
	printf("h:%04lu t:%04lu %04lu\n", mrbuf->head, mrbuf->tail, len);

	len = mrbuffer_read(mrbuf, LEN, data);
	printf("h:%04lu t:%04lu %04lu\n", mrbuf->head, mrbuf->tail, len);
	len = mrbuffer_read(mrbuf, LEN, data);
	printf("h:%04lu t:%04lu %04lu\n", mrbuf->head, mrbuf->tail, len);
	len = mrbuffer_read(mrbuf, LEN, data);
	printf("h:%04lu t:%04lu %04lu\n", mrbuf->head, mrbuf->tail, len);

	if (MRBUF_EMPTY(mrbuf))
		printf("empty\n");

	len = mrbuffer_write(mrbuf, LEN, data);
	printf("h:%04lu t:%04lu %04lu\n", mrbuf->head, mrbuf->tail, len);

	len = mrbuffer_write(mrbuf, LEN, data);
	printf("h:%04lu t:%04lu %04lu\n", mrbuf->head, mrbuf->tail, len);

	len = mrbuffer_write(mrbuf, LEN, data);
	printf("h:%04lu t:%04lu %04lu\n", mrbuf->head, mrbuf->tail, len);

	len = mrbuffer_write(mrbuf, LEN, data);
	printf("h:%04lu t:%04lu %04lu\n", mrbuf->head, mrbuf->tail, len);

	if (MRBUF_FULL(mrbuf))
		printf("full\n");

	memcpy(mrbuf->vaddr + (mrbuf->size >> 1), data, LEN);

	if (!memcmp(mrbuf->vaddr, mrbuf->vaddr + mrbuf->size, mrbuf->size))
		printf("mrbuffer is working\n");


	mrbuffer_free(mrbuf);

	return 0;
}

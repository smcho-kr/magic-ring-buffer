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

#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mrbuffer.h"

#ifndef min
#define min(x, y) (((x) < (y)) ? (x) : (y))
#endif

struct mrbuffer *mrbuffer_alloc(unsigned int order)
{
	struct mrbuffer *mrbuf;
	long page_size = sysconf(_SC_PAGE_SIZE);;

	mrbuf = malloc(sizeof(struct mrbuffer));
	if (mrbuf == NULL)
		goto mrbuf_error;

	mrbuf->order = order;
	mrbuf->size = (1UL << order) * page_size;

	mrbuf->vaddr = mmap(NULL, mrbuf->size << 1, PROT_READ | PROT_WRITE, \
			MAP_ANONYMOUS | MAP_SHARED, -1, 0);
	if (mrbuf->vaddr == MAP_FAILED)
		goto mmap_error;

	if (remap_file_pages(mrbuf->vaddr + mrbuf->size, mrbuf->size, 0, 0, 0))
		goto remap_error;

	mrbuf->tail = 0UL;
	mrbuf->head = 0UL;

	return mrbuf;

remap_error:
	munmap(mrbuf->vaddr, mrbuf->size << 1);

mmap_error:
	free(mrbuf);

mrbuf_error:
	return NULL;
}

void mrbuffer_free(struct mrbuffer *mrbuf)
{
	if (!mrbuf)
		return;
	
	munmap(mrbuf->vaddr, mrbuf->size << 1);
	free(mrbuf);
}

size_t mrbuffer_bytes_to_write(struct mrbuffer *mrbuf)
{
	size_t tail = mrbuf->tail;
	size_t head = mrbuf->head;
	size_t size = mrbuf->size;

	return (size - head + tail - 1UL) % size;
}

size_t mrbuffer_bytes_to_read(struct mrbuffer *mrbuf)
{
	size_t tail = mrbuf->tail;
	size_t head = mrbuf->head;
	size_t size = mrbuf->size;

	return (size - tail + head) % size;
}

size_t mrbuffer_write(struct mrbuffer *mrbuf, size_t len, void *data)
{
	size_t tail = mrbuf->tail;
	size_t head = mrbuf->head;
	size_t size = mrbuf->size;
	size_t bytes_to_write;

	bytes_to_write = (size - head + tail - 1UL) % size;

	if (bytes_to_write == 0UL)
		return 0UL;

	bytes_to_write = min(bytes_to_write, len);

	memcpy(mrbuf->vaddr + head, data, bytes_to_write);

	mrbuf->head = (head + bytes_to_write) % size;

	return bytes_to_write;
}

size_t mrbuffer_read(struct mrbuffer *mrbuf, size_t len, void *data)
{
	size_t tail = mrbuf->tail;
	size_t head = mrbuf->head;
	size_t size = mrbuf->size;
	size_t bytes_to_read;

	bytes_to_read = (size - tail + head) % size;

	if (bytes_to_read == 0UL)
		return 0UL;

	bytes_to_read = min(bytes_to_read, len);

	memcpy(data, mrbuf->vaddr + tail, bytes_to_read);

	mrbuf->tail = (tail + bytes_to_read) % size;

	return bytes_to_read;
}

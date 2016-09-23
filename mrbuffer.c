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
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mrbuffer.h"

#ifndef min
#define min(x, y) (((x) < (y)) ? (x) : (y))
#endif

/* create mrbuffer whose size equals to "PAGE_SIZE x (2 ^ order)" */
struct mrbuffer *mrbuffer_alloc(unsigned int flags, unsigned int order)
{
	struct mrbuffer *mrbuf;
	long page_size;
	int shm_id;

	if (!(flags & ( MRBUF_FLAG_MMAP | MRBUF_FLAG_SHMAT)))
		return NULL;

#ifdef _SC_PAGESIZE
    page_size = sysconf( _SC_PAGESIZE );
#else
    page_size = getpagesize();
#endif

	mrbuf = malloc(sizeof(struct mrbuffer));
	if (mrbuf == NULL)
		goto mrbuf_error;

	mrbuf->order = order;
	mrbuf->size = (1UL << order) * page_size;
	mrbuf->tail = 0UL;
	mrbuf->head = 0UL;
	mrbuf->flags = flags;

	/* create memory mapping whose size equals to two times
	 * the buffer size */
	mrbuf->vaddr = mmap(NULL, mrbuf->size << 1, PROT_READ | PROT_WRITE, \
			MAP_ANONYMOUS | MAP_SHARED, -1, 0);

	if (mrbuf->vaddr == MAP_FAILED)
		goto mmap_error;

	if (MRBUF_IS_MMAP(mrbuf)) {

		if (remap_file_pages(mrbuf->vaddr + mrbuf->size, \
				mrbuf->size, 0, 0, 0))
			goto remap_error;

	} else if (MRBUF_IS_SHMAT(mrbuf)) {

		shm_id = shmget(IPC_PRIVATE, mrbuf->size, IPC_CREAT | 0700);

		if (shm_id < 0)
			goto shmget_error;

		munmap(mrbuf->vaddr, mrbuf->size << 1);

		if (mrbuf->vaddr != shmat(shm_id, mrbuf->vaddr, 0))
			goto shmat_error;

		if ((mrbuf->vaddr + mrbuf->size) != \
				shmat(shm_id, mrbuf->vaddr + mrbuf->size, 0))
			goto shmat2_error;

		if (shmctl(shm_id, IPC_RMID, NULL) < 0)
			goto shmctl_error;

	}

	return mrbuf;

shmctl_error:
	shmdt(mrbuf->vaddr + mrbuf->size);

shmat2_error:
	shmdt(mrbuf->vaddr);
	
shmat_error:
	shmctl(shm_id, IPC_RMID, NULL);
	goto mmap_error;

shmget_error:
remap_error:
	munmap(mrbuf->vaddr, mrbuf->size << 1);

mmap_error:
	memset(mrbuf, 0, sizeof(struct mrbuffer));
	free(mrbuf);

mrbuf_error:
	return NULL;

}

void mrbuffer_free(struct mrbuffer *mrbuf)
{
	if (!mrbuf)
		return;
	
	if (MRBUF_IS_SHMAT(mrbuf)) {
		shmdt(mrbuf->vaddr);
		shmdt(mrbuf->vaddr + mrbuf->size);
	} else if (MRBUF_IS_MMAP(mrbuf)) {
		munmap(mrbuf->vaddr, mrbuf->size << 1);
	}
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

void mrbuffer_give(struct mrbuffer *mrbuf, size_t len)
{
    len = min(len, MRBUF_FREE_LEN(mrbuf));
    mrbuf->head = (mrbuf->head + len) % mrbuf->size;
}

void mrbuffer_take(struct mrbuffer *mrbuf, size_t len)
{
    len = min(len, MRBUF_DATA_LEN(mrbuf));
    mrbuf->tail = (mrbuf->tail + len) % mrbuf->size;
}


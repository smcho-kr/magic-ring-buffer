#ifndef _MRBUFFER_H
#define _MRBUFFER_H

#include <sys/types.h>

struct mrbuffer {
	size_t head;
	size_t tail;
	size_t size;
	unsigned int order;
	void *vaddr;
};

#define MRBUF_FREE_LEN(x) ((x->size - x->head + x->tail - 1UL) % x->size)
#define MRBUF_DATA_LEN(x) ((x->size - x->tail + x->head) % x->size)
#define MRBUF_READ_ADDR(x) (x->vaddr + x->tail)
#define MRBUF_WRITE_ADDR(x) (x->vaddr + x->head)
#define MRBUF_FULL(x) (!MRBUF_FREE_LEN(x))
#define MRBUF_EMPTY(x) (!MRBUF_DATA_LEN(x))

struct mrbuffer *mrbuffer_alloc(unsigned int order);
void mrbuffer_free(struct mrbuffer *mrbuf);
size_t mrbuffer_bytes_to_write(struct mrbuffer *mrbuf);
size_t mrbuffer_bytes_to_read(struct mrbuffer *mrbuf);
size_t mrbuffer_write(struct mrbuffer *mrbuf, size_t len, void *data);
size_t mrbuffer_read(struct mrbuffer *mrbuf, size_t len, void *data);

#endif /* _MRBUFFER_H */

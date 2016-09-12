# Linux Magic Ring Buffer

Keywords: magic ring buffer mmap remap_file_pages

### Ring Buffer

Ring buffer is a nice data structure which can be used in many applications.

I believe most of you already know what a ring buffer is and how it is typically implemented.

To use a ring buffer, you have to keep read/write offsets to find where to read/write correctly.

When the write offset becomes larger than the buffer size, it starts again from the zero.

If that happens, you often have to copy the data out to a linear block in memory first because typical library routines are written assuming that data is completely linear in memory.

### The Magic

There is a technique to solve this problem that uses memory mapping function.

The basic concept is use two identical buffers which are adjacent to each other.

Theoretically, you need an infinite number of identical buffers but, practically, only two buffers are enough.

To implement two adjacent indentical buffers, you actually need to have only one buffer and map it on two different virtual addresses.

If you google about "magic ring buffer", you can find several implementations.

I wrote my own code because all the implementations which work on Linux platform need temporary files, which degrades performance.

The only drawback is that it works only on Linux platform bacause other Unix platforms don't provide a function that is equivalent to 'remap_file_pages' in Linux. 

I hope you find it useful. :)

# Linux Magic Ring Buffer

Keywords: magic ring buffer mmap remap_file_pages

### Ring Buffer

Ring buffer is a nice data structure which can be used in many applications.

I believe most of you already know what a ring buffer is and how it is typically implemented.

One problem about ring buffers is that they can't always exist on linear address space in memory.

You often have to copy the data out to a linear block in memory first because typical library routines are written assuming that data is completely linear in memory.

### The Magic

There is a technique to solve this problem that uses memory mapping function.

The basic concept is use two identical buffers which are adjacent to each other.

Theoretically, you need an infinite number of identical buffers but, practically, only two buffers are enough.

To implement two adjacent indentical buffers, you actually need to have only one buffer and map it on two different virtual addresses.

If you google about "magic ring buffer", you can find several implementations.

I made an another one because all the implementations which can work on Linux platfrom need temporary files, which degrades performance.

The only drawback is that it works only on Linux platform bacases other Unix platforms doesn't provide a function that is equivalent to 'remap_file_pages' in Linux.

I hope you find it useful. :)

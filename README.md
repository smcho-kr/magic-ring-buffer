# Linux Magic Ring Buffer

Keywords: magic ring buffer mmap remap_file_pages

### Ring Buffer

Ring buffer is a nice data structure which can be used in many applications.

I believe most of you already know what a ring buffer is and how it is typically implemented.

To use a ring buffer, you have to keep read/write pointers to find where to read/write correctly.

When data is written, the data fills out the buffer and the write pointer advances. The read pointer follows when data is read.

When the write pointer becomes larger than the buffer size, it wraps back to the beginning:

         read
          |   
          v   
    +----------+
    |xxx  xxxxx|
    +----------+
        ^   
        |   
       write


At this point, the data in the ring buffer is no longer contiguous(linear in memory).

When that happens, you often have to copy the data out to a linear block in memory because typical library routines are written assuming that data is completely linear in memory.

### The Magic

There is a technique to solve this problem that uses memory mapping functions.

The basic concept is use **two identical buffers** which are adjacent to each other.

Theoretically, you need an infinite number of identical buffers but, practically, only two buffers are enough. 

To implement two adjacent indentical buffers, you actually need to create only one buffer and map it on two different virtual addresses.

If you google about "magic ring buffer", you can find a few implementations.

All the implementations I found which work on \*nix platform need to create temporary files to *mmap*, which causes performance degradation.

So, I made my own ring buffer which doesn't need any temporary files to *mmap*.

The only drawback is that it works only on Linux platform bacause other \*nix platforms don't provide a function that is equivalent to 'remap_file_pages' in Linux. 

I hope you find it useful. :)

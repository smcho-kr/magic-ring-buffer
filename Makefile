NDEBUG ?= 0

ifeq ($(NDEBUG), 1)
CFLAGS += -ggdb -O0 -DDEBUG
else
CFLAGS += -O2 -s
endif

all: mrbuffer

mmrbuffer.o:.o mrbuffer.o

mrbuffer: test.o mrbuffer.o

clean:
	rm -rf cscope.* *.o mrbuffer

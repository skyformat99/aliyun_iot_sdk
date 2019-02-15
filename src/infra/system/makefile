AR := arm-openwrt-linux-ar
CC := arm-openwrt-linux-gcc
LD := arm-openwrt-linux-ld


INCLS := -I../../../src/infra/utils \
-I../../../src/infra/utils/misc \
-I../../../src/infra/utils/digest \
-I../../../src/infra/log \
-I../../../include \
-I./ \


LIBA_TARGET := libiot_system.a

CFLAGS += -Wall -Wunused -g -O2 
CFLAGS += $(G_CFLAGS)
SOURCE := $(wildcard *.c)
SOURCE += $(wildcard facility/*.c)
OBJS = $(patsubst %.c,%.o,$(SOURCE))  


all:bts toAR 
bts:$(OBJS)  
%.o:%.c  
	$(CC)  -c $(CFLAGS)  $(INCLS) $< -o $@

toAR:
	$(AR) cr $(LIBA_TARGET) $(OBJS)
	
clean:                                                                                        
	rm -rf *.o facility/*.o *.a




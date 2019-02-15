AR := arm-openwrt-linux-ar
CC := arm-openwrt-linux-gcc
LD := arm-openwrt-linux-ld

INCLS := -I../../../../src/infra/utils \
-I../../../../src/infra/utils/misc \
-I../../../../src/infra/utils/digest \
-I../../../../src/infra/log \
-I../../../../src/infra/system \
-I../../../../src/infra/utils \
-I../../../../include \
-I./client \
-I./server \
-I./ \
-I./../cm/include \
-I./../../ota \


LIBA_TARGET := libiot_dm.a

CFLAGS += -Wall -Wunused -g -O2 
CFLAGS += $(G_CFLAGS)

SOURCE := $(wildcard *.c)
SOURCE += $(wildcard client/*.c)
SOURCE += $(wildcard server/*.c)
#SOURCE += $(wildcard $(SOURCE_ROOT)/$(LINKKIT_MODULE)/*/*.c)
#SOURCE := $(foreach S,$(SOURCES),$(subst $(SOURCE_ROOT)/$(LINKKIT_MODULE),.,$(S)))
OBJS = $(patsubst %.c,%.o,$(SOURCE))  


all:bts toAR 
bts:$(OBJS)  
%.o:%.c
	$(CC) -c $(CFLAGS)  $(INCLS) $< -o $@

toAR:
	$(AR) cr $(LIBA_TARGET) $(OBJS)
	
clean:                                                                                        
	rm -rf *.o *.a client/*.o server/*.o




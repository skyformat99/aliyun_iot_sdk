AR := arm-openwrt-linux-ar
CC := arm-openwrt-linux-gcc
LD := arm-openwrt-linux-ld

INCLS := -I../../../src/ref-impl/tls \
-I../../../src/ref-impl/tls/include \
-I../../../src/ref-impl/tls/include/mbedtls \
-I../../../src/ref-impl/tls/include/mbedtls/uOTA_required \
-I../../../src/ref-impl/tls/library \
-I../../../src/ref-impl/tls/library/uOTA_required \
-I../../../src/infra \
-I../../../src/infra/log \
-I../../../src/infra/utils \
-I../../../src/infra/utils/misc \
-I../../../src/infra/utils/digest \
-I../../../src/infra/system \
-I ../../../src/security/pro \
-I../../../src/security/pro/crypto \
-I../../../src/security/pro/id2 \
-I../../../src/security/pro/km \
-I../../../src/security/pro/itls \
-I../../../include \
-I./ \

#INCLS := -I../../../src/infra \
-I../../../src/infra/log \
-I../../../src/infra/utils \
-I../../../src/infra/utils/misc \
-I../../../src/infra/utils/digest \
-I../../../src/infra/system \
-I../../../include \
-I./ \





LIBA_TARGET := libiot_tls.a

CFLAGS += -Wall -Wunused -g -O2 
CFLAGS += $(G_CFLAGS)

#SOURCE = $(wildcard *.c)
SOURCE := $(wildcard library/*.c)
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
	rm -rf *.o *.a library/*.o 




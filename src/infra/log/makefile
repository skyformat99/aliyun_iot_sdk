AR := arm-openwrt-linux-ar
CC := arm-openwrt-linux-gcc
LD := arm-openwrt-linux-ld

INCLUDES := -I../../../src/infra/utils \
-I../../../src/infra/utils/misc \
-I../../../src/infra/utils/digest \
-I../../../include \
-I./ \


LIBA_TARGET := libiot_log.a
all:
	$(CC) -c iotx_log.c $(INCLUDES) $(G_CFLAGS)
	$(AR) cr $(LIBA_TARGET) iotx_log.o
clean:
	rm -rf *.o *.a




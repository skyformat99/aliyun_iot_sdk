AR := arm-openwrt-linux-ar
CC := arm-openwrt-linux-gcc
LD := arm-openwrt-linux-ld

INCLS := -I./../include \
-I./ \
-I./../src/infra/log \
-I./../src/infra/system \
-I./../src/infra/utils \



CFLAGS += -Wall -Wunused -g -O2 

LIBS := -L./../src/protocol/mqtt -liot_mqtt
LIBS += ./../src/sdk-impl/libsdk_impl.a
LIBS += ./../src/services/linkkit/dm/libiot_dm.a
LIBS += ./../src/services/linkkit/cm/libiot_cm.a
LIBS += ./../src/services/ota/libiot_ota.a
LIBS += -L./../src/infra/system -liot_system
LIBS += ./../src/infra/utils/libiot_utils.a
LIBS += ./../src/ref-impl/hal/libiot_hal.a
LIBS += -L./../src/ref-impl/tls -liot_tls
LIBS += ./../src/infra/log/libiot_log.a
LIBS += -lpthread -lrt



#SOURCE = $(wildcard *.c)
#SOURCE := $(wildcard client/*.c)
#SOURCE += $(wildcard MQTTPacket/*.c)
#SOURCE += $(wildcard $(SOURCE_ROOT)/$(LINKKIT_MODULE)/*/*.c)
#SOURCE := $(foreach S,$(SOURCES),$(subst $(SOURCE_ROOT)/$(LINKKIT_MODULE),.,$(S)))
#OBJS = $(patsubst %.c,%.o,$(SOURCE))  

SRCS_funjsq_iot_client          := app_entry.c mqtt/mqtt_example_rrpc.c
BIN_TARGET = ali_iot

all:
	$(CC)  $(CFLAGS)  $(SRCS_funjsq_iot_client) -o ./../bin/$(BIN_TARGET)  $(INCLS) $(LIBS)
	
	
clean:                                                                                        
	rm -rf *.o ./../bin/$(BIN_TARGET) 




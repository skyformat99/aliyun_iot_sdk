include make.settings

G_CFLAGS := -D_PLATFORM_IS_LINUX_ -DCOAP_SERV_MULTITHREAD -DOTA_SIGNAL_CHANNEL=1
ifeq ($(FEATURE_MQTT_COMM_ENABLED),y)
G_CFLAGS += -DMQTT_COMM_ENABLED
endif

ifeq ($(FEATURE_MQTT_DIRECT),y)
G_CFLAGS += -DMQTT_DIRECT	
endif

ifeq ($(FEATURE_DEVICE_MODEL_ENABLED),y)
G_CFLAGS += -DDEVICE_MODEL_ENABLED	
endif

ifeq ($(FEATURE_OTA_ENABLED),y)
G_CFLAGS += -DOTA_ENABLED
endif

ifeq ($(FEATURE_SUPPORT_TLS),y)
G_CFLAGS += -DSUPPORT_TLS
endif

all:
	make -C ./src/infra/log G_CFLAGS="$(G_CFLAGS)" 
	make -C ./src/infra/system G_CFLAGS="$(G_CFLAGS)"
	make -C ./src/infra/utils G_CFLAGS="$(G_CFLAGS)"
	make -C ./src/protocol/mqtt G_CFLAGS="$(G_CFLAGS)"
	make -C ./src/ref-impl/hal G_CFLAGS="$(G_CFLAGS)"
	make -C ./src/ref-impl/tls G_CFLAGS="$(G_CFLAGS)"
	make -C ./src/sdk-impl G_CFLAGS="$(G_CFLAGS)"
	make -C ./src/services/ota G_CFLAGS="$(G_CFLAGS)"
	make -C ./src/services/linkkit/dm G_CFLAGS="$(G_CFLAGS)"
	make -C ./src/services/linkkit/cm G_CFLAGS="$(G_CFLAGS)"
	make -C ./examples G_CFLAGS="$(G_CFLAGS)"
clean:
	make -C ./src/infra/log clean
	make -C ./src/infra/system clean
	make -C ./src/infra/utils clean
	make -C ./src/protocol/mqtt clean
	make -C ./src/ref-impl/hal clean
	make -C ./src/ref-impl/tls clean
	make -C ./src/sdk-impl clean
	make -C ./src/services/ota clean
	make -C ./src/services/linkkit/dm clean
	

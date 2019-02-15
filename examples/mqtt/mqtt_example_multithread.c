/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "iot_import.h"
#include "iot_export.h"
#include "app_entry.h"

//定义iot设备的三元组信息，产品KEY，设备名，设备密钥
#define PRODUCT_KEY             "a1I03ZK7sas"
#define DEVICE_NAME             "test"
#define DEVICE_SECRET           "Sc59fsDoDAHgPVDn6jla7rhvO3kRodtn"



/* These are pre-defined topics */
#define TOPIC_UPDATE            "/"PRODUCT_KEY"/"DEVICE_NAME"/user/update"
#define TOPIC_ERROR             "/"PRODUCT_KEY"/"DEVICE_NAME"/user/update/error"
#define TOPIC_GET               "/"PRODUCT_KEY"/"DEVICE_NAME"/user/get"

/*
主题订阅一次即可，无需重复订阅，重启程序不用重新订阅
*/
//该主题用来进行 云端Pub的消息通信
#define TOPIC_DATA              "/"PRODUCT_KEY"/"DEVICE_NAME"/user/data"

//该主题用来进行 云端PubBroadcast的消息通信
#define TOPIC_BOARDCAST_DATA              "/broadcast/"PRODUCT_KEY"/test1"

#define MQTT_MSGLEN             (1024)


// rrpc
#define TOPIC_RRPC_REQ       "/sys/"PRODUCT_KEY"/"DEVICE_NAME"/rrpc/request/"
#define TOPIC_RRPC_RSP       "/sys/"PRODUCT_KEY"/"DEVICE_NAME"/rrpc/response/"
#define RRPC_MQTT_MSGLEN    (1024)
#define MSG_ID_LEN_MAX      (64)
#define TOPIC_LEN_MAX       (1024)


//mqtt 连接变量
static void *pclient;


static void *g_thread_pub_1 = NULL;
static void *g_thread_pub_2 = NULL;

static void *g_thread_do_subscribes = NULL;

static int g_thread_pub_1_running = 1;
static int g_thread_pub_2_running = 1;


static int g_while_yield_running = 1;

#define EXAMPLE_TRACE(fmt, ...)  \
    do { \
        HAL_Printf("%s|%03d :: ", __func__, __LINE__); \
        HAL_Printf(fmt, ##__VA_ARGS__); \
        HAL_Printf("%s", "\r\n"); \
    } while(0)

//用来监听一些阿里iot自带的系统topic，比如控制台上的延迟检测
void event_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    uintptr_t packet_id = (uintptr_t)msg->msg;
    iotx_mqtt_topic_info_pt topic_info = (iotx_mqtt_topic_info_pt)msg->msg;

    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_UNDEF:
            EXAMPLE_TRACE("undefined event occur.");
            break;

        case IOTX_MQTT_EVENT_DISCONNECT:
            EXAMPLE_TRACE("MQTT disconnect.");
            break;

        case IOTX_MQTT_EVENT_RECONNECT:
            EXAMPLE_TRACE("MQTT reconnect.");
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS:
            EXAMPLE_TRACE("subscribe success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT:
            EXAMPLE_TRACE("subscribe wait ack timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_NACK:
            EXAMPLE_TRACE("subscribe nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS:
            EXAMPLE_TRACE("unsubscribe success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT:
            EXAMPLE_TRACE("unsubscribe timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_NACK:
            EXAMPLE_TRACE("unsubscribe nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_SUCCESS:
            EXAMPLE_TRACE("publish success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_TIMEOUT:
            EXAMPLE_TRACE("publish timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_NACK:
            EXAMPLE_TRACE("publish nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
            EXAMPLE_TRACE("topic message arrived but without any related handle: topic=%.*s, topic_msg=%.*s",
                          topic_info->topic_len,
                          topic_info->ptopic,
                          topic_info->payload_len,
                          topic_info->payload);
            break;

        default:
            EXAMPLE_TRACE("Should NOT arrive here.");
            break;
    }
}

//监听 data主题 和 broadcast 主题的消息。
static void mqtt_message_arrive(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    iotx_mqtt_topic_info_pt ptopic_info = (iotx_mqtt_topic_info_pt) msg->msg;

    /* print topic name and topic message */
    EXAMPLE_TRACE("----");
    EXAMPLE_TRACE("Topic: '%.*s' (Length: %d)",
                  ptopic_info->topic_len,
                  ptopic_info->ptopic,
                  ptopic_info->topic_len);
    EXAMPLE_TRACE("Payload: '%.*s' (Length: %d)",
                  ptopic_info->payload_len,
                  ptopic_info->payload,
                  ptopic_info->payload_len);
    EXAMPLE_TRACE("----");
}


//处理rrpc主题发送来的消息，并回复
void mqtt_rrpc_msg_arrive(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
	iotx_mqtt_topic_info_pt     ptopic_info = (iotx_mqtt_topic_info_pt) msg->msg;
	uintptr_t packet_id = (uintptr_t)msg->msg;
	switch (msg->event_type) {


		case IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS:
			EXAMPLE_TRACE("subscribe success, packet-id=%u", (unsigned int)packet_id);
			break;

		case IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT:
			EXAMPLE_TRACE("subscribe wait ack timeout, packet-id=%u", (unsigned int)packet_id);
			break;

		case IOTX_MQTT_EVENT_SUBCRIBE_NACK:
			EXAMPLE_TRACE("subscribe nack, packet-id=%u", (unsigned int)packet_id);
			break;
		case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
		{	 
			iotx_mqtt_topic_info_t      topic_msg;
			char                        msg_pub[RRPC_MQTT_MSGLEN] = {0};
			char                        topic[TOPIC_LEN_MAX] = {0};
			char                        msg_id[MSG_ID_LEN_MAX] = {0};

			/* print topic name and topic message */
			EXAMPLE_TRACE("----\n");
			EXAMPLE_TRACE("Topic: '%.*s' (Length: %d)\n",
					ptopic_info->topic_len,
					ptopic_info->ptopic,
					ptopic_info->topic_len);
			EXAMPLE_TRACE("Payload: '%.*s' (Length: %d)\n",
					ptopic_info->payload_len,
					ptopic_info->payload,
					ptopic_info->payload_len);
			EXAMPLE_TRACE("----\n");

			if (snprintf(msg_id,
						ptopic_info->topic_len - strlen(TOPIC_RRPC_REQ) + 1,
						"%s",
						ptopic_info->ptopic + strlen(TOPIC_RRPC_REQ))
					> sizeof(msg_id)) {
				EXAMPLE_TRACE("snprintf error!\n");
				return;
			}

			EXAMPLE_TRACE("response msg_id = %s\n", msg_id);
			if (snprintf(topic, sizeof(topic), "%s%s", TOPIC_RRPC_RSP, msg_id) > sizeof(topic)) {
				EXAMPLE_TRACE("snprintf error!\n");
				return;
			}
			EXAMPLE_TRACE("response topic = %s\n", topic);

			sprintf(msg_pub, "rrpc client has received message!\n");
			topic_msg.qos = IOTX_MQTT_QOS0;
			topic_msg.retain = 0;
			topic_msg.dup = 0;
			topic_msg.payload = (void *)msg_pub;
			topic_msg.payload_len = strlen(msg_pub);

			if (IOT_MQTT_Publish(pclient, topic, &topic_msg) < 0) {
				EXAMPLE_TRACE("error occur when publish!\n");
			}

			break;
		}
		case IOTX_MQTT_EVENT_BUFFER_OVERFLOW:
			EXAMPLE_TRACE("buffer overflow, %s", msg->msg);
			break;

		default:
			EXAMPLE_TRACE("Should NOT arrive here.");
			break;

	}

}




int do_subscribes(void *pclient)
{
    int     ret = -1;
    //订阅主题 data
    ret = IOT_MQTT_Subscribe(pclient, TOPIC_BOARDCAST_DATA, IOTX_MQTT_QOS1, mqtt_message_arrive, NULL);
    if (ret < 0) {
	    EXAMPLE_TRACE("subscribe error");
	    return -1;
    }

    HAL_SleepMs(1000);
   
    //订阅主题 boardcast
    ret = IOT_MQTT_Subscribe(pclient, TOPIC_DATA, IOTX_MQTT_QOS1, mqtt_message_arrive, NULL);
    if (ret < 0) {
	    EXAMPLE_TRACE("subscribe error");
	    return -1;
    }


    /* Subscribe the specific topic */
    ret = IOT_MQTT_Subscribe(pclient, TOPIC_RRPC_REQ "+", IOTX_MQTT_QOS0, mqtt_rrpc_msg_arrive, NULL);
    if (ret < 0) {
        EXAMPLE_TRACE("IOT_MQTT_Subscribe failed, rc = %d\n", ret);
        return -1;
    }

    if(g_thread_do_subscribes)
	    HAL_ThreadDelete(g_thread_do_subscribes); //必须线程已经起来了，不然执行这个函数会出现程序崩溃

    return 0;
}

int do_unsubscribe(void *pclient)
{
	int     ret = -1;
	ret = IOT_MQTT_Unsubscribe(pclient, TOPIC_DATA);
	if (ret < 0) {
		EXAMPLE_TRACE("subscribe error");
		return NULL;
	}
}




void *thread_publish1(void *pclient)
{
    int         ret = -1;
    char        msg_pub[MQTT_MSGLEN] = {0};

    iotx_mqtt_topic_info_t topic_msg;

    strcpy(msg_pub, "thread_publish1 message: hello! start!");
    topic_msg.qos = IOTX_MQTT_QOS1;
    topic_msg.retain = 0;
    topic_msg.dup = 0;
    topic_msg.payload = (void *)msg_pub;
    topic_msg.payload_len = strlen(msg_pub);

    while (g_thread_pub_1_running) {
        ret = IOT_MQTT_Publish(pclient, TOPIC_DATA, &topic_msg);
        EXAMPLE_TRACE("publish thread 1:ret = %d\n", ret);
        HAL_SleepMs(300);
    }

    return NULL;
}

void *thread_publish2(void *pclient)
{
    int         ret = -1;
    char        msg_pub[MQTT_MSGLEN] = {0};
    iotx_mqtt_topic_info_t topic_msg;

    strcpy(msg_pub, "thread_publish2 message: hello! start!");
    topic_msg.qos = IOTX_MQTT_QOS1;
    topic_msg.retain = 0;
    topic_msg.dup = 0;
    topic_msg.payload = (void *)msg_pub;
    topic_msg.payload_len = strlen(msg_pub);

    while (g_thread_pub_2_running) {
        ret = IOT_MQTT_Publish(pclient, TOPIC_DATA, &topic_msg);
        EXAMPLE_TRACE("publish thread 2:ret = %d\n", ret);
        HAL_SleepMs(200);
    }

    return NULL;
}

// publish
void CASE1(void *pclient)
{
    int   ret = -1;

    if (pclient == NULL) {
        EXAMPLE_TRACE("param error");
        return;
    }

    int stack_used = 0;
    hal_os_thread_param_t task_parms1 = {0};
    task_parms1.stack_size = 4096;
    task_parms1.name = "thread_publish1";
    ret = HAL_ThreadCreate(&g_thread_pub_1, thread_publish1, (void *)pclient, &task_parms1, &stack_used);
    if (ret != 0) {
        EXAMPLE_TRACE("Thread created failed!\n");
        return;
    }

    hal_os_thread_param_t task_parms2 = {0};
    task_parms2.stack_size = 4096;
    task_parms2.name = "thread_publish2";
    ret = HAL_ThreadCreate(&g_thread_pub_2, thread_publish2, (void *)pclient, &task_parms2, &stack_used);
    if (ret != 0) {
        EXAMPLE_TRACE("Thread created failed!\n");
        return;
    }
}

// ota 经过测试， 程序启动时会对比，线上版本跟当前版本，版本不对会自动下载。
#define OTA_MQTT_MSGLEN         (2048)
int ota_mqtt_loop( void *pclient )
{
	EXAMPLE_TRACE("into ota_mqtt_loop");
#define OTA_BUF_LEN        (5000)

	int rc = 0, ota_over = 0;
	char *h_ota = NULL;
	iotx_conn_info_pt pconn_info;
	iotx_mqtt_param_t mqtt_params;
	char *msg_buf = NULL, *msg_readbuf = NULL;
	FILE *fp;
	char buf_ota[OTA_BUF_LEN];

	if (NULL == (fp = fopen("/tmp/ota.bin", "wb+"))) {
		EXAMPLE_TRACE("open file failed");
		goto do_exit;
	}   


	if (NULL == (msg_buf = (char *)HAL_Malloc(OTA_MQTT_MSGLEN))) {
		EXAMPLE_TRACE("not enough memory");
		rc = -1; 
		goto do_exit;
	}   

	if (NULL == (msg_readbuf = (char *)HAL_Malloc(OTA_MQTT_MSGLEN))) {
		EXAMPLE_TRACE("not enough memory");
		rc = -1; 
		goto do_exit;
	}   


	h_ota = IOT_OTA_Init(PRODUCT_KEY, DEVICE_NAME, pclient);
	if (NULL == h_ota) {
		rc = -1;
		EXAMPLE_TRACE("initialize OTA failed");
		goto do_exit;
	}

	HAL_SleepMs(1000);


	do {
		uint32_t firmware_valid;

		EXAMPLE_TRACE("wait ota upgrade command....");

		if (IOT_OTA_IsFetching(h_ota)) {
			uint32_t last_percent = 0, percent = 0;
			char md5sum[33];
			char version[128] = {0};
			uint32_t len, size_downloaded, size_file;
			do {

				len = IOT_OTA_FetchYield(h_ota, buf_ota, OTA_BUF_LEN, 1);
				if (len > 0) {
					if (1 != fwrite(buf_ota, len, 1, fp)) {
						EXAMPLE_TRACE("write data to file failed");
						rc = -1;
						break;
					}
				} else {
					IOT_OTA_ReportProgress(h_ota, IOT_OTAP_FETCH_FAILED, NULL);
					EXAMPLE_TRACE("ota fetch fail");
				}

				/* get OTA information */
				IOT_OTA_Ioctl(h_ota, IOT_OTAG_FETCHED_SIZE, &size_downloaded, 4);
				IOT_OTA_Ioctl(h_ota, IOT_OTAG_FILE_SIZE, &size_file, 4);
				IOT_OTA_Ioctl(h_ota, IOT_OTAG_MD5SUM, md5sum, 33);
				IOT_OTA_Ioctl(h_ota, IOT_OTAG_VERSION, version, 128);

				last_percent = percent;
				percent = (size_downloaded * 100) / size_file;
				if (percent - last_percent > 0) { 
					IOT_OTA_ReportProgress(h_ota, percent, NULL);
					IOT_OTA_ReportProgress(h_ota, percent, "hello");
				}
			} while (!IOT_OTA_IsFetchFinish(h_ota));

			IOT_OTA_Ioctl(h_ota, IOT_OTAG_CHECK_FIRMWARE, &firmware_valid, 4);
			if (0 == firmware_valid) {
				EXAMPLE_TRACE("The firmware is invalid");
			} else {
				EXAMPLE_TRACE("The firmware is valid");
			}

			ota_over = 1;
		}
		HAL_SleepMs(2000);
	} while (!ota_over);

	HAL_SleepMs(200);


do_exit:
	EXAMPLE_TRACE(" do_exit ota");

	if (NULL != h_ota) {
		IOT_OTA_Deinit(h_ota);
	}


	if (NULL != msg_buf) {
		HAL_Free(msg_buf);
	}

	if (NULL != msg_readbuf) {
		HAL_Free(msg_readbuf);
	}

	if (NULL != fp) {
		fclose(fp);
	}


	return rc;
}


int  thread_rrpc(void *pclient)
{

    ota_mqtt_loop( pclient );

}



static uint64_t user_update_sec(void)
{
    static uint64_t time_start_ms = 0;

    if (time_start_ms == 0) {
        time_start_ms = HAL_UptimeMs();
    }


    EXAMPLE_TRACE("user_update_sec %d seconds\n",  (HAL_UptimeMs() - time_start_ms) / 1000 );
    return (HAL_UptimeMs() - time_start_ms) / 1000;
}

int thread_subscribes()
{
	int   ret = -1;

	if (pclient == NULL) {
		EXAMPLE_TRACE("param error");
		return;
	}

	int stack_used = 0;
	hal_os_thread_param_t task_parms1 = {0};
	task_parms1.stack_size = 4096;
	task_parms1.name = "do_subscribes";
	ret = HAL_ThreadCreate(&g_thread_do_subscribes, do_subscribes, (void *)pclient, &task_parms1, &stack_used);
	if (ret != 0) {
		EXAMPLE_TRACE("Thread created failed!\n");
		return;
	}
}


//关闭线程，以及 关闭mqtt连接
void close_mqtt_client()
{
	HAL_ThreadDelete(g_thread_pub_1);
	HAL_ThreadDelete(g_thread_pub_2);

	if(g_thread_do_subscribes)
	    HAL_ThreadDelete(g_thread_do_subscribes); //必须线程已经起来了，不然执行这个函数会出现程序崩溃

	IOT_MQTT_Destroy(&pclient);
}


int mqtt_client(void *params)
{
    int rc = 0;//, msg_len, cnt = 0;
    iotx_conn_info_pt pconn_info;
    iotx_mqtt_param_t mqtt_params;

//    int                             argc = ((app_main_paras_t *)params)->argc;
//    char                          **argv = ((app_main_paras_t *)params)->argv;


    /* Device AUTH */
    /*在与服务器尝试建立MQTT连接前, 填入设备身份认证信息:*/
    if (0 != IOT_SetupConnInfo(PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, (void **)&pconn_info)) {
        EXAMPLE_TRACE("AUTH request failed!");
        return -1;
    }

    /* Initialize MQTT parameter */
    memset(&mqtt_params, 0x0, sizeof(mqtt_params));

    mqtt_params.port = pconn_info->port;
    mqtt_params.host = pconn_info->host_name;
    mqtt_params.client_id = pconn_info->client_id;
    mqtt_params.username = pconn_info->username;
    mqtt_params.password = pconn_info->password;
    mqtt_params.pub_key = pconn_info->pub_key;

    mqtt_params.request_timeout_ms = 2000;
    mqtt_params.clean_session = 0;
    mqtt_params.keepalive_interval_ms = 60000;
    mqtt_params.read_buf_size = MQTT_MSGLEN;
    mqtt_params.write_buf_size = MQTT_MSGLEN;

    mqtt_params.handle_event.h_fp = event_handle;
    mqtt_params.handle_event.pcontext = NULL;


    /* Construct a MQTT client with specify parameter */
    pclient = IOT_MQTT_Construct(&mqtt_params);
    if (NULL == pclient) {
        EXAMPLE_TRACE("MQTT construct failed");
        return -1;
    }

    //先执行订阅操作，即向 iot中心 订阅主题，iot可以推送信息给当前设备。
    //    do_subscribes( pclient ); //不用线程方式订阅
    thread_subscribes();  //使用线程方式订阅，异步向一步操作，避免阻塞在此。
   
    // 推送信息给 iot 中心的线程，先注释掉
    //    CASE1(pclient);

    // 该循环用来监听以及订阅的主题（可以多个）的iot消息回复 
    while (g_while_yield_running) {
	    //阻塞并获取IoT回复的消息。200为 200ms阻塞时间
	    IOT_MQTT_Yield(pclient, 200);

	    //延迟200ms
	    HAL_SleepMs(200);
    }

    HAL_SleepMs(3000);
    
do_exit:
    close_mqtt_client();
    return rc;
}


int linkkit_main(void *params)
{
	//设置log 等级
	IOT_SetLogLevel(IOT_LOG_DEBUG);
	/**< set device info*/

	HAL_SetProductKey(PRODUCT_KEY);
	HAL_SetDeviceName(DEVICE_NAME);
	HAL_SetDeviceSecret(DEVICE_SECRET);

	/**< end*/
	mqtt_client(params);
	IOT_DumpMemoryStats(IOT_LOG_DEBUG);
	IOT_SetLogLevel(IOT_LOG_NONE);

	EXAMPLE_TRACE("out of sample!");

	return 0;
}



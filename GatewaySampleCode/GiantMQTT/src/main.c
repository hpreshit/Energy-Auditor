
#include "mqtt/MQTTClient.h"
#include "mqtt/MQTTConnect.h"
#include "wrapper/MQTTOsWrapper.h"
#include "em_chip.h"
#include "em_rtc.h"
#include "rtcdriver.h"
#include "udelay.h"

#include "retargetserial.h"

#define MQTT_PUB_TOPIC              "sdk/test/Python"
#define MQTT_SUB_TOPIC              "sdk/test/Python_get"
#define MQTT_CLIENT_ID              "basicPubSub"
#define MQTT_USER                   "aakashpradipkumar@gmail.com"
#define MQTT_PASSWORD               "112THree"
#define MQTT_HOST                   "a3gj7zzjc39ic0-ats.iot.us-east-2.amazonaws.com"
#define MQTT_PORT                   MQTT_CLEAR_PORT //MQTT_SECURE_PORT

#define PROXY_IP					"10.0.0.200"
#define PROXY_PORT					8000

#define MQTT_PUBLISH_PERIOD         5000
#define MQTT_MESSAGE                "{\"state\":{\"desired\":{\"message\":\"Time now: %lu\"}}}"
#define COMMAND_START_PERIODIC      "{\"state\":{\"desired\":{\"message\":\"PERIODIC\"}}"
#define COMMAND_STOP_PERIODIC       "{\"state\":{\"desired\":{\"message\":\"STOP\"}}"
#define COMMAND_READ_ONCE           "{\"state\":{\"desired\":{\"message\":\"READ\"}}"

static RTCDRV_TimerID_t periodic_timer;
static MQTTClient client = DefaultClient;
static bool publish_now = true;//false;


static void publish();
static void timer_handler( RTCDRV_TimerID_t id, void *user);
static void receive_handler(MessageData* rx_msg);

/******************************************************************************
 * @brief  Main
 *****************************************************************************/
int main(void)
{
    int ret;
    unsigned char send_buf[512];
    unsigned char recv_buf[2048];

    Network network;
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;

    /* Chip errata */
    CHIP_Init();

    /* Initialize RTC timer. */
    RTCDRV_Init();
    RTCDRV_AllocateTimer(&periodic_timer);

    //
    RETARGET_SerialInit();
    RETARGET_SerialCrLf(true);

    printf("Initialized \n");




    //
    data.MQTTVersion = 4;
    data.clientID.cstring = MQTT_CLIENT_ID;
    data.username.cstring = MQTT_USER;
    data.password.cstring = MQTT_PASSWORD;


    NetworkInit(&network);
    set_machine_mode();
//    Sleep_ms(250);
    UDELAY_Delay(250000);
    printf("Network Init Done\n");
	#define DEBUG 1

    #if DEBUG
    uint32_t val;
    val = get_time();
	printf("%d", val);

	static uint16_t node = 9876, current = 1000;

    //ret = NetworkConnect(&network, MQTT_HOST, MQTT_PORT);
    while(1)
    {
    	MQTT_publish(PROXY_IP, PROXY_PORT,node,current);

    	for(int i=0;i< 500;i++)
    		UDELAY_Delay(1000000);

    	node ++;
    	current++;
    }
	#endif


    #if !DEBUG

    uint32_t val;
	val = get_time();
	printf("%d", val);

    ret = NetworkConnect(&network, MQTT_HOST, MQTT_PORT);
    if(ret == FAILURE)
    {
    	printf("Connect Fail\n");
    	while(1){}
        // error, return
        return ret;
    }
//    Sleep_ms(250);
    UDELAY_Delay(250000);

    printf("Connect Done\n");
    MQTTClientInit(&client, &network, 10000, send_buf, sizeof(send_buf), recv_buf, sizeof(recv_buf));
//    Sleep_ms(250);
    UDELAY_Delay(250000);
    printf("MQTT Init Done\n");

    ret = MQTTConnect(&client, &data);
    if(ret != SUCCESS)
    {
    	printf("Failure\n");
        // error, return
        return ret;
    }
//    Sleep_ms(250);
    UDELAY_Delay(250000);
    printf("MQTT Connect\n");

//---    MQTTSubscribe(&client, MQTT_SUB_TOPIC, QOS0, receive_handler);
//    Sleep_ms(250);
    UDELAY_Delay(250000);

    static int temp_cnt =0;
    // running forever to receive messages from server
    while(true)
    {
        // can not execute UART blocking operations in timer handler,
        // so, we just set a flag and check it here in main app..
    	printf("%d \n",temp_cnt++);
    	if(publish_now == true)
        {
            publish_now = false;
            publish();
        }
        MQTTYield(&client, 500);
    }

    // won't reach here as MQTTRun is blocking
    MQTTUnsubscribe(&client, MQTT_SUB_TOPIC);
    MQTTDisconnect(&client);
    NetworkDisconnect(&network);

    #endif
    return 0;
}

/******************************************************************************
 * @brief  Static functions
 *****************************************************************************/

static void publish()
{
    char buf[100];
    MQTTMessage message;

    sprintf(buf, MQTT_MESSAGE, (long unsigned int) TICKS_TO_MSEC(RTCC_CounterGet())/1000);
    message.qos = QOS0;
    message.retained = false;
    message.dup = false;
    message.payload = (void*)buf;
    message.payloadlen = strlen(buf);

    MQTTPublish(&client, MQTT_PUB_TOPIC, &message);
}

/*****************************************************************************/
static void timer_handler( RTCDRV_TimerID_t id, void *user)
{
    // time to publish.. set the flag and let the main app executes the publish
    publish_now = true;
}

/*****************************************************************************/
static void receive_handler(MessageData* rx_msg)
{
    char cmd[2048];

    snprintf(cmd, rx_msg->message->payloadlen + 1, rx_msg->message->payload);
    if(strncmp(cmd, COMMAND_READ_ONCE, strlen(COMMAND_READ_ONCE)) == 0)
    {
        publish_now = true;
    }
    else if(strncmp(cmd, COMMAND_START_PERIODIC, strlen(COMMAND_START_PERIODIC)) == 0)
    {
        RTCDRV_StartTimer(periodic_timer, rtcdrvTimerTypePeriodic, MQTT_PUBLISH_PERIOD, timer_handler, NULL);
        publish_now = true;
    }
    else if(strncmp(cmd, COMMAND_STOP_PERIODIC, strlen(COMMAND_STOP_PERIODIC)) == 0)
    {
        RTCDRV_StopTimer(periodic_timer);
    }

}




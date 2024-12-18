#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <phydat.h>
#include "timex.h"
#include "ztimer.h"
#include "shell.h"
#include "thread.h"
#include "mutex.h"
#include "paho_mqtt.h"
#include "MQTTClient.h"
#include "saul_reg.h"
#include "unistd.h"
#include "math.h"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

#define BUF_SIZE                        1024
#define MQTT_VERSION_v311               4
#define COMMAND_TIMEOUT_MS              4000

#ifndef DEFAULT_MQTT_CLIENT_ID
#define DEFAULT_MQTT_CLIENT_ID          ""
#endif

#ifndef DEFAULT_MQTT_USER
#define DEFAULT_MQTT_USER               ""
#endif

#ifndef DEFAULT_MQTT_PWD
#define DEFAULT_MQTT_PWD                ""
#endif

#define DEFAULT_MQTT_PORT               1883

#define DEFAULT_KEEPALIVE_SEC           10

#ifndef MAX_LEN_TOPIC
#define MAX_LEN_TOPIC                   100
#endif

#ifndef MAX_TOPICS
#define MAX_TOPICS                      4
#endif

#define IS_CLEAN_SESSION                1
#define IS_RETAINED_MSG                 0

#define REMOTE_IP "test.mosquitto.org"



static MQTTClient client;
static Network network;

static int publish(char* topic, char* payload) {
    enum QoS qos = QOS0;

    char buffer[100];
    sprintf(buffer, "{ \"text\": \"%s\" }", payload);

    MQTTMessage message;
    message.qos = qos;
    message.retained = IS_RETAINED_MSG;
    message.payload = buffer;
    message.payloadlen = strlen(buffer);

    int rc;
    if ((rc = MQTTPublish(&client, topic, &message)) < 0) {
        puts("Unable to publish");
    }
    else {
        printf("Successfully published \"%s\"\n", (char*)payload);
    }

    return rc;
}

static void read_temperature(char* temp) {
    saul_reg_t* reg = saul_reg_find_type(SAUL_SENSE_TEMP);

    phydat_t data;
    data.unit = UNIT_TEMP_C;

    saul_reg_read(reg, &data);

    sprintf(temp, "%.2f °C", ((int)data.val[0]) * pow(10, data.scale));
}

static unsigned char buf[BUF_SIZE];
static unsigned char readbuf[BUF_SIZE];

int main(void)
{
    if (IS_USED(MODULE_GNRC_ICMPV6_ECHO)) {
        msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    }

    NetworkInit(&network);

    MQTTClientInit(&client, &network, COMMAND_TIMEOUT_MS, buf, BUF_SIZE, readbuf, BUF_SIZE);

    MQTTStartTask(&client);

    gnrc_netif_ipv6_wait_for_global_address(NULL, 2 * MS_PER_SEC);

    int ret = -1;
    char* remote_ip = REMOTE_IP;
    int port = DEFAULT_MQTT_PORT;
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = MQTT_VERSION_v311;
    data.cleansession = IS_CLEAN_SESSION;
    data.willFlag = 0;

    ret = NetworkConnect(&network, remote_ip, port);
    if (ret < 0) {
        puts("Unable to connect to network");
        return ret;
    }

    ret = MQTTConnect(&client, &data);
    if (ret < 0) {
        puts("Unable to connect client");
        return ret;
    }
    else {
        puts("Successfully connected");
    }

    puts("Waiting for initial DHT sensor delay...");

    sleep(3); // required, or ECANCELED

    puts("Entering loop...");

    while (1) {
        char temp[10];
        read_temperature(temp);

        printf("DEBUG: %s\n", temp);

        publish("awtrix_f53da4/notify", temp);

        sleep(5);
    }

    return 0;
}

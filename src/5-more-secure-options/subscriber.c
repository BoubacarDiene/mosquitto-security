//////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                              //
//          Copyright © 2020 Boubacar DIENE                                                     //
//                                                                                              //
//          This file is part of mosquitto-security project.                                    //
//                                                                                              //
//          mosquitto-security is free software: you can redistribute it and/or modify          //
//          it under the terms of the GNU General Public License as published by                //
//          the Free Software Foundation, either version 2 of the License, or                   //
//          (at your option) any later version.                                                 //
//                                                                                              //
//          mosquitto-security is distributed in the hope that it will be useful,               //
//          but WITHOUT ANY WARRANTY; without even the implied warranty of                      //
//          MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                       //
//          GNU General Public License for more details.                                        //
//                                                                                              //
//          You should have received a copy of the GNU General Public License                   //
//          along with mosquitto-security. If not, see <http://www.gnu.org/licenses/>           //
//          or write to the Free Software Foundation, Inc., 51 Franklin Street,                 //
//          51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.                       //
//                                                                                              //
//////////////////////////////////////////////////////////////////////////////////////////////////

/*!
 * \file subsriber.c
 * \brief Subscriber code to test "topic-more-secure-options" security mechanism
 * \author Boubacar DIENE
 */

/* -------------------------------------------------------------------------------------------- */
/* ////////////////////////////////////////// HEADERS ///////////////////////////////////////// */
/* -------------------------------------------------------------------------------------------- */

#include <errno.h>
#include <semaphore.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include <mosquitto.h>

#include "Log.h"

/* -------------------------------------------------------------------------------------------- */
/* ////////////////////////////////////////// MACROS ////////////////////////////////////////// */
/* -------------------------------------------------------------------------------------------- */

#undef  TAG
#define TAG "subscriber"

#define BROKER_HOSTNAME "localhost"
#define BROKER_PORT     8883

#define TOPIC "/topic/more/secure/options"

#define CERTS_DIRECTORY BUILD_ETC_DIR"/certificates"
#define CAFILE          CERTS_DIRECTORY"/ca/ca.crt"
#define CERTFILE        CERTS_DIRECTORY"/clients/subscriber/client.crt"
#define KEYFILE         CERTS_DIRECTORY"/clients/subscriber/client.key"

/* -------------------------------------------------------------------------------------------- */
/* //////////////////////////////////////// VARIABLES ///////////////////////////////////////// */
/* -------------------------------------------------------------------------------------------- */

static sem_t stopSem;

/* -------------------------------------------------------------------------------------------- */
/* //////////////////////////////////////// PROTOTYPES //////////////////////////////////////// */
/* -------------------------------------------------------------------------------------------- */

static void signalHandler(int signalNumber);

static void onSubscribe(struct mosquitto *mosq,
                        void *obj,
                        int mid,
                        int qos_count,
                        const int *granted_qos);
static void onMessage(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message);
static void onLog(struct mosquitto *mosq, void *obj, int level, const char *str);

/* -------------------------------------------------------------------------------------------- */
/* /////////////////////////////////////////// MAIN /////////////////////////////////////////// */
/* -------------------------------------------------------------------------------------------- */

int main(int argc, char **argv)
{
    Logd("Initialize semaphore and signal handler");
    sem_init(&stopSem, 0, 0);

    struct sigaction sa;    
    (void)sigemptyset(&sa.sa_mask);
    sa.sa_flags    = 0;
    sa.sa_restorer = NULL;
    sa.sa_handler  = signalHandler;
    (void)sigaction(SIGINT, &sa, NULL); 

    Logd("Initialize mosquitto library");
    mosquitto_lib_init();

    Logd("Initialize client");
    struct mosquitto *mosq = mosquitto_new("secure-subscriber-id", true, NULL);

    Logd("Set client's username");
    mosquitto_username_pw_set(mosq, "secure-subscriber-username", NULL);

    Logd("Configure the client for certificate based SSL/TLS support");
    mosquitto_tls_set(mosq, CAFILE, NULL, CERTFILE, KEYFILE, NULL);

    Logd("Register callbacks to get notified about subscription, message and logs");
    mosquitto_subscribe_callback_set(mosq, &onSubscribe);
    mosquitto_message_callback_set(mosq, &onMessage);
    mosquitto_log_callback_set(mosq, &onLog);

    Logd("Connect client to the broker");
    mosquitto_connect(mosq, BROKER_HOSTNAME, BROKER_PORT, 10);

    /**
     * The topic can also be set in command line to test the behaviour
     * when a value not allowed in the ACL is used
     */
    const char *topic = (argc == 2 ? argv[1] : TOPIC);
    Logd("Subscribe to topic: %s", topic);
    mosquitto_subscribe(mosq, NULL, topic, 0);

    /**
     * Loop: wait in a different thread until sem_post() is called by the signal
     * handler then move to next step
     */
    mosquitto_loop_start(mosq);
    Logd("Loop started");
    sem_wait(&stopSem);
    Logd("Loop stopped");
    mosquitto_loop_stop(mosq, /*true => Do not wait for mosquitto_disconnect()*/true);

    Logd("Release resources");
    mosquitto_unsubscribe(mosq, NULL, topic);
    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);

    mosquitto_lib_cleanup();

    sem_destroy(&stopSem);

    return EXIT_SUCCESS;
}

/* -------------------------------------------------------------------------------------------- */
/* ///////////////////////////////////// SIGNAL HANDLERS ////////////////////////////////////// */
/* -------------------------------------------------------------------------------------------- */

static void signalHandler(int signalNumber)
{
    (void)signalNumber;

    int savedErrno = errno;

    Logd_safe("sem_post() from handler\n", 24);
    if (sem_post(&stopSem) == -1) {
        Loge_safe("sem_post() failed\n", 18);
        _exit(EXIT_FAILURE);
    }

    errno = savedErrno;
}

/* -------------------------------------------------------------------------------------------- */
/* //////////////////////////////////////// CALLBACKS ///////////////////////////////////////// */
/* -------------------------------------------------------------------------------------------- */

static void onSubscribe(struct mosquitto *mosq,
                        void *obj,
                        int mid,
                        int qos_count,
                        const int *granted_qos)
{
    (void)mosq;
    (void)obj;
    (void)granted_qos;

    Logi("\n\tmid=%d\n\tqos_count=%d", mid, qos_count);
}

static void onMessage(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
    (void)mosq;
    (void)obj;

    Logi("\n\tmid=%d\n\ttopic=%s\n\tpayload=%s\n\tpayloadlen=%u\n\tqos=%d\n\tretain=%d",
          message->mid, message->topic, (char*)message->payload, message->payloadlen,
          message->qos, message->retain);
}

static void onLog(struct mosquitto *mosq, void *obj, int level, const char *str)
{
    (void)mosq;
    (void)obj;

    Logd("\n\tlevel=%d\n\tstr=%s", level, str);
}

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
 * \brief Subscriber code to test "client id prefix" security mechanism
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
#define BROKER_PORT     1883

#define TOPIC "/topic/client/id/prefix"

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

    /**
     * Initialize client with id starting with "secure-" (see mosquitto.conf)
     * The clientId can also be provided in command line to test the behaviour
     * when a non-compliant value is provided
     */
    const char *clientId = (argc == 2 ? argv[1] : "secure-subscriber-id");
    Logd("Initialize client with id: %s", clientId);
    struct mosquitto *mosq = mosquitto_new(clientId, true, NULL);

    Logd("Register callbacks to get notified about subscription, message and logs");
    mosquitto_subscribe_callback_set(mosq, &onSubscribe);
    mosquitto_message_callback_set(mosq, &onMessage);
    mosquitto_log_callback_set(mosq, &onLog);

    Logd("Connect client to the broker");
    mosquitto_connect(mosq, BROKER_HOSTNAME, BROKER_PORT, 10);

    Logd("Subscribe to topic: %s", TOPIC);
    mosquitto_subscribe(mosq, NULL, TOPIC, 0);

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
    mosquitto_unsubscribe(mosq, NULL, TOPIC);
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

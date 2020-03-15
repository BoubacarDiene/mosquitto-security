# mosquitto-security

## Description

One purpose of this project is to show how to use [mosquitto](https://mosquitto.org/).

With the provided Dockerfile and sample codes, one can easily set up a working
environment to start testing mosquitto and quickly prototyping solutions.

However, the main purpose is to experiment security mechanisms which can allow
to authenticate clients and/or secure communication between clients and the broker.

For more details, see:
- [Introduction to MQTT Security Mechanisms](http://www.steves-internet-guide.com/mqtt-security-mechanisms/)
- [MQTTS : Comment utiliser MQTT avec TLS ?](https://openest.io/2019/02/06/chiffrement-communication-mqtt-tls-ssl-mosquitto-et-paho/)
- [mosquitto-tls man page](https://mosquitto.org/man/mosquitto-tls-7.html)
- [OpenSSL Quick Reference Guide](https://www.digicert.com/kb/ssl-support/openssl-quick-reference-guide.htm)

## Build and run examples

### Generate docker image

```
docker build -t mosquitto-image ci/
```

### Build
```
docker run --privileged -it -u $(id -u) --rm -v $(pwd):/workdir mosquitto-image:latest

make
```

### Run

| Which bash terminal? | Commands |
| --- | --- |
| #1 | ```docker run --privileged -it -u $(id -u) --rm -v $(pwd):/workdir mosquitto-image:latest```<br><br>```mosquitto -v -c out/<example_directory_name>/mosquitto.conf``` |
| #2 | ```docker exec -it <container_id> /bin/bash```<br><br>```./out/<example_directory_name>/subscriber <args>``` |
| #3 | ```docker exec -it <container_id> /bin/bash```<br><br>```./out/<example_directory_name>/publisher <args>``` |

**NB:** ```docker ps``` to find out the container_id

## Notes

### Stop a running program: Ctrl+C

A "semaphore + signal handler" is used to handle the way the process is stopped.

Why a semaphore?
- sem_post() is async-signal-safe meaning it can be safely called inside a signal handler
- mosquitto_loop_forever() is stopped when mosquitto_disconnect() is called.
  mosquitto_disconnect() seems not reentrant (internal use of malloc()/free() and
  printf-family functions, ...) so it is not safe to call it inside a handler
- Another possible solution is to make the signal handler only update an atomic variable
  of type "volatile sig_atomic_t" then unblock the main program.

Also note that
- Errno is always saved and restored by the signal handler to make sure it will not cause
  any unexpected behaviour.
  For example, the following scenario may occur:
  + A function call sets errno value
  + That function call gets interrupted by the signal handler
  + Back to the function call which checks errno value

### Error checking

To improve readability and keep focused on the purpose of this project, no error checking
is performed.

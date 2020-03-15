#!/bin/bash

# ./scripts/generate-certificates.sh <path_to_directory_where_to_create_certificates_folder>
# or (for current directory)
# ./scripts/generate-certificates.sh
DEST_PATH=${1:-`pwd`}
CERTS_PATH=$DEST_PATH/certificates

PEM_PASSPHRASE=ca-pem-passphrase
NB_DAYS=10

COMMON_NAME=localhost

#
# Certification authority
#
mkdir -p $CERTS_PATH/ca && cd $CERTS_PATH/ca
openssl req -new \
        -x509 -days $NB_DAYS -extensions v3_ca -keyout ca.key \
        -out ca.crt \
        -passout pass:$PEM_PASSPHRASE \
        -subj "/C=SN/ST=Senegal/L=Thies/O=Mosquitto Security/OU=MS/CN=mosquitto-security.com"

#
# Broker
#
mkdir -p $CERTS_PATH/broker && cd $CERTS_PATH/broker
openssl genrsa -out broker.key 2048
openssl req -out broker.csr -key broker.key -new \
        -subj "/C=SN/ST=Senegal/L=Dakar/O=Broker Inc./OU=BI/CN=$COMMON_NAME"
openssl x509 -req -in broker.csr \
        -CA $CERTS_PATH/ca/ca.crt -CAkey $CERTS_PATH/ca/ca.key -CAcreateserial \
        -out broker.crt -days $NB_DAYS \
        -passin pass:$PEM_PASSPHRASE

#
# Subscriber
#
mkdir -p $CERTS_PATH/clients/subscriber && cd $CERTS_PATH/clients/subscriber
openssl genrsa -out client.key 2048
openssl req -out client.csr -key client.key -new \
        -subj "/C=FR/ST=France/L=Paris/O=Subscriber Inc./OU=SI/CN=$COMMON_NAME"
openssl x509 -req -in client.csr \
        -CA $CERTS_PATH/ca/ca.crt -CAkey $CERTS_PATH/ca/ca.key -CAcreateserial \
        -out client.crt -days $NB_DAYS \
        -passin pass:$PEM_PASSPHRASE

#
# Publisher
#
mkdir -p $CERTS_PATH/clients/publisher && cd $CERTS_PATH/clients/publisher
openssl genrsa -out client.key 2048
openssl req -out client.csr -key client.key -new \
        -subj "/C=KE/ST=Kenya/L=Nairobi/O=Publisher Inc./OU=PI/CN=$COMMON_NAME"
openssl x509 -req -in client.csr \
        -CA $CERTS_PATH/ca/ca.crt -CAkey $CERTS_PATH/ca/ca.key -CAcreateserial \
        -out client.crt -days $NB_DAYS \
        -passin pass:$PEM_PASSPHRASE

exit 0

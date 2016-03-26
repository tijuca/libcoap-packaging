#!/bin/sh

set -e
set -x

cd debian/tests

apt-get -y install libcoap-1-0-bin libcoap-1-0-dev pkg-config

echo "Compiling, linking the CoAP testing server ..."
gcc -g -o coap-test-server coap-test-server.c -O2 -D_GNU_SOURCE -DWITH_POSIX `pkg-config --libs libcoap-1`
[ "$?" != "0" ] && echo "Failed!" && exit 1

echo "Starting the CoAP testing server ..."
./coap-test-server &

PID_COAP_SERVER=$(pidof coap-test-server)
[ "${PID_COAP_SERVER}" = "" ] && echo "Failed!" && exit 1

echo "Probing CoAP testing server using IPv4 ..."
RET=$(coap-client -m get coap://127.0.0.1/ | grep CoAP)

[ "${RET}" = "" ] && echo "Failed!" && exit 1
RET=""

echo "Probing CoAP testing server using IPv6 ..."
RET=$(coap-client -m get coap://[::1]/ | grep CoAP)

[ "${RET}" = "" ] && echo "Failed!" && exit 1
RET=""

kill ${PID_COAP_SERVER}

echo 'Testing coap-test-server succesful!'
exit 0
